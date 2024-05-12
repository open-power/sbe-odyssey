/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/avsbus/pst_avsbus_driver.c $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */

#include "pst_avsbus_driver.h"
#include <pst_ppb_hub.H>
#include <ppehw_common.h>
#include <pst_ppb_avsvrm.H>
#include <pst_ppb_global.H>
#include <pst_ppb_tap.H>
#include "ppe42_scom.h"
#include "pst_ppb_avsvrm.H"
#include "pst_pm_hcd_flags.h"
#include "pst_hcd_common.H"
#include "pst_ppb_global_base.H"
#include "pst_hcd_memmap_occ_sram.H"

//-------------------------------------------------------------------------
//Globals
Avsbus_t G_avsbus;
GPPBAVSBusVRM_t* G_gppb_avsbus;
GPPBBase_t*      G_gppb_base;
extern GlobalPstateParmBlock_t* G_gppb;
//-------------------------------------------------------------------------

typedef union
{
    uint64_t value;
    struct
    {
        uint32_t upper;
        uint32_t lower;
    } words;
} tod_t;

//Local Functions
// uint32_t avsbus_calc_crc(uint32_t data);
// uint32_t avsbus_poll_trans_done(uint32_t i_bus_num);
// uint32_t avsbus_drive_idle_frame(uint32_t i_bus_num);
// uint32_t avsbus_drive_write(uint32_t i_cmd_data_type, uint32_t i_cmd_data, uint32_t i_bus_num, uint32_t i_rail_num);
// uint32_t avsbus_drive_read(uint32_t i_cmd_data_type, uint32_t* i_cmd_data, uint32_t i_bus_num, uint32_t i_rail_num);

static uint32_t AVS_CONTROL_RETRIES = 500;
static uint32_t AVS_RESYNC_RETRIES = 1;


inline uint32_t round_dec( uint32_t i_value )
{
    return ( i_value * 10 + 5 ) / 10;
}

// Function to compute the difference in the timebase
uint32_t delta_tb( uint32_t i_start_time, uint32_t i_end_time )
{
    uint32_t l_tb_delta;

    if( i_start_time > i_end_time )
    {
        i_start_time += 0xFFFFFFFF;
    }

    l_tb_delta = i_end_time - i_start_time;
    return l_tb_delta;
}

// Function to compute number of timebase ticks given time (in us)
uint32_t us_to_tb( uint32_t i_us )
{
    // ns/(ns/tb) = tb
    return round_dec( i_us * 1000 / G_avsbus.timebase_tick_ns );
}

static inline void probe0_assert()
{
    if( in32(OCB_OCCFLG2) & BIT32(PGPE_PROBE_ENABLE) )
    {
        uint64_t l_data = 0;

        // Assert
        // putscom pu 01000008 0 8 28     # read-modify-write
        // 28 = 0010 1000
        PPE_GETSCOM( 0x01000008, l_data );

        l_data = l_data & ~BIT64(11) & ~BIT64(13); //clear deassert bits

        PPE_PUTSCOM(0x01000008, (l_data | BIT64(10) | BIT64 (12)));
    }
}

static inline void probe0_deassert()
{
    if( in32(OCB_OCCFLG2) & BIT32(PGPE_PROBE_ENABLE) )
    {
        uint64_t l_data = 0;

        // Deassert
        // putscom pu 01000008 0 8 14     # read-modify-write
        // 14 = 0001 0100

        PPE_GETSCOM(0x01000008, l_data);

        l_data = l_data & ~BIT64(10) & ~BIT64(12); //clear assert bits 2 and 4

        PPE_PUTSCOM(0x01000008, (l_data | BIT64(11) | BIT64 (13)));
    }
}

void avs_profile( Avs_profile_t* i_p, uint32_t i_start_time )
{
    if( in32(OCB_OCCFLG2) & BIT32(PGPE_AVS_PROFILE_ENABLE) )
    {
        uint32_t i_end_time = in32( OCB_OTBR );
        uint32_t tb_delta = i_end_time - i_start_time;

        if( i_start_time > i_end_time )
        {
            tb_delta += 0xFFFFFFFF;
        }

        if( tb_delta > i_p->max_time )
        {
            i_p->max_time = tb_delta;
        }

        if( i_p->min_time == 0 || tb_delta < i_p->min_time )
        {
            i_p->min_time = tb_delta;
        }

        i_p->total_time += tb_delta;
        i_p->cnt++;

        i_p->avg_time = i_p->total_time / i_p->cnt;
    }
}

uint32_t avsbus_calc_crc( uint32_t i_data )
{
    //Polynomial =  x^3 + x^2 + 1 = 1*x^3 + 0*x^2 + 1*x^1 + 1*x^0  = divisor(1011)
    uint32_t l_crc_value = 0;
    uint32_t l_polynomial = 0xb0000000;
    uint32_t l_msb = 0x80000000;

    l_crc_value = i_data & 0xfffffff8ull;

    while ( l_crc_value & 0xfffffff8ull )
    {
        if ( l_crc_value & l_msb )
        {
            //if l_msb is 1'b1, divide by polynomial and shift polynomial to the right
            l_crc_value = l_crc_value ^ l_polynomial;
            l_polynomial = l_polynomial >> 1;
        }
        else
        {
            // if l_msb is zero, shift polynomial
            l_polynomial = l_polynomial >> 1;
        }

        l_msb = l_msb >> 1;
    }

    return l_crc_value;
}

//#################################################################################################
// Function polls OCB status register O2SST for o2s_ongoing=0
//#################################################################################################
uint32_t avsbus_poll_trans_done( uint32_t i_bus_num )
{
    uint32_t l_rc = 0;
    uint32_t l_ocb_o2sst0a = 0;
    uint32_t l_ongoing = 1;
    uint32_t l_count = 0;
    uint32_t l_bus_mask = i_bus_num << O2S_BUSNUM_OFFSET_SHIFT;

    // The point of MAX_POLL_COUNT_AVS is to verify that ongoingFlag turns to
    //   zero very fast. Otherwise, something wrong with this i/f and error out.
    //PK_TRACE_INF("PV:OCB_O2SST0A =0x%x",OCB_O2SST0A | BusMask);
    while ( l_ongoing || (l_count <= MAX_POLL_COUNT_AVS) )
    {
        l_ocb_o2sst0a = in32( 0xc0063830 | l_bus_mask ); //TP_TPCHIP_OCC_OCI_OCB_O2SST0A
        l_ocb_o2sst0a = l_ocb_o2sst0a & 0x80000000;

        if ( !l_ocb_o2sst0a )
        {
            l_ongoing = 0;
        }

        l_count++;
    }

    if ( l_ongoing )
    {
        l_rc = AVS_RC_ONGOING_TIMEOUT;
    }
    else
    {
        l_rc = AVS_RC_SUCCESS;
    }

    return l_rc;
}

//#################################################################################################
// Function which writes to OCB registers to initialize the AVS Slave with an idle frame
//#################################################################################################
uint32_t avsbus_drive_idle_frame( uint32_t i_bus_num )
{
    uint32_t l_rc = 0;
    uint32_t l_idleframe = 0xFFFFFFFF;
    uint32_t l_bus_mask = i_bus_num << O2S_BUSNUM_OFFSET_SHIFT;

    // Clear sticky bits in o2s_status_reg
    out32( 0xc0063838 | l_bus_mask, 0x40000000 );//TP_TPCHIP_OCC_OCI_OCB_O2SCMD0A

    // Compose and send frame
    out32( 0xc0063840  | l_bus_mask, l_idleframe );//TP_TPCHIP_OCC_OCI_OCB_O2SWD0A

    // Wait on o2s_ongoing = 0
    l_rc = avsbus_poll_trans_done( i_bus_num );

    return l_rc;
}

//#################################################################################################
// Function which writes to OCB registers to initiate a AVS write transaction
//#################################################################################################
uint32_t avsbus_drive_write( uint32_t i_cmd_data_type, uint32_t i_cmd_data,  int32_t delta_volt_mv, uint32_t i_bus_num,
                             uint32_t i_rail_num )
{
    uint8_t  l_rc = 0, retry_cnt = 0, l_done = 0, retry_cnt_avsbus_not_in_ctrl = 0;
    uint32_t l_cmd_frame = 0;
    uint32_t l_slave_ack  = 0;
    uint32_t l_rail_select =  i_rail_num;
    uint32_t l_start_code  = 1;
    uint32_t l_cmd_type = 0; // 0:write+commit, 1:write+hold, 2: d/c, 3:read
    uint32_t l_cmd_group = 0;
    uint32_t l_crc = 0;
    uint32_t l_bus_mask = i_bus_num << O2S_BUSNUM_OFFSET_SHIFT;

    // Clear sticky bits in o2s_status_reg
    out32(0xc0063838  | l_bus_mask, 0x40000000); //TP_TPCHIP_OCC_OCI_OCB_O2SCMD0A

    // Compose frame
    // CRC(31:29), CmdData(28:13), RailSelect(12:9), CmdDataType(8:5),
    //   CmdGroup(4), CmdType(3:2), StartCode(1:0)
    l_cmd_frame = (l_start_code << 30) | (l_cmd_type << 28) | (l_cmd_group << 27) |
                  (i_cmd_data_type << 23) | (l_rail_select << 19) | (i_cmd_data << 3);

    // Generate CRC
    l_crc = avsbus_calc_crc(l_cmd_frame);
    l_cmd_frame = l_cmd_frame | l_crc;

    PK_TRACE_INF("AVS: Drive_W Cmd_Frame=0x%08x %d", l_cmd_frame, i_cmd_data);

    do
    {
        // Send frame
        out32(0xc0063840 | l_bus_mask, l_cmd_frame);//TP_TPCHIP_OCC_OCI_OCB_O2SWD0A

        // Wait on o2s_ongoing = 0
        l_rc = avsbus_poll_trans_done(i_bus_num);
        l_slave_ack = in32(0xc0063848 | l_bus_mask); //TP_TPCHIP_OCC_OCI_OCB_O2SRD0A

        if (!l_rc)
        {
            l_done = 1;
        }
        else
        {
            //Non-zero SlaveAck
            if(l_slave_ack & AVS_ACK_PREFIX)
            {
                //If AVSBUS Control taken away then retry multiple times
                if (!(l_slave_ack & AVS_ACK_BUS_CONTROL))
                {
                    if (retry_cnt_avsbus_not_in_ctrl < AVS_CONTROL_RETRIES)
                    {
                        PK_TRACE_INF("AVS: Drive_W, Not in PGPE Control, retry_cnt=(%u/%u)", retry_cnt_avsbus_not_in_ctrl, AVS_CONTROL_RETRIES);
                        retry_cnt_avsbus_not_in_ctrl++;
                    }
                    else
                    {
                        PK_TRACE_INF("AVS: Drive_W Error, Not in PGPE Control, retry_cnt=(%u/%u)", retry_cnt_avsbus_not_in_ctrl,
                                     AVS_CONTROL_RETRIES);
                        l_rc = AVS_RC_AVSBUS_NOT_IN_PGPE_CONTROL;
                        l_done = 1;
                    }
                }
                else if (retry_cnt > AVS_RESYNC_RETRIES)
                {
                    PK_TRACE_INF("AVS: Drive_W Error Slave Ack, O2SRD0A=0x%04x", l_slave_ack);

                    if(l_slave_ack & 0x40000000)
                    {
                        l_rc = AVS_RC_NO_ACTION;
                    }
                    else
                    {
                        l_rc = AVS_RC_RESYNC_ERROR;
                    }

                    l_done = 1;
                }
                //Retry once on resync error
                else
                {
                    retry_cnt++;
                    l_rc = avsbus_drive_idle_frame(i_bus_num);

                    if (l_rc)
                    {
                        l_done = 1;
                    }
                }
            }
            else
            {
                l_done = 1;
            }
        }
    }
    while(!l_done);

    return l_rc;
}

//#################################################################################################
// Function which writes to OCB registers to initiate a AVS read transaction
//#################################################################################################
uint32_t avsbus_drive_read( uint32_t i_cmd_data_type, uint32_t* i_cmd_data, uint32_t i_bus_num, uint32_t i_rail_num )
{
    uint8_t  l_rc = 0, retry_cnt = 0, l_done = 0 , retry_cnt_avsbus_not_in_ctrl = 0 ;
    uint32_t l_cmd_frame = 0;
    uint32_t l_slave_ack = 0;
    uint32_t l_rail_select    = i_rail_num;
    uint32_t l_start_code     = 1;
    uint32_t l_cmd_type       = 3; // 0:write+commit, 1:write+hold, 2: d/c, 3:read
    uint32_t l_cmd_group      = 0;
    uint32_t reserved       = 0xFFFF;
    uint32_t l_crc            = 0;
    uint32_t l_bus_mask = i_bus_num << O2S_BUSNUM_OFFSET_SHIFT;

    // Clear sticky bits in o2s_status_reg
    out32(0xc0063838  | l_bus_mask, 0x40000000); //TP_TPCHIP_OCC_OCI_OCB_O2SCMD0A

    // Compose frame
    // CRC(31:29), Reserved(28:13), RailSelect(12:9), CmdDataType(8:5),
    //   CmdGroup(4),CmdType(3:2),StartCode(1:0)
    l_cmd_frame = (l_start_code << 30) | (l_cmd_type << 28) | (l_cmd_group << 27) |
                  (i_cmd_data_type << 23) | (l_rail_select << 19) | (reserved << 3);

    // Generate CRC
    l_crc = avsbus_calc_crc(l_cmd_frame);
    l_cmd_frame = l_cmd_frame | l_crc;

    // PK_TRACE_DBG("AVS: Drive_R Cmd_Frame=0x%08x", l_cmd_frame);

    do
    {
        // Send frame
        out32(0xc0063840  | l_bus_mask, l_cmd_frame); //TP_TPCHIP_OCC_OCI_OCB_O2SWD0A

        // Wait on o2s_ongoing = 0
        l_rc = avsbus_poll_trans_done(i_bus_num);

        // Read returned voltage value from Read frame
        l_slave_ack = in32(0xc0063848 | l_bus_mask); //TP_TPCHIP_OCC_OCI_OCB_O2SRD0A

        //                  bits
        // Slave Ack Frame: 0:1 Slave Ack encode (with 0b00 being a "good" value)
        //                  2   Reserved
        //                  3:7 Status Response
        //                      3: Vdone
        //                      4: Status Alert,
        //                      5: AVSControl
        //                      6-7: MfgSpecific

        //Non-zero SlaveAck
        if( l_slave_ack & AVS_ACK_PREFIX )
        {

            //If AVSBUS Control taken away then retry multiple times
            if ( !( l_slave_ack & AVS_ACK_BUS_CONTROL ))
            {
                if ( retry_cnt_avsbus_not_in_ctrl < AVS_CONTROL_RETRIES )
                {
                    PK_TRACE_INF( "AVS: Drive_R, Not in PGPE Control, retry_cnt =( %u, %u )",
                                  retry_cnt_avsbus_not_in_ctrl, AVS_CONTROL_RETRIES );
                    retry_cnt_avsbus_not_in_ctrl++;
                }
                else
                {
                    PK_TRACE_INF( "AVS: Drive_R Error, Not in PGPE Control, retry_cnt =( %u, %u )",
                                  retry_cnt_avsbus_not_in_ctrl, AVS_CONTROL_RETRIES);
                    l_rc = AVS_RC_AVSBUS_NOT_IN_PGPE_CONTROL;
                    l_done = 1;
                }
            }
            else if (retry_cnt > AVS_RESYNC_RETRIES)
            {
                PK_TRACE_INF( "AVS: Drive_R Error Slave Ack, O2SRD0A = 0x%04x", l_slave_ack );
                l_rc = AVS_RC_RESYNC_ERROR;
                l_done = 1;
            }
            //Retry one-time for resync error
            else
            {
                retry_cnt++;
                l_rc = avsbus_drive_idle_frame( i_bus_num );

                if ( !l_rc )
                {
                    l_done = 1;
                }
            }
        }
        else
        {
            *i_cmd_data = ( l_slave_ack >> 8 ) & 0x0000FFFF;
            l_done = 1;
        }
    }
    while( !l_done );

    return l_rc;
}

//#################################################################################################
void* avsbus_data_addr()
{
    return &G_avsbus;
}

//#################################################################################################
void avsbus_init( uint32_t i_tap_id )
{
    PK_TRACE_INF("AVS: Init");

    uint32_t l_vdd_bus_num = gppb_get_avs_bus_vrm_data(i_tap_id + RAIL_IDX_VDD_C0, avsbus_num);
    uint32_t l_vcs_bus_num = gppb_get_avs_bus_vrm_data(i_tap_id + RAIL_IDX_VCS_C0, avsbus_num);
    uint32_t l_vdn_bus_num = gppb_get_avs_bus_vrm_data(RAIL_IDX_VDN, avsbus_num);
    PK_TRACE_INF("AVS: VDDBUS=%u,VDNBUS=%u,VCSBUS=%u", l_vdd_bus_num, l_vdn_bus_num, l_vcs_bus_num);

    G_avsbus.to_dly_mult = 10;  // Timeout multiplier

    G_avsbus.voltage_zero_cnt = 0;
    G_avsbus.current_zero_cnt = 0;
    G_avsbus.idd_current_thrshd = 0;
    G_avsbus.ics_current_thrshd = 0;

    //Initialize PGPE cycle time to a picosecond value (for integer representation)
    // 1/600MHz = 0.001667 => 1e7/600 = 16666 (1666.6ps)
    // Round:  16666 + 5 => 16671 / 10 = 1667ps
    G_avsbus.occ_cyc_time_ps = round_dec(1000000 / 500/* (gppb_get_base_data(occ_complex_frequency_mhz))*/);

    G_avsbus.timebase_tick_ns = round_dec(2 * 10 * 1000 / 500 /*gppb_get_base_data(occ_complex_frequency_mhz)*/);

    PK_TRACE_INF("AVS: occ_freq Mhz                         = %u",    gppb_get_base_data(occ_complex_frequency_mhz));
    PK_TRACE_INF("AVS: occ_cyc_time_ps                      = %u",    G_avsbus.occ_cyc_time_ps);
    PK_TRACE_INF("AVS: TB tick time                         = %u ns", G_avsbus.timebase_tick_ns);

    //Initialize VDD
    if ( l_vdd_bus_num != 0xFF )
    {
        avsbus_init_bus( l_vdd_bus_num );

        // Set the Vdone timeout
        G_avsbus.incr_to_dly_tb[RUNTIME_RAIL_VDD] = us_to_tb(2000);  // 2ms
        G_avsbus.decr_to_dly_tb[RUNTIME_RAIL_VDD] = G_avsbus.incr_to_dly_tb[RUNTIME_RAIL_VDD];
    }
    else
    {
        PK_TRACE_ERR( "AVS: VDD Bus Not Available, BusNum = 0xFF" );
        //error_handle_fault(PGPE_ERR_CODE_AVSBUS_VDD_INVALID_BUSNUM);
        //error_state_loop();
    }

//TBD do we need to support in pgpe
#if 0

    //Initialize VDN
    if ( l_vdn_bus_num != 0xFF )
    {
        avsbus_init_bus( l_vdn_bus_num );

        // Set the Vdone timeout
        G_avsbus.decr_to_dly_tb[RUNTIME_RAIL_VDN] = us_to_tb(2000);  // 2ms
    }
    else
    {
        //This is expected on some systems, and PGPE will simply report 0s
        //for any voltage and current reads on VDN Bus
        PK_TRACE_INF("AVS: VDN Bus Not Available");
    }

#endif

    //Initialize VCS
    if ( l_vcs_bus_num != 0xFF )
    {
        avsbus_init_bus( l_vcs_bus_num );

        // Set the Vdone timeout
        G_avsbus.incr_to_dly_tb[RUNTIME_RAIL_VCS] = us_to_tb( 2000 );  // 2ms
        G_avsbus.decr_to_dly_tb[RUNTIME_RAIL_VCS] = G_avsbus.incr_to_dly_tb[RUNTIME_RAIL_VCS];
    }
    else
    {
        PK_TRACE_ERR( "AVS: VCS Bus Not Available, BusNum=0xFF" );
        //error_handle_fault(PGPE_ERR_CODE_AVSBUS_VCS_INVALID_BUSNUM);
        //error_state_loop();
    }

    PK_TRACE_INF( "AVS: VDD TB tick time (incr, decr)         = %u %u tb ticks",
                  G_avsbus.incr_to_dly_tb[RUNTIME_RAIL_VDD], G_avsbus.decr_to_dly_tb[RUNTIME_RAIL_VDD] );
    PK_TRACE_INF( "AVS: VCS TB tick time (incr, decr)         = %u %u tb ticks",
                  G_avsbus.incr_to_dly_tb[RUNTIME_RAIL_VCS], G_avsbus.decr_to_dly_tb[RUNTIME_RAIL_VCS] );
}

//#################################################################################################
void avsbus_init_bus( uint32_t i_bus_num )
{
    uint32_t  l_rc = 0;
    uint32_t  l_data = 0;
    uint32_t  O2SCTRLF_value = 0b10000010000011111100000000000000; //0x820FC000
    uint32_t  O2SCTRLS_value = 0b00000000000010000000000000000000; //0x00080000
    uint32_t  O2SCTRL2_value = 0b00000000000000000000000000000000; //0x00000000
    uint32_t  O2SCTRL1_value = 0b10000000000000000100000000000000 |
                               (500 /*gppb_get_base_data(occ_complex_frequency_mhz)*/ / (8 * CLOCK_SPIVID_MHZ) - 1) << 18;

    // OCI to SPIPMBus (O2S) bridge initialization
    uint32_t l_bus_mask = i_bus_num << O2S_BUSNUM_OFFSET_SHIFT;

    // O2SCTRLF
    l_data = in32( 0xc0063800 | l_bus_mask ); //TP_TPCHIP_OCC_OCI_OCB_O2SCTRLF0
    l_data = ( 0x000000FF & l_data ) | O2SCTRLF_value;
    out32( 0xc0063800 | l_bus_mask, l_data ); //TP_TPCHIP_OCC_OCI_OCB_O2SCTRLF0

    // O2SCTRLS
    l_data = in32( 0xc0063808 | l_bus_mask ); //TP_TPCHIP_OCC_OCI_OCB_O2SCTRLS0
    l_data = ( 0x00003FFF & l_data ) | O2SCTRLS_value;
    out32( 0xc0063808 | l_bus_mask, l_data );

    // O2SCTRL2
    l_data = in32( 0xc0063818 | l_bus_mask ); //TP_TPCHIP_OCC_OCI_OCB_O2SCTRL20
    l_data = ( 0x00007FFF & l_data ) | O2SCTRL2_value;
    out32( 0xc0063818 | l_bus_mask, l_data );

    // O2SCTRL1
    l_data = in32( 0xc0063810 | l_bus_mask ); //TP_TPCHIP_OCC_OCI_OCB_O2SCTRL10
    l_data = ( 0x4FFCBFFF & l_data ) | O2SCTRL1_value;
    out32( 0xc0063810 | l_bus_mask, l_data );

    //
    // AVS slave initialization
    //
    // Drive AVS transaction with a frame value 0xFFFFFFFF (idle frame)
    // to initialize the AVS slave.
    l_rc = avsbus_drive_idle_frame( i_bus_num );

    if (l_rc)
    {
        PK_TRACE_ERR( "AVS: Init Bus, DriveIdleFrame FAIL" );
        //error_handle_fault(PGPE_ERR_CODE_AVSBUS_INIT_ERR);
        //error_state_loop();
    }
    else
    {
        PK_TRACE( "AVS: Initialized i_bus_num=0x%x", i_bus_num );
    }
}


//#################################################################################################
// Note: this is called by routine already in protected context
uint32_t avsbus_status_read( uint32_t i_bus_num, uint32_t i_rail_num, uint32_t* ret_status )
{
    uint32_t l_rc = AVS_RC_SUCCESS;

    if ( i_bus_num != 0xFF )
    {
        l_rc = avsbus_drive_read( AVS_CMD_STATUS_RW, ret_status, i_bus_num, i_rail_num );

        switch (l_rc)
        {
            case AVS_RC_SUCCESS:
                PK_TRACE_DBG( "AVS: Stat_R, Success!" );
                break;

            case AVS_RC_ONGOING_TIMEOUT:
                PK_TRACE_ERR( "AVS: Stat_R, OnGoing Flag Timeout" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_VOLTAGE_READ_ONGOING_TIMEOUT);
                //error_state_loop();
                break;

            case AVS_RC_NO_ACTION:
                PK_TRACE_ERR( "AVS: Stat_R, OnGoing Flag Timeout" );
                //error_handle_fault_w_safe_mode(PGPE_ERR_CODE_AVSBUS_VOLTAGE_WRITE_GOOD_CRC_NO_ACTION);
                //error_state_loop();
                break;

            case AVS_RC_RESYNC_ERROR:
                PK_TRACE_ERR( "AVS: Stat_R, Resync Error" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_VOLTAGE_READ_RESYNC_ERROR);
                //error_state_loop();
                break;

            case AVS_RC_AVSBUS_NOT_IN_PGPE_CONTROL:
                PK_TRACE_ERR( "AVS: Stat_R, Not in PGPE Control" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_VOLTAGE_READ_NOT_IN_PGPE_CONTROL);
                //error_state_loop();
                break;

            default:
                PK_TRACE_ERR( "AVS: Stat_R, Unknown Error" );
                // error_handle_fault(PGPE_ERR_CODE_AVSBUS_VOLTAGE_READ_UNKNOWN_ERROR);
                // error_state_loop();
                break;
        }
    }
    else
    {
        PK_TRACE_ERR( "AVS: Stat_R, i_bus_num = %u not available", i_bus_num );
        *ret_status = 0;
        l_rc = 0xFF;
    }

    return l_rc;
}

//#################################################################################################
uint32_t avsbus_voltage_write( uint32_t i_bus_num, uint32_t i_rail_num, uint32_t volt_mv,
                               int32_t delta_volt_mv, uint32_t i_rail )
{
    uint32_t l_rc = 0;
    uint32_t l_done = 0;
//    uint32_t vdone_retry_cnt = 0;
//   uint32_t write_start_tb = 0;
//   uint32_t vdone_end_tb = 0;
    //  uint32_t vdone_timeout_delta_tb = 0;

    PkMachineContext ctx;
    pk_critical_section_enter(&ctx);

    if ( i_bus_num != 0xFF )
    {
        if ( volt_mv > AVS_DRIVER_MAX_EXTERNAL_VOLTAGE  ||
             volt_mv < AVS_DRIVER_MIN_EXTERNAL_VOLTAGE)
        {
            //error_handle_fault(PGPE_ERR_CODE_AVSBUS_VOLTAGE_OUT_OF_BOUNDS);
            //error_state_loop();
        }

        // Assert probe0
        probe0_assert();

        //uint32_t time = in32(OCB_OTBR);

        if ( delta_volt_mv >= 0 )
        {
            G_avsbus.delta_tb[i_rail] = G_avsbus.incr_to_dly_tb[i_rail];
        }
        else
        {
            G_avsbus.delta_tb[i_rail] = G_avsbus.decr_to_dly_tb[i_rail];
        }

        // Drive write transaction with a target voltage on a particular rail and wait on o2s_ongoing=0
        //write_start_tb = in32(OCB_OTBR);

        l_rc = avsbus_drive_write( AVS_CMD_VOLTAGE_RW, volt_mv, delta_volt_mv, i_bus_num, i_rail_num );

        switch ( l_rc )
        {
            case AVS_RC_SUCCESS:
                PK_TRACE_INF( "AVS: Volt_W Success! %d", volt_mv );
                break;

            case AVS_RC_ONGOING_TIMEOUT:
                PK_TRACE_ERR( "AVS: Volt_W Flag Timeout" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_VOLTAGE_WRITE_ONGOING_TIMEOUT);
                //error_state_loop();
                break;

            case AVS_RC_NO_ACTION:
                PK_TRACE_ERR( "AVS: Volt_W, Good CRC, but no action" );
                // error_handle_fault_w_safe_mode(PGPE_ERR_CODE_AVSBUS_VOLTAGE_WRITE_GOOD_CRC_NO_ACTION);
                //error_state_loop();
                break;

            case AVS_RC_RESYNC_ERROR:
                PK_TRACE_ERR( "AVS: Volt_W Resync Error" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_VOLTAGE_WRITE_RESYNC_ERROR);
                //error_state_loop();
                break;

            case AVS_RC_AVSBUS_NOT_IN_PGPE_CONTROL:
                PK_TRACE_ERR( "AVS: Volt_W, Not in PGPE Control" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_VOLTAGE_WRITE_NOT_IN_PGPE_CONTROL);
                //error_state_loop();
                break;

            default:
                PK_TRACE_ERR( "AVS: Volt_W, Unknown Error" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_VOLTAGE_WRITE_UNKNOWN_ERROR);
                // error_state_loop();
                break;
        }

        // Poll for Vdone with timeout
        do
        {
            uint32_t ret_status;

            l_rc = avsbus_status_read( i_bus_num, i_rail_num, &ret_status);

            if ( !l_rc )
            {
                l_done = 1;  // due to error
            }
            else
            {
#if 0
                // Check for Vdone being set to indicate the transition has completed
                //
                // <VDone> - A single bit flag that will be 0b while the rail
                // is off or powering up, it will change to 1b as soon as the
                // voltage has reached the set operating point, and will
                // again transition to 0b when a new target is committed.
                vdone_end_tb = in32(OCB_OTBR);
                vdone_timeout_delta_tb = delta_tb(write_start_tb, vdone_end_tb);

                if ( ret_status & AVS_STAT_VDONE )
                {
                    probe0_deassert();
                    avs_profile(&G_avsbus.voltage_write[i_rail], time);
                    l_done = 1;
                }
                else
                {
                    if ( vdone_timeout_delta_tb > G_avsbus.delta_tb[i_rail] )
                    {
                        PK_TRACE_INF("AVS: Volt_W Timeout:  retries %d", vdone_retry_cnt);
                        PK_TRACE_INF("AVS: Volt_W Timeout:  actual %d threshold %d", vdone_timeout_delta_tb, G_avsbus.delta_tb[rail]);
                        l_rc = AVS_RC_VDONE_TIMEOUT;
                        //error_handle_fault(PGPE_ERR_CODE_AVSBUS_VOLTAGE_WRITE_VDONE_TIMEOUT);
                        //error_state_loop();
                    }
                    else
                    {
                        vdone_retry_cnt++;
                    }
                }

#endif
            }
        }
        while( !l_done );
    }
    else
    {
        PK_TRACE_ERR( "AVS: Volt_W i_bus_num=%u not available", i_bus_num );
    }

    pk_critical_section_exit(&ctx);
    return l_rc;
}

//#################################################################################################
uint32_t avsbus_voltage_read( uint32_t i_bus_num, uint32_t i_rail_num, uint32_t* ret_volt )
{
    PkMachineContext ctx;
    pk_critical_section_enter(&ctx);
    uint32_t l_rc = 0;

    if ( i_bus_num != 0xFF )
    {
        l_rc = avsbus_drive_read( AVS_CMD_VOLTAGE_RW, ret_volt, i_bus_num, i_rail_num );

        switch ( l_rc )
        {
            case AVS_RC_SUCCESS:
                PK_TRACE_DBG( "AVS: Volt_R, Success!" );

                if( *ret_volt == 0 )
                {
                    //error_info_log_usrdata(PGPE_ERR_CODE_AVSBUS_VOLTAGE_READ_ZERO_VALUE, i_bus_num, i_rail_num,
                    //                            G_avsbus.voltage_zero_cnt);
                    //error_notify_info(PGPE_ERR_CODE_AVSBUS_VOLTAGE_READ_ZERO_VALUE);
                }

                break;

            case AVS_RC_ONGOING_TIMEOUT:
                PK_TRACE_ERR( "AVS: Volt_R, OnGoing Flag Timeout" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_VOLTAGE_READ_ONGOING_TIMEOUT);
                //error_state_loop();
                break;

            case AVS_RC_NO_ACTION:
                PK_TRACE_ERR( "AVS: Volt_R, OnGoing Flag Timeout" );
                // error_handle_fault_w_safe_mode(PGPE_ERR_CODE_AVSBUS_VOLTAGE_WRITE_GOOD_CRC_NO_ACTION);
                // error_state_loop();
                break;

            case AVS_RC_RESYNC_ERROR:
                PK_TRACE_ERR( "AVS: Volt_R, Resync Error" );
                // error_handle_fault(PGPE_ERR_CODE_AVSBUS_VOLTAGE_READ_RESYNC_ERROR);
                //error_state_loop();
                break;

            case AVS_RC_AVSBUS_NOT_IN_PGPE_CONTROL:
                PK_TRACE_ERR( "AVS: Volt_R, Not in PGPE Control" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_VOLTAGE_READ_NOT_IN_PGPE_CONTROL);
                //error_state_loop();
                break;

            default:
                PK_TRACE_ERR( "AVS: Volt_R, Unknown Error" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_VOLTAGE_READ_UNKNOWN_ERROR);
                //error_state_loop();
                break;
        }
    }
    else
    {
        PK_TRACE_ERR( "AVS: Volt_R, i_bus_num=%u not available", i_bus_num );
        *ret_volt = 0;
    }

    pk_critical_section_exit(&ctx);

    return l_rc;
}

//#################################################################################################
void avsbus_current_read( uint32_t i_bus_num, uint32_t i_rail_num, uint32_t* ret_current,
                          uint32_t i_current_scale_idx )
{
    PkMachineContext ctx;
    pk_critical_section_enter(&ctx);

    uint32_t l_rc = 0;

    if ( i_bus_num != 0xFF )
    {
        l_rc = avsbus_drive_read( AVS_CMD_CURRENT_READ, ret_current, i_bus_num, i_rail_num );

        switch ( l_rc )
        {
            case AVS_RC_SUCCESS:

                if( *ret_current )
                {
                    *ret_current = *ret_current * gppb_get_avs_bus_current_factor( i_current_scale_idx );
                }

                break;

            case AVS_RC_ONGOING_TIMEOUT:
                PK_TRACE_ERR( "AVS: Curr_R, OnGoing Flag Timeout Error" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_CURRENT_READ_ONGOING_TIMEOUT);
                //error_state_loop();
                break;

            case AVS_RC_RESYNC_ERROR:
                PK_TRACE_ERR( "AVS: Curr_R, Resync Error" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_CURRENT_READ_RESYNC_ERROR);
                //error_state_loop();
                break;

            case AVS_RC_AVSBUS_NOT_IN_PGPE_CONTROL:
                PK_TRACE_ERR( "AVS: Curr_R, Not In PGPE Control" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_CURRENT_READ_NOT_IN_PGPE_CONTROL);
                //error_state_loop();
                break;

            default:
                PK_TRACE_ERR( "AVS: Curr_R, Unknown Error" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_CURRENT_READ_UNKNOWN_ERROR);
                //error_state_loop();
                break;
        }

        //PK_TRACE( "AVS_READ_CURRENT: i_bus_num=%u, i_rail_num=%u,current=%u scale=%u",
        //          i_bus_num, i_rail_num, *ret_current,gppb_get_current_scale_factor(i_current_scale_idx));
    }
    else
    {
        PK_TRACE_ERR( "AVS: Curr_R, i_bus_num=%u not available", i_bus_num );
        *ret_current = 0;
    }

    pk_critical_section_exit(&ctx);
}

//#################################################################################################
void avsbus_temp_read( uint32_t i_bus_num, uint32_t i_rail_num, uint32_t* i_ret_temp,
                       uint32_t i_current_scale_idx )
{
    PkMachineContext ctx;
    pk_critical_section_enter(&ctx);

    uint32_t l_rc = 0;

    if ( i_bus_num != 0xFF )
    {
        l_rc = avsbus_drive_read( AVS_CMD_TEMPURATURE_READ, i_ret_temp, i_bus_num, i_rail_num );

        switch ( l_rc )
        {
            case AVS_RC_SUCCESS:

                break;

            case AVS_RC_ONGOING_TIMEOUT:
                PK_TRACE_ERR( "AVS: Temp_R, OnGoing Flag Timeout Error" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_CURRENT_READ_ONGOING_TIMEOUT);
                //error_state_loop();
                break;

            case AVS_RC_RESYNC_ERROR:
                PK_TRACE_ERR( "AVS: Temp_R, Resync Error" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_CURRENT_READ_RESYNC_ERROR);
                //error_state_loop();
                break;

            case AVS_RC_AVSBUS_NOT_IN_PGPE_CONTROL:
                PK_TRACE_ERR( "AVS: Temp_R, Not In PGPE Control" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_CURRENT_READ_NOT_IN_PGPE_CONTROL);
                //error_state_loop();
                break;

            default:
                PK_TRACE_ERR( "AVS: Temp_R, Unknown Error" );
                //error_handle_fault(PGPE_ERR_CODE_AVSBUS_CURRENT_READ_UNKNOWN_ERROR);
                //error_state_loop();
                break;
        }

        //PK_TRACE( "AVS_READ_CURRENT: i_bus_num=%u, i_rail_num=%u,current=%u scale=%u",
        //          i_bus_num, i_rail_num, *ret_current,gppb_get_current_scale_factor(i_current_scale_idx));
    }
    else
    {
        PK_TRACE_ERR( "AVS: Temp_R, i_bus_num=%u not available", i_bus_num );
        *i_ret_temp = 0;
    }

    pk_critical_section_exit(&ctx);
}

//#################################################################################################
void init_avs_bus_setup()
{
    G_gppb_avsbus = ( GPPBAVSBusVRM_t*) ( PSTATE_PARAM_BLOCK_BASE_ADDR + G_gppb->hdr_avsbus.offset );
    PK_TRACE( "Address G_gppb_avsbus : 0x%08x", (uint32_t)(G_gppb_avsbus) );

}
//#################################################################################################
