/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/sbecmdexecutehwp.H $     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
#ifndef __SBE_CMDEXECUTEHWP_H
#define __SBE_CMDEXECUTEHWP_H

#include <stdint.h>

// IO HWP Num enum. DO not change the sequence. Refer the POZ specs.
enum iohwpnum
{
    ody_omi_hss_load_ppe_hwp,
    ody_omi_hss_config_hwp,
    ody_omi_hss_start_ppe_hwp,
    ody_omi_hss_bist_init_hwp,
    ody_omi_hss_bist_start_hwp,
    ody_omi_hss_bist_poll_hwp,
    ody_omi_hss_bist_cleanup_hwp,
    ody_omi_hss_init_hwp,
    ody_omi_hss_dccal_start_hwp,
    ody_omi_hss_dccal_poll_hwp,
    ody_omi_hss_tx_zcal_hwp,
    ody_omi_pretrain_adv_hwp,
    ody_omi_setup_hwp,
    ody_omi_train_hwp,
    ody_omi_train_check_hwp,
    ody_omi_posttrain_adv_hwp,
};

// MEM HWP Num enum
enum memhwpnum
{
    ody_scominit_hwp,
    ody_ddrphyinit_hwp,
    ody_ddrphyinit_ate_hwp,
    ody_load_imem_hwp,
    ody_load_dmem_hwp,
    ody_sppe_draminit_hwp,
    ody_ate_fw_hwp,
    ody_load_pie_hwp,
    ody_draminit_mc_hwp,
    ody_enable_ecc_hwp,
    ody_thermal_init_hwp,
    
};
/**
  * @brief Execute HWP message header
  */
typedef struct
{
    uint32_t :8;
    uint32_t hwpClass:8;
    uint32_t :8;
    uint32_t hwpNum:8;

}sbeExecuteHwpMsgHdr_t;

/**
 * @brief sbeExecuteHWP: Implement sbeExecuteHWP chipop. (0xA104)
 *
 * @param[in] i_pArg Buffer to be passed to the function
 *
 * @return    Rc from the FIFO access utility
 */
uint32_t sbeExecuteHWP(uint8_t *i_pArg);

#endif /* __SBE_CMDEXECUTEHWP_H */
