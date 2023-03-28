/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/core/imgcustomize.C $         */
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

#include <imgcustomize.H>
#include "sbeutil.H"
#include "errorcodes.H"
#include "measurementregs.H"
#include "sbetrace.H"
#include "poz_ppe.H"
#include "odysseylink.H"
#include "globals.H"
#include "sbeglobals.H"

void sbeSecurityCheckWrap(void)
{
    //Read the SROM measurement control register and validate if boot complete bit is set
    secureBootCtrlSettings_t sromSecureBootCtrlSettings;
    sromSecureBootCtrlSettings.getSecureBootCtrlSettings(MEASUREMENT_REG_24);
    SBE_INFO(SBE_FUNC "SROM Secure Boot Control Measurement Reg Value: 0x%08x",
            sromSecureBootCtrlSettings.secureBootControl);
    if(sromSecureBootCtrlSettings.bootComplete != 0x1)
    {
        SBE_ERROR(SBE_FUNC "SROM Boot Complete bit not set.");
        SBE::updateErrorCodeAndHalt(BOOT_RC_SROM_COMPLETE_BIT_NOT_SET_IN_RUNTIME);
    }

    //Read the Boot Loader measurement control register and validate if boot complete bit is set
    secureBootCtrlSettings_t bldrSecureBootCtrlSettings;
    bldrSecureBootCtrlSettings.getSecureBootCtrlSettings(MEASUREMENT_REG_25);
    SBE_INFO(SBE_FUNC "BLDR Secure Boot Control Measurement Reg Value: 0x%08x",
            bldrSecureBootCtrlSettings.secureBootControl);
    if(bldrSecureBootCtrlSettings.bootComplete != 0x1)
    {
        SBE_ERROR(SBE_FUNC "BLDR Boot Complete bit not set.");
        SBE::updateErrorCodeAndHalt(BOOT_RC_BLDR_COMPLETE_BIT_NOT_SET_IN_RUNTIME);
    }
}

void sbePakSearchStartOffset(void)
{
    // Get the partition start offset
    sbe_local_LFR lfrReg;
    PPE_LVD(scomt::poz_ppe::TP_TPCHIP_PIB_SBE_SBEPRV_LCL_LFR_SCRATCH_RW, lfrReg);

    g_partitionOffset = getAbsPartitionAddr(lfrReg.boot_selection);
    g_partitionSize = NOR_PARTITION_SIZE;


    SBE_INFO(SBE_FUNC "Partition offset is : 0x%08x, "
                        "Partition size is : 0x%08x, "
                        "Partition selected is : 0x%02x",
                        g_partitionOffset, g_partitionSize,
                        (uint8_t)lfrReg.boot_selection);
}
