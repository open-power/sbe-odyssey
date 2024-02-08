/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/security/securityutils.C $          */
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

#include "securityutils.H"
#include "sberegaccess.H"
#include "sbetrace.H"
#include "sbeglobals.H"
#include "attribute.H"

using namespace fapi2;

void securityUtils::setSecurityLvl()
{
    #define SBE_FUNC " setSecurityLvl "
    SBE_ENTER(SBE_FUNC);

    //By default we will always enforce highest security level
    ATTR_SECURITY_LEVEL_Type securityLevel = ENUM_ATTR_SECURITY_LEVEL_ENFORCING;

    bool isSecurityEnabled = SbeRegAccess::theSbeRegAccess().getSecureMode() ||
                            SbeRegAccess::theSbeRegAccess().getEmulateSABSet();

    bool isProductionSigned = ((!SbeRegAccess::theSbeRegAccess().getIsImprintMode()) ||
                                SbeRegAccess::theSbeRegAccess().getForceProductionMode());

    if(isSecurityEnabled == true && isProductionSigned == false)
            securityLevel = ENUM_ATTR_SECURITY_LEVEL_PERMISSIVE;

    if(isSecurityEnabled == false)
            securityLevel = ENUM_ATTR_SECURITY_LEVEL_DISABLED;

    // Keep a copy of the security level attr in globals for all internal use cases(chipop fencing, istep enforcing etc)
    iv_securityLevel = securityLevel;

    // Update ATTR_SECURITY_LEVEL
    setAttrSecurityLvl();

    // Set the soft security check level for various functionalities
    setSoftSecurityCheckLvl();

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}

softSecurityCheckLvl securityUtils::getSoftSecurityCheckLvl(bool isUserRequestedDisable)
{
    #define SBE_FUNC " getSoftSecurityCheckLvl "
    SBE_ENTER(SBE_FUNC);

    softSecurityCheckLvl softSecurityCheckLvlForFunc = SOFT_SECURITY_CHECK_ENABLED;

    do{

        if(getSecurityLevel() == fapi2::ENUM_ATTR_SECURITY_LEVEL_DISABLED)
        {
            softSecurityCheckLvlForFunc = SOFT_SECURITY_CHECK_DISABLED;
            break;
        }

        if(getSecurityLevel() == fapi2::ENUM_ATTR_SECURITY_LEVEL_PERMISSIVE && isUserRequestedDisable)
        {
            softSecurityCheckLvlForFunc = SOFT_SECURITY_CHECK_PERMISSIVE_ENABLED;
            break;
        }

    }while(false);

    SBE_INFO(SBE_FUNC "Has user requested to disable soft security check via scratch 11: %x, "
                          "Soft security check level: %x",
                            isUserRequestedDisable, softSecurityCheckLvlForFunc);

    SBE_EXIT(SBE_FUNC);
    return softSecurityCheckLvlForFunc;
    #undef SBE_FUNC
}

void securityUtils::setSoftSecurityCheckLvl()
{
    #define SBE_FUNC " setSoftSecurityCheckLvl "
    SBE_ENTER(SBE_FUNC)

    iv_scomFilteringCheckLvl = getSoftSecurityCheckLvl(SbeRegAccess::theSbeRegAccess().getDisableScomFilteringBit());
    SBE_INFO(SBE_FUNC "Soft security check level for scom filtering functionality %d", iv_scomFilteringCheckLvl);

    iv_invalidAddrCheckLvl = getSoftSecurityCheckLvl(SbeRegAccess::theSbeRegAccess().getDisableInvalidScomAddrBit());
    SBE_INFO(SBE_FUNC "Soft security check level for invalid scom address check functionality %d", iv_invalidAddrCheckLvl);

    iv_allowAttrOverrideLvl = getSoftSecurityCheckLvl(SbeRegAccess::theSbeRegAccess().getAllowAttrOverrideBit());
    SBE_INFO(SBE_FUNC "Soft security check level for allow attribute override functionality %d", iv_allowAttrOverrideLvl);

    SBE_EXIT(SBE_FUNC)
    #undef SBE_FUNC
}

bool securityUtils::isHwAccessPathTrusted()
{
    #define SBE_FUNC " isHwAccessPathTrusted "
    SBE_ENTER(SBE_FUNC)

    bool isHwAccessPathTrusted = true;

    for(uint8_t i = 0; i < iv_noOfNonSecureHwPaths; i++)
    {
        if(iv_nonSecureHwPaths[i] == static_cast<sbeFifoType>(SBE_GLOBAL->activeUsFifo))
        {
            isHwAccessPathTrusted = false;
            break;
        }
    }

    SBE_EXIT(SBE_FUNC)
    return isHwAccessPathTrusted;
    #undef SBE_FUNC
}

securityUtils *g_pSbeSecurityUtils = &securityUtils::getInstance();
