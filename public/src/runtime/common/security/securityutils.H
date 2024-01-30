/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/security/securityutils.H $          */
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

#pragma once

#include "stddef.h"
#include "attrutils.H"
#include "sbefifo.H"

/**
 * @brief SBE internal enum for soft security checks levels
 *
 * SOFT_SECURITY_CHECK_ENABLED
 *      - If ATTR_SECURITY_LEVEL = ENFORCING
 *      - If ATTR_SECURITY_LEVEL = PERMISSIVE & user has not req to disable soft security checks via scratch
 *
 * SOFT_SECURITY_CHECK_DISABLED
 *      - If ATTR_SECURITY_LEVEL = DISABLED
*       - If HW access path is trusted
 *
 * SOFT_SECURITY_CHECK_PERMISSIVE_ENABLED
 *      - If ATTR_SECURITY_LEVEL = PERMISSIVE & user has req to disable soft security checks via scratch
 */
enum softSecurityCheckLvl{
    SOFT_SECURITY_CHECK_ENABLED = 1,
    SOFT_SECURITY_CHECK_DISABLED = 2,
    SOFT_SECURITY_CHECK_PERMISSIVE_ENABLED = 3,
};

class securityUtils {

    public:

    /**
     * @brief Returns the instance of this class
     *
     * @return A reference to securityUtils
     */
    static securityUtils& getInstance(){
        static securityUtils iv_instance;
        return iv_instance;
    }

    /**
     * @brief Set the Security lvl of iv_securityLevel
     *
     */
    void setSecurityLvl();

    /**
     * @brief Get ATTR_SECURITY_LEVEL from global copy
     *
     */
    inline fapi2::ATTR_SECURITY_LEVEL_Type getSecurityLevel(){
        return iv_securityLevel;
    }

    /**
     * @brief Get the Scom Filtering soft security Check Lvl
     *        This api internally also checks if the current HW access path is trusted or not
     *
     * @return softSecurityCheckLvl
     */
    inline softSecurityCheckLvl getScomFilteringCheckLvl()
    {
        return (iv_noOfNonSecureHwPaths ?
                (isHwAccessPathTrusted() ? SOFT_SECURITY_CHECK_DISABLED : iv_scomFilteringCheckLvl) :
                SOFT_SECURITY_CHECK_DISABLED);
    }

    /**
     * @brief Get the Invalid Scom Addr Check soft security Check Lvl
     *        This api internally also checks if the current HW access path is trusted or not
     *
     * @return softSecurityCheckLvl
     */
    inline softSecurityCheckLvl getInvalidScomAddrCheckLvl()
    {
        return (iv_noOfNonSecureHwPaths ?
                (isHwAccessPathTrusted() ? SOFT_SECURITY_CHECK_DISABLED : iv_invalidAddrCheckLvl) :
                SOFT_SECURITY_CHECK_DISABLED);
    }

    protected:

    /**
     * @brief Set the ATTR_SECURITY_LEVEL attribute
     *
     */
    virtual void setAttrSecurityLvl()
    {};

    /**
     * @brief Default Constructor for securityUtils object
     *        By default we will always enforce highest security level
     *
     **/
    securityUtils() :
        iv_securityLevel(fapi2::ENUM_ATTR_SECURITY_LEVEL_ENFORCING),
        iv_scomFilteringCheckLvl(SOFT_SECURITY_CHECK_ENABLED),
        iv_invalidAddrCheckLvl(SOFT_SECURITY_CHECK_ENABLED),
        iv_nonSecureHwPaths(NULL),
        iv_noOfNonSecureHwPaths(0)
    {};

    /**
     * @brief Parameterized Constructor for securityUtils object
     *        By default we will always enforce highest security level
     *
     * @param *nonSecureHwPaths Pointer to array of non secure HW paths for which soft security
     *        checks has to be applied
     * @param noOfNonSecureHwPaths Number of non secure HW paths
     *
     **/
    securityUtils(const sbeFifoType *nonSecureHwPaths, const size_t noOfNonSecureHwPaths) :
        iv_securityLevel(fapi2::ENUM_ATTR_SECURITY_LEVEL_ENFORCING),
        iv_scomFilteringCheckLvl(SOFT_SECURITY_CHECK_ENABLED),
        iv_invalidAddrCheckLvl(SOFT_SECURITY_CHECK_ENABLED),
        iv_nonSecureHwPaths(nonSecureHwPaths),
        iv_noOfNonSecureHwPaths(noOfNonSecureHwPaths)
    {};

    private:

    // Disable copy construction and assignment operators
    securityUtils(const securityUtils&) = delete;
    securityUtils& operator=(const securityUtils&) = delete;

    /**
     * @brief Set the Soft Security Check Lvl for various functionalities
     *
     */
    void setSoftSecurityCheckLvl();

    /**
     * @brief Api to get soft security level of a functionality
     *
     * @param isUserRequestedDisable Check if user has requested to disable soft security checks via scratch 11
     *                               (Accounted only if ATTR_SECURITY_LEVEL is PERMISSIVE)
     *
     * @return softSecurityCheckLvl Security lvl for requested functionality
     */
    softSecurityCheckLvl getSoftSecurityCheckLvl(bool isUserRequestedDisable);

    /**
     * @brief Check if requested chipop HW path from user is trusted
     *        By default base class trusts all paths.
     *        Any untrusted path checks can be implemented in derived class
     *
     * @return true if trusted (Internal paths like pipes,HB FIFO etc)
     * @return false if untrusted (External paths like FIFO)
     */
    bool isHwAccessPathTrusted();

    /**
     * @brief SBE Global Security Level
     *        Same as ATTR_SECURITY_LEVEL
     * Level 0 - DISABLED   - Security disabled, SAB=0
     * Level 1 - PERMISSIVE - Security enabled but not strictly enforced,
     *           SAB=1 but firmware signed with imprint keys
     * Level 2 - ENFORCING  - Security enabled and strictly enforced,
     *           SAB=1 and firmware signed with production keys
     */
    fapi2::ATTR_SECURITY_LEVEL_Type iv_securityLevel;

    // Used for istep and secure mode chipop fence check as well
    softSecurityCheckLvl iv_scomFilteringCheckLvl;
    softSecurityCheckLvl iv_invalidAddrCheckLvl;

    const sbeFifoType *iv_nonSecureHwPaths;
    const size_t iv_noOfNonSecureHwPaths;
};

extern securityUtils* g_pSbeSecurityUtils;