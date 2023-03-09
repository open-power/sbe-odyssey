/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/odyssey/sppe/plat/targeting/target_service.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
#ifndef __TARGET_SERVICE__
#define __TARGET_SERVICE__

#include "plat_target_service.H"
namespace fapi2
{
    /*
     * Derived class for target services.
     * It will be derived in each image.
     */

    class target_service : public sbe_target_service{
        public:
        // Disable copy contructor and assignment operator
        target_service(const target_service & ) = delete;
        target_service& operator=(const target_service & ) = delete;
        static target_service& getInstance()
        {
            return iv_instance;
        }

        ///
        /// @brief Function to apply the target state attributes
        ///     to corresponding target states in target vector
        ///
        void plat_TargetStateUpdateFromAttribute();

        private:
        static target_service iv_instance;
        /*
         * Constructor
         */
         target_service():sbe_target_service()
         {
         }
    };
}
extern fapi2::target_service *g_platTarget;
#endif
