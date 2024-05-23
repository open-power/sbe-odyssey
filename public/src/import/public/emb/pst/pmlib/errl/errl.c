/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/errl/errl.c $          */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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
#include <stdint.h>

#include "ppe42_scom.h"
#include "gpehw_common.h"
#include "ppehw_common.h"
#include "pst_hcd_memmap_occ_sram.H"
#include "pst_hcd_memmap_tcc_sram.H"
#include "pst_hcd_memmap_base.H"
#include "iota.h"
#include "ppe42_string.h"
#include "iota_trace.h"
#include "errldefs.h"
#include "errlqmeproxy.h"
#include "errl.h"
#include "ocb_register_addresses.h"
#include "occ_hcode_errldefs.h"
#include "hcode_occ_api.h"
#include "ppehw_common.h"
//------------------------------------------------------------------------------------------------

/**
 * @brief   constants local to file
 */
const uint32_t  CRITICAL_LOG_PENDING    =   0x00000040;
const uint32_t  INFO_LOG_PENDING        =   0x00000020;
const uint32_t  ELOG_SLOT_FULL          =   0x00000060;
const uint32_t  MAX_HCODE_ELOG_ENGINE   =   10;
const uint32_t  MAX_ELOG_TYPE           =   2;
const uint32_t  CRITICAL_LOG_SLOT       =   0;
const uint32_t  INFO_LOG_SLOT           =   1;
const uint32_t  ELOG_WRAP_AROUND_ID     =   31;
const uint32_t  MAX_ELOG_SIZE           =   4096; ///< TWO KB
const uint32_t  SUCCESS                 =   0;
const uint32_t  ELOG_SLOTS_FULL         =   0x01;
const uint32_t  ERRL_USR_DATA_SZ_MIN    =   128;
const uint32_t  REG_BIT0_MASK           =   0x80000000;
const uint32_t  TRACE_SIZE              =   2048;

//------------------------------------------------------------------------------------------------

/**
 * @brief   local function declarations
 */
uint32_t report_error_log ( errlHndl_t io_err );
uint32_t get_elog_broadcast_reg();
void get_ppe_regs ( const uint8_t  i_errl_source, const uint8_t  i_ppe_instance,
                    errlPpeRegs_t* o_data );

//------------------------------------------------------------------------------------------------

/**
 * @brief global variables
 */
errlHndl_t G_elog_bank[MAX_ELOG_PER_ENGINE]  = {0};

uint32_t G_elog_id;

uint32_t G_elog_slots;

hcodeErrlConfigData_t G_errlConfigData = {0};

uint32_t G_elog_add_map[8][2] =
{
    { 0x00000000, 0x00000000 },                       // PGPE
    { XGPE_CRITICAL_LOG_BASE, XGPE_INFO_LOG_BASE },   // XGPE
    { 0x00000000, 0x00000000 },                       // QME
    { 0x00000000, 0x00000000 },                       // PCE
    { XCE_CRITICAL_LOG_BASE, XCE_INFO_LOG_BASE },     // XCE
    { 0x00000000, 0x00000000 },                       // DCE
    { 0x00000000, 0x00000000 },                       // OCE
    { 0x00000000, 0x00000000 }                        // NGPE
};

//------------------------------------------------------------------------------------------------
/**
 * @brief   determines if error log slot is available
 * @param[in] i_sev     error log severity
 * @param[in] o_status  elog slot detection status
 * @return  error log handle if slot is found, NULL otherwise, look status code for details
 */
errlHndl_t get_error_log_slot( const ERRL_SEVERITY i_sev, uint32_t* o_status )
{

    uint32_t l_reg_address  = 0;
    uint32_t l_reg_data     = 0;
    errlHndl_t l_errl       = NULL;

    l_reg_address = get_elog_broadcast_reg();
    l_reg_data = in32( l_reg_address );

    if( ERRL_SEV_UNRECOVERABLE == i_sev )
    {
        if( !( ELOG_SLOT_FULL & l_reg_data ) )
        {
            if( l_reg_data & CRITICAL_LOG_PENDING )
            {
                //Info log slot is free. Let us use it
                l_errl = (errlHndl_t )( G_elog_add_map[ G_elog_slots ][INFO_LOG_SLOT] );
            }
            else
            {
                //Critical log slot is free. Let us use it.
                l_errl = (errlHndl_t )( G_elog_add_map[ G_elog_slots ][CRITICAL_LOG_SLOT] );
            }
        }
        else
        {
            *o_status = ELOG_SLOTS_FULL;
        }
    }
    else if( ERRL_SEV_INFORMATIONAL == i_sev )
    {
        if( l_reg_data & INFO_LOG_PENDING )
        {
            *o_status = ELOG_SLOTS_FULL;
        }
        else
        {
            //Info log slot is free. Let us use it
            l_errl = (errlHndl_t )( G_elog_add_map[ G_elog_slots ][INFO_LOG_SLOT] );
        }
    }

    return l_errl;
}

//------------------------------------------------------------------------------------------------

void init_err_logging ( const uint8_t              i_errl_source,
                        hcode_error_table_t*       i_perr_table )
{
    G_errlConfigData.source = i_errl_source;
    G_errlConfigData.errId = 0;
    G_errlConfigData.procVersion = mfspr (SPRN_PVR);
    G_errlConfigData.ppeId = (uint16_t) ( mfspr(SPRN_PIR) & 0x0000001F );
    G_errlConfigData.traceSz = TRACE_SIZE;

    switch( i_errl_source )
    {
        case ERRL_SOURCE_QME:
            G_elog_slots = 2;
            break;

        case ERRL_SOURCE_XCE:
            G_elog_slots = 4;
            break;

        case ERRL_SOURCE_OCE:
            G_elog_slots = 6;
            break;

        case ERRL_SOURCE_DCE:
            G_elog_slots = 5;
            break;

        case ERRL_SOURCE_PCE:
            G_elog_slots = 3;
            break;

        case ERRL_SOURCE_XGPE:
            G_elog_slots = 1;
            break;

        case ERRL_SOURCE_PGPE:
            G_elog_slots = 0;
            break;

        case ERRL_SOURCE_NGPE:
            G_elog_slots = 7;
            break;

        default:
            G_errlConfigData.source = ERRL_SOURCE_INVALID;
            break;
    }

    PK_TRACE( "init_err_logging = 0x%08x", G_errlConfigData.source );
}

//------------------------------------------------------------------------------------------------

/**
 * @brief  reports a new error log to hostboot
 * @param[in] io_err    points to error log
 * @return    none
 */
uint32_t report_error_log ( errlHndl_t io_err )
{
    uint32_t l_status   =   ERRL_STATUS_SUCCESS;
    uint32_t l_elog_bcast_reg = 0;
    uint32_t l_elog_bit_pos = 0;
    uint32_t l_eng_elog_pos = 0;
    HcodeOCCSharedData_t* l_pocc_shared_data = NULL;
    HcodeTCCSharedData_t* l_ptcc_shared_data = NULL;
    hcode_error_table_t* l_perr_table = NULL;

    PK_TRACE(  "report_error_log  : G_errlConfigData.source 0x%08x", G_errlConfigData.source );

    switch( G_errlConfigData.source )
    {
        case ERRL_SOURCE_QME:
            //FIXME EWM 145591
            break;

        case ERRL_SOURCE_XCE:
            {
                if( ERRL_SEV_UNRECOVERABLE == io_err->iv_severity )
                {
                    l_eng_elog_pos = 2;
                    l_elog_bit_pos = 25;
                }
                else if ( ERRL_SEV_INFORMATIONAL == io_err->iv_severity )
                {
                    l_eng_elog_pos = 3;
                    l_elog_bit_pos = 26;
                }

                l_elog_bcast_reg = OCB_OCCFLG3;
            }
            break;

        case ERRL_SOURCE_OCE:
            //FIXME EWM 61247
            break;

        case ERRL_SOURCE_DCE:
            //FIXME EWM 61246
            break;

        case ERRL_SOURCE_PCE:
            //FIXME EWM 61243
            break;

        case ERRL_SOURCE_XGPE:
            {
                if( ERRL_SEV_UNRECOVERABLE == io_err->iv_severity )
                {
                    l_elog_bit_pos = 25;
                    l_eng_elog_pos = 2;
                }
                else if ( ERRL_SEV_INFORMATIONAL == io_err->iv_severity )
                {
                    l_elog_bit_pos = 26;
                    l_eng_elog_pos = 3;
                }

                l_elog_bcast_reg = OCB_OCCFLG3;
            }
            break;

        case ERRL_SOURCE_PGPE:
            //FIXME EWM 61242
            {

                if( ERRL_SEV_UNRECOVERABLE == io_err->iv_severity )
                {
                    l_eng_elog_pos = 0;
                    l_elog_bit_pos = 25;
                }
                else if ( ERRL_SEV_INFORMATIONAL == io_err->iv_severity )
                {
                    l_eng_elog_pos = 1;
                    l_elog_bit_pos = 26;
                }

                l_elog_bcast_reg = OCB_OCCFLG2;
            }
            break;

        case ERRL_SOURCE_NGPE:
            G_errlConfigData.traceSz = ERRL_TRACE_DATA_SZ_XGPE;
            break;

        default:
            G_errlConfigData.source = ERRL_SOURCE_INVALID;
            break;
    }

    switch( G_errlConfigData.source )
    {
        case ERRL_SOURCE_QME:
            break;

        case ERRL_SOURCE_XCE:
        case ERRL_SOURCE_DCE:
        case ERRL_SOURCE_PCE:
        case ERRL_SOURCE_OCE:
            l_ptcc_shared_data = (HcodeTCCSharedData_t*)TCC_SHARED_SRAM_BASE_ADDR;
            l_perr_table  = (hcode_error_table_t* )( TCC_SHARED_SRAM_BASE_ADDR + l_ptcc_shared_data->header.errlog_table_offset );
            l_ptcc_shared_data->errlog_idx.dw0.fields.log_address_valid |=  G_errlConfigData.source;
            break;

        case ERRL_SOURCE_XGPE:
        case ERRL_SOURCE_PGPE:
        case ERRL_SOURCE_NGPE:
            l_pocc_shared_data = (HcodeOCCSharedData_t*)OCC_SHARED_SRAM_BASE_ADDR;
            l_perr_table  = (hcode_error_table_t* )( OCC_SHARED_SRAM_BASE_ADDR + l_pocc_shared_data->header.errlog_table_offset );
            l_pocc_shared_data->errlog_idx.dw0.fields.log_address_valid |=  G_errlConfigData.source;
            break;
    }

    //Updating Error Log table
    l_perr_table->elog[l_eng_elog_pos].dw0.fields.errlog_id   = io_err->iv_entryId;
    l_perr_table->elog[l_eng_elog_pos].dw0.fields.errlog_src  = G_errlConfigData.source;
    l_perr_table->elog[l_eng_elog_pos].dw0.fields.errlog_len  = io_err->iv_userDetails.iv_entrySize;
    l_perr_table->elog[l_eng_elog_pos].dw0.fields.errlog_addr =  (uint32_t)( (uint32_t*) ( io_err ) );
    out32( l_elog_bcast_reg, ( REG_BIT0_MASK >> l_elog_bit_pos ) );

    return l_status;
}

//------------------------------------------------------------------------------------------------

errlHndl_t create_errl( ElogOrginSumm_t i_elog_orig, const ERRL_SEVERITY i_sev,
                        errlUDWords_t* i_pdwords, uint32_t* o_status )
{
    PK_TRACE_INF (">> create_errl: modid 0x%X rc 0x%X extrc 0x%X sev 0x%X",
                  i_elog_orig.iv_moduleId, i_elog_orig.iv_reasonCode,
                  i_elog_orig.iv_extReasonCode, i_sev );

    errlHndl_t  l_errl = NULL;

    do
    {
        l_errl = get_error_log_slot( i_sev, o_status );

        if( !l_errl )
        {
            PK_TRACE_INF( "Error log slot not found" );
            break;
        }

        l_errl->iv_entryId      =   G_elog_id++ % ELOG_WRAP_AROUND_ID;
        l_errl->iv_version      =   ERRL_STRUCT_VERSION_1;
        l_errl->iv_reasonCode   =   i_elog_orig.iv_reasonCode;
        l_errl->iv_severity     =   i_sev;
        l_errl->iv_numCallouts  =   0;
        l_errl->iv_maxSize      =   MAX_ELOG_SIZE;
        l_errl->iv_userDetails.iv_committed = 0;
        l_errl->iv_userDetails.iv_entrySize = sizeof( ErrlEntry_t );
        l_errl->iv_userDetails.iv_userDetailEntrySize = 0;
        l_errl->iv_userDetails.iv_timeStamp = pk_timebase_get();
        l_errl->iv_userDetails.iv_modId = i_elog_orig.iv_moduleId;

        if( i_pdwords )
        {
            l_errl->iv_userDetails.iv_userData1 = i_pdwords->iv_userdata1;
            l_errl->iv_userDetails.iv_userData2 = i_pdwords->iv_userdata2;
            l_errl->iv_userDetails.iv_userData3 = i_pdwords->iv_userdata3;
        }

        l_errl->iv_userDetails.iv_version = ERRL_USR_DTL_STRUCT_VERSION_1;
        l_errl->iv_userDetails.iv_procVersion = G_errlConfigData.procVersion;
        l_errl->iv_userDetails.iv_ppeId = G_errlConfigData.ppeId;

        // Default other unused fields
        l_errl->iv_reserved3 = 0;
        l_errl->iv_userDetails.iv_reserved1 = 0; // reserved by def
        l_errl->iv_userDetails.iv_reserved2 = 0; // reuse OCC State
        l_errl->iv_userDetails.iv_reserved4 = 0; // Alignment

        *o_status = ERRL_STATUS_SUCCESS;

    }
    while( 0 );

    PK_TRACE_INF ("<< create_errl EID: 0x%08X Status: %d",
                  (( l_errl != NULL ) ? ( l_errl->iv_entryId ) : 0ull),
                  *o_status);
    return l_errl;
}

//------------------------------------------------------------------------------------------------

uint32_t commit_errl ( errlHndl_t io_err )
{
    uint32_t l_status = ERRL_STATUS_USER_ERROR;

    if ( NULL != io_err )
    {
        l_status = ERRL_STATUS_SUCCESS;
        // this is the last common place holder to change or override the error
        // log fields like actions, severity, callouts, etc. based on generic
        // handling on cases like xstop, etc., before the error is 'commited'
        // for OCC to notice and trigger (H)TMGT

        // mark the last callout by zeroing out the next one
        uint8_t l_lastCallout = ( io_err )->iv_numCallouts;

        if( l_lastCallout < ERRL_MAX_CALLOUTS )
        {
            PK_TRACE_INF ( "Zeroing last+1 callout %u", l_lastCallout );

            ( io_err )->iv_callouts[l_lastCallout].iv_type = 0;
            ( io_err )->iv_callouts[l_lastCallout].iv_calloutValue = 0;
            ( io_err )->iv_callouts[l_lastCallout].iv_priority = 0;
        }

        // numCallouts must be the max value as defined by the TMGT-OCC spec.
        ( io_err )->iv_numCallouts = ERRL_MAX_CALLOUTS;

        // calculate checksum & save it off
        uint32_t    l_cnt = 2;  // starting point is after checksum field
        uint32_t    l_sum = 0;
        uint32_t    l_size = ( io_err )->iv_userDetails.iv_entrySize;
        uint8_t*    l_p = ( uint8_t* )io_err;

        for( ; l_cnt < l_size ; l_cnt++ )
        {
            l_sum += *( l_p + l_cnt );
        }

        (  io_err )->iv_checkSum = l_sum;

        // save off committed
        ( io_err )->iv_userDetails.iv_committed = 1;

        // report error to hostboot
        l_status = report_error_log ( io_err );
    }

    return l_status;
}

//------------------------------------------------------------------------------------------------

/**
 * @brief adds a user defined section to error log
 * @param[in]   io_err      points to an instance of error log
 * @param[in]   i_data_ptr   points to user data section
 * @param[in]   i_size      size of user data section
 * @param[in]   i_version   version of user defined section
 * @param[in]   i_type      type of user detail section
 * @return      SUCCESS if function succeeds, error code otherwise.
 */
uint32_t  add_usr_dtls_to_errl ( errlHndl_t io_err, uint8_t* i_data_ptr, const uint16_t i_size,
                                 const uint8_t i_version, const ERRL_USR_DETAIL_TYPE i_type )
{
    uint32_t l_status = ERRL_STATUS_USER_ERROR;

    // 1.  check if handle is valid
    // 2.  NOT empty
    // 3.  not committed
    // 4.  size being passed in is valid
    // 5.  data pointer is valid
    // 6.  and we have enough size

    if (( io_err != NULL ) && ( io_err->iv_userDetails.iv_committed == 0 ) &&
        ( i_size != 0 ) && ( i_data_ptr != NULL ) &&
        (( io_err->iv_userDetails.iv_entrySize ) < ERRL_MAX_ENTRY_SZ ))
    {
        //adjust user details entry payload size to available size
        uint16_t l_availableSize = ERRL_MAX_ENTRY_SZ -
                                   ( io_err->iv_userDetails.iv_entrySize +
                                     sizeof (ErrlUserDetailsEntry_t) );

        // Add user details section only if ERRL_USR_DATA_SZ_MIN dwords fit
        if ( l_availableSize >= ERRL_USR_DATA_SZ_MIN )
        {
            //local copy of the usr details entry
            ErrlUserDetailsEntry_t l_usrDtlsEntry;

            l_usrDtlsEntry.iv_type = (uint8_t)i_type;
            l_usrDtlsEntry.iv_version = i_version;
            l_usrDtlsEntry.iv_size = (i_size < l_availableSize) ? i_size :
                                     l_availableSize;

            PK_TRACE_INF( "Available Size 0x%08x", l_availableSize );
            PK_TRACE_INF( "User Detail type 0x%08x version 0x%08x size 0x%08x",
                          l_usrDtlsEntry.iv_type, l_usrDtlsEntry.iv_version,
                          l_usrDtlsEntry.iv_size );

            void* l_p = io_err;

            // add user detail entry to end of the current error log
            // copy header of the user detail entry
            l_p = memcpy ( l_p + ( io_err->iv_userDetails.iv_entrySize ),
                           &l_usrDtlsEntry,
                           sizeof (ErrlUserDetailsEntry_t) );

            // If we have more cases of user detail section payloads needing
            // additional logic to copy the payload, the below if-else could
            // be moved into a new function

            // copy payload of the user detail entry
            l_p += sizeof ( ErrlUserDetailsEntry_t );

            if ( l_usrDtlsEntry.iv_type == ERRL_USR_DTL_SR_FFDC )
            {

            }
            else
            {
                memcpy ( l_p, i_data_ptr, l_usrDtlsEntry.iv_size );
            }

            // any errors in copying payload are ignored by errl infrastructure
            // and space will be accounted for that in the error log
            uint16_t l_totalSizeOfUsrDtls = sizeof ( ErrlUserDetailsEntry_t ) +
                                            l_usrDtlsEntry.iv_size;
            //update usr data entry size
            io_err->iv_userDetails.iv_userDetailEntrySize +=
                l_totalSizeOfUsrDtls;

            //update error log size
            io_err->iv_userDetails.iv_entrySize += l_totalSizeOfUsrDtls;
            l_status = ERRL_STATUS_SUCCESS;

        }
        else
        {
            l_status = ERRL_STATUS_LOG_FULL;
            PK_TRACE_ERR ( "No space to add usr dtl! I/p %dB Avail: %dB",
                           i_size, l_availableSize );
        }
    }

    return l_status;
}

//------------------------------------------------------------------------------------------------

void add_trace_to_errl ( errlHndl_t io_err )
{
    PkMachineContext ctx;

    pk_critical_section_enter (&ctx);

    add_usr_dtls_to_errl ( io_err, (uint8_t*) &g_pk_trace_buf, G_errlConfigData.traceSz,
                           ERRL_TRACE_VERSION_1, ERRL_USR_DTL_TRACE_DATA );
    pk_critical_section_exit (&ctx);
}

//------------------------------------------------------------------------------------------------

uint32_t ppe_log_error ( ElogOrginSumm_t i_elog_orig,
                         const ERRL_SEVERITY i_sev,
                         errlUDWords_t*      i_pu_dwords,
                         errlDataUsrDtls_t*  i_pusr_dtls,
                         errlDataCallout_t*  i_pcallouts,
                         uint32_t  i_elogSectn )
{
    uint32_t l_status = ERRL_STATUS_SUCCESS;
    errlHndl_t l_errl = NULL;

    // 1. Create an error log, with basic info
    l_errl = create_errl(   i_elog_orig,
                            ERRL_SEV_UNRECOVERABLE,
                            i_pu_dwords,
                            &l_status );

    if ( NULL != l_errl )
    {
        // Base error log created successfully
        while ( (( ERRL_STATUS_SUCCESS == l_status)   ||
                 (ERRL_STATUS_LOG_FULL == l_status)) && i_pusr_dtls )
        {
            // 2. Add user details sections passed by user
            //    Try fitting as many user data sections as possible

            if( !( i_elogSectn & i_pusr_dtls->iv_type ) )
            {
                //Skip if error log section is marked irrelvant
                i_pusr_dtls = i_pusr_dtls->iv_pNext;
                continue;
            }

            l_status = add_usr_dtls_to_errl(
                           l_errl,
                           i_pusr_dtls->iv_pData,
                           i_pusr_dtls->iv_size,
                           i_pusr_dtls->iv_version,
                           i_pusr_dtls->iv_type );

            i_pusr_dtls = i_pusr_dtls->iv_pNext;
        }

        if ( ERRL_STATUS_LOG_FULL == l_status )
        {
            // continue as good, even if last added user detail did not fit
            l_status = ERRL_STATUS_SUCCESS;
        }

        if ( ( ERRL_STATUS_SUCCESS == l_status ) && ( i_elogSectn & TRACE_SECTN ) )
        {
            // 3. Add traces to the error log, as default
            //    If no space, traces are dropped from the log favouring
            //    user details added before

            // skip if section is marked irrelevant
            add_trace_to_errl( l_errl );
        }

        while (( ERRL_STATUS_SUCCESS == l_status ) && i_pcallouts )
        {
            // 4. Add callouts passed by user
            l_status = add_callout_to_errl(  l_errl,
                                             i_pcallouts->iv_type,
                                             i_pcallouts->iv_value,
                                             i_pcallouts->iv_priority );
            i_pcallouts = i_pcallouts->iv_pNext;
        }

        PK_TRACE( "Critical eLOG After Callout" );

        if ( ERRL_STATUS_SUCCESS == l_status )
        {
            // 5. Commit error log to be noticed for retrieval
            //    note: err gets NULL here on success
            l_status = commit_errl ( l_errl );
        }

        PK_TRACE( "Critical eLOG After Commit" );
    }

    return l_status;
}

//------------------------------------------------------------------------------------------------

/**
 * @brief   returns the address of register meant for announcing pending elog
 * @return  flag register associated with a PM engine
 */
uint32_t get_elog_broadcast_reg()
{
    uint32_t l_reg_address = 0;

    switch( G_errlConfigData.source )
    {
        case ERRL_SOURCE_QME:
            break;

        case ERRL_SOURCE_XCE:
            l_reg_address = OCB_OCCFLG3;
            break;

        case ERRL_SOURCE_OCE:
            l_reg_address = OCB_OCCFLG1;
            break;

        case ERRL_SOURCE_DCE:
            l_reg_address = OCB_OCCFLG0;
            break;

        case ERRL_SOURCE_PCE:
            l_reg_address = OCB_OCCFLG2;
            break;

        case ERRL_SOURCE_XGPE:
            l_reg_address = OCB_OCCFLG3;
            break;

        case ERRL_SOURCE_PGPE:
            l_reg_address = OCB_OCCFLG2;
            break;

        case ERRL_SOURCE_NGPE:
            break;
    }

    return l_reg_address;
}

//------------------------------------------------------------------------------------------------

void get_ppe_regs_usr_dtls ( const uint8_t i_source,
                             const uint8_t i_instance,
                             errlPpeRegs_t* o_ppe_regs,
                             errlDataUsrDtls_t* o_usrDtls)
{
    o_usrDtls->iv_type  =   ERRL_USR_DTL_PPE_REGS;
    o_usrDtls->iv_size  = sizeof (errlPpeRegs_t);
    o_usrDtls->iv_pData = (uint8_t*) o_ppe_regs;
    o_usrDtls->iv_version = ERRL_PPE_REGS_VERSION_1;
    o_usrDtls->iv_pNext = NULL;

    get_ppe_regs ( i_source, i_instance, o_ppe_regs );
}
//------------------------------------------------------------------------------------------------

/**
 * @brief  reads XIRs associated with a given power mgmt engine
 * @param[in]   i_errl_source     engine responsible for creating elog
 * @param[in]   i_ppe_instance    instance associated with elog engine
 * @param[in]   o_data           data read for a list of XIRs
 * @return      none
 */
void get_ppe_regs ( const uint8_t  i_errl_source,
                    const uint8_t  i_ppe_instance,
                    errlPpeRegs_t* o_data )
{
    uint32_t l_ppeId = 0x3;
    uint32_t l_ppeXirs[][ERRL_PPE_REGS_MAX] =
    {
        { 0x64010, 0x64013, 0x64014, 0x64015, 0x6401F }, // PGPE
        { 0x66010, 0x66013, 0x66014, 0x66015, 0x6601F }, // XGPE
        {
            PPE_SCOM_ADDR_UC_Q( 0x200e0200, i_ppe_instance ),
            PPE_SCOM_ADDR_UC_Q( 0x200e020c, i_ppe_instance ),
            PPE_SCOM_ADDR_UC_Q( 0x200e0210, i_ppe_instance ),
            PPE_SCOM_ADDR_UC_Q( 0x200e0214, i_ppe_instance ),
            PPE_SCOM_ADDR_UC_Q( 0x200e023c, i_ppe_instance )
        } //QME
    };

    switch ( i_errl_source )
    {
        case ERRL_SOURCE_PGPE:
            l_ppeId = 0;
            break;

        case ERRL_SOURCE_XGPE:
            l_ppeId = 1;
            break;

        default:
            break;
    }

    if ( l_ppeId <= 2 )
    {
        uint32_t l_regId = 0;

        for ( ; l_regId < ERRL_PPE_REGS_MAX; ++l_regId )
        {
            PPE_GETSCOM (l_ppeXirs[l_ppeId][l_regId], o_data->iv_ppeRegs[l_regId]);
        }
    }
}

//------------------------------------------------------------------------------------------------

uint32_t  add_callout_to_errl( errlHndl_t io_err, const ERRL_CALLOUT_TYPE i_type,
                               const uint64_t i_calloutValue, const ERRL_CALLOUT_PRIORITY i_priority )
{
    uint32_t l_status = ERRL_STATUS_SUCCESS;

    // 1. check if handle is valid (not null or invalid)
    // 2. not committed
    // 3. severity is not informational (unless mfg action flag is set)
    // 4. callouts still not full
    if ( ( io_err != NULL ) &&
         ( io_err->iv_userDetails.iv_committed == 0 ) &&
         ( io_err->iv_severity != ERRL_SEV_INFORMATIONAL) &&
         ( io_err->iv_numCallouts < ERRL_MAX_CALLOUTS ) )
    {
        //set callout type
        io_err->iv_callouts[ io_err->iv_numCallouts ].iv_type = (uint8_t)i_type;

        //set callout value
        io_err->iv_callouts[ io_err->iv_numCallouts ].iv_calloutValue = i_calloutValue;

        //set priority
        io_err->iv_callouts[ io_err->iv_numCallouts].iv_priority = (uint8_t)i_priority;

        //increment actual number of callout
        io_err->iv_numCallouts++;
    }
    else
    {
        l_status = ERRL_STATUS_USER_ERROR;
        PK_TRACE_INF ("Callout type 0x%02X was NOT added to elog", i_type);
    }

    return l_status;
}

//------------------------------------------------------------------------------------------------
