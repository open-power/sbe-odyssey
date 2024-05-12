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
#include "pst_hcd_memmap_base.H"

#include "iota.h"
#include "ppe42_string.h"
#include "iota_trace.h"

#include "errldefs.h"
#include "errlqmeproxy.h"
#include "errl.h"

/// Function Definitions

// Function Specification
// Name:        initErrLogging
// Description: init error log module to enable its operation
// End Function Specification
void initErrLogging ( const uint8_t              i_errlSource,
                      hcode_error_table_t*       i_pErrTable )
{

}

// Function Specification
// Name:        reportErrorLog
// Description: reports a new error log to htmgt
// End Function Specification
uint32_t reportErrorLog ( errlHndl_t* io_err, bool i_report )
{
    uint32_t l_status = ERRL_STATUS_SUCCESS;


    return l_status;
}

// Function Specification
// Name:        createErrl
// Description: Create an Error Log
// End Function Specification
errlHndl_t createErrl(
    ElogOrginSumm_t i_elogOrig,
    const ERRL_SEVERITY i_sev,
    errlUDWords_t* p_uDWords,
    ElogSectnSumm_t   i_usrFfdcSectn,
    uint32_t*      o_status )
{
    PK_TRACE_INF (">> createErrl: modid 0x%X rc 0x%X extrc 0x%X sev 0x%X",
                  i_elogOrig.iv_moduleId, i_elogOrig.iv_reasonCode,
                  i_elogOrig.iv_extReasonCode, i_sev );

    errlHndl_t  l_rc = NULL;

    PK_TRACE_INF ("<< createErrl EID: 0x%08X Status: %d",
                  ((l_rc != NULL) ? (l_rc->iv_entryId) : 0ull),
                  *o_status);
    return l_rc;
}

// Function Specification
// Name:        commitErrl
// Description: Commit an Error Log
// End Function Specification
uint32_t commitErrl ( errlHndl_t* io_err )
{
    uint32_t l_status = ERRL_STATUS_USER_ERROR;


    return l_status;
}

// Function Specification
// Name:        deleteErrl
// Description: Deletes an error log that has already been created, but not
//              yet committed. It cleans up the internal errl framework so
//              that new errors of the same sev can be created in the same
//              space
// End Function Specification
uint32_t deleteErrl ( errlHndl_t* io_err )
{
    PK_TRACE_INF ("deleteErrl");
    return (reportErrorLog ( io_err, false ));
}

// Function Specification
// Description: Utility function to accept error log params and orchestrate
// all errl API calls to create, add user details/callouts and
// commit an Error Log
// End Function Specification

uint32_t ppeLogError ( ElogOrginSumm_t i_elogOrig,
                       const ERRL_SEVERITY i_sev,
                       errlUDWords_t*      p_uDWords,
                       errlDataUsrDtls_t*  p_usrDtls,
                       errlDataCallout_t*  p_callOuts,
                       uint32_t  i_elogSectn )
{
    uint32_t status = ERRL_STATUS_SUCCESS;
    return status;
}
