/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/wof/wof_tables.C $     */
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
/* This is an automatically generated prolog.                             */

/**
 * @file wof_tables.C
 * @brief Access WOF Adjustment/Limit Tables
 */

// *HWP HWP Owner        : Greg Still <stillgs@us.ibm.com>
// *HWP FW Owner         : Prasad BG Ranganath <prasadbgr@in.ibm.com>
// *Team                 : PM
// *Consumed by          : HB:CRO

#include <stdint.h>
#include <endian.h>
#include <wof_tables.H>

// Internal functions
int16_t wftr_speed_decode(
    int8_t                           i_input,
    uint8_t                          i_speed_type);

int16_t wftr_throt_decode(
    int8_t                           i_input,
    uint8_t                          i_throt_type);

//--------------------------------------------------------------------------
uint32_t wft_get_ceff_steps(
    uint8_t* i_wft)
{
    WofTablesHeader_t* l_wfth = (WofTablesHeader_t*)i_wft;
    return (uint32_t)l_wfth->vddceff_steps;
}

//--------------------------------------------------------------------------
WOFTableRecordHeader_t* wft_get_dim_address(
    uint8_t* i_wft,
    const uint32_t i_dim)
{
    WofTablesHeader_t* l_wfth = (WofTablesHeader_t*)i_wft;
    uint16_t l_dim_offset = be16toh(l_wfth->dim_ofslen[i_dim].offset);
    WOFTableRecordHeader_t* l_dim_wtr_address =  (WOFTableRecordHeader_t*)(i_wft + l_dim_offset);
#ifdef __PPE_PLAT
    WOF_DBG("i_wft %x i_dim %d", (uint32_t)i_wft, i_dim);
    WOF_DBG("l_dim_offset %X l_dim_wtr_address %x", l_dim_offset, (uint32_t)l_dim_wtr_address);
#else
    WOF_DBG("i_wft %p i_dim %d", i_wft, i_dim);
    WOF_DBG("l_dim_offset %X l_dim_wtr_address %p", l_dim_offset, l_dim_wtr_address);
#endif
    return l_dim_wtr_address;
}

//--------------------------------------------------------------------------
char* wft_get_dim_magic(
    WOFTableRecordHeader_t* i_wtr)
{
    return i_wtr->magic_value;
}

//--------------------------------------------------------------------------
uint32_t wft_get_dim_version(
    WOFTableRecordHeader_t* i_wtr)
{
    return i_wtr->version;
}

//--------------------------------------------------------------------------
uint32_t wft_get_dim_rec_size(
    WOFTableRecordHeader_t* i_wtr)
{
    return be16toh(i_wtr->rec_size);
}

//--------------------------------------------------------------------------
uint32_t wft_get_dim_start(
    WOFTableRecordHeader_t* i_wtr)
{
    return be16toh(i_wtr->dim_start);
}

//--------------------------------------------------------------------------
int32_t wft_get_dim_stride(
    WOFTableRecordHeader_t* i_wtr)
{
    int16_t s = be16toh(i_wtr->dim_stride);
    return (int32_t)s;
}

//--------------------------------------------------------------------------
uint32_t wft_get_dim_steps(
    WOFTableRecordHeader_t* i_wtr)
{
    return be16toh(i_wtr->dim_steps);
}

//--------------------------------------------------------------------------
uint32_t wft_get_dim_precision(
    WOFTableRecordHeader_t* i_wtr)
{
    return i_wtr->dim_precision;
}

//--------------------------------------------------------------------------
uint32_t wft_get_dim_type(
    WOFTableRecordHeader_t* i_wtr)
{
    return i_wtr->dim_type;
}

//--------------------------------------------------------------------------
uint32_t wft_get_dim_rec_type(
    WOFTableRecordHeader_t* i_wtr)
{
    return ((i_wtr->rec_type >> (7 - 3)) & 0x3);
}

//--------------------------------------------------------------------------
uint32_t wft_get_dim_speed_type(
    WOFTableRecordHeader_t* i_wtr)
{
    return ((i_wtr->rec_type >> (7 - 5)) & 0x3);
}

//--------------------------------------------------------------------------
uint32_t wft_get_dim_throt_type(
    WOFTableRecordHeader_t* i_wtr)
{
    return (i_wtr->rec_type & 0x3);
}

//--------------------------------------------------------------------------
uint32_t wft_get_dim_tdp_idx(
    WOFTableRecordHeader_t* i_wtr)
{
    return (i_wtr->tdp_idx);
}

//--------------------------------------------------------------------------
uint32_t wft_get_dim_speed(
    WOFTableRecordHeader_t* i_wtr,
    uint32_t i_dim_idx,
    uint32_t i_ceff_steps,
    uint32_t i_ceff_idx)
{
    WOFTableRecordEntry_t* p_wtre_base = (WOFTableRecordEntry_t*)((uint8_t*)i_wtr + sizeof(WOFTableRecordHeader_t));
    WOFTableRecordEntry_t* p_wtre = p_wtre_base + (i_dim_idx * i_ceff_steps) + i_ceff_idx;
    return p_wtre->freq_ps;
}

//--------------------------------------------------------------------------

uint32_t wft_get_dim_throt(
    WOFTableRecordHeader_t* i_wtr,
    uint32_t i_dim_idx,
    uint32_t i_ceff_steps,
    uint32_t i_ceff_idx)
{
    WOFTableRecordEntry_t* p_wtre_base = (WOFTableRecordEntry_t*)((uint8_t*)i_wtr + sizeof(WOFTableRecordHeader_t));
    WOFTableRecordEntry_t* p_wtre = p_wtre_base + (i_dim_idx * i_ceff_steps) + i_ceff_idx;
    return p_wtre->throt;
}

//--------------------------------------------------------------------------

WOFTableRecordEntry_t* wft_get_dim_entry(
    WOFTableRecordHeader_t* i_wtr,
    uint32_t i_dim_idx,
    uint32_t i_ceff_steps,
    uint32_t i_ceff_idx)
{
    WOFTableRecordEntry_t* p_wtre_base = (WOFTableRecordEntry_t*)((uint8_t*)i_wtr + sizeof(WOFTableRecordHeader_t));
    WOFTableRecordEntry_t* p_wtre = p_wtre_base + (i_dim_idx * i_ceff_steps) + i_ceff_idx;
#ifdef __PPE_PLAT
    WOF_DBG("Pointers: i_wtr %x p_wtre_base %x p_wtre %x sizeof(WOFTableRecordHeader_t) %d ",
            (uint32_t)i_wtr, (uint32_t)p_wtre_base,  (uint32_t)p_wtre, sizeof(WOFTableRecordHeader_t));
#else
    WOF_DBG("Pointers: i_wtr %p p_wtre_base %p p_wtre %p sizeof(WOFTableRecordHeader_t) %d (0x%2X)",
            i_wtr, p_wtre_base,  p_wtre, sizeof(WOFTableRecordHeader_t));
#endif


    WOF_DBG(" Dim_idx 0x%X ceff_steps %d ceff_idx 0x%X ",
            i_dim_idx, i_ceff_steps, i_ceff_idx);
    return p_wtre;
}

//--------------------------------------------------------------------------

WOFTableRecordEntry_t wft_compute_clip(
    uint8_t* i_wft,
    uint32_t i_indexes[])
{
    WofTablesHeader_t* p_wfth = (WofTablesHeader_t*)i_wft;
    uint32_t l_ceff_steps = p_wfth->vddceff_steps;

    WOFTableRecordHeader_t* p_dim_wtr_address;

    WOFTableRecordEntry_t l_wtre_running;
    WOFTableRecordEntry_t* p_wtre_running = &l_wtre_running;

    WOFTableRecordEntry_t l_wtre_temp;
    WOFTableRecordEntry_t* p_wtre_temp = &l_wtre_temp;

    uint32_t l_dim_processed = 0;

#ifndef __PPE_PLAT
    const char* WFT_DIMENSION_NAMES_VAR;
#endif

    // Get the base (dimension 0) using ceff_idx
    p_dim_wtr_address = wft_get_dim_address(i_wft, VDD_CEFFRATIO);
#ifdef __PPE_PLAT
    WOF_DBG("Pointers:  i_wft %p p_dim_wtr_address %p delta %d",
            (uint32_t)i_wft, (uint32_t)p_dim_wtr_address, (uint8_t*)p_dim_wtr_address - i_wft );
#else
    WOF_DBG("Pointers:  i_wft %p p_dim_wtr_address %p delta %d",
            i_wft, p_dim_wtr_address, (uint8_t*)p_dim_wtr_address - i_wft );
#endif

    p_wtre_temp = wft_get_dim_entry(p_dim_wtr_address, VDD_CEFFRATIO, l_ceff_steps, i_indexes[VDD_CEFFRATIO]);
    WOF_INF("VDD_CEFFRATIO  :  Base l_wtre_temp    freq_ps 0x%2X (%3d) (%4d) ",
            p_wtre_temp->freq_ps,
            p_wtre_temp->freq_ps,
            wftr_speed_decode(p_wtre_temp->freq_ps, ABS_PSTATE));
    WOF_INF("VDD_CEFFRATIO  :  Base l_wtre_temp     throt %2X (%3d) (dec %d)",
            p_wtre_temp->throt,
            p_wtre_temp->throt,
            wftr_throt_decode(p_wtre_temp->throt, SIGNED_OVG));
    p_wtre_running->freq_ps = p_wtre_temp->freq_ps;
    p_wtre_running->throt = p_wtre_temp->throt;
    WOF_INF("VDD_CEFFRATIO  :  Base l_wtre_running freq_ps 0x%2X (%3d) (%4d) ",
            p_wtre_running->freq_ps,
            p_wtre_running->freq_ps,
            wftr_speed_decode(p_wtre_running->freq_ps, ABS_PSTATE));
    WOF_INF("VDD_CEFFRATIO  :  Base l_wtre_running  throt %2X (%3d) (dec %d)",
            p_wtre_running->throt,
            p_wtre_running->throt,
            wftr_throt_decode(p_wtre_running->throt, SIGNED_OVG));

    l_dim_processed++;

    // Deal with all adjustments.
    for (int a = 0; a < p_wfth->adjust_num; ++a)
    {
        p_dim_wtr_address = wft_get_dim_address(i_wft, l_dim_processed);
        p_wtre_temp = wft_get_dim_entry(p_dim_wtr_address, i_indexes[l_dim_processed], l_ceff_steps, i_indexes[VDD_CEFFRATIO]);
#ifdef __PPE_PLAT
        WOF_INF("p_wtre_temp base    %p", (uint32_t)p_wtre_temp);
        WOF_INF("DIM num %d  :  Adjustments %2d  l_wtre_temp    freq_ps 0x%2X (%4d) ",
                l_dim_processed,
                i_indexes[l_dim_processed],
                (int8_t)p_wtre_temp->freq_ps,
                wftr_speed_decode(p_wtre_temp->freq_ps, DELTA_FREQ));
        WOF_INF("DIM num %d  :  Adjustments %2d  l_wtre_temp    throt %2X (%3d) ",
                l_dim_processed,
                i_indexes[l_dim_processed],
                p_wtre_temp->throt,
                wftr_throt_decode(p_wtre_temp->throt, SIGNED_OVG));
#else
        WOF_INF("p_wtre_temp base    %p ADJUST num %x", p_wtre_temp, p_wfth->adjust_num);
        WOF_INF("%-15s  :  %-12s %2d  l_wtre_temp    freq_ps 0x%2X (%3d) (%4d) throt %2X (%3d) (dec %d)",
                WFT_DIMENSION_NAMES[l_dim_processed],
                "Adjustments",
                i_indexes[l_dim_processed],
                (int8_t)p_wtre_temp->freq_ps,
                (int8_t)p_wtre_temp->freq_ps,
                wftr_speed_decode(p_wtre_temp->freq_ps, DELTA_FREQ),
                p_wtre_temp->throt,
                p_wtre_temp->throt,
                wftr_throt_decode(p_wtre_temp->throt, SIGNED_OVG));

#endif

        p_wtre_running->freq_ps -= (int8_t)p_wtre_temp->freq_ps;
        p_wtre_running->throt += p_wtre_temp->throt;

#ifdef __PPE_PLAT
        WOF_INF("DIMM num %d  :  Adjustments l_wtre_running freq_ps 0x%2X (%3d) (%4d) ",
                l_dim_processed,
                (int8_t)p_wtre_running->freq_ps,
                (int8_t)p_wtre_running->freq_ps,
                wftr_speed_decode(p_wtre_running->freq_ps, ABS_PSTATE));
        WOF_INF("DIMM num %d  :  Adjustments l_wtre_running throt_ps 0x%2X (%3d) (%4d) ",
                l_dim_processed,
                p_wtre_running->throt,
                p_wtre_running->throt,
                wftr_throt_decode(p_wtre_running->throt, SIGNED_OVG));
#else
        WOF_INF("%-15s  :  %-12s     l_wtre_running freq_ps 0x%2X (%3d) (%4d) throt %2X (%3d) (dec %d)",
                WFT_DIMENSION_NAMES[l_dim_processed],
                "Adjustments",
                (int8_t)p_wtre_running->freq_ps,
                (int8_t)p_wtre_running->freq_ps,
                wftr_speed_decode(p_wtre_running->freq_ps, ABS_PSTATE),
                p_wtre_running->throt,
                p_wtre_running->throt,
                wftr_throt_decode(p_wtre_running->throt, SIGNED_OVG));

#endif


        l_dim_processed++;
    }

    /// @todo:  EWM 303250 define and add an error macro that can be used for across the platforms to assert
    /// l_dim_processed != 0

    // Apply all limits
    for (int l = 0; l < p_wfth->limit_num; ++l)
    {
        p_dim_wtr_address = wft_get_dim_address(i_wft, l_dim_processed);
        p_wtre_temp = wft_get_dim_entry(p_dim_wtr_address, i_indexes[l_dim_processed], l_ceff_steps, i_indexes[VDD_CEFFRATIO]);

#ifdef __PPE_PLAT
        WOF_INF("p_wtre_temp base    %p", (uint32_t)p_wtre_temp);
        WOF_INF("DIMM num %d  :  Limit l_wtre_temp freq_ps 0x%2X (%3d) (%4d) ",
                l_dim_processed,
                p_wtre_temp->freq_ps,
                p_wtre_temp->freq_ps,
                wftr_speed_decode(p_wtre_temp->freq_ps, ABS_PSTATE));
        WOF_INF("DIMM num %d  :  Limit l_wtre_temp throt 0x%2X (%3d) (%4d) ",
                l_dim_processed,
                p_wtre_temp->throt,
                p_wtre_temp->throt,
                wftr_throt_decode(p_wtre_temp->throt, SIGNED_OVG));
#else
        WOF_INF("p_wtre_temp base    %p", p_wtre_temp);
        WOF_INF("%-15s  :  %-12s %2d  l_wtre_temp    freq_ps 0x%2X (%3d) (%4d) throt %2X (%3d) (dec %d)",
                WFT_DIMENSION_NAMES[l_dim_processed],
                "Limit",
                i_indexes[l_dim_processed],
                p_wtre_temp->freq_ps,
                p_wtre_temp->freq_ps,
                wftr_speed_decode(p_wtre_temp->freq_ps, ABS_PSTATE),
                p_wtre_temp->throt,
                p_wtre_temp->throt,
                wftr_throt_decode(p_wtre_temp->throt, SIGNED_OVG));
#endif


        if (p_wtre_running->freq_ps < p_wtre_temp->freq_ps)
        {
            p_wtre_running->freq_ps = p_wtre_temp->freq_ps;
        }

        if (p_wtre_running->throt > p_wtre_temp->throt)
        {
            p_wtre_running->throt = p_wtre_temp->throt;
        }

#ifdef __PPE_PLAT
        WOF_INF("DIMM num %d  :  Limit l_wtre_running freq_ps 0x%2X (%3d) (%4d) ",
                l_dim_processed,
                p_wtre_running->freq_ps,
                p_wtre_running->freq_ps,
                wftr_speed_decode(p_wtre_running->freq_ps, ABS_PSTATE));
        WOF_INF("DIMM num %d  :  Limit l_wtre_running throt 0x%2X (%3d) (%4d) ",
                p_wtre_running->throt,
                p_wtre_running->throt,
                wftr_throt_decode(p_wtre_running->throt, SIGNED_OVG));
#else
        WOF_INF("%-15s  :  %-12s     l_wtre_running freq_ps 0x%2X (%3d) (%4d) throt %2X (%3d) (dec %d)",
                WFT_DIMENSION_NAMES[l_dim_processed],
                "Limit",
                p_wtre_running->freq_ps,
                p_wtre_running->freq_ps,
                wftr_speed_decode(p_wtre_running->freq_ps, ABS_PSTATE),
                p_wtre_running->throt,
                p_wtre_running->throt,
                wftr_throt_decode(p_wtre_running->throt, SIGNED_OVG));
#endif

        l_dim_processed++;
    }

    return l_wtre_running;
};

#ifndef __PPE_PLAT
/// @brief Trace the index structure
/// @param[in] i_indexes Refereence to the index structure
void wft_trace_indexes(uint32_t i_indexes[])
{
    WOF_INF("WOF Indexes:  VDD Ceff %d Cratio %d Amb/DIMM %d I/O %d Icredit/MMA %d Noisy Neighbor %d",
            i_indexes[VDD_CEFFRATIO],
            i_indexes[CRATIO],
            i_indexes[HEATSINK],
            i_indexes[IO],
            i_indexes[ICREDIT_MMA],
            i_indexes[NOISY_NEIGHBOR]);
}

/// @brief Internal API test function
/// @param[in] i_wft Pointer to WOF Table
void wft_api_test(uint8_t* i_wft)
{
    if (i_wft == NULL)
    {
        WOF_DBG("wft_api_test returning due to detected NULL pointer");
        return;
    }

    WofTablesHeader_t* p_wfth = (WofTablesHeader_t*)i_wft;
    char magic[4];
    memcpy(magic, &p_wfth->magic_value, 3);
    WOF_DBG("%-15s %s", "magic_value", magic);

    for(uint32_t d = 0; d < NUM_DIMS; ++d)
    {
        WOFTableRecordHeader_t* p_dim_address = wft_get_dim_address(i_wft, d);
        WOF_DBG("i_wft %p i_dim %d p_dim_address %p", i_wft, d, p_dim_address);

        char* p_magic = wft_get_dim_magic(p_dim_address);
        memcpy(magic, p_magic, 3);

        uint8_t l_wtrh_version = wft_get_dim_version(p_dim_address);
        WOF_DBG("wtr_magic %s version %X", magic, l_wtrh_version);

        uint32_t l_dim_type = wft_get_dim_type(p_dim_address);
        uint32_t l_dim_start = wft_get_dim_start(p_dim_address);
        int32_t l_dim_stride = wft_get_dim_stride(p_dim_address);
        uint32_t l_dim_steps = wft_get_dim_steps(p_dim_address);
        uint32_t l_dim_precision = wft_get_dim_precision(p_dim_address);
        uint16_t l_rec_type = wft_get_dim_rec_type(p_dim_address);
        uint16_t l_rec_speed_type = wft_get_dim_speed_type(p_dim_address);
        uint16_t l_rec_throt_type = wft_get_dim_throt_type(p_dim_address);

        uint32_t l_dim_tdp_idx = wft_get_dim_tdp_idx(p_dim_address);

        WOF_INF("l_dim_start 0x%02X (%d) l_dim_stride 0x%02X (%d) l_dim_steps 0x%02X (%d) l_dim_precision %d TDP %d",
                l_dim_start, l_dim_start, l_dim_stride, l_dim_stride, l_dim_steps, l_dim_steps,  l_dim_precision, l_dim_tdp_idx);

        WOF_INF("l_dim_type %02X l_rec_type %02X l_rec_speed_type %02X l_rec_throt_type %02X",
                l_dim_type, l_rec_type, l_rec_speed_type, l_rec_throt_type);
    }

    WOF_INF("wft_compute_clip test");
    WOFTableRecordEntry_t l_wtre;
    uint32_t l_indexes[NUM_DIMS] = {0};
    l_indexes[VDD_CEFFRATIO] = 5;
    l_indexes[CRATIO] = 4;
    l_indexes[HEATSINK] = 10;
    l_indexes[ICREDIT_MMA] = 3;

    WOF_INF("API test - wft %p ", i_wft);

    wft_trace_indexes(l_indexes);
    l_wtre = wft_compute_clip(i_wft, l_indexes);
    WOF_INF("API test - wft_compute_clip  l_wtre speed %2X (%d) throt %2X (%d)",
            l_wtre.freq_ps, l_wtre.freq_ps, l_wtre.throt, l_wtre.throt);

}


//--------------------------------------------------------------------------
void wfth_print(
    uint8_t*    i_wft,
    char*       i_hub_target_str)
{
    WofTablesHeader_t* p_wfth = (WofTablesHeader_t*)i_wft;

    /// @cond WFTH_PRINT
    // Put out the endian-corrected scalars
#define WFTH_PRINT8_h4_d1(_member) \
    WOF_INF("%-25s : 0x%04X (%01d)", #_member, p_wfth->_member, p_wfth->_member);

#define WFTH_PRINT8_h4_d2(_member) \
    WOF_INF("%-25s : 0x%04X (%02d)", #_member, p_wfth->_member, p_wfth->_member);

#define WFTH_PRINT8_h4_d3(_member) \
    WOF_INF("%-25s : 0x%04X (%03d)", #_member, p_wfth->_member, p_wfth->_member);

#define WFTH_PRINT8_h4_d5(_member) \
    WOF_INF("%-25s : 0x%04X (%05d)", #_member, p_wfth->_member, p_wfth->_member);

#define WFTH_PRINT16_h4_d5(_member) \
    WOF_INF("%-25s : 0x%04X (%05d)", #_member, be16toh(p_wfth->_member), be16toh(p_wfth->_member));

#define WFTH_PRINT16_h8_d0(_member) \
    WOF_INF("%-25s : 0x%08X", #_member, be16toh(p_wfth->_member), be16toh(p_wfth->_member));

#define WFTH_PRINT32(_member) \
    WOF_INF("%-25s : 0x%08X", #_member, be32toh(p_wfth->_member));

#define WFTH_PRINT_STR(_member, _size) \
    { \
        std::string fstr( p_wfth->_member, p_wfth->_member + sizeof(char) * _size ); \
        WOF_INF("%-25s : %s", #_member, fstr.c_str()); \
    }

#define WFTH_PRINT_MAGIC(_member) \
    { \
        char magic[4]; \
        memcpy(magic, &p_wfth->_member, 3); \
        WOF_INF("%-25s : %s", #_member, magic); \
    } \

#define WFTH_PRINT_VER(_member) \
    WOF_INF("%-25s : 0x%02X", #_member, p_wfth->_member, p_wfth->_member);

    /// @endcond

    std::string s_line(100, '-');
    WOF_INF("%s", s_line.c_str());
    WOF_INF("WOF Table Header - %s", i_hub_target_str);
    WOF_INF("%s", s_line.c_str());

    WFTH_PRINT_MAGIC  (magic_value                     );
    WFTH_PRINT_VER    (version                         );
    WFTH_PRINT8_h4_d1 (dd_major                        );
    WFTH_PRINT8_h4_d1 (dd_minor                        );
    WFTH_PRINT8_h4_d1 (override_enable                 );
    WFTH_PRINT8_h4_d2 (mod_sm_core_count               );
    WFTH_PRINT8_h4_d2 (mod_sm_core_count_cfg           );
    WFTH_PRINT8_h4_d2 (tap_mod_count                   );
    WFTH_PRINT8_h4_d3 (base_rec_offset                 );
    WFTH_PRINT8_h4_d2 (adjust_num                      );
    WFTH_PRINT8_h4_d2 (limit_num                       );
    WFTH_PRINT8_h4_d2 (avail_table_entries             );
    WFTH_PRINT16_h4_d5(table_size                      );
    WFTH_PRINT8_h4_d3 (vddceff_start                   );
    WFTH_PRINT8_h4_d3 (vddceff_stride                  );
    WFTH_PRINT8_h4_d3 (vddceff_steps                   );
    WFTH_PRINT8_h4_d1 (vddceff_data_size               );
    WFTH_PRINT16_h4_d5(override_match_freq_mhz         );
    WFTH_PRINT16_h4_d5(override_match_power_w          );
    WFTH_PRINT16_h4_d5(sort_wof_base_freq_mhz          );
    WFTH_PRINT16_h4_d5(sort_power_save_freq_mhz        );
    WFTH_PRINT16_h4_d5(sort_ultraturbo_freq_mhz        );
    WFTH_PRINT16_h4_d5(sort_fixed_freq_mhz             );
    WFTH_PRINT16_h4_d5(sort_throttle_freq_mhz          );
    WFTH_PRINT16_h4_d5(tot_skt_power_tgt_w             );
    WFTH_PRINT16_h4_d5(tot_tap_power_tgt_w             );
    WFTH_PRINT16_h4_d5(tot_spl_power_tgt_w             );
    WFTH_PRINT16_h4_d5(ff_power_tgt_w                  );
    WFTH_PRINT16_h4_d5(vdn_mv                          );
    WFTH_PRINT16_h4_d5(vio_mv                          );
    WFTH_PRINT16_h4_d5(rdp_current_a                   );
    WFTH_PRINT16_h4_d5(boost_current_a                 );
    WFTH_PRINT32      (table_date_timestamp            );
    WFTH_PRINT_STR    (table_version, 16               );
    WFTH_PRINT_STR    (part_number, 8                  );

    // Print the dimension offsets (base + number of adjustments + nubmer of limits) starting at base_rec_offset
    for (int d = 0; d < p_wfth->adjust_num + p_wfth->limit_num + 1; ++d)
    {
        uint16_t ofs = be16toh(p_wfth->dim_ofslen[d].offset);
        uint16_t ent = be16toh(p_wfth->dim_ofslen[d].entries);
        WOF_INF("%-22s[%d] : 0x%0X (Offset) %2d (Entries)", "Dimension", d, ofs, ent);
    }
}
#endif

    /**
     * @brief Extract the rec_type byte into its' fields
     *
     * @param[in]  i_rec_type   Value from the WTR header
     * @param[out] o_type       Record type
     * @param[out] o_speed      Speed type
     * @param[out] o_throt      Throttle type
     */
    void wfrh_extract_rec_type_info(
        uint8_t     i_rec_type,
        uint8_t&    o_type,
        uint8_t&    o_speed,
        uint8_t&    o_throt)
    {
        o_type  = (i_rec_type >> (7 - 3)) & 0x3;
        o_speed = (i_rec_type >> (7 - 5)) & 0x3;
        o_throt = i_rec_type & 0x3;
    }

/// @brief Decode the speed contents
///
/// @param[in] i_input       Encoded speed input
/// @param[in] i_speed_type  Encoded speed type
/// @return int16_t
    int16_t wftr_speed_decode(
        int8_t                           i_input,
        uint8_t                          i_speed_type)
    {
        int16_t l_value = 0x8000;
        uint8_t l_input_corrected = (uint8_t)i_input;

        if (i_speed_type == ABS_FREQ)
        {
            // interpret the byte value as an unsigned, 1B number.
            l_value = int(5050 - ((l_input_corrected * 16667) / 1000) + 1 / 2);
        }
        else if (i_speed_type == ABS_PSTATE)
        {
            l_value = int(5050 - ((l_input_corrected * 16667) / 1000) + 1 / 2);
        }
        else if (i_speed_type == DELTA_FREQ)
        {
            if (i_input >= 0)
            {
                l_value = (int(i_input * 16667) / 1000);
            }
            else
            {
                l_value = int((i_input * 16667) / 1000);
            }
        }
        else
        {
            WOF_ERR("ERROR:  Unsupported type %2X passed to speed_decode())", i_speed_type);
        }

#ifndef __PPE_PLAT
        const char* WFT_SPEED_TYPE_NAMES_VAR;
        WOF_DBG("%-15s i_input %d (0x%2X) value 0x%2X (%d)", WFT_SPEED_TYPE_NAMES[i_speed_type], i_input, i_input, l_value,
                l_value);
#endif
        return l_value;
    }

/// @brief Decode the throttle contents
///
/// @param[in] i_input       Encoded throttle input
/// @param[in] i_throt_type  Encoded throttle type
/// @return int16_t
    int16_t wftr_throt_decode(
        int8_t                           i_input,
        uint8_t                          i_throt_type)
    {
        int16_t l_value = 0x8000;

        if (i_throt_type == SIGNED_OVG)
        {
            if (i_input < -127 || i_input > 128)
            {
                WOF_ERR("ERROR:  Signed Ceff Overage encode i_input of %d outside of legal bounds of -127 <= i_input <= 128)",
                        i_input);
            }

            if (i_input >= 0)
            {
                l_value = int(((i_input * 15625 / 10000) / 2) + 1 / 2);
            }
            else
            {
                l_value = int(((i_input * 15625 / 10000) / 2) - 1 / 2);
            }

            // WOF_DBG("%-15s i_input %d (0x%2X) value 0x%2X (%d)", "Signed Ceff Overage", i_input, i_input, l_value, l_value);
        }
        else if (i_throt_type == THROT_INDEX)
        {
            if (i_input < 0 or i_input > 47)
            {
                WOF_ERR("ERROR:  Throttle Index encode i_input of %d outside of legal bounds of 0 <= i_input <= 47)", i_input);
            }

            l_value = i_input;
            // WOF_DBG("%-15s i_input %d (0x%2X) value 0x%2X (%d)", "Throttle Index", i_input, i_input, l_value, l_value);
        }
        else
        {
            WOF_ERR("ERROR:  Unsupported type %s passed to throt_decode())", i_throt_type);
        }

#ifndef __PPE_PLAT
        const char* WFT_THROT_TYPE_NAMES_VAR;
        WOF_DBG("%-15s i_input %d (0x%2X) value 0x%2X (%d)", WFT_THROT_TYPE_NAMES[i_throt_type], i_input, i_input, l_value,
                l_value);
#endif
        return l_value;
    }

#ifndef __PPE_PLAT
//--------------------------------------------------------------------------
    void wftd_print(
        uint8_t*                    i_wft,
        char*                       i_hub_target_str,
        const wft_print_view_t      i_view)
    {
        WofTablesHeader_t* p_wfth = (WofTablesHeader_t*)i_wft;

        char l_buffer_str[256];
        std::string s_data_type;

        // Aspect Strings
        const char* WFT_DIMENSION_NAMES_VAR;
        const char* WFT_REC_TYPE_NAMES_VAR;
        const char* WFT_SPEED_TYPE_NAMES_VAR;
        const char* WFT_THROT_TYPE_NAMES_VAR;

        /// Build the CeffRatio header line with separator
        if (i_view == WFT_PRINT_RAW)
        {
            s_data_type = "(hex data)";
        }
        else
        {
            s_data_type = "(decimal data)";
        }

        std::string l_ceffratio_str;
        std::string l_ceffratio_sep_str;
        uint8_t ceffratio = p_wfth->vddceff_steps;
        l_ceffratio_str = "Ceff Ratios(%)  :";

        for (int c = 0; c < p_wfth->vddceff_steps; ++c)
        {
            sprintf(l_buffer_str, "%5d", ceffratio);
            l_ceffratio_str.append(l_buffer_str);
            ceffratio += p_wfth->vddceff_stride;
        }

        sprintf(l_buffer_str, "%15s  ", s_data_type.c_str());
        l_ceffratio_sep_str.append(l_buffer_str);

        for (int c = 0; c < p_wfth->vddceff_steps; ++c)
        {
            sprintf(l_buffer_str, "%5s", "---");
            l_ceffratio_sep_str.append(l_buffer_str);
        }

        const char* WFT_VIEW_NAMES_VAR; //Defines storage for WFT_VIEW_NAME
        std::string s_line(100, '-');
        WOF_INF("%s", s_line.c_str());
        WOF_INF("WOF Table Data (%s) - %s", WFT_VIEW_NAME(i_view), i_hub_target_str);
        WOF_INF("%s", s_line.c_str());

        uint32_t l_record_data_len = p_wfth->vddceff_steps * sizeof(WOFTableRecordEntry_t);
        WOF_DBG("l_record_data_len = 0x%0X (%2d)", l_record_data_len, l_record_data_len);

        for (int d = 0; d < p_wfth->adjust_num + p_wfth->limit_num + 1; ++d)
        {
            uint16_t ofs = be16toh(p_wfth->dim_ofslen[d].offset);
            uint16_t ent = be16toh(p_wfth->dim_ofslen[d].entries);
            WOF_DBG("%-22s[%d] = 0x%0X (Offset) %2d (Entries)", "Dimension", d, ofs, ent);

            WOFTableRecordHeader_t* p_wtrh = (WOFTableRecordHeader_t*)(i_wft + ofs);
            WOF_DBG("p_wfth %p ofs %X p_wtrh %p", p_wfth, ofs, p_wtrh);

            char magic[4];
            memcpy(magic, &p_wtrh->magic_value, 3);
            WOF_DBG("WTFR %-25s: %s", "magic_value", magic);

            /// @cond WFTR_PRINT
#define WFTR_PRINT(_member) \
    WOF_DBG("WFTR %-25s: %05d (0x%04X)", #_member, \
            p_wtrh->_member, \
            p_wtrh->_member);

#define WFTR_PRINT_16(_member) \
    WOF_DBG("WFTR %-25s: %05d (0x%04X)", #_member, \
            be16toh(p_wtrh->_member), \
            be16toh(p_wtrh->_member));

#define WFTR_PRINT_16S(_member) \
    { \
        int16_t s = be16toh(p_wtrh->_member); \
        WOF_DBG("WFTR %-25s: %05d (0x%04X)", #_member, s, s);\
    }
            /// @endcond

            WFTR_PRINT    (version);
            WFTR_PRINT_16 (rec_size);
            WFTR_PRINT    (rec_type);
            WFTR_PRINT    (dim_type);
            WFTR_PRINT_16 (dim_start);
            WFTR_PRINT_16S(dim_stride);
            WFTR_PRINT_16 (dim_steps);
            WFTR_PRINT    (tdp_idx);

            uint8_t l_rec_type, l_speed_type, l_throt_type;
            wfrh_extract_rec_type_info(p_wtrh->rec_type, l_rec_type, l_speed_type, l_throt_type);
            WOF_DBG("hdr rec_type 0x%2X rec_type  0x%2X rec_type 0x%2X rec_type 0x%2X",
                    p_wtrh->rec_type, l_rec_type, l_speed_type, l_speed_type);

            sprintf(l_buffer_str,
                    "Dim Type %1X : Type = %1X; Start = %2u; Stride = %2d; Steps = %2u; Start/Stride Precision (/10^x): %u; TDP Index = %u; Record Size = %u",
                    p_wtrh->dim_type,
                    p_wtrh->rec_type,
                    be16toh(p_wtrh->dim_start),
                    (int16_t)be16toh(p_wtrh->dim_stride),
                    be16toh(p_wtrh->dim_steps),
                    p_wtrh->dim_precision,
                    p_wtrh->tdp_idx,
                    be16toh(p_wtrh->rec_size));
            WOF_DBG("%s", l_buffer_str);

            sprintf(l_buffer_str,
                    "%-15s : Type = %s; Speed = %s; Throttle = %s; Start = %2u; Stride = %2d; Steps = %2u; Start/Stride Precision (/10^x): %u; TDP Index = %u",
                    WFT_DIMENSION_NAMES[p_wtrh->dim_type],
                    WFT_REC_TYPE_NAMES[l_rec_type],
                    WFT_SPEED_TYPE_NAMES[l_speed_type],
                    WFT_THROT_TYPE_NAMES[l_throt_type],
                    be16toh(p_wtrh->dim_start),
                    (int16_t)be16toh(p_wtrh->dim_stride),
                    be16toh(p_wtrh->dim_steps),
                    p_wtrh->dim_precision,
                    p_wtrh->tdp_idx);
            WOF_INF("%s", l_buffer_str);

            uint16_t dim_steps = be16toh(p_wtrh->dim_steps);
            WOFTableRecordEntry_t* p_wtre_base = (WOFTableRecordEntry_t*)((uint8_t*)p_wtrh + sizeof(WOFTableRecordHeader_t));

            WOF_DBG("dim_steps offsetof  %X", offsetof(WOFTableRecordHeader_t, dim_steps));
            WOF_DBG("dim_steps: %X Emd %X", dim_steps, p_wtrh->dim_steps);
            WOF_DBG("p_wtre_base: %p", p_wtre_base);

            WOF_INF("%s", l_ceffratio_str.c_str());
            WOF_INF("%s", l_ceffratio_sep_str.c_str());
            std::string l_out_str;

            for (int r = 0; r < dim_steps; ++r)
            {
                WOFTableRecordEntry_t* p_wtre_idx = (WOFTableRecordEntry_t*)((uint8_t*)p_wtre_base + (r * l_record_data_len));
                WOFTableRecordEntry_t* p_wtre_begin = p_wtre_idx;
                WOF_DBG("p_wtre_idx: %p  <- p_wtre_base %p + r (%X) * rlen (%X)", p_wtre_idx, p_wtre_base, r, l_record_data_len);
                l_out_str = "";
                sprintf(l_buffer_str, "%-6s", " Index ");
                l_out_str.append(l_buffer_str);
                sprintf(l_buffer_str, "%2d", r);
                l_out_str.append(l_buffer_str);
                sprintf(l_buffer_str, "%s", " Freq  :");
                l_out_str.append(l_buffer_str);

                for (int c = 0; c < p_wfth->vddceff_steps; ++c)
                {
                    if (i_view == WFT_PRINT_RAW)
                    {
                        sprintf(l_buffer_str, "   %02X", (uint8_t)p_wtre_idx->freq_ps);
                    }
                    else
                    {
                        sprintf(l_buffer_str, "%5d", wftr_speed_decode(p_wtre_idx->freq_ps, l_speed_type));
                    }

                    l_out_str.append(l_buffer_str);
                    p_wtre_idx += 1;
                    WOF_DBG("p_wtre_idx speed inc: %p", p_wtre_idx);
                }

                WOF_INF("%s", l_out_str.c_str());

                p_wtre_idx = p_wtre_begin;
                WOF_DBG("p_wtre_idx restore: %p", p_wtre_idx);
                l_out_str = "";
                sprintf(l_buffer_str, "%17s", " Throt :");
                l_out_str.append(l_buffer_str);

                for (int c = 0; c < p_wfth->vddceff_steps; ++c)
                {
                    if (i_view == WFT_PRINT_RAW)
                    {
                        sprintf(l_buffer_str, "   %02X", (uint8_t)p_wtre_idx->throt);
                    }
                    else
                    {
                        sprintf(l_buffer_str, "%5d", wftr_throt_decode(p_wtre_idx->throt, l_throt_type));
                    }

                    l_out_str.append(l_buffer_str);
                    p_wtre_idx += 1;
                    WOF_DBG("p_wtre_idx throt inc: %p", p_wtre_idx);
                }

                WOF_INF("%s", l_out_str.c_str());
            }

            WOF_INF("%s", "");
        }
    }
#endif