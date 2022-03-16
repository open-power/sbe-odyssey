/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/chipop_table.H $         */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022                             */
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
#ifndef __CHIPOP_TABLE_H
#define __CHIPOP_TABLE_H

/**
  * @brief enums for SBE Istep Control command class
  *
  */
enum sbeIplControlCommands
{
    SBE_CMD_EXECUTE_ISTEP      = 0x01, /* Execute istep */
};

/**
  * @brief enums for SCOM Access Messages
  *
  */
enum sbeScomAccessCommands
{
    SBE_CMD_GETSCOM          = 0x01, /* Get SCOM */
    SBE_CMD_PUTSCOM          = 0x02, /* Put SCOM */
    SBE_CMD_MODIFYSCOM       = 0x03, /* Modify SCOM */
    SBE_CMD_PUTSCOM_MASK     = 0x04, /* Put SCOM under mask */
    SBE_CMD_MULTISCOM        = 0x05, /* Execute Multi SCOM */
};

#endif
