/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/include/pmheaders/pstate_pgpe_qme_api.h $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2024                        */
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

/// @file  pstate_pgpe_qme_api.h
/// @brief Elements between the QME and PGPE
/// @todo this doesn't make sense for PFuture.  Is this really between PCE and QME?

#pragma once

///
/// @brief PGPE to QME Doorbell 2 message IDs
///
enum MESSAGE_ID_DB2
{
    MSGID_DB2_DDS_AUTO          = 0xFC,
    MSGID_DB2_DDS_DISABLE       = 0xFD,
};

///
/// @brief QME to PGPE PCB Type 2 acknowledgement IDs
///
enum MESSAGE_ID_PCB_TYPE2_ACK_TYPES
{
    MSGID_PCB_TYPE2_ACK_AUTO_DDS_SUCCESS        = 0xFC,
    MSGID_PCB_TYPE2_ACK_DDS_DISABLE_SUCCESS     = 0xFD,
    MSGID_PCB_TYPE2_ACK_QME_ERROR               = 0xFF,
};

///
/// @brief QME to PGPE PCB Pstate request structure
///
typedef union
{
    uint32_t value; ///< Raw value
    struct
    {
#ifdef _BIG_ENDIAN
        uint32_t reserved       : 24;       ///< Reserved
        uint32_t pstate_target  : 8;        ///< Pstate target (Host or Characterization mode)
#else
        uint32_t pstate_target  : 8;        ///< Pstate target (Host or Characterization mode)
        uint32_t reserved       : 24;       ///< Reserved
#endif
    } fields;       ///< Sub fields
}   qme_pgpe_pcb0_t;