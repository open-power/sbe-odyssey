/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/explorer/hwp/fire_omi_setup_bars.C $ */
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

///
/// @file fire_omi_setup_bars.C
/// @brief Contains the explorer OMI setup bar regs for apollo
///
// *HWP HWP Owner: Edgar Cordero <ecordero@us.ibm.com>
// *HWP HWP Backup: Ryan King <rpking@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 3
// *HWP Consumed by: Memory

#include <fapi2.H>
#include <generic/memory/lib/utils/find.H>
#include <generic/memory/lib/utils/c_str.H>
#include <generic/memory/lib/utils/pos.H>
#include <fire_omi_setup_bars.H>

#include <lib/apollo/apollo_fire_consts.H>

extern "C"
{
    ///
    /// @brief Sets bar regs in Fire for the OCMB link on Apollo
    /// @param[in] i_target the MCC in Fire target to operate on
    /// @return FAPI2_RC_SUCCESS iff ok
    ///

    fapi2::ReturnCode fire_omi_setup_bars(const fapi2::Target<fapi2::TARGET_TYPE_MCC>& i_target)
    {
        uint8_t l_barOffset(0);
        uint8_t l_bitToSet(0);

        uint8_t l_chipUnitNum(0);
        fapi2::buffer<uint64_t> l_scomaddress;
        fapi2::buffer<uint64_t> l_scomdata;

        const auto& l_myomis = mss::find_targets<fapi2::TARGET_TYPE_OMI>(i_target);

        if ( l_myomis.size() > 0 )
        {
            const auto l_proc = mss::find_target<fapi2::TARGET_TYPE_PROC_CHIP>(l_myomis[0]);

            // declare all variables before a FAPI_TRY
            FAPI_TRY( FAPI_ATTR_GET(fapi2::ATTR_CHIP_UNIT_POS, i_target, l_chipUnitNum) );
            FAPI_TRY( FAPI_ATTR_GET(fapi2::ATTR_PROC_DSTLCFG_MMIO_ADDRBIT_POS, i_target, l_barOffset) );
            FAPI_INF("ATTR_PROC_DSTLCFG_MMIO_ADDRBIT_POS is (%d) on pos %s", l_barOffset, mss::c_str(i_target) );

            FAPI_INF("Running Setup_bars on (%s)", mss::c_str(l_proc));

            if( l_chipUnitNum >= FIRE_MAX_OMI )
            {
                FAPI_INF("%d is out of range for max omi less than %d (%s), skipping...", l_chipUnitNum, FIRE_MAX_OMI,
                         mss::c_str(i_target));
                return fapi2::FAPI2_RC_SUCCESS;
            }

            l_bitToSet = FIRE_OPENCAPI_BAR_START - l_barOffset;

            l_scomaddress = FIRE_OPENCAPI_BAR_BASE | FIRE_DDIMM_PORT_OFFSET[l_chipUnitNum];

            l_scomdata.flush<0>();
            FAPI_TRY( l_scomdata.setBit( l_bitToSet ) );
            FAPI_INF("Setting reg to  (0x%016x) on target (%s)", l_scomdata, mss::c_str(i_target));

            FAPI_TRY( fapi2::putScom(l_proc, l_scomaddress, l_scomdata) );

        }

        return fapi2::FAPI2_RC_SUCCESS;

    fapi_try_exit:

        return fapi2::current_err;

    }
}
