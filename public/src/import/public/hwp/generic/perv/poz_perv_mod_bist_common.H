/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_perv_mod_bist_common.H $ */
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
struct OPCGCaptureSettings
{
    uint64_t OPCG_CAPT1;
    uint64_t OPCG_CAPT2;
    uint64_t OPCG_CAPT3;
};

const struct OPCGCaptureSettings ccN0F =
{
    .OPCG_CAPT1 = 0xE000000000000000ULL,
    .OPCG_CAPT2 = 0xE000000000000000ULL,
    .OPCG_CAPT3 = 0xE000000000000000ULL
};

const struct OPCGCaptureSettings ccN1F =
{
    .OPCG_CAPT1 = 0xE070000000000000ULL,
    .OPCG_CAPT2 = 0xE0001C0000000000ULL,
    .OPCG_CAPT3 = 0xE000000000000000ULL
};

const struct OPCGCaptureSettings ccN2F =
{
    .OPCG_CAPT1 = 0xE073800000000000ULL,
    .OPCG_CAPT2 = 0xE0001CE000000000ULL,
    .OPCG_CAPT3 = 0xE000000000000000ULL
};

const struct OPCGCaptureSettings ccN3F =
{
    .OPCG_CAPT1 = 0xE0739C0000000000ULL,
    .OPCG_CAPT2 = 0xE0001CE700000000ULL,
    .OPCG_CAPT3 = 0xE000000000000000ULL
};

const struct OPCGCaptureSettings ccN5F =
{
    .OPCG_CAPT1 = 0xE0739CE700000000ULL,
    .OPCG_CAPT2 = 0xE0001CE739c00000ULL,
    .OPCG_CAPT3 = 0xE000000000000000ULL
};

const struct OPCGCaptureSettings ccN9F =
{
    .OPCG_CAPT1 = 0xE0739CE739CE7000ULL,
    .OPCG_CAPT2 = 0xE0001CE739CE739CULL,
    .OPCG_CAPT3 = 0xE000000000000000ULL
};

const struct OPCGCaptureSettings ccNS1 =
{
    .OPCG_CAPT1 = 0xE14A000000000000ULL,
    .OPCG_CAPT2 = 0xE108528000000000ULL,
    .OPCG_CAPT3 = 0xE000000000000000ULL
};

const struct OPCGCaptureSettings ccNS2 =
{
    .OPCG_CAPT1 = 0xE14A100000000000ULL,
    .OPCG_CAPT2 = 0xE108528400000000ULL,
    .OPCG_CAPT3 = 0xE000000000000000ULL
};

const struct OPCGCaptureSettings ccNS4 =
{
    .OPCG_CAPT1 = 0xE14A108400000000ULL,
    .OPCG_CAPT2 = 0xE108528421000000ULL,
    .OPCG_CAPT3 = 0xE000000000000000ULL
};

const struct OPCGCaptureSettings ccNS5 =
{
    .OPCG_CAPT1 = 0xE14A108420000000ULL,
    .OPCG_CAPT2 = 0xE108528421080000ULL,
    .OPCG_CAPT3 = 0xE000000000000000ULL
};

const struct OPCGCaptureSettings ccNS8 =
{
    .OPCG_CAPT1 = 0xE14A108421084000ULL,
    .OPCG_CAPT2 = 0xE108528421084210ULL,
    .OPCG_CAPT3 = 0xE000000000000000ULL
};

const struct OPCGCaptureSettings opcgCaptureSettings[] =  { ccN0F, ccN1F, ccN2F, ccN3F, ccN5F, ccN9F, ccNS1, ccNS2, ccNS4, ccNS5, ccNS8 };

struct STUMPSWeightSettings
{
    uint8_t PRIMARY_WEIGHT;
    uint8_t SECONDARY_WEIGHT;
};

const struct STUMPSWeightSettings ccw0164 =
{
    .PRIMARY_WEIGHT = 0b1010ULL,
    .SECONDARY_WEIGHT = 0b1010ULL,
};

const struct STUMPSWeightSettings ccw0132 =
{
    .PRIMARY_WEIGHT = 0b1000ULL,
    .SECONDARY_WEIGHT = 0b1000ULL,
};

const struct STUMPSWeightSettings ccw0116 =
{
    .PRIMARY_WEIGHT = 0b0110ULL,
    .SECONDARY_WEIGHT = 0b0110ULL,
};

const struct STUMPSWeightSettings ccw0108 =
{
    .PRIMARY_WEIGHT = 0b0100ULL,
    .SECONDARY_WEIGHT = 0b0100ULL,
};

const struct STUMPSWeightSettings ccw0104 =
{
    .PRIMARY_WEIGHT = 0b0010ULL,
    .SECONDARY_WEIGHT = 0b0010ULL,
};

const struct STUMPSWeightSettings ccw0102 =
{
    .PRIMARY_WEIGHT = 0b0000ULL,
    .SECONDARY_WEIGHT = 0b0000ULL,
};

const struct STUMPSWeightSettings ccw0304 =
{
    .PRIMARY_WEIGHT = 0b0011ULL,
    .SECONDARY_WEIGHT = 0b0011ULL,
};

const struct STUMPSWeightSettings ccw0708 =
{
    .PRIMARY_WEIGHT = 0b0101ULL,
    .SECONDARY_WEIGHT = 0b0101ULL,
};

const struct STUMPSWeightSettings ccw1516 =
{
    .PRIMARY_WEIGHT = 0b0111ULL,
    .SECONDARY_WEIGHT = 0b0111ULL,
};

const struct STUMPSWeightSettings ccw3132 =
{
    .PRIMARY_WEIGHT = 0b1001ULL,
    .SECONDARY_WEIGHT = 0b1001ULL,
};

const struct STUMPSWeightSettings ccw6364 =
{
    .PRIMARY_WEIGHT = 0b1011ULL,
    .SECONDARY_WEIGHT = 0b1011ULL,
};

const struct STUMPSWeightSettings stumpsWeightSettings[] =  { ccw0164, ccw0132, ccw0116, ccw0108, ccw0104, ccw0102, ccw0304, ccw0708, ccw1516, ccw3132, ccw6364 };
