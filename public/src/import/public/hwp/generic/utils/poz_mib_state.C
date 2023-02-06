/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/utils/poz_mib_state.C $  */
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
///
/// @file  poz_mib_state.C
/// @brief Get MIB's internal state
///
/// *HWP HW Owner        : Greg Still <stillgs.@us.ibm.com>
/// *HWP HW Backup Owner : Yue Du <daviddu@us.ibm.com>
/// *HWP FW Owner        : Prasad BG Ranganath <prasadbgr@in.ibm.com>
/// *HWP Team            : PM
/// *HWP Level           : 2
/// *HWP Consumed by     : SBE, Cronus
///
/// @verbatim
///
/// Procedure Summary:
///   - Dump out PPE's internal state
///
/// @endverbatim

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <fapi2.H>
#include <poz_mib_state.H>
#include <poz_ppe_utils.H>
#include <map>

static std::map<enum PPE_MIB_IDX, const char*> v_mib_name =
{
    {PPE_IDX_XISIB  ,   "SIB_INFO"      },
    {PPE_IDX_XIMEM  ,   "MEM_INFO"      },
    {PPE_IDX_XISGB  ,   "SGB_INFO"      },
    {PPE_IDX_XIICAC ,   "ICACHE_INFO"   },
    {PPE_IDX_XIDCAC ,   "DCACHE_INFO"   },
};


/**
 * @brief Offsets from base address for CME regs.
 */

/*
std::vector<uint16_t> v_cme_mib_xirs =
{
    { SIB_INFO       },
    { MEM_INFO       },
    { SGB_INFO       },
    { ICACHE_INFO    },
    { PCB_QUEUE0_INFO},
    { PCB_QUEUE1_INFO},
    { PCBMUX0_DATA   },
    { PCBMUX1_DATA   },
    { EI_PCBMUX0_INFO},
    { EI_PCBMUX0_INFO},

};
std::vector<uint16_t> v_gpe_mib_xirs =
{
    { SIB_INFO          },
    { MEM_INFO          },
    { SGB_INFO          },
    { ICACHE_INFO       },
    { DCACH_INFO    },
};
std::vector<uint16_t> v_sbe_mib_xirs =
{
    { SBE_SIB_INFO       },
    { SBE_MEM_INFO       },
    { SBE_SGB_INFO       },
    { SBE_ICACHE_INFO    },


};
*/



//-----------------------------------------------------------------------------

/**
 * @brief Perform MIB internal reg "read" operation
 * @param[in]   i_target        Chip Target
 * @param[in]   i_ppe_type      PPE Type(SBE, GPE, QME, IOPPE, etc)
 * @param[in]   i_ppe_inst_num  PPE Type Instance Number
 * @param[out]  v_mib_xirs_value   Returned data
 * @return  fapi2::ReturnCode
 */
fapi2::ReturnCode
poz_mib_state_data(const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
                   const enum PPE_TYPES i_ppe_type, const uint32_t i_ppe_inst_num,
                   std::vector<Reg64Value_t>& v_mib_xirs_value)
{
    fapi2::buffer<uint64_t> l_data64;
    Reg64Value_t l_regVal;
    uint64_t t_addr;

    FAPI_INF("MIB PPE Type: %s Base Address : 0x%08llX", G_PPE_Types[i_ppe_type].name,
             G_PPE_Types[i_ppe_type].base_address);

    std::map<uint16_t, uint64_t>  v_mibs =  ppe_get_mib_offsets(i_ppe_type);

    for (auto it : v_mibs)
    {
        t_addr = ppe_get_mib_address(i_ppe_type, (enum PPE_MIB_IDX)it.first, i_ppe_inst_num);
        l_regVal.number = it.first;
        l_regVal.name = v_mib_name[(enum PPE_MIB_IDX)it.first];
        l_regVal.value = l_data64;
        FAPI_INF("%s = 0x%08llX", l_regVal.name, l_regVal.value);
        FAPI_TRY(getScom(i_target, t_addr, l_data64), "Error in GETSCOM");
        v_mib_xirs_value.push_back(l_regVal);
    }

fapi_try_exit:
    return fapi2::current_err;
}

// Hardware procedure
fapi2::ReturnCode
poz_mib_state(const fapi2::Target<fapi2::TARGET_TYPE_ANY_POZ_CHIP>& i_target,
              const enum PPE_TYPES i_ppe_type, const uint32_t i_ppe_inst_num,
              std::vector<Reg64Value_t>& v_mib_xirs_value)
{
    //Call the function to collect the data.
    poz_mib_state_data(i_target,
                       i_ppe_type, i_ppe_inst_num,
                       v_mib_xirs_value);


    return fapi2::current_err;
} // Procedure
