/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sberegaccess.C $               */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2023                        */
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
/*
 * @file: ppe/sbe/sbefw/sberegaccess.C
 *
 * @brief This file contains interfaces to get/set FW flags either in the
 * scratch registers and/or the FW attributes.
 */

#include "sberegaccess.H"
#include "sbetrace.H"
#include "sbeglobals.H"
#include "sbeutil.H"
#include "poz_scom_perv_cfam.H"
#include <ppe42_scom.h>
#include "poz_ppe.H"

SbeRegAccess SbeRegAccess::cv_instance __attribute__((section (".sbss")));

/**
 * @brief Initialize the class
 *
 * @return An RC indicating success/failure
 */
static uint64_t getPPEMessageRegisterAddress() {
    if (SBE_GLOBAL->pibCtrlId == PIBCTRL_SBE)
    {
        return scomt::poz::FSXCOMP_FSXLOG_SCRATCH_REGISTER_15_RW;
    }
    if (SBE_GLOBAL->pibCtrlId == PIBCTRL_PME)
    {
        return scomt::poz::FSXCOMP_FSXLOG_SCRATCH_REGISTER_2_RW;
    }
    return scomt::poz::FSXCOMP_FSXLOG_SB_MSG;
}

/**
 * @brief Initialize the class
 *
 * @return An RC indicating success/failure
 */
uint32_t SbeRegAccess::init(bool forced)
{
    #define SBE_FUNC "SbeRegAccess::init "
    static bool l_initDone = false;
    uint32_t rc = 0;
    do
    {
        if(l_initDone && !forced)
        {
            break;
        }

        //Read the LFR reg
        PPE_LVD(scomt::poz_ppe::TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_RW, lfrReg);

        // Read SBE messaging register into iv_messagingReg
        rc = getscom_abs(getPPEMessageRegisterAddress(), &messagingReg.iv_messagingReg);
        if(PCB_ERROR_NONE != rc)
        {
            SBE_ERROR(SBE_FUNC"Failed reading sbe messaging reg., RC: 0x%08X. ",
                      rc);
            break;
        }

        // Read Mailbox register 16
        rc = getscom_abs(scomt::poz::FSXCOMP_FSXLOG_SCRATCH_REGISTER_16_RW, &mbx16.iv_mbx16);
        if(PCB_ERROR_NONE != rc)
        {
            SBE_ERROR(SBE_FUNC"Failed reading mailbox reg 16, RC: 0x%08X. ", rc);
            break;
        }
        SBE_INFO(SBE_FUNC "MBX_reg16 read : 0x%08X", (uint32_t)(mbx16.iv_mbx16 >> 32));

        if(mbx16.iv_mbx11Valid)
        {
            // Read mbx11
            rc = getscom_abs(scomt::poz::FSXCOMP_FSXLOG_SCRATCH_REGISTER_11_RW, &mbx11.iv_mbx11);
            if(PCB_ERROR_NONE != rc)
            {
                SBE_ERROR(SBE_FUNC"Failed reading mailbox reg 11, RC: 0x%08X. ",
                        rc);
                break;
            }
            SBE_INFO(SBE_FUNC "MBX_reg11 from scratch : 0x%08X", (uint32_t)(mbx11.iv_mbx11 >> 32));
        }
        else
        {
           platInitAttrBootFlags();
        }
    }while(false);
    l_initDone =true;
    SBE_EXIT(SBE_FUNC);
    return rc;
    #undef SBE_FUNC
}

void SbeRegAccess::updateMbx11(const uint64_t i_mbx11)
{
    mbx11.iv_mbx11 = i_mbx11;
}

/**
 * @brief Update the SBE states into the SBE messaging register. The
 * function does a read-modify-write, so any bits other than the state
 * bits are preserved. The current state of the register is set to
 * i_state, whereas the old current state is copied to previous state
 *
 * @param [in] i_state The current SBE state
 *
 * @return RC indicating success/failure.
 *
 */
uint32_t SbeRegAccess::updateSbeState(const uint8_t &i_state)
{
    #define SBE_FUNC "SbeRegAccess::updateSbeState "
    uint32_t rc = 0;

    getscom_abs(getPPEMessageRegisterAddress(), &messagingReg.iv_messagingReg);

    messagingReg.iv_prevState = messagingReg.iv_currState;
    messagingReg.iv_currState = i_state;

    rc = putscom_abs(getPPEMessageRegisterAddress(), messagingReg.iv_messagingReg);
    if(PCB_ERROR_NONE != rc)
    {
        SBE_ERROR(SBE_FUNC"Failed to update STATE: 0x%08X to messaging "
                "register. RC: 0x%08X", i_state, rc);
    }
    return rc;
    #undef SBE_FUNC
}

/**
 * @brief Update the SBE IPL steps into the SBE messaging register. The
 * function does a read-modify-write, so any bits other than the IPL
 * steps are retianed
 *
 * @param [in] i_major IPL major step number
 * @param [in] i_minor IPL minor step number
 *
 * @return RC indicating success/failure.
 *
 */
uint32_t SbeRegAccess::updateSbeStep(const uint8_t i_major,
                                     const uint8_t i_minor)
{
    #define SBE_FUNC "SbeRegAccess::updateSbeStep "
    uint32_t rc = 0;

    getscom_abs(getPPEMessageRegisterAddress(), &messagingReg.iv_messagingReg);

    messagingReg.iv_majorStep = i_major;
    messagingReg.iv_minorStep = i_minor;

    rc = putscom_abs(getPPEMessageRegisterAddress(), messagingReg.iv_messagingReg);
    if(rc)
    {
        SBE_ERROR(SBE_FUNC"Failed to update SBE step: 0x%08X.0x%08X to messaging "
                "register. RC: 0x%08X", i_major, i_minor, rc);
    }
    return rc;
    #undef SBE_FUNC
}

/**
 * @brief Set the SBE ready bit into the SBE messaging register
 * (meaning that SBE control loop is initialized) The function does a
 * read-modify-write, so any bits other than the SBE ready bit remain
 * unchanged.
 *
 * @return RC indicating success/failure.
 *
 */
uint32_t SbeRegAccess::setSbeReady()
{
    #define SBE_FUNC "SbeRegAccess::setSbeReady "
    uint32_t rc = 0;

    getscom_abs(getPPEMessageRegisterAddress(), &messagingReg.iv_messagingReg);

    messagingReg.iv_sbeBooted = true;
    rc = putscom_abs(getPPEMessageRegisterAddress(), messagingReg.iv_messagingReg);
    if(rc)
    {
        SBE_ERROR(SBE_FUNC"Failed to update SBE ready state to "
                "messaging register. RC: 0x%08X", rc);
    }
    return rc;
    #undef SBE_FUNC
}

uint32_t SbeRegAccess::updateAsyncFFDCBit( bool i_on )
{
    #define SBE_FUNC "SbeRegAccess::updateAsyncFFDCBit "
    uint32_t rc = 0;

    getscom_abs(getPPEMessageRegisterAddress(), &messagingReg.iv_messagingReg);

    messagingReg.iv_asyncFFDC = i_on;

    rc = putscom_abs(getPPEMessageRegisterAddress(), messagingReg.iv_messagingReg);
    if(rc)
    {
        SBE_ERROR(SBE_FUNC"Failed to update SBE Aync bit in message "
                "register. RC: 0x%08X", rc);
    }
    return rc;
    #undef SBE_FUNC
}

