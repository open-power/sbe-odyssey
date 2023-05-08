/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/odyssey/perv/ody_sppe_attr_setup.C $ */
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
//------------------------------------------------------------------------------
/// @file  ody_sppe_attr_setup.C
/// @brief Establish attribute state in SPPE platform
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Anusha Reddy (anusrang@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#include <ody_sppe_attr_setup.H>
#include <ody_scratch_regs.H>
#include <ody_scratch_regs_utils.H>

using namespace fapi2;

constexpr uint32_t ODY_TP_ATTR_PG = 0xFFE0FFBF;
constexpr uint32_t ODY_MC_ATTR_PG = 0xFFE0001F;

ReturnCode ody_sppe_attr_setup(const Target<TARGET_TYPE_OCMB_CHIP>& i_target_chip)
{
    fapi2::buffer<uint64_t> l_scratch16_reg = 0;

    FAPI_DBG("Start");

    // populate scratch registers from attribute state/targeting model, if not
    // set by sppe_config_update execution
    FAPI_TRY(ody_scratch_regs_update(i_target_chip, false, true));

    FAPI_DBG("Read Scratch16 mailbox register (valid)");
    FAPI_TRY(fapi2::getScom(i_target_chip, SCRATCH_REGISTER16.scom_addr, l_scratch16_reg));

    // scratch 5 -- SPI bus clock divider
    {
        fapi2::buffer<uint64_t> l_scratch5_reg = 0;
        fapi2::ATTR_SPI_BUS_DIV_REF_Type l_attr_spi_bus_div_ref = 0;

        if (l_scratch16_reg.getBit<SCRATCH5_REG_VALID_BIT>())
        {
            FAPI_DBG("Reading Scratch 5 mailbox register");
            FAPI_TRY(fapi2::getScom(i_target_chip, SCRATCH_REGISTER5.scom_addr, l_scratch5_reg));

            FAPI_DBG("Setting up ATTR_SPI_BUS_DIV_REF");
            l_scratch5_reg.extract<ATTR_SPI_BUS_DIV_REF_STARTBIT, ATTR_SPI_BUS_DIV_REF_LENGTH>(l_attr_spi_bus_div_ref);
            FAPI_TRY(FAPI_ATTR_SET(fapi2::ATTR_SPI_BUS_DIV_REF, i_target_chip, l_attr_spi_bus_div_ref));
        }
    }

    // scratch 8 -- PLL bypass/OCMB position
    {
        fapi2::buffer<uint64_t> l_scratch8_reg = 0;
        fapi2::ATTR_IO_TANK_PLL_BYPASS_Type l_attr_io_tank_pll_bypass = fapi2::ENUM_ATTR_IO_TANK_PLL_BYPASS_NO_BYPASS;
        fapi2::ATTR_BUS_POS_Type l_attr_bus_pos = 0xFF;

        if (l_scratch16_reg.getBit<SCRATCH8_REG_VALID_BIT>())
        {
            FAPI_DBG("Reading Scratch 8 mailbox register");
            FAPI_TRY(fapi2::getScom(i_target_chip, SCRATCH_REGISTER8.scom_addr, l_scratch8_reg));

            FAPI_DBG("Setting up ATTR_IO_TANK_PLL_BYPASS");

            if (l_scratch8_reg.getBit<ATTR_IO_TANK_PLL_BYPASS_BIT>())
            {
                l_attr_io_tank_pll_bypass = fapi2::ENUM_ATTR_IO_TANK_PLL_BYPASS_BYPASS;
            }

            FAPI_TRY(FAPI_ATTR_SET(fapi2::ATTR_IO_TANK_PLL_BYPASS, i_target_chip, l_attr_io_tank_pll_bypass));

            FAPI_DBG("Setting up ATTR_BUS_POS");
            l_scratch8_reg.extractToRight<ATTR_BUS_POS_STARTBIT, ATTR_BUS_POS_LENGTH>(l_attr_bus_pos);
            FAPI_TRY(FAPI_ATTR_SET(fapi2::ATTR_BUS_POS, i_target_chip, l_attr_bus_pos));
        }
    }

    // scratch 6 -- PLL bucket/frequency selection
    {
        fapi2::buffer<uint64_t> l_scratch6_reg = 0;
        fapi2::ATTR_OCMB_PLL_BUCKET_Type l_attr_ocmb_pll_bucket = 0;
        fapi2::ATTR_IO_TANK_PLL_BYPASS_Type l_attr_io_tank_pll_bypass = fapi2::ENUM_ATTR_IO_TANK_PLL_BYPASS_NO_BYPASS;
        uint32_t l_freq_grid_exp_mhz = 0;
        uint32_t l_freq_grid_act_mhz = 0;
        uint32_t l_freq_link_mhz = 0;

        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_IO_TANK_PLL_BYPASS, i_target_chip, l_attr_io_tank_pll_bypass));

        if (l_scratch16_reg.getBit<SCRATCH6_REG_VALID_BIT>())
        {
            FAPI_DBG("Reading Scratch 6 mailbox register");
            FAPI_TRY(fapi2::getScom(i_target_chip, SCRATCH_REGISTER6.scom_addr, l_scratch6_reg));

            FAPI_DBG("Setting up ATTR_OCMB_PLL_BUCKET");
            l_scratch6_reg.extractToRight<ATTR_OCMB_PLL_BUCKET_STARTBIT, ATTR_OCMB_PLL_BUCKET_LENGTH>(l_attr_ocmb_pll_bucket);
            FAPI_TRY(FAPI_ATTR_SET(fapi2::ATTR_OCMB_PLL_BUCKET, i_target_chip, l_attr_ocmb_pll_bucket));

            // cmdtable execution will have posted PLL frequency feedback into this mailbox
            // read it and use it to setup value of attribute reflecting OMI link frequency
            FAPI_DBG("Setting up ATTR_FREQ_OMI_MHZ");
            l_scratch6_reg.extractToRight<ATTR_OCMB_PLL_FREQ_STARTBIT, ATTR_OCMB_PLL_FREQ_LENGTH>(l_freq_grid_act_mhz);

            // in bypass, just assign the posted grid frequency to the link frequency attribute, and skip the bucket check
            if (l_attr_io_tank_pll_bypass == fapi2::ENUM_ATTR_IO_TANK_PLL_BYPASS_BYPASS)
            {
                l_freq_link_mhz = l_freq_grid_act_mhz;
            }
            else
            {
                FAPI_TRY(ody_scratch_regs_get_pll_freqs(i_target_chip, l_attr_ocmb_pll_bucket, l_freq_grid_exp_mhz, l_freq_link_mhz));

                FAPI_ASSERT(l_freq_grid_act_mhz == l_freq_grid_exp_mhz,
                            fapi2::ODY_SPPE_ATTR_SETUP_GRID_FREQ_MISMATCH()
                            .set_TARGET_CHIP(i_target_chip)
                            .set_FREQ_GRID_ACT(l_freq_grid_act_mhz)
                            .set_FREQ_GRID_EXP(l_freq_grid_exp_mhz)
                            .set_PLL_BUCKET(l_attr_ocmb_pll_bucket),
                            "Actual grid frequency feedback (%d) did not match expected value (%d) based on PLL bucket (%d)!",
                            l_freq_grid_act_mhz, l_freq_grid_exp_mhz, l_attr_ocmb_pll_bucket);
            }

            FAPI_TRY(FAPI_ATTR_SET(fapi2::ATTR_FREQ_OMI_MHZ, i_target_chip, l_freq_link_mhz));
        }
    }

    // scratch 7 -- clockstop-on-checkstop setup
    {
        fapi2::buffer<uint64_t> l_scratch7_reg = 0;
        fapi2::ATTR_CLOCKSTOP_ON_XSTOP_Type l_attr_clockstop_on_xstop = fapi2::ENUM_ATTR_CLOCKSTOP_ON_XSTOP_DISABLED;
        uint8_t l_clockstop_on_xstop = ATTR_CLOCKSTOP_ON_XSTOP_DISABLED;

        if (l_scratch16_reg.getBit<SCRATCH7_REG_VALID_BIT>())
        {
            FAPI_DBG("Reading Scratch 7 mailbox register");
            FAPI_TRY(fapi2::getScom(i_target_chip, SCRATCH_REGISTER7.scom_addr, l_scratch7_reg));

            FAPI_DBG("Setting up ATTR_CLOCKSTOP_ON_XSTOP");
            l_scratch7_reg.extractToRight<ATTR_CLOCKSTOP_ON_XSTOP_STARTBIT, ATTR_CLOCKSTOP_ON_XSTOP_LENGTH>
            (l_clockstop_on_xstop);

            if (l_clockstop_on_xstop == ATTR_CLOCKSTOP_ON_XSTOP_XSTOP)
            {
                l_attr_clockstop_on_xstop = fapi2::ENUM_ATTR_CLOCKSTOP_ON_XSTOP_STOP_ON_XSTOP;
            }
            else if (l_clockstop_on_xstop == ATTR_CLOCKSTOP_ON_XSTOP_XSTOP_SPATTN)
            {
                l_attr_clockstop_on_xstop = fapi2::ENUM_ATTR_CLOCKSTOP_ON_XSTOP_STOP_ON_XSTOP_AND_SPATTN;
            }
            else if (l_clockstop_on_xstop == ATTR_CLOCKSTOP_ON_XSTOP_STAGED_XSTOP)
            {
                l_attr_clockstop_on_xstop = fapi2::ENUM_ATTR_CLOCKSTOP_ON_XSTOP_STOP_ON_STAGED_XSTOP;
            }

            FAPI_TRY(FAPI_ATTR_SET(fapi2::ATTR_CLOCKSTOP_ON_XSTOP, i_target_chip, l_attr_clockstop_on_xstop));
        }
    }

    // scratch 11 -- FW Mode flags
    {
        fapi2::buffer<uint64_t> l_scratch11_reg = 0;
        fapi2::Target<fapi2::TARGET_TYPE_SYSTEM> FAPI_SYSTEM;
        fapi2::ATTR_IS_SIMULATION_Type l_attr_is_simulation = 0;
        fapi2::ATTR_ENABLE_ABIST_Type l_enable_abist = fapi2::ENUM_ATTR_ENABLE_ABIST_ENABLE;
        fapi2::ATTR_ENABLE_LBIST_Type l_enable_lbist = fapi2::ENUM_ATTR_ENABLE_LBIST_ENABLE;

        if (l_scratch16_reg.getBit<SCRATCH11_REG_VALID_BIT>())
        {
            FAPI_DBG("Reading Scratch 11 mailbox register");
            FAPI_TRY(fapi2::getScom(i_target_chip, SCRATCH_REGISTER11.scom_addr, l_scratch11_reg));

            FAPI_DBG("Setting up ATTR_IS_SIMULATION");
            l_scratch11_reg.extractToRight<ATTR_IS_SIMULATION_STARTBIT, ATTR_IS_SIMULATION_LENGTH>
            (l_attr_is_simulation);
            FAPI_TRY(FAPI_ATTR_SET(fapi2::ATTR_IS_SIMULATION, FAPI_SYSTEM, l_attr_is_simulation));

            // mailbox bits act as disables
            FAPI_DBG("Setting up ATTR_ENABLE_ABIST");

            if (l_scratch11_reg.getBit<ATTR_ENABLE_ABIST_DISABLE_BIT>())
            {
                l_enable_abist = fapi2::ENUM_ATTR_ENABLE_ABIST_DISABLE;
            }

            FAPI_TRY(FAPI_ATTR_SET(fapi2::ATTR_ENABLE_ABIST, i_target_chip, l_enable_abist));

            FAPI_DBG("Setting up ATTR_ENABLE_LBIST");

            if (l_scratch11_reg.getBit<ATTR_ENABLE_LBIST_DISABLE_BIT>())
            {
                l_enable_lbist = fapi2::ENUM_ATTR_ENABLE_LBIST_DISABLE;
            }

            FAPI_TRY(FAPI_ATTR_SET(fapi2::ATTR_ENABLE_LBIST, i_target_chip, l_enable_lbist));
        }
    }


#ifdef __PPE__
    // scratch 12 -- dynamic inits
    {
        fapi2::buffer<uint64_t> l_scratch12_reg = 0;

        fapi2::ATTR_DYNAMIC_INIT_FEATURE_VEC_Type l_dynamic_init_feature_vec = { 0 };
        uint32_t l_dynamic_init_feature_vec_0_31 = 0;

        if (l_scratch16_reg.getBit<SCRATCH12_REG_VALID_BIT>())
        {
            FAPI_DBG("Reading Scratch 12 mailbox register");
            FAPI_TRY(fapi2::getScom(i_target_chip, SCRATCH_REGISTER12.scom_addr, l_scratch12_reg));

            FAPI_DBG("Setting up ATTR_DYNAMIC_INIT_FEATURE_VEC");
            l_scratch12_reg.extractToRight<ATTR_DYNAMIC_INIT_FEATURE_STARTBIT, ATTR_DYNAMIC_INIT_FEATURE_LENGTH>
            (l_dynamic_init_feature_vec_0_31);

            l_dynamic_init_feature_vec[0] |= (((uint64_t) l_dynamic_init_feature_vec_0_31) << 32);

            FAPI_TRY(FAPI_ATTR_SET(fapi2::ATTR_DYNAMIC_INIT_FEATURE_VEC, i_target_chip, l_dynamic_init_feature_vec));
        }
    }
#endif

    // set platform attributes for partial good
    // since there are no optional/configurable regions on Odyssey, a functional chip has static partial good attributes
    for (const auto& l_perv : i_target_chip.getChildren<TARGET_TYPE_PERV>(TARGET_STATE_PRESENT))
    {
        fapi2::ATTR_CHIP_UNIT_POS_Type l_chip_unit_pos;
        fapi2::ATTR_PG_Type l_pg;
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CHIP_UNIT_POS, l_perv, l_chip_unit_pos));

        switch(l_chip_unit_pos)
        {
            case 1:
                l_pg = ODY_TP_ATTR_PG;
                break;

            case 8:
                l_pg = ODY_MC_ATTR_PG;
                break;

            default:
                FAPI_ASSERT(false,
                            fapi2::ODY_SPPE_ATTR_SETUP_INVALID_TARGET()
                            .set_TARGET_CHIP(i_target_chip)
                            .set_TARGET_PERV(l_perv)
                            .set_UNIT_POS_PERV(l_chip_unit_pos),
                            "Unexpected pervasive target unit ID!");

        }

        FAPI_DBG("Setting up ATTR_PG (chiplet ID: 0x%02X, value: 0x%08X",
                 l_chip_unit_pos, l_pg);
        FAPI_TRY(FAPI_ATTR_SET(fapi2::ATTR_PG, l_perv, l_pg));
    }

    // set platform attributes needed for basic multicast setup used in following HWPs
    FAPI_TRY(ody_scratch_regs_setup_plat_multicast_attrs(i_target_chip));

fapi_try_exit:
    FAPI_DBG("End");
    return current_err;
}
