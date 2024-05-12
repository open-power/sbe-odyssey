/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/pmlib/pspi/pspi_test.c $     */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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

#include <pspi.h>

#ifdef __OCC_PLAT
    #define TEST_TARGET PSPI_TAP0
#endif

#ifdef __TCC_PLAT
    #define TEST_TARGET PSPI_HUB
#endif

// scratch 0
#define TEST_PCOM   0x6C0A0
#define TEST_DATA   0xDEADBEEFABCDEF00
#define TEST_DB     0xED
#define TEST_VC     PSPI_VC2

void
pspi_test_ping()
{
#ifdef __OCC_PLAT
    uint32_t command = PSPI_PING_SEND        |
                       PSPI_CLK_DIV_DEFAULT  |
                       PSPI_PING_MODE_SINGLE |
                       PSPI_PING_GENERATOR   |
                       PSPI_PING_DATAOP_NULL;
    uint32_t header =  PSPI_PING_HDR_REQ_DB0_DAT8;
    uint64_t* data = NULL;
    pspi_link_ping_write( TEST_TARGET, header, TEST_DATA );
    pspi_link_ping(       TEST_TARGET, command );
    header = pspi_link_ping_read( TEST_TARGET, data );
#endif
#ifdef __PLAT_TAP
    uint32_t command = PSPI_CLK_DIV_DEFAULT  |
                       PSPI_PING_MODE_SINGLE |
                       PSPI_PING_RESPONDER   |
                       PSPI_PING_DATAOP_NULL;
    uint32_t header =  PSPI_PING_HDR_RSP_VC0_NOCODE | PSPI_PING_HDR_RSP_CODE_NOERR;
#endif
}

void
pspi_test_pcom()
{
    uint64_t data = 0;

    uint32_t config = PSPI_VC0_EN | PSPI_VC0_FSM_FREEZE_EN;
    pspi_vc0_config( TEST_TARGET, config );

    pspi_pcom_put( TEST_TARGET, TEST_PCOM, TEST_DATA);
    pspi_pcom_get( TEST_TARGET, TEST_PCOM, &data );
}

void
pspi_test_db()
{
    uint32_t config = PSPI_VC0_EN | PSPI_VC0_FSM_FREEZE_EN;
    pspi_vc0_config( TEST_TARGET, config );

    pspi_doorbell_send(     TEST_TARGET, PSPI_DB0, TEST_DB);
    pspi_msg_doorbell_send( TEST_TARGET, PSPI_DB1, TEST_DATA);
}


void
pspi_test_outbound()
{
    uint32_t i;
    uint64_t test = TEST_DATA;
    pspi_buffer_t buffer;

    pspi_vc_buffer_init(&buffer,
                        TEST_TARGET,
                        TEST_VC,
                        PSPI_CNFG | PSPI_CTRL_PUSH | PSPI_CTRL_SEND,
                        0xfff3200, 100, 0);

    pspi_vc_put(TEST_TARGET,
                TEST_VC,
                PSPI_PUSH | PSPI_SEND | PSPI_START,
                test);

    for (i = 0; i < 6; i++)
    {
        pspi_vc_put(TEST_TARGET,
                    TEST_VC,
                    PSPI_PUSH | PSPI_SEND | PSPI_NEXT,
                    ++test);
    }

    pspi_vc_put(TEST_TARGET,
                TEST_VC,
                PSPI_PUSH | PSPI_SEND | PSPI_LAST,
                ++test);

    pspi_vc_unicast_go(TEST_TARGET,
                       TEST_VC,
                       PSPI_GO | PSPI_CTRL_PUSH | PSPI_CTRL_SEND);
}

void
pspi_test_inbound()
{
    uint32_t i;
    uint64_t test;
    pspi_buffer_t buffer;

    pspi_vc_buffer_init(&buffer,
                        TEST_TARGET,
                        TEST_VC,
                        PSPI_CNFG | PSPI_CTRL_PUSH | PSPI_CTRL_RECV,
                        0xfff3400, 100, 0);

    pspi_vc_get(TEST_TARGET,
                TEST_VC,
                PSPI_PUSH | PSPI_RECV | PSPI_START,
                &test);

    PSPI_TRACE("Data Read %x %x", (uint32_t)(test >> 32), (uint32_t)(test & 0xffffffff));

    for (i = 0; i < 6; i++)
    {
        pspi_vc_get(TEST_TARGET,
                    TEST_VC,
                    PSPI_PUSH | PSPI_RECV | PSPI_NEXT,
                    &test);
        PSPI_TRACE("Data Read %x %x", (uint32_t)(test >> 32), (uint32_t)(test & 0xffffffff));
    }

    pspi_vc_get(TEST_TARGET,
                TEST_VC,
                PSPI_PUSH | PSPI_RECV | PSPI_LAST,
                &test);
    PSPI_TRACE("Data Read %x %x", (uint32_t)(test >> 32), (uint32_t)(test & 0xffffffff));
}


void
pspi_test()
{
    uint32_t config = PSPI_LINK_ENABLE |
                      PSPI_LINK_NOT_WRAP_MODE |
                      PSPI_LINK_PKT_CAPTURE   |
                      PSPI_LINK_ECC_GEN_EN    |
                      PSPI_LINK_ECC_CHECK_EN  |
                      PSPI_LINK_RX_FEM_FREEZE;
    pspi_link_config( TEST_TARGET, config );
    pspi_test_db();
    pspi_test_pcom();

#ifdef __OCC_PLAT
    pspi_test_outbound();
#endif

#ifdef __TCC_PLAT
    pspi_test_inbound();
#endif

    pspi_test_ping();
}
