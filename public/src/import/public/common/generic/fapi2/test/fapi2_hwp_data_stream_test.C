/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/generic/fapi2/test/fapi2_hwp_data_stream_test.C $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2022                        */
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
/**
 * @file fapi2_hwp_data_stream_test.C
 * @brief test program for testing HWP data streaming classes
 */
#define FAPI2_DEBUG 1
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <stdint.h>
#include <iostream>
#include <thread>

#include <hwp_data_stream.H>
#include <assert.h>
#include <utils.H>

TEST_CASE("hwp_array_istream test")
{
    static const fapi2::hwp_data_unit testdata[] =
    {
        0x12345678, 0xDEADBEEF, 0xCAFEBABE
    };
    fapi2::hwp_array_istream is(testdata, ARRAY_SIZE(testdata));

    fapi2::hwp_data_unit x;
    REQUIRE(is.get(x) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(x == testdata[0]);
    REQUIRE(is.get(x) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(x == testdata[1]);
    REQUIRE(is.get(x) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(x == testdata[2]);
    REQUIRE(is.get(x) == fapi2::FAPI2_RC_FALSE);
}

TEST_CASE("hwp_array_ostream test")
{
    fapi2::hwp_data_unit writebuf[4] = {0};
    static const fapi2::hwp_data_unit testdata[ARRAY_SIZE(writebuf)] =
    {
        0x12345678, 0xDEADBEEF, 0xCAFEBABE
    };
    fapi2::hwp_array_ostream os(writebuf, ARRAY_SIZE(writebuf));

    for (int i = 0; i < 3; i++)
    {
        REQUIRE(os.put(testdata[i]) == fapi2::FAPI2_RC_SUCCESS);
    }

    REQUIRE(os.getLength() == 3);

    REQUIRE(!memcmp(writebuf, testdata, sizeof(testdata)));
}

TEST_CASE("hwp_array_ostream overrun test")
{
    fapi2::hwp_data_unit writebuf[1] = {0};
    fapi2::hwp_array_ostream os(writebuf, ARRAY_SIZE(writebuf));

    REQUIRE(os.put(0xCAFEBABE) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(os.put(0xDEADBEEF) == fapi2::FAPI2_RC_OVERFLOW);
    REQUIRE(writebuf[0] == 0xCAFEBABE);
}

TEST_CASE("hwp_bit_istream test")
{
    static const fapi2::hwp_data_unit testdata[] =
    {
        0x12345678, 0xDEADBEEF, 0xCAFEBABE, 0xABADF00D, 0x98765432,
        0xC8E85681, 0x7F010000
    };
    fapi2::hwp_array_istream _is(testdata, ARRAY_SIZE(testdata));
    fapi2::hwp_bit_istream is(_is);

    uint8_t  x8;
    uint16_t x16;
    uint32_t x32;
    uint64_t x64;
    REQUIRE(is.get8(x8) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(x8 == 0x12);
    REQUIRE(is.get32(x32) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(x32 == 0x345678DE);
    REQUIRE(is.get16(x16) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(x16 == 0xADBE);
    REQUIRE(is.get64(x64) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(x64 == 0xEFCAFEBABEABADF0ULL);
    REQUIRE(is.get(x64, 10, true) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(x64 == 0x0D80000000000000ULL);
    REQUIRE(is.get(x64, 30) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(x64 == 0x18765432ULL);
    REQUIRE(is.get_var_len_int(x32) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(x32 == 0x123456);
    REQUIRE(is.get_var_len_int(x32) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(x32 == 0xFF);
    REQUIRE(is.get_var_len_int(x32) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(x32 == 1);
    REQUIRE(is.get16(x16) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(x16 == 0);
    REQUIRE(is.get8(x8) == fapi2::FAPI2_RC_FALSE);
    REQUIRE(is.get(x64, 24) == fapi2::FAPI2_RC_FALSE);
}

TEST_CASE("hwp_bit_ostream test")
{
    fapi2::hwp_data_unit writebuf[8] = {0};
    static const fapi2::hwp_data_unit testdata[ARRAY_SIZE(writebuf)] =
    {
        0x12345678, 0xDEADBEEF, 0xCAFEBABE, 0xABADF00D, 0x98765432,
        0xC8E85681, 0x7F010000
    };
    fapi2::hwp_array_ostream _os(writebuf, ARRAY_SIZE(writebuf));
    fapi2::hwp_bit_ostream os(_os);

    REQUIRE(os.put8 (0x12                 ) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(os.put32(0x345678DE           ) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(os.put16(0xADBE               ) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(os.put64(0xEFCAFEBABEABADF0ULL) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(os.put(  0x0D987ULL,            20       ) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(os.put(  0x6543200000000000ULL, 20, true ) == fapi2::FAPI2_RC_SUCCESS);

    REQUIRE(os.put_var_len_int(0x123456) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(os.put_var_len_int(0xFF) == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(os.put_var_len_int(1) == fapi2::FAPI2_RC_SUCCESS);

    REQUIRE(os.flush() == fapi2::FAPI2_RC_SUCCESS);
    REQUIRE(_os.getLength() == 7);

    for (unsigned int i = 0; i < ARRAY_SIZE(testdata); i++)
    {
        REQUIRE(writebuf[i] == testdata[i]);
    }
}

TEST_CASE("hwp_bit_iostream communication test")
{
    uint64_t test_instructions[][2] =
    {
        {  8, 125 },
        { 32, 1253262 },
        { 16, 12523 },
        { 64, 1251234143651234 },
        {  3, 6 },
        { 12, 1251 },
        { 25, 12525623 },
    };

    fapi2::hwp_data_unit buf[1000];
    fapi2::hwp_array_ostream _os(buf, ARRAY_SIZE(buf));
    fapi2::hwp_bit_ostream os(_os);

    for (auto i : test_instructions)
    {
        REQUIRE(os.put(i[1], i[0]) == fapi2::FAPI2_RC_SUCCESS);
    }

    REQUIRE(os.flush() == fapi2::FAPI2_RC_SUCCESS);

    fapi2::hwp_array_istream _is(buf, _os.getLength());
    fapi2::hwp_bit_istream is(_is);

    for (auto i : test_instructions)
    {
        uint64_t value;
        REQUIRE(is.get(value, i[0]) == fapi2::FAPI2_RC_SUCCESS);
        REQUIRE(value == i[1]);
    }
}
