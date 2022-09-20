/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/io/io_ppe_common.H $     */
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

//#pragma once
//
#include <fapi2.H>
#include <io_ppe_lib.H>

enum IO_PPE_CONSTS
{
    IO_PPE_MAX_THREADS = 9,
    IO_PPE_DCCAL_DONE_POLL_TRYS = 100,
    IO_PPE_DCCAL_DONE_POLL_DELAY_NS = 10000,
    IO_PPE_DCCAL_DONE_POLL_DELAY_SIM_CYCLES = 100000000,
    IO_PPE_EL_DONE_POLL_TRYS = 100,
    IO_PPE_EL_DONE_POLL_DELAY_NS = 10000,
    IO_PPE_EL_DONE_POLL_DELAY_SIM_CYCLES = 100000000,
    IO_PPE_HARD_RESET_DELAY_NS = 5000,
    IO_PPE_HARD_RESET_DELAY_SIM_CYCLES = 10000000,
    IO_PPE_RESUME_DELAY_NS = 5000,
    IO_PPE_RESUME_DELAY_SIM_CYCLES = 10000000,
};

enum IO_PPE_EXT_CMD_REQ_BITS
{
    HW_REG_INIT_PG    = 0b1000000000000000,
    IORESET_PL        = 0b0100000000000000,
    RX_DETECT_PL      = 0b0010000000000000,
    DCCAL_PL          = 0b0001000000000000,
    TX_ZCAL_PL        = 0b0000100000000000,
    TX_FFE_PL         = 0b0000010000000000,
    POWER_OFF_PL      = 0b0000001000000000,
    POWER_ON_PL       = 0b0000000100000000,
    TX_FIFO_INIT_PL   = 0b0000000010000000,
    RX_BIST_TESTS_PL  = 0b0000000001000000,
    TX_BIST_TESTS_PL  = 0b0000000000100000,
    TRAIN_PL          = 0b0000000000010000,
    RECAL_PL          = 0b0000000000001000,
    BIST_FINAL        = 0b0000000000000100,
    RESERVED_14       = 0b0000000000000010,
    RESERVED_15       = 0b0000000000000001,
};

template<fapi2::TargetType K, fapi2::MulticastType M = fapi2::MULTICAST_OR, typename V = fapi2::plat_target_handle_t>
class io_ppe_common
{
    public:
        io_ppe_regs<K, M, V>* iv_regs;

        io_ppe_common(io_ppe_regs<K, M, V>* i_regs) :
            iv_regs(i_regs)
        {
            FAPI_DBG("io_ppe_common constructor");
        }

        fapi2::ReturnCode config_threads(const fapi2::Target<K, M, V>& i_target,
                                         const fapi2::buffer<uint64_t> i_num_threads);

        fapi2::ReturnCode config_lanes(const fapi2::Target<K, M, V>& i_target,
                                       const fapi2::buffer<uint64_t> i_thread,
                                       const fapi2::buffer<uint64_t> i_gcr_id,
                                       const fapi2::buffer<uint64_t> i_num_rx_lanes,
                                       const fapi2::buffer<uint64_t> i_num_tx_lanes,
                                       const fapi2::buffer<uint64_t> i_spread_en,
                                       const fapi2::buffer<uint64_t> i_pcie_mode,
                                       const fapi2::buffer<uint64_t> i_serdes_16_to_1);

        fapi2::ReturnCode config(const fapi2::Target<K, M, V>& i_target,
                                 const fapi2::buffer<uint64_t> i_num_threads,
                                 const fapi2::buffer<uint64_t> i_gcr_ids[],
                                 const fapi2::buffer<uint64_t> i_rx_lanes[],
                                 const fapi2::buffer<uint64_t> i_tx_lanes[],
                                 const fapi2::buffer<uint64_t> i_spread_en,
                                 const fapi2::buffer<uint64_t> i_pcie_mode,
                                 const fapi2::buffer<uint64_t> i_serdes_16_to_1);

        fapi2::ReturnCode ppe_start(const fapi2::Target<K, M, V>& i_target);


        fapi2::ReturnCode clear_ext_cmd_thread(const fapi2::Target<K, M, V>& i_target,
                                               const fapi2::buffer<uint64_t> i_thread);

        fapi2::ReturnCode clear_ext_cmd(const fapi2::Target<K, M, V>& i_target,
                                        const fapi2::buffer<uint64_t> i_num_threads);

        fapi2::ReturnCode issue_ext_cmd_req_thread(const fapi2::Target<K, M, V>& i_target,
                const fapi2::buffer<uint64_t> i_thread,
                const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
                const fapi2::buffer<uint64_t> i_tx_lanes_0_31,
                const fapi2::buffer<uint64_t> i_ext_cmd_req);

        fapi2::ReturnCode issue_ext_cmd_req(const fapi2::Target<K, M, V>& i_target,
                                            const fapi2::buffer<uint64_t> i_num_threads,
                                            const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
                                            const fapi2::buffer<uint64_t> i_tx_lanes_0_31,
                                            const fapi2::buffer<uint64_t> i_ext_cmd_req);

        fapi2::ReturnCode check_ext_cmd_req_done_thread(const fapi2::Target<K, M, V>& i_target,
                const fapi2::buffer<uint64_t> i_thread,
                const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
                const fapi2::buffer<uint64_t> i_tx_lanes_0_31,
                const fapi2::buffer<uint64_t> i_ext_cmd_req,
                fapi2::buffer<uint8_t>& o_done);

        fapi2::ReturnCode check_ext_cmd_req_done(const fapi2::Target<K, M, V>& i_target,
                const fapi2::buffer<uint64_t> i_num_threads,
                const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
                const fapi2::buffer<uint64_t> i_tx_lanes_0_31,
                const fapi2::buffer<uint64_t> i_ext_cmd_req,
                fapi2::buffer<uint8_t>& o_done);


        fapi2::ReturnCode enable_lanes(const fapi2::Target<K, M, V>& i_target,
                                       const fapi2::buffer<uint64_t> i_num_threads,
                                       const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
                                       const fapi2::buffer<uint64_t> i_tx_lanes_0_31);

        fapi2::ReturnCode check_enable_lanes_done(const fapi2::Target<K, M, V>& i_target,
                const fapi2::buffer<uint64_t> i_num_threads,
                const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
                const fapi2::buffer<uint64_t> i_tx_lanes_0_31,
                fapi2::buffer<uint8_t>& o_done);

        fapi2::ReturnCode dccal_start(const fapi2::Target<K, M, V>& i_target,
                                      const fapi2::buffer<uint64_t> i_num_threads,
                                      const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
                                      const fapi2::buffer<uint64_t> i_tx_lanes_0_31,
                                      const uint8_t i_enable_lanes);

        fapi2::ReturnCode dccal_start_done(const fapi2::Target<K, M, V>& i_target,
                                           const fapi2::buffer<uint64_t> i_num_threads,
                                           const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
                                           const fapi2::buffer<uint64_t> i_tx_lanes_0_31,
                                           const uint8_t i_enable_lanes,
                                           fapi2::buffer<uint8_t>& o_done);

        fapi2::ReturnCode fast_mode_thread(const fapi2::Target<K, M, V>& i_target,
                                           const fapi2::buffer<uint64_t> i_thread);

        fapi2::ReturnCode fast_mode(const fapi2::Target<K, M, V>& i_target,
                                    const fapi2::buffer<uint64_t> i_num_threads);

};

// ===============================================================
// phy_config
// ===============================================================
template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::config_threads(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_num_threads)
{
    FAPI_DBG("Start");
    FAPI_TRY(IMG_REG_PPE_NUM_THREADS_PUT(iv_regs, i_target, i_num_threads));
fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}


template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::config_lanes(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_thread,
        const fapi2::buffer<uint64_t> i_gcr_id,
        const fapi2::buffer<uint64_t> i_num_rx_lanes,
        const fapi2::buffer<uint64_t> i_num_tx_lanes,
        const fapi2::buffer<uint64_t> i_spread_en,
        const fapi2::buffer<uint64_t> i_pcie_mode,
        const fapi2::buffer<uint64_t> i_serdes_16_to_1)
{
    FAPI_DBG("Start");
#ifdef IOT_IMAGE
    FAPI_TRY(FW_REG_FW_TX_CLOCK_LANE_PUT(iv_regs, i_target, i_thread, i_num_tx_lanes));
#endif
    FAPI_TRY(FW_REG_FW_GCR_BUS_ID_PUT(iv_regs, i_target, i_thread, i_gcr_id));
    FAPI_TRY(FW_REG_FW_NUM_LANES_RX_PUT(iv_regs, i_target, i_thread, i_num_rx_lanes));
    FAPI_TRY(FW_REG_FW_NUM_LANES_TX_PUT(iv_regs, i_target, i_thread, i_num_tx_lanes));
    FAPI_TRY(FW_REG_FW_SERDES_16_TO_1_MODE_PUT(iv_regs, i_target, i_thread, i_serdes_16_to_1));
    FAPI_TRY(FW_REG_FW_SPREAD_EN_PUT(iv_regs, i_target, i_thread, i_spread_en));
#ifndef IOT_IMAGE
    FAPI_TRY(FW_REG_FW_PCIE_MODE_PUT(iv_regs, i_target, i_thread, i_pcie_mode));
#endif
    FAPI_TRY(FW_REG_FW_STOP_THREAD_PUT(iv_regs, i_target, i_thread, 0));

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}

template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::config(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_num_threads,
        const fapi2::buffer<uint64_t> i_gcr_ids[],
        const fapi2::buffer<uint64_t> i_rx_lanes[],
        const fapi2::buffer<uint64_t> i_tx_lanes[],
        const fapi2::buffer<uint64_t> i_spread_en,
        const fapi2::buffer<uint64_t> i_pcie_mode,
        const fapi2::buffer<uint64_t> i_serdes_16_to_1)
{
    FAPI_DBG("Start");

    FAPI_TRY(config_threads(i_target, i_num_threads));

    for (uint32_t l_thread = 0; l_thread < i_num_threads; l_thread++)
    {
        FAPI_TRY(config_lanes(i_target,
                              l_thread,
                              i_gcr_ids[l_thread],
                              i_rx_lanes[l_thread],
                              i_tx_lanes[l_thread],
                              i_spread_en,
                              i_pcie_mode,
                              i_serdes_16_to_1));
    }

    //Turn off unused threads.
    for (uint32_t l_thread = i_num_threads + 1; l_thread < IO_PPE_MAX_THREADS; l_thread++)
    {
        FAPI_TRY(FW_REG_FW_STOP_THREAD_PUT(iv_regs, i_target, l_thread, 1));
    }

    iv_regs->flushCache(i_target);

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}

// ===============================================================
// phy_ppe_start
// ===============================================================
template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::ppe_start(const fapi2::Target<K, M, V>& i_target)
{
    FAPI_DBG("Start");

    FAPI_TRY(iv_regs->hardReset(i_target));
    fapi2::delay(IO_PPE_HARD_RESET_DELAY_NS, IO_PPE_HARD_RESET_DELAY_SIM_CYCLES);
    FAPI_TRY(iv_regs->resume(i_target));
    fapi2::delay(IO_PPE_RESUME_DELAY_NS, IO_PPE_RESUME_DELAY_SIM_CYCLES);

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}


// ===============================================================
// CLEAR_EXT_CMD_REQ
// ===============================================================
template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::clear_ext_cmd_thread(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_thread)
{
    FAPI_DBG("Start");

    fapi2::buffer<uint64_t> l_data = 0;

    FAPI_TRY(FW_REG_EXT_CMD_REQ_00_15_PUT(iv_regs, i_target, i_thread, l_data));
    FAPI_TRY(FW_REG_EXT_CMD_REQ_16_31_PUT(iv_regs, i_target, i_thread, l_data));

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}

template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::clear_ext_cmd(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_num_threads)
{
    FAPI_DBG("Start");

    for (fapi2::buffer<uint64_t> l_thread = 0; l_thread < i_num_threads; l_thread++)
    {
        FAPI_TRY(clear_ext_cmd_thread(i_target, l_thread));
    }

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}

// ===============================================================
// ISSUE_EXT_CMD_REQ
// ===============================================================
template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::issue_ext_cmd_req_thread(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_thread,
        const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
        const fapi2::buffer<uint64_t> i_tx_lanes_0_31,
        const fapi2::buffer<uint64_t> i_ext_cmd_req)
{
    FAPI_DBG("Start");

    fapi2::buffer<uint64_t> l_data = 0;
    // RX
    fapi2::buffer<uint64_t> l_rx_lanes_0_15 = (i_rx_lanes_0_31 >> 16) & 0xFFFF;
    fapi2::buffer<uint64_t> l_rx_lanes_16_31 = i_rx_lanes_0_31 & 0xFFFF;
    // TX
    fapi2::buffer<uint64_t> l_tx_lanes_0_15 = (i_tx_lanes_0_31 >> 16) & 0xFFFF;
    fapi2::buffer<uint64_t> l_tx_lanes_16_31 = i_tx_lanes_0_31 & 0xFFFF;

    FAPI_DBG("l_rx_lanes_0_15 %x l_rx_lanes_16_31: %x l_tx_lanes_0_15: %x l_tx_lanes_16_31: %x", l_rx_lanes_0_15,
             l_rx_lanes_16_31, l_tx_lanes_0_15, l_tx_lanes_16_31);

    // RX
    FAPI_TRY(FW_REG_EXT_CMD_LANES_RX_00_15_PUT(iv_regs, i_target, i_thread, l_rx_lanes_0_15));
    FAPI_TRY(FW_REG_EXT_CMD_LANES_RX_16_31_PUT(iv_regs, i_target, i_thread, l_rx_lanes_16_31));
    // TX
    FAPI_TRY(FW_REG_EXT_CMD_LANES_TX_00_15_PUT(iv_regs, i_target, i_thread, l_tx_lanes_0_15));
    FAPI_TRY(FW_REG_EXT_CMD_LANES_TX_16_31_PUT(iv_regs, i_target, i_thread, l_tx_lanes_16_31));


    //MUST flush lanes before start
    FAPI_TRY(iv_regs->flushCache(i_target));

    if (l_rx_lanes_0_15 || l_tx_lanes_0_15)
    {
        FAPI_DBG("EXT_CMD_REQ_00_15");
        FAPI_TRY(FW_REG_EXT_CMD_REQ_00_15_PUT(iv_regs, i_target, i_thread, i_ext_cmd_req));
    }

    if (l_rx_lanes_16_31 || l_tx_lanes_16_31)
    {
        FAPI_DBG("EXT_CMD_REQ_16_31");
        FAPI_TRY(FW_REG_EXT_CMD_REQ_16_31_PUT(iv_regs, i_target, i_thread, i_ext_cmd_req));
    }

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}


template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::issue_ext_cmd_req(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_num_threads,
        const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
        const fapi2::buffer<uint64_t> i_tx_lanes_0_31,
        const fapi2::buffer<uint64_t> i_ext_cmd_req)
{
    FAPI_DBG("Start");

    for (fapi2::buffer<uint64_t> l_thread = 0; l_thread < i_num_threads; l_thread++)
    {
        FAPI_TRY(issue_ext_cmd_req_thread(i_target, l_thread, i_rx_lanes_0_31, i_tx_lanes_0_31, i_ext_cmd_req));
    }

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}


// ===============================================================
// CHECK_EXT_CMD_REQ_DONE
// ===============================================================
template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::check_ext_cmd_req_done_thread(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_thread,
        const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
        const fapi2::buffer<uint64_t> i_tx_lanes_0_31,
        const fapi2::buffer<uint64_t> i_ext_cmd_req,
        fapi2::buffer<uint8_t>& o_done)
{
    FAPI_DBG("Start");

    fapi2::buffer<uint64_t> l_data = 0;

    // RX
    fapi2::buffer<uint64_t> l_rx_lanes_0_15 = (i_rx_lanes_0_31 >> 16) & 0xFFFF;
    fapi2::buffer<uint64_t> l_rx_lanes_16_31 = i_rx_lanes_0_31 & 0xFFFF;
    // TX
    fapi2::buffer<uint64_t> l_tx_lanes_0_15 = (i_tx_lanes_0_31 >> 16) & 0xFFFF;
    fapi2::buffer<uint64_t> l_tx_lanes_16_31 = i_tx_lanes_0_31 & 0xFFFF;

    FAPI_DBG("l_rx_lanes_0_15: %x l_rx_lanes_16_31: %x l_tx_lanes_0_15: %x l_tx_lanes_16_31: %x",
             l_rx_lanes_0_15, l_rx_lanes_16_31, l_tx_lanes_0_15, l_tx_lanes_16_31);
    o_done = 1;

    if (l_rx_lanes_0_15 || l_tx_lanes_0_15)
    {
        FAPI_TRY(FW_REG_EXT_CMD_DONE_00_15_GET(iv_regs, i_target, i_thread, l_data));
        FAPI_DBG("io_ppe_common::check_ext_cmd_req_done_thread 00_15 done: %llx exp: %llx\n", l_data, i_ext_cmd_req);
        o_done = ((l_data & i_ext_cmd_req) == i_ext_cmd_req);
    }

    if (l_rx_lanes_16_31 || l_tx_lanes_16_31)
    {
        FAPI_TRY(FW_REG_EXT_CMD_DONE_16_31_GET(iv_regs, i_target, i_thread, l_data));
        FAPI_DBG("io_ppe_common::check_ext_cmd_req_done_thread 16_31 done: %llx exp: %llx\n", l_data, i_ext_cmd_req);
        o_done = o_done && ((l_data & i_ext_cmd_req) == i_ext_cmd_req);
    }

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}

template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::check_ext_cmd_req_done(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_num_threads,
        const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
        const fapi2::buffer<uint64_t> i_tx_lanes_0_31,
        const fapi2::buffer<uint64_t> i_ext_cmd_req,
        fapi2::buffer<uint8_t>& o_done)
{
    FAPI_DBG("Start");

    fapi2::buffer<uint8_t> l_done = 0;

    o_done = 1;

    for (fapi2::buffer<uint64_t> l_thread = 0; l_thread < i_num_threads; l_thread++)
    {
        FAPI_TRY(check_ext_cmd_req_done_thread(i_target, l_thread, i_rx_lanes_0_31, i_tx_lanes_0_31, i_ext_cmd_req, l_done));
        o_done = (o_done && l_done);
    }

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}


// ===============================================================
// ENABLE_LANES
// ===============================================================
static const fapi2::buffer<uint64_t> ENABLE_LANES_CMD = POWER_ON_PL | TX_FIFO_INIT_PL;

template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::enable_lanes(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_num_threads,
        const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
        const fapi2::buffer<uint64_t> i_tx_lanes_0_31)
{
    FAPI_DBG("Start");

    FAPI_TRY(issue_ext_cmd_req(i_target, i_num_threads, i_rx_lanes_0_31, i_tx_lanes_0_31, ENABLE_LANES_CMD));

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}


template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::check_enable_lanes_done(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_num_threads,
        const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
        const fapi2::buffer<uint64_t> i_tx_lanes_0_31,
        fapi2::buffer<uint8_t>& o_done)
{
    FAPI_DBG("Start");

    FAPI_TRY(check_ext_cmd_req_done(i_target, i_num_threads, i_rx_lanes_0_31, i_tx_lanes_0_31, ENABLE_LANES_CMD, o_done));

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}

// ===============================================================
// phy_dccal_start
// ===============================================================
static const fapi2::buffer<uint64_t> INIT_START_CMD = HW_REG_INIT_PG |
        DCCAL_PL |
        TX_ZCAL_PL |
        TX_FFE_PL |
        POWER_OFF_PL;

template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::dccal_start(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_num_threads,
        const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
        const fapi2::buffer<uint64_t> i_tx_lanes_0_31,
        const uint8_t i_enable_lanes)
{
    FAPI_DBG("Start");

    fapi2::buffer<uint64_t> l_cmd = INIT_START_CMD;

    if (i_enable_lanes)
    {
        l_cmd |= ENABLE_LANES_CMD;
    }

    FAPI_TRY(issue_ext_cmd_req(i_target, i_num_threads, i_rx_lanes_0_31, i_tx_lanes_0_31, l_cmd));

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}


template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::dccal_start_done(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_num_threads,
        const fapi2::buffer<uint64_t> i_rx_lanes_0_31,
        const fapi2::buffer<uint64_t> i_tx_lanes_0_31,
        const uint8_t i_enable_lanes,
        fapi2::buffer<uint8_t>& o_done)
{
    FAPI_DBG("Start");

    fapi2::buffer<uint64_t> l_cmd = INIT_START_CMD;

    if (i_enable_lanes)
    {
        l_cmd |= ENABLE_LANES_CMD;
    }

    FAPI_TRY(check_ext_cmd_req_done(i_target, i_num_threads, i_rx_lanes_0_31, i_tx_lanes_0_31, l_cmd, o_done));

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}

// ===============================================================
// FAST MODE (SIM ONLY)
// ===============================================================
template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::fast_mode_thread(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_thread)
{
    FAPI_DBG("Start");

    fapi2::buffer<uint64_t> l_data = 1;
    FAPI_TRY(MEM_REG_LOFF_SETTING_OVR_ENB_PUT(iv_regs, i_target, i_thread, l_data));
    FAPI_TRY(MEM_REG_AMP_SETTING_OVR_ENB_PUT(iv_regs, i_target, i_thread, l_data));
    l_data = 0;
#ifndef IOT_IMAGE
    FAPI_TRY(MEM_REG_RX_EO_CONVERGED_END_COUNT_PUT(iv_regs, i_target, i_thread, l_data));
    FAPI_TRY(MEM_REG_RX_MIN_RECAL_CNT_PUT(iv_regs, i_target, i_thread, l_data));
#endif
    l_data = 4;
    FAPI_TRY(MEM_REG_TX_DCC_MAIN_MIN_SAMPLES_PUT(iv_regs, i_target, i_thread, l_data));

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}

template<fapi2::TargetType K, fapi2::MulticastType M, typename V>
fapi2::ReturnCode io_ppe_common<K, M, V>::fast_mode(const fapi2::Target<K, M, V>& i_target,
        const fapi2::buffer<uint64_t> i_num_threads)
{
    FAPI_DBG("Start");

    for (fapi2::buffer<uint64_t> l_thread; l_thread < i_num_threads; l_thread++)
    {
        FAPI_TRY(fast_mode_thread(i_target, l_thread));
    }

fapi_try_exit:
    FAPI_DBG("End");
    return fapi2::current_err;
}
