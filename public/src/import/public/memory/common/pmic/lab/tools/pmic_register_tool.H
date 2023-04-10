/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/memory/common/pmic/lab/tools/pmic_register_tool.H $ */
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
/// @file pmic_register_tool.H
/// @brief Contains pmic register tool functions
///
// *HWP HWP Owner: Michael Pardeik <pardeik@us.ibm.com>
// *HWP HWP Backup: Louis Stermole <stermole@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 2
// *HWP Consumed by: Memory Lab

#ifndef _PMIC_REGISTER_TOOL_H
#define _PMIC_REGISTER_TOOL_H

#include <fapi2.H>
#include <lib/i2c/i2c_pmic.H>
#include <generic/memory/lab/mss_lab_input_processing.H>
#include <string>

namespace mss
{
namespace lab
{
namespace pmic
{
namespace reg
{

///
/// @brief PMIC ID List
///
enum class pmic_id_list
{
    PMIC0 = 0,
    PMIC1 = 1,
    PMIC2 = 2,
    PMIC3 = 3,
    ALL   = 4,
};

///
/// @brief structure for pmic_register_tool options
///
struct options_pmic_register_tool
{
    bool iv_use_help = false;
    mss::lab::pmic::reg::pmic_id_list iv_pmic_id = mss::lab::pmic::reg::pmic_id_list::ALL;
    // changed from uint8_t to uint16_t so LESS than check can be done when options are processed
    uint16_t iv_pmic_reg_addr = 0x00;
    uint16_t iv_pmic_reg_data = 0x00;
    bool iv_use_pmic_reg_data = false;

    ///
    /// @brief Equals comparison operator
    /// @param[in] i_rhs right hand side to compare against
    /// @return true iff everything is equal
    ///
    bool operator==(const options_pmic_register_tool& i_rhs) const
    {
        return iv_use_help            == i_rhs.iv_use_help &&
               iv_pmic_id             == i_rhs.iv_pmic_id &&
               iv_pmic_reg_addr       == i_rhs.iv_pmic_reg_addr &&
               iv_pmic_reg_data       == i_rhs.iv_pmic_reg_data &&
               iv_use_pmic_reg_data   == i_rhs.iv_use_pmic_reg_data;
    }
};

///
/// @brief Input helper class for pmic_register_tool
///
class input_helper_pmic_register_tool
{
    public:

        static constexpr mss::lab::input::traits HELP1 =
            mss::lab::input::create_traits< mss::lab::input::process_type::FLAG,
            mss::lab::input::required::OPTIONAL>(
                "-h",
                "help screen flag",
                "false");
        static constexpr mss::lab::input::traits HELP2 =
            mss::lab::input::create_traits< mss::lab::input::process_type::FLAG,
            mss::lab::input::required::OPTIONAL>(
                "-help",
                "help screen flag",
                "false");
        static constexpr mss::lab::input::traits PMIC_ID =
            mss::lab::input::create_traits< mss::lab::input::process_type::ENUM,
            mss::lab::input::required::REQUIRED>(
                "-pmic=",
                "PMIC position to act on",
                "");
        static constexpr mss::lab::input::traits PMIC_REG_ADDR =
            mss::lab::input::create_traits< mss::lab::input::process_type::HEX,
            mss::lab::input::required::REQUIRED,
            mss::lab::input::compare::LESS>(
                "-addr=",
                "PMIC register address in hex to read or write",
                "",
                0x100);
        static constexpr mss::lab::input::traits PMIC_REG_DATA =
            mss::lab::input::create_traits< mss::lab::input::process_type::HEX,
            mss::lab::input::required::OPTIONAL,
            mss::lab::input::compare::LESS>(
                "-data=",
                "PMIC register data (1 byte) in hex to write (MSB is bit 7)",
                "0x00",
                0x100);
        static const std::vector<std::pair<mss::lab::input::traits, std::vector<std::string>>> FIELDS;
};

///
/// @brief constexpr for pmic_register_tool
///
constexpr mss::lab::input::traits input_helper_pmic_register_tool::HELP1;
constexpr mss::lab::input::traits input_helper_pmic_register_tool::HELP2;
constexpr mss::lab::input::traits input_helper_pmic_register_tool::PMIC_ID;
constexpr mss::lab::input::traits input_helper_pmic_register_tool::PMIC_REG_ADDR;
constexpr mss::lab::input::traits input_helper_pmic_register_tool::PMIC_REG_DATA;

///
/// @brief field vector for pmic_register_tool
///
const std::vector<std::pair<mss::lab::input::traits, std::vector<std::string>>> input_helper_pmic_register_tool::FIELDS
=
{
    {input_helper_pmic_register_tool::HELP1, {}},
    {input_helper_pmic_register_tool::HELP2, {}},
    {
        input_helper_pmic_register_tool::PMIC_ID, {
            "PMIC0, PMIC1, PMIC2, PMIC3, ALL"
        }
    },
    {input_helper_pmic_register_tool::PMIC_REG_ADDR, {}},
    {input_helper_pmic_register_tool::PMIC_REG_DATA, {}},
};

///
/// @brief Forward declaration of mappings from text to mss::lab::pmic::reg::pmic_id_list enum
///
template <>
const std::map<std::string, mss::lab::pmic::reg::pmic_id_list>
mss::lab::enum_traits<mss::lab::pmic::reg::pmic_id_list>::iv_string_to_enum_map
{
    {"PMIC0", mss::lab::pmic::reg::pmic_id_list::PMIC0},
    {"PMIC1", mss::lab::pmic::reg::pmic_id_list::PMIC1},
    {"PMIC2", mss::lab::pmic::reg::pmic_id_list::PMIC2},
    {"PMIC3", mss::lab::pmic::reg::pmic_id_list::PMIC3},
    {"ALL",   mss::lab::pmic::reg::pmic_id_list::ALL},
};

template <>
const char* mss::lab::enum_traits<mss::lab::pmic::reg::pmic_id_list>::iv_type = "mss::lab::pmic::reg::pmic_id_list";

///
/// @brief help for pmic_register_tool
///
void help_pmic_register_tool()
{
    const std::vector<std::string> l_examples =
    {
        "",
        "-pall -pmic=ALL -addr=0x30",
        "-p0 -pmic=PMIC0 -addr=0x30 -data=0x00",
        "-help"
    };

    mss::lab::input::print_help_screen("pmic_register_tool", input_helper_pmic_register_tool::FIELDS, l_examples);
    return;
}

///
/// @brief Process tool inputs for pmic_register_tool
///
/// @param[in/out]  io_argc              number of arguments passed in by the user
/// @param[in/out]  io_argv              list of arguments passed in by the user
/// @param[in/out]  io_options       data structure for pmic_register_tool options
///
/// @return FAPI2_RC_SUCCESS if success, else error code.
///
fapi2::ReturnCode process_pmic_register_tool_options(int& io_argc,
        char* io_argv[],
        struct options_pmic_register_tool& io_options)
{

    const uint8_t DEFAULT_0 = 0x00;

    // process tool options
    // flag options
    io_options.iv_use_help = false;
    io_options.iv_use_pmic_reg_data = false;
    FAPI_TRY(mss::lab::input::process_input<mss::lab::pmic::reg::input_helper_pmic_register_tool::HELP1>(io_argc, io_argv,
             io_options.iv_use_help));
    FAPI_TRY(mss::lab::input::process_input<mss::lab::pmic::reg::input_helper_pmic_register_tool::HELP2>(io_argc, io_argv,
             io_options.iv_use_help));

    // print help screen and return
    if(io_options.iv_use_help)
    {
        help_pmic_register_tool();
        return fapi2::RC_MSS_LAB_HELP_SCREEN_CALLED;
    }

    // required flag with value options
    FAPI_TRY(mss::lab::input::process_input<mss::lab::pmic::reg::input_helper_pmic_register_tool::PMIC_ID>(io_argc, io_argv,
             io_options.iv_pmic_id));
    FAPI_TRY(mss::lab::input::process_input<mss::lab::pmic::reg::input_helper_pmic_register_tool::PMIC_REG_ADDR>(io_argc,
             io_argv,
             io_options.iv_pmic_reg_addr));
    // optional flag with value options
    FAPI_TRY(mss::lab::input::process_tool_option<mss::lab::pmic::reg::input_helper_pmic_register_tool::PMIC_REG_DATA>
             (io_argc, io_argv,
              io_options.iv_use_pmic_reg_data, DEFAULT_0, io_options.iv_pmic_reg_data));

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief PMIC register tool function
///
/// @param[in] i_ocmb_target OCMB target
/// @param[in] input options specified
/// @return fapi2::ReturnCode FAPI2_RC_SUCCESS iff success
///
fapi2::ReturnCode get_set_pmic_register(const fapi2::Target<fapi2::TARGET_TYPE_OCMB_CHIP>& i_ocmb_target,
                                        struct options_pmic_register_tool i_options)
{

    std::vector<fapi2::Target<fapi2::TARGET_TYPE_PMIC>> l_pmic_targets;
    std::vector<fapi2::Target<fapi2::TARGET_TYPE_PMIC>> l_pmics;
    fapi2::buffer<uint8_t> l_reg_contents;

    static std::map<uint8_t, mss::lab::pmic::reg::pmic_id_list> PMIC_SELECT_MAP =
    {
        {0, mss::lab::pmic::reg::pmic_id_list::PMIC0},
        {1, mss::lab::pmic::reg::pmic_id_list::PMIC1},
        {2, mss::lab::pmic::reg::pmic_id_list::PMIC2},
        {3, mss::lab::pmic::reg::pmic_id_list::PMIC3},
    };

    mss::logf(mss::TRACE, "PMICs on OCMB %s", mss::c_str(i_ocmb_target));

    l_pmic_targets = mss::find_targets<fapi2::TARGET_TYPE_PMIC>(i_ocmb_target, fapi2::TARGET_STATE_PRESENT);

    for (const auto& l_pmic : l_pmic_targets)
    {
        if ((i_options.iv_pmic_id == mss::lab::pmic::reg::pmic_id_list::ALL) ||
            (i_options.iv_pmic_id == PMIC_SELECT_MAP[mss::index(l_pmic)]))
        {
            l_pmics.push_back(l_pmic);
        }
    }

    for (const auto& l_pmic : l_pmics)
    {
        FAPI_TRY(mss::pmic::i2c::reg_read(l_pmic, i_options.iv_pmic_reg_addr, l_reg_contents));
        mss::logf(mss::TRACE,
                  "%s PMIC%u READ REG 0x%02x = 0x%02x",
                  mss::c_str(l_pmic), mss::index(l_pmic), i_options.iv_pmic_reg_addr, l_reg_contents);

        if (i_options.iv_use_pmic_reg_data)
        {
            l_reg_contents = i_options.iv_pmic_reg_data;
            FAPI_TRY(mss::pmic::i2c::reg_write(l_pmic, i_options.iv_pmic_reg_addr, l_reg_contents));
            mss::logf(mss::TRACE,
                      "%s PMIC%u WRITE REG 0x%02x = 0x%02x",
                      mss::c_str(l_pmic), mss::index(l_pmic), i_options.iv_pmic_reg_addr, l_reg_contents);
        }
    }

    return fapi2::FAPI2_RC_SUCCESS;

fapi_try_exit:
    return fapi2::current_err;
}



} // reg
} // pmic
} // lab
} // mss

#endif
