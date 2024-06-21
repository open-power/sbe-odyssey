/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/errl/errl.h $          */
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

#ifndef ERRL_H
#define ERRL_H

/**
 * @file errl.h
 * @brief Hcode Error Log Structures
 */

#include <stdbool.h>
#include "hcode_errl_table.h"
#include "occ_hcode_errldefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Max registers (XIRs) to be collected for belly-up PPEs
#define ERRL_PPE_REGS_MAX       5
/// Max Hcode scoreboard size
#define ERRL_PPE_SCRBRD_SIZE    256
/// Max size for an hcode error log
#define ERRL_MAX_ENTRY_SZ       0x1000

/// Status code of error logging from error logging infrastructure
enum errlStatusCodes
{
    ERRL_STATUS_SUCCESS = 0,               ///< errl operation(s) completed successfully
    ERRL_STATUS_INIT_ERROR,                ///< errl framework uninitialized on PPE
    ERRL_STATUS_GLOBAL_SLOTS_FULL,          ///< prev error not yet consumed by FW, a retry may work
    ERRL_STATUS_LOCAL_SLOTS_FULL,          ///< another error being processed on PPE, a retry may work
    ERRL_STATUS_USER_ERROR,                ///< errl API called with bad params / sequence
    ERRL_STATUS_LOG_FULL,                  ///< errl does not have space for adding data
    ERRL_STATUS_INTERNAL_ERROR,            ///< errl internal framework error
    ERRL_STATUS_UNKNOWN,                   ///< errl framework got an unknown error
};

/// Enum specifying order in which PPE XIRs are read and stored
enum errlPpeXirIdx
{
    ERRL_XIR_IDX_XIXCR    = 0,              ///< xxx  | CTR
    ERRL_XIR_IDX_XIRAMDBG = 1,              ///< XSR  | SPRG    0
    ERRL_XIR_IDX_XIRAMEDR = 2,              ///< IR   | EDR
    ERRL_XIR_IDX_XIDBGPRO = 3,              ///< XSR  | IAR
    ERRL_XIR_IDX_XIDBGINF = 4,              ///< SRR0 | LR
};

/// Enum specifying all user data section associated with error log.
enum ElogSectnSumm
{
    SCORE_BOARD_SECTN   =   0x00000001,
    TRACE_SECTN         =   0x00000002,
    SPR_SECTN           =   0x00000004,
    GPR_SECTN           =   0x00000008,
    SCOM_SECTN          =   0x00000010,
    OP_TRACE_SECTN      =   0x00000020,
};
typedef enum   ElogSectnSumm ElogSectnSumm_t;

/// Self referential structure to add code/hardware  (e.g. code, core, cache,
/// etc.) callouts to an error log
struct errlDataCallout
{
    ERRL_CALLOUT_TYPE         iv_type;      ///< see ERRL_CALLOUT_TYPE
    ERRL_CALLOUT_PRIORITY     iv_priority;  ///< see ERRL_CALLOUT_PRIORITY
    uint64_t                  iv_value;     ///< fapi target to be called out
    struct errlDataCallout*   iv_pNext;     ///< pointer to next callout, NULL if none
};

/// Map errlDataCallout_t to errlDataCallout
typedef struct errlDataCallout errlDataCallout_t;

/// Self referential structure to add user detail sections to an error log
struct errlDataUsrDtls
{
    ElogSectnSumm_t           iv_type;      ///< see ElogSectnSumm
    uint16_t                  iv_size;      ///< size of data at pData, multiples of 8B
    uint8_t*                  iv_pData;     ///< ptr. to user details data, 8B aligned
    uint8_t                   iv_version;   ///< version on the user details added
    struct errlDataUsrDtls*   iv_pNext;     ///< ptr to next user detail, NULL if none
};

/// Map errlDataUsrDtls_t to errlDataUsrDtls
typedef struct errlDataUsrDtls errlDataUsrDtls_t;

/// Structure aggregating the user data words 1-3 that get into a PEL's
/// SRC words 4-6, as additional FFDC info for the error
struct errlUserDataWords
{
    uint32_t iv_userdata1;                 ///< User data1 word
    uint32_t iv_userdata2;                 ///< User data2 word
    uint32_t iv_userdata3;                 ///< User data3 word
};

/// Structure aggregating information pertaining to origin of hcode error log.
struct ElogOrginSumm
{
    uint16_t iv_moduleId;                  ///< Module ID
    uint16_t iv_extReasonCode;             ///< Extended Reason Code
    uint8_t  iv_reasonCode;                ///< Reason Code
    uint8_t  reserve[3];                   ///< Reserved
};

/// Map errlUserDataWords to errlUDWords_t
typedef struct errlUserDataWords errlUDWords_t;

/// Structure to collect PPE XIRs in order specified by errlPpeXirIdx
struct errlPpeRegs
{
    uint64_t iv_ppeRegs[ERRL_PPE_REGS_MAX];     ///< PPE register capture structure
};

/// Map errlPpeRegs_t to errlPpeRegs
typedef struct errlPpeRegs errlPpeRegs_t;
/// Map ElogOrginSumm_t to ElogOrginSumm
typedef struct ElogOrginSumm    ElogOrginSumm_t;
/// Section summary

/// @addtogroup pm_hcode_errl
/// @{

/// @brief Initialize common error logging framework based on the PPE instance
///        trying to use it. Required once per PPE init/boot before the rest of
///        error log APIs are used
///
/// @param [in] i_errlSource Engine (this) using error logging, see ERRL_SOURCE
/// @param [in] i_pErrTable  Pointer to error log index table for this engine
///
/// @note All other APIs will execute as no-ops if framework is not initialized
void init_err_logging ( const uint8_t        i_errlSource,
                        hcode_error_table_t* i_pErrTable );

/// @brief Creates an Error Log in the PPE's local SRAM
///
/// @param [in] i_elogOrig  provides info pertaining to origin of error log
/// @param [in] i_sev       Severity this Error Log should be created with
/// @param [in] p_uDWords   User data words 1-3 to add to the Error Log as FFDC
/// @param [in] i_usrFfdcSectn   User FFDC section
/// @param[out] o_status    See errlStatusCodes
///
/// @return On Success: A non-NULL handle to the Error Log created
///         On Failure: NULL, and o_status indicating reason for failure
///
/// @note: (COMP_ID | i_reasonCode) become bits 16-31 of SRC
/// @note: (i_modId<<16 | i_extReasonCode) becomes userdata4 in a PEL
/// @note: Until pending Error Logs are processed and room is created for a new
///        HCode Error Log in SRAM by OCC/(H)TMGT, attempts to create new Error
///        Log via createErrl will fail and HCode error logs will be dropped
errlHndl_t create_errl (
    ElogOrginSumm_t i_elogOrig,
    const ERRL_SEVERITY i_sev,
    errlUDWords_t* p_uDWords,
    uint32_t*      o_status );

/// @brief Adds User Details Section to the Error log
///
/// @param [inout] io_err A valid error log handle returned via by createErrl
/// @param [in] i_dataPtr Pointer to the data being added
/// @param [in] i_size Size of the data being added in bytes. Min. 128B
/// @param [in] i_version Version of the User Details Section Header
/// @param [in] i_type Type of the user details section being added
///
/// @return uint32_t status of the operation. See errlStatusCodes
///
/// @note: Generic method to add user specific data like traces, dashboard, etc.
/// @note: i_size must be a multiple of 8, min. 128B  & data must be 8B aligned
/// @note: If i_size is more than available space, an attempt is made to add
///        user data truncated to the size that can fit in the log (min. 128 B)
/// @note: If there is an error adding user details section to the Error Log,
///        the user details section will be dropped from the Error Log
uint32_t add_usr_dtls_to_errl (
    errlHndl_t io_err,
    uint8_t* i_dataPtr,
    const uint16_t i_size,
    const uint8_t i_version,
    const ElogSectnSumm_t i_type );

/// @brief Add Trace Data to the Error log
///
/// @param [inout] io_err A valid error log handle returned via by createErrl
///
/// @return void
///
/// @note: Common method to add Hcode traces from PK trace buffer to Error Log
/// @note: If there is an error adding traces to the Error Log, the trace data
///        will be dropped from the Error Log
void add_trace_to_errl ( errlHndl_t io_errl );

/// @brief Captures PPE debug registers & sets up an user details section for it
///
/// @param [in] i_source PPE whose registers are to be captured, see ERRL_SOURCE
/// @param [in] i_instance For ERRL_SOURCE_QME, 0..7
///                        For ERRL_SOURCE_PGPE/ERRL_SOURCE_XGPE, ignored
/// @param [out] o_ppRegs  Pointer to a valid instance of errlPpeRegs_t
/// @param [out] o_usrDtls Pointer to a valid instance of errlDataUsrDtls_t
///
/// @return void
///
/// @note On successful execution,
///       o_ppeRegs contains PPE XIRs in order specified by errlPpeXirIdx,
///       o_usrDtls contains valid metadata with payload pointing to o_ppeRegs,
///       such that it can be directly added to an error log
void get_ppe_regs_usr_dtls ( const uint8_t i_source,
                             const uint8_t i_instance,
                             errlPpeRegs_t* o_ppeRegs,
                             errlDataUsrDtls_t* o_usrDtls );

/// @brief Commit the Error Log to the Error Log Table for FW processing
///
/// @param [inout] io_err Input: Pointer to a valid error log handle
///                       Output: NULL if committed without errors
///
/// @return uint32_t status of the operation. See errlStatusCodes
///
/// @note: No further changes can be made to an error log once it is committed
/// @note: It can take time for OCC & (H)TMGT to consume an error log commited
///        to the OCC SRAM and convert it to a PEL/SEL
/// @note: OCC or (H)TMGT being busy or not functional due to other reasons, can
///        can cause HCode commited errors not converting to PELs/SELs
uint32_t commit_errl ( errlHndl_t io_err );

/// @brief Adds a callout to the Error Log
///
/// @param [inout] io_err A valid error log handle returned via by createErrl
/// @param [in] i_type Type of the callout (hardware FRU, code, etc.)
/// @param [in] i_calloutValue Specific instance of the type being called out
/// @param [in] i_priority Priority of this callout for service action
///
/// @return uint32_t status of the operation. See errlStatusCodes
///
//  @note: Callouts help a service engineer isolate the failing part/subsystem
/// @note: Customer visible errors (Pred/Unrec) need at least 1 callout.
/// @note: If there is an error adding callout to the Error Log, the callout
///        will be dropped from the Error Log

/// @TODO via RTC 211557: Support adding callouts to Hcode Error Logs
///       TMGT adds a Processor callout as default, until this is supported
uint32_t  add_callout_to_errl (
    errlHndl_t io_err,
    const ERRL_CALLOUT_TYPE i_type,
    const uint64_t i_calloutValue,
    const ERRL_CALLOUT_PRIORITY i_priority );

/// @brief  Creates a PM hcode error log
///
/// @param[in]  i_elogOrig  summarizes information pertaining to origin point of elog
/// @param[in]  i_sev       severity of error log
/// @param[in]  i_pDWords   collection of userdata1, userdata2 and userdata3
/// @param[in]  i_pusrDtls  points to user data section
/// @param[in]  i_pcallOuts callouts associated with error log
/// @param[in]  i_elogSectn user data section to be populated for the log
/// @return     SUCCESS if function succeeds, error code otherwise.
///
uint32_t ppe_log_error ( ElogOrginSumm_t i_elogOrig,
                         const ERRL_SEVERITY i_sev,
                         errlUDWords_t*      i_pDWords,
                         errlDataUsrDtls_t*  i_pusrDtls,
                         errlDataCallout_t*  i_pcallOuts,
                         uint32_t  i_elogSectn ) ;
/// @}  end addtogroup

#ifdef __cplusplus
}
#endif

#endif // ERRL_H
