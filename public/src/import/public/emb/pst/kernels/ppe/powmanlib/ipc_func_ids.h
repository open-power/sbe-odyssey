/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/powmanlib/ipc_func_ids.h $ */
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

#pragma once

/// \file ipc_func_ids.h
/// \brief This file contains the definition of the valid IPC function IDs
/// used for interprocessor communications (IPC) in the OCC.

#if defined(__TCC_PLAT)

// NOTE: MT Multi Target, ST = Single Target
IPC_FUNCIDS_TABLE_START

IPC_FUNCIDS_MT_START
IPC_FUNCIDS_MT_END

IPC_FUNCIDS_ST_START(PMHW_INST_ID_GPE0)
IPC_FUNCIDS_ST_END(PMHW_INST_ID_GPE0)

IPC_FUNCIDS_ST_START(PMHW_INST_ID_GPE1)
IPC_FUNCIDS_ST_END(PMHW_INST_ID_GPE1)

IPC_FUNCIDS_ST_START(PMHW_INST_ID_GPE2)
IPC_FUNCIDS_ST_END(PMHW_INST_ID_GPE2)

IPC_FUNCIDS_ST_START(PMHW_INST_ID_GPE3)
IPC_FUNCIDS_ST_END(PMHW_INST_ID_GPE3)

IPC_FUNCIDS_TABLE_END

#elif defined(__OCC_PLAT)

// Comment this out if you do not want to use the ping utility provided by the IPC library
#define IPC_ENABLE_PING

// NOTE: MT Multi Target, ST = Single Target
IPC_FUNCIDS_TABLE_START

//This is where common function ID's that any instance can support should be
//defined.
IPC_FUNCIDS_MT_START
#ifdef IPC_ENABLE_PING
    IPC_FUNC_ID(IPC_MT_PING) //anyone can ping anyone else
#endif
IPC_FUNCIDS_MT_END

//Functions that are only supported by GPE0 should be defined here
//These function ID's can only be sent to GPE0
IPC_FUNCIDS_ST_START(PMHW_INST_ID_GPE0)
IPC_FUNC_ID(IPC_ST_TEST_FUNC0)
IPC_FUNC_ID(IPC_ST_APSS_INIT_GPIO_FUNCID)
IPC_FUNC_ID(IPC_ST_APSS_INIT_MODE_FUNCID)
IPC_FUNC_ID(IPC_ST_APSS_START_PWR_MEAS_READ_FUNCID)
IPC_FUNC_ID(IPC_ST_APSS_CONTINUE_PWR_MEAS_READ_FUNCID)
IPC_FUNC_ID(IPC_ST_APSS_COMPLETE_PWR_MEAS_READ_FUNCID)
IPC_FUNC_ID(IPC_ST_GET_CORE_DATA_FUNCID)
IPC_FUNC_ID(IPC_ST_SCOM_OPERATION)
IPC_FUNC_ID(IPC_ST_GPE0_NOP)
IPC_FUNC_ID(IPC_ST_GET_NEST_DTS_FUNCID)
IPC_FUNC_ID(IPC_ST_GET_TOD_FUNCID)
IPC_FUNC_ID(IPC_ST_APSS_RESET_FUNCID)
IPC_FUNC_ID(IPC_ST_GET_QUAD_DATA_FUNCID)
IPC_FUNCIDS_ST_END(PMHW_INST_ID_GPE0)

//Functions that are only supported by GPE1 should be defined here
//These function ID's can only be sent to GPE1
IPC_FUNCIDS_ST_START(PMHW_INST_ID_GPE1)
IPC_FUNC_ID(IPC_ST_GPE1_NOP)
IPC_FUNC_ID(IPC_ST_24_X_7_FUNCID)
IPC_FUNC_ID(IPC_ST_GPU_SM_FUNCID)
IPC_FUNC_ID(IPC_ST_GPE_GPU_INIT_FUNCID)
IPC_FUNC_ID(IPC_ST_MEMBUF_SCOM_FUNCID)
IPC_FUNC_ID(IPC_ST_MEMBUF_DATA_FUNCID)
IPC_FUNC_ID(IPC_ST_MEMBUF_INIT_FUNCID)
IPC_FUNCIDS_ST_END(PMHW_INST_ID_GPE1)

//Functions that are only supported by GPE2 should be defined here
//These function ID's can only be sent to GPE2
IPC_FUNCIDS_ST_START(PMHW_INST_ID_GPE2)
IPC_FUNC_ID(IPC_MSGID_405_INVALID)
IPC_FUNC_ID(IPC_MSGID_405_PSTATE_CONTROL)
IPC_FUNC_ID(IPC_MSGID_405_CLIPS)
IPC_FUNC_ID(IPC_MSGID_405_SET_PMCR)
IPC_FUNC_ID(IPC_MSGID_405_WOF_CONTROL)
IPC_FUNC_ID(IPC_MSGID_405_WOF_CEFF_INFO)
IPC_FUNC_ID(IPC_MSGID_405_RESERVED_1)
IPC_FUNC_ID(IPC_MSGID_405_RESERVED_2)
IPC_FUNC_ID(IPC_MSGID_XGPE_PGPE_BEACON_STOP)
IPC_FUNCIDS_ST_END(PMHW_INST_ID_GPE2)

//Functions that are only supported by GPE3 should be defined here
//These function ID's can only be sent to GPE3
IPC_FUNCIDS_ST_START(PMHW_INST_ID_GPE3)
IPC_FUNC_ID(IPC_MSGID_PGPE_XGPE_VRET_UPDT)
IPC_FUNCIDS_ST_END(PMHW_INST_ID_GPE3)

//Functions that are only supported by PPC should be defined here
//These function ID's can only be sent to the PPC
IPC_FUNCIDS_ST_START(PMHW_INST_ID_PPC)
IPC_FUNCIDS_ST_END(PMHW_INST_ID_PPC)

IPC_FUNCIDS_TABLE_END

#endif // End of if(__TCC_PLAT or __OCC_PLAT)
