/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/security/platsecurityutils.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2024                             */
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

#include "securityutils.H"
#include "sbefifo.H"

#define NUMBER_OF_NON_SECURE_HW_PATHS 0x1

class platSecurityUtils : public securityUtils {

    public:

    /**
     * @brief Returns the instance of this class
     *
     * @return A reference to platSecurityUtils
    */
    static platSecurityUtils& getInstance(){
        static platSecurityUtils iv_instance;
        return iv_instance;
    }

    /**
     * @brief Initialise plat security class.
     *        Override g_pSbeSecurityUtils to point to derived class
     *
    */
    void init(){
        g_pSbeSecurityUtils = this;
    }

    protected:

    /**
     * @brief Set the Attr Security Lvl
     *
     */
    void setAttrSecurityLvl() override;

    private:

    /**
     * @brief Constructor for a new plat Security Utils object
     *
     */
    platSecurityUtils() :
        securityUtils(iv_nonSecureHwPaths, (size_t)NUMBER_OF_NON_SECURE_HW_PATHS)
    {};

    // Disable copy construction and assignment operators
    platSecurityUtils(const platSecurityUtils&) = delete;
    platSecurityUtils& operator=(const platSecurityUtils&) = delete;

    const sbeFifoType iv_nonSecureHwPaths[NUMBER_OF_NON_SECURE_HW_PATHS] = {
            SBE_FIFO
        };
};
