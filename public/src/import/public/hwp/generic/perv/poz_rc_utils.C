/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_rc_utils.C $    */
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

#include <poz_rc_utils.H>

using namespace fapi2;

bool rcIsPibError(const uint32_t i_rc)
{
    return (i_rc == RC_POZ_PIB_XSCOM_ERROR ||
            i_rc == RC_POZ_PIB_OFFLINE_ERROR ||
            i_rc == RC_POZ_PIB_PARTIAL_ERROR ||
            i_rc == RC_POZ_PIB_ADDRESS_ERROR ||
            i_rc == RC_POZ_PIB_CLOCK_ERROR ||
            i_rc == RC_POZ_PIB_PARITY_ERROR ||
            i_rc == RC_POZ_PIB_TIMEOUT_ERROR);
}

bool rcIsPutRingError(const uint32_t i_rc)
{
    return (i_rc == RC_PUTRING_OPCG_DONE_TIMEOUT ||
            i_rc == RC_PUTRING_CHECKWORD_DATA_MISMATCH ||
            i_rc == RC_PARALLEL_SCAN_COMPARE_ERR ||
            i_rc == RC_PUTRING_RS4_STRING_OVERRUN ||
            i_rc == RC_SCAN_CHIPLET_NOT_FUNCTIONAL ||
            i_rc == RC_INVALID_SCAN_TARGET_TYPE ||
            i_rc == RC_UNSUPPORTED_SCAN_TARGET_TYPE ||
            i_rc == RC_CORE_MULTICAST_SCAN_INVALID_ARGS ||
            i_rc == RC_INVALID_RING_IMAGE ||
            i_rc == RC_UNTRUSTED_RING_IMAGE);
}
