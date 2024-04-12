/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/initfiles/pt/scom/pt_l3_scom.C $     */
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
#include "pt_l3_scom.H"
#include <stdint.h>
#include <stddef.h>
#include <fapi2.H>

using namespace fapi2;

constexpr uint64_t literal_3 = 3;
constexpr uint64_t literal_1 = 1;
constexpr uint64_t literal_8 = 8;
constexpr uint64_t literal_0b0001 = 0b0001;
constexpr uint64_t literal_0b0100 = 0b0100;

fapi2::ReturnCode pt_l3_scom(const fapi2::Target<fapi2::TARGET_TYPE_L3CACHE>& TGT0,
                             const fapi2::Target<fapi2::TARGET_TYPE_SYSTEM>& TGT1, const fapi2::Target<fapi2::TARGET_TYPE_COMPUTE_CHIP>& TGT2)
{
    {
        fapi2::ATTR_EC_Type   l_chip_ec;
        fapi2::ATTR_NAME_Type l_chip_id;
        FAPI_TRY(FAPI_ATTR_GET_PRIVILEGED(fapi2::ATTR_NAME, TGT2, l_chip_id));
        FAPI_TRY(FAPI_ATTR_GET_PRIVILEGED(fapi2::ATTR_EC, TGT2, l_chip_ec));
        fapi2::ATTR_PROC_LCO_TARGETS_COUNT_Type l_TGT2_ATTR_PROC_LCO_TARGETS_COUNT;
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_LCO_TARGETS_COUNT, TGT2, l_TGT2_ATTR_PROC_LCO_TARGETS_COUNT));
        fapi2::ATTR_PROC_LCO_MODE_DISABLE_Type l_TGT1_ATTR_PROC_LCO_MODE_DISABLE;
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_LCO_MODE_DISABLE, TGT1, l_TGT1_ATTR_PROC_LCO_MODE_DISABLE));
        fapi2::ATTR_CHIP_UNIT_POS_Type l_TGT0_ATTR_CHIP_UNIT_POS;
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_CHIP_UNIT_POS, TGT0, l_TGT0_ATTR_CHIP_UNIT_POS));
        fapi2::ATTR_PROC_LCO_TARGETS_VECTOR_Type l_TGT2_ATTR_PROC_LCO_TARGETS_VECTOR;
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_LCO_TARGETS_VECTOR, TGT2, l_TGT2_ATTR_PROC_LCO_TARGETS_VECTOR));
        uint64_t l_def_L3_EPS_DIVIDE = literal_1;
        fapi2::ATTR_PROC_EPS_READ_CYCLES_T0_Type l_TGT1_ATTR_PROC_EPS_READ_CYCLES_T0;
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_EPS_READ_CYCLES_T0, TGT1, l_TGT1_ATTR_PROC_EPS_READ_CYCLES_T0));
        fapi2::ATTR_PROC_EPS_READ_CYCLES_T1_Type l_TGT1_ATTR_PROC_EPS_READ_CYCLES_T1;
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_EPS_READ_CYCLES_T1, TGT1, l_TGT1_ATTR_PROC_EPS_READ_CYCLES_T1));
        fapi2::ATTR_PROC_EPS_READ_CYCLES_T2_Type l_TGT1_ATTR_PROC_EPS_READ_CYCLES_T2;
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_EPS_READ_CYCLES_T2, TGT1, l_TGT1_ATTR_PROC_EPS_READ_CYCLES_T2));
        fapi2::ATTR_PROC_EPS_WRITE_CYCLES_T1_Type l_TGT1_ATTR_PROC_EPS_WRITE_CYCLES_T1;
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_EPS_WRITE_CYCLES_T1, TGT1, l_TGT1_ATTR_PROC_EPS_WRITE_CYCLES_T1));
        fapi2::ATTR_PROC_EPS_WRITE_CYCLES_T2_Type l_TGT1_ATTR_PROC_EPS_WRITE_CYCLES_T2;
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_EPS_WRITE_CYCLES_T2, TGT1, l_TGT1_ATTR_PROC_EPS_WRITE_CYCLES_T2));
        fapi2::ATTR_PROC_L3_HASH_DISABLE_Type l_TGT1_ATTR_PROC_L3_HASH_DISABLE;
        FAPI_TRY(FAPI_ATTR_GET(fapi2::ATTR_PROC_L3_HASH_DISABLE, TGT1, l_TGT1_ATTR_PROC_L3_HASH_DISABLE));
        fapi2::buffer<uint64_t> l_scom_buffer;
        {
            FAPI_TRY(fapi2::getScom( TGT0, 0x2001060aull, l_scom_buffer ));

            if (((l_TGT1_ATTR_PROC_LCO_MODE_DISABLE == fapi2::ENUM_ATTR_PROC_LCO_MODE_DISABLE_FALSE)
                 && (l_TGT2_ATTR_PROC_LCO_TARGETS_COUNT >= literal_3)))
            {
                constexpr auto l_ECP_L3_L3_MISC_L3CERRS_L3_LCO_ENABLE_CFG_ON = 0x1;
                l_scom_buffer.insert<0, 1, 63, uint64_t>(l_ECP_L3_L3_MISC_L3CERRS_L3_LCO_ENABLE_CFG_ON );
            }
            else if (literal_1)
            {
                constexpr auto l_ECP_L3_L3_MISC_L3CERRS_L3_LCO_ENABLE_CFG_OFF = 0x0;
                l_scom_buffer.insert<0, 1, 63, uint64_t>(l_ECP_L3_L3_MISC_L3CERRS_L3_LCO_ENABLE_CFG_OFF );
            }

            l_scom_buffer.insert<1, 3, 61, uint64_t>(l_TGT0_ATTR_CHIP_UNIT_POS );
            l_scom_buffer.insert<4, 8, 56, uint64_t>(l_TGT2_ATTR_PROC_LCO_TARGETS_VECTOR );
            FAPI_TRY(fapi2::putScom(TGT0, 0x2001060aull, l_scom_buffer));
        }
        {
            FAPI_TRY(fapi2::getScom( TGT0, 0x20010629ull, l_scom_buffer ));

            l_scom_buffer.insert<0, 12, 52, uint64_t>((((l_TGT1_ATTR_PROC_EPS_READ_CYCLES_T0 / literal_8) / l_def_L3_EPS_DIVIDE) +
                    literal_1) );
            l_scom_buffer.insert<12, 12, 52, uint64_t>((((l_TGT1_ATTR_PROC_EPS_READ_CYCLES_T1 / literal_8) / l_def_L3_EPS_DIVIDE) +
                    literal_1) );
            l_scom_buffer.insert<24, 12, 52, uint64_t>((((l_TGT1_ATTR_PROC_EPS_READ_CYCLES_T2 / literal_8) / l_def_L3_EPS_DIVIDE) +
                    literal_1) );
            FAPI_TRY(fapi2::putScom(TGT0, 0x20010629ull, l_scom_buffer));
        }
        {
            FAPI_TRY(fapi2::getScom( TGT0, 0x2001062aull, l_scom_buffer ));

            l_scom_buffer.insert<0, 12, 52, uint64_t>((((l_TGT1_ATTR_PROC_EPS_WRITE_CYCLES_T1 / literal_8) / l_def_L3_EPS_DIVIDE) +
                    literal_1) );
            l_scom_buffer.insert<12, 12, 52, uint64_t>((((l_TGT1_ATTR_PROC_EPS_WRITE_CYCLES_T2 / literal_8) / l_def_L3_EPS_DIVIDE) +
                    literal_1) );
            l_scom_buffer.insert<30, 4, 60, uint64_t>(l_def_L3_EPS_DIVIDE );
            FAPI_TRY(fapi2::putScom(TGT0, 0x2001062aull, l_scom_buffer));
        }
        {
            FAPI_TRY(fapi2::getScom( TGT0, 0x2001062bull, l_scom_buffer ));

            if (( true ))
            {
                constexpr auto l_ECP_L3_L3_MISC_L3CERRS_L3_CERRS_PF_CFG_SKIP_GRP_SCOPE_EN_ON = 0x1;
                l_scom_buffer.insert<5, 1, 63, uint64_t>(l_ECP_L3_L3_MISC_L3CERRS_L3_CERRS_PF_CFG_SKIP_GRP_SCOPE_EN_ON );
            }

            if ((l_TGT1_ATTR_PROC_L3_HASH_DISABLE == fapi2::ENUM_ATTR_PROC_L3_HASH_DISABLE_ON))
            {
                constexpr auto l_ECP_L3_L3_MISC_L3CERRS_L3_ADDR_HASH_EN_CFG_OFF = 0x0;
                l_scom_buffer.insert<11, 1, 63, uint64_t>(l_ECP_L3_L3_MISC_L3CERRS_L3_ADDR_HASH_EN_CFG_OFF );
            }
            else if (( true ))
            {
                constexpr auto l_ECP_L3_L3_MISC_L3CERRS_L3_ADDR_HASH_EN_CFG_ON = 0x1;
                l_scom_buffer.insert<11, 1, 63, uint64_t>(l_ECP_L3_L3_MISC_L3CERRS_L3_ADDR_HASH_EN_CFG_ON );
            }

            constexpr auto l_ECP_L3_L3_MISC_L3CERRS_L3_RDSN_LINEDEL_UE_EN_ON = 0x1;
            l_scom_buffer.insert<2, 1, 63, uint64_t>(l_ECP_L3_L3_MISC_L3CERRS_L3_RDSN_LINEDEL_UE_EN_ON );
            constexpr auto l_ECP_L3_L3_MISC_L3CERRS_L3_DMAP_CI_EN_CFG_OFF = 0x0;
            l_scom_buffer.insert<1, 1, 63, uint64_t>(l_ECP_L3_L3_MISC_L3CERRS_L3_DMAP_CI_EN_CFG_OFF );
            l_scom_buffer.insert<14, 4, 60, uint64_t>(literal_0b0001 );
            l_scom_buffer.insert<18, 4, 60, uint64_t>(literal_0b0100 );
            FAPI_TRY(fapi2::putScom(TGT0, 0x2001062bull, l_scom_buffer));
        }

    };
fapi_try_exit:
    return fapi2::current_err;
}
