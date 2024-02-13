/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_fastarray.C $   */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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
/// @file  poz_fastarray.C
///
/// @brief SBE part of fast array dump: Sets up the ABIST engines, clocks ABIST
///        cycles and dumps the interesting bits of the FARR ring back to the
///        service processor.
///        The information about which bits are interesting during which ABIST
///        cycle is supplied by the caller.
//------------------------------------------------------------------------------
// *HWP HW Owner        : Joachim Fenkes <fenkes@de.ibm.com>
// *HWP HW Backup Owner : Joe McGill <jmcgill@us.ibm.com>
// *HWP FW Owner        : Raja Das <rajadas2@in.ibm.com>
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------------
#include <poz_fastarray.H>
#include <poz_scan_via_scom.H>
#include <poz_scom_perv_tpchip.H>

SCOMT_PERV_USE_OPCG_REG0;
SCOMT_PERV_USE_BIST;
SCOMT_PERV_USE_CLK_REGION;
SCOMT_PERV_USE_CPLT_CTRL0;
SCOMT_PERV_USE_CPLT_CONF0;
SCOMT_PERV_USE_CPLT_STAT0;

using namespace fapi2;
using namespace scomt::poz;

enum fastarray_constants
{
    SCAN_TYPES_ALL_BUT_RTG  = 0x000000000000DCF0,
    REGION_VITL             = 0x1000000000000000,
    SCAN_HEADER             = 0xA5A55A5A00000000,
    RUN_ABIST_DELAY_NS      = 10000,
    RUN_ABIST_DELAY_SIM     = 50000,
    RUN_ABIST_TIMEOUT       = 100,
    CLEANUP_ABIST_CYCLES    = 0x1000,
    CLEANUP_ABIST_TIMEOUT   = 16,
    SETUP_IMAGE_NAME_MAXLEN = 32,
    MAX_CARE_WORDS          = 100,
    NUM_FLUSH_CYCLES        = 16,  // 4 should be plenty but let's go for broke
    FA_TAG_HEADER           = 0x4E,
    FA_TAG_FAD_VERSION      = 0xFA,
    FA_TAG_SCAN_IMAGE       = 0x5C,
    FA_TAG_SCAN_ZERO        = 0x50,
    FA_TAG_CAREDATA         = 0xCA,
    FA_FLAG_RUNN_MODE       = 0x000001,
    FA_FLAG_SET_SDIS        = 0x000002,
    FA_FLAG_COMPLETE_ABIST  = 0x000004,
    FA_FORMAT_VERSION       = 6,
    OPCG_CAPT_SL_NSL_ARY    = 0x1C,
    OPCG_CAPT_SL_NSL        = 0x18,
    ARY_PIPE_MAX_LEN        = 10,
};

/*
 * Input stream format description:
 *
 * The input stream is a tag-length-value (TLV) stream as described in the header file.
 *
 * The FA_TAG_HEADER tag is optional but will usually be included. If it is not provided
 * a set of default parameters will be used - see the fastarray_header constructor.
 *
 * FA_TAG_SCAN_IMAGE and FA_TAG_SCAN_ZERO are also optional and will instruct the HWP to
 * apply either a named scan image or to scan0 a region. They will be applied in the order
 * in which they are encountered in the input stream.
 *
 * FA_TAG_CAREDATA is mandatory and starts the actual fastarray dump operation.
 * The first 32-bit word in the care data stream is the ring address of the ring to be dumped,
 * in the format we all know and hate.
 *
 * The rest of the input stream consists of (ncycles, nwords, stepsize, carebits) tuples:
 *   The first word (the "header") contains ncycles, nwords and stepsize.
 *     ncycles: The amount of array rows to dump out
 *     stepsize: The number of cycles to clock for each of those rows
 *     nwords: The size of the carebits data that follows, in words
 *   The header is followed by the carebits information.
 *
 * The carebits block can be decoded by hwp_bit_istream and consists of a series of variable width integers,
 * terminated by a zero value. The values represent alternating run lengths of don't care and care bits,
 * starting with don't care.
 *
 * For each tuple, perform the step/dump process ncycles times. If nwords is zero, skip that many cycles.
 * If ncycles is zero, the input stream is finished and no further dump data is required.
 *
 *
 * Output stream format description:
 *
 * The output stream optionally starts with a FA_TAG_FAD_VERSION TLV to indicate the fad file and version used
 * to generate the dump. This information is copied from the control file if it exists.
 *
 * The output stream format is identical to the input stream format minus the initial ring address word, except
 * after each tuple the read data is appended in a separate byte_ostream.
 *
 * For each dumped row of care bits, the care bits according to the care data are appended to the stream in
 * ring order (resulting in exactly as many bits as we care about to be appended). At the end of a tuple, zero
 * padding bits are appended to bring the stream to a word boundary before the next tuple header starts.
 *
 * This way, the output stream contains all necessary information about its own composition, and the offline
 * post processor can decode it without knowledge of the SBE image that produced it.
 */

static ReturnCode load64(hwp_data_istream& i_data, uint64_t& o_field)
{
    uint32_t l_hi, l_lo;
    FAPI_TRY(i_data.get(l_hi));
    FAPI_TRY(i_data.get(l_lo));
    o_field = ((uint64_t)l_hi << 32) | l_lo;
fapi_try_exit:
    return current_err;
}

/**
 * @brief Header for fastarray control data, contains format version, flags etc.
 */
struct fastarray_header
{
    uint32_t version;
    uint32_t flags;
    uint64_t opcg_align_override, opcg_align_mask;
    uint32_t slow_clock_phases, fast_clock_phases;
    uint32_t bist_region_override;

    fastarray_header() : version(FA_FORMAT_VERSION), flags(FA_FLAG_SET_SDIS),
        // SNOP_ALIGN = 5
        // SNOP_WAIT  = 7
        opcg_align_override(0x0500000700000000ULL),
        opcg_align_mask(    0x0F000FFF00000000ULL),
        slow_clock_phases(0x80000000),
        fast_clock_phases(0x40000000) {};

    ReturnCode load(hwp_data_istream& i_data)
    {
        uint32_t l_value;
        FAPI_TRY(i_data.get(l_value));
        version = l_value >> 24;
        flags = l_value & 0xFFFFFF;

        if (version != FA_FORMAT_VERSION) // only load additional fields if version matches
        {
            goto fapi_try_exit;
        }

        FAPI_TRY(load64(i_data, opcg_align_override));
        FAPI_TRY(load64(i_data, opcg_align_mask));

        FAPI_TRY(i_data.get(l_value));
        slow_clock_phases = l_value & 0xFFFF0000;
        fast_clock_phases = l_value << 16;

        FAPI_TRY(i_data.get(l_value));
        bist_region_override = l_value & 0xFFFF0000;

    fapi_try_exit:
        return current_err;
    }
};

#define FA_HEADER_VERSION(x) ((x).version_flags >> 24)
#define FA_HEADER_FLAGS(x)   ((x).version_flags & 0x00FFFFFF)

/**
 * @brief Saved values of registers, to be restored at the end of the dump
 */
struct reg_save
{
    uint64_t opcg_align;
};

/**
 * @brief Set up an OPCG_CAPTx register
 * @param[in] i_target_chiplet   Target chiplet
 * @param[in] i_addr             The address of the OPCG_CAPTx register
 * @param[in] i_seq_len          Sequence length field value
 * @param[in] i_capt_value       Cycle settings
 * @param[in] i_clock_phases     Cycle positions at which to insert the settings
 */
static ReturnCode setup_opcg_capt(
    const Target<TARGET_TYPE_PERV>& i_target_chiplet,
    const uint64_t i_addr,
    const uint8_t i_seq_len,
    const uint8_t i_capt_value,
    const uint32_t i_clock_phases)
{
    buffer<uint64_t> l_buf = 0;
    l_buf.insertFromRight<OPCG_CAPT1_COUNT, OPCG_CAPT1_COUNT_LEN>(i_seq_len);

    for (int i = 0; i < 12; i++)
    {
        if (i_clock_phases & (0x80000000 >> i))
        {
            FAPI_TRY(l_buf.insertFromRight(i_capt_value,
                                           OPCG_CAPT1_SEQ_01 + OPCG_CAPT1_SEQ_01_LEN * i,
                                           OPCG_CAPT1_SEQ_01_LEN));
        }
    }

    FAPI_TRY(putScom(i_target_chiplet, i_addr, l_buf));

fapi_try_exit:
    return current_err;
}

static ReturnCode setup_opcg_sequence(
    const Target<TARGET_TYPE_PERV>& i_target_chiplet,
    const uint8_t i_capt_value,
    const fastarray_header& i_options)
{
    /* calculate OPCG_CAPT sequence length */
    unsigned int l_seq_len = 0;

    for (int i = 0; i < 12; i++)
    {
        if ((i_options.slow_clock_phases & (0x80000000 >> i)) or (i_options.fast_clock_phases & (0xC0000000 >> (i * 2))))
        {
            l_seq_len = i + 1;
        }
    }

    /* set up all OPCG_CAPT regs */
    FAPI_TRY(setup_opcg_capt(i_target_chiplet, OPCG_CAPT1, l_seq_len,
                             i_capt_value, i_options.slow_clock_phases),
             "Failed to set up OPCG_CAPT1");
    FAPI_TRY(setup_opcg_capt(i_target_chiplet, OPCG_CAPT2, 0, i_capt_value,
                             i_options.fast_clock_phases & 0xFFF00000),
             "Failed to set up OPCG_CAPT2");
    FAPI_TRY(setup_opcg_capt(i_target_chiplet, OPCG_CAPT3, 0, i_capt_value,
                             i_options.fast_clock_phases << 12),
             "Failed to set up OPCG_CAPT3");

fapi_try_exit:
    return current_err;
}

/**
 * @brief Set up a chiplet for fast array dump
 * @param[in] i_target_chiplet The chiplet to prepare
 * @param[in] i_clock_regions  The clock regions to set up,
 *                             formatted like bits 4:15 of SCAN_REGION_TYPE
 * @param[in] i_setup_flush    If true, set up for initial flush cycles, where ARY clocks are
 *                             excluded and the ABIST engines are not being started.
 *                             If false, set up for the real thing.
 * @return FAPI2_RC_SUCCESS if success, else error code.
 */
static ReturnCode setup(
    const Target<TARGET_TYPE_PERV>& i_target_chiplet,
    const uint64_t i_clock_regions,
    const uint64_t i_bist_regions,
    const fastarray_header& i_options,
    reg_save* o_reg_save,
    const bool i_setup_flush)
{
    // Five-bit value for OPCG_CAPT registers. Bits are SL, NSL, ARY, SE, FCE in that order
    // For flush cycles, we set up SL+NSL, for actual dumps SL+NSL+ARY
    const uint8_t l_capt_value = i_setup_flush ? OPCG_CAPT_SL_NSL : OPCG_CAPT_SL_NSL_ARY;
    buffer<uint64_t> l_buf;

    /* Switch dual-clocked arrays to the local ABIST engine */
    {
        CPLT_CTRL0_t l_cplt_ctrl0 = 0;
        l_cplt_ctrl0.set_ABSTCLK_MUXSEL(1);
        FAPI_TRY(l_cplt_ctrl0.putScom_SET(i_target_chiplet), "Failed to set ABSTCLK_MUXSEL_DC");
    }

    /* Set up clock controller to do single BIST pulses */
    FAPI_TRY(getScom(i_target_chiplet, OPCG_ALIGN, l_buf), "Failed to read OPCG_ALIGN register");

    if (o_reg_save != nullptr)
    {
        o_reg_save->opcg_align = l_buf;
    }

    l_buf = (l_buf & ~i_options.opcg_align_mask) | i_options.opcg_align_override;
    FAPI_TRY(putScom(i_target_chiplet, OPCG_ALIGN, l_buf), "Failed to update OPCG_ALIGN register");

    {
        CLK_REGION_t l_clk_region = i_clock_regions;
        l_clk_region.set_SEL_THOLD_SL(1);
        l_clk_region.set_SEL_THOLD_NSL(1);
        l_clk_region.set_SEL_THOLD_ARY(!i_setup_flush);
        FAPI_TRY(l_clk_region.putScom(i_target_chiplet), "Failed to set up clock regions");
    }

    {
        BIST_t l_bist = i_bist_regions;
        l_bist.set_TC_SRAM_ABIST_MODE_DC(1);
        l_bist.set_TC_BIST_START_TEST_DC(!i_setup_flush);
        FAPI_TRY(l_bist.putScom(i_target_chiplet), "Failed to set up BIST register");
    }

    l_buf.flush<0>();
    FAPI_TRY(putScom(i_target_chiplet, OPCG_REG1, l_buf), "Failed to clear OPCG_REG1");
    FAPI_TRY(putScom(i_target_chiplet, OPCG_REG2, l_buf), "Failed to clear OPCG_REG2");

    FAPI_TRY(setup_opcg_sequence(i_target_chiplet, l_capt_value, i_options));

    if (i_options.flags & FA_FLAG_SET_SDIS)
    {
        /* Assert SDIS_N, some arrays don't dump right if we don't set this */
        CPLT_CONF0_t l_cplt_conf0 = 0;
        l_cplt_conf0.set_SDIS_N(1);
        FAPI_TRY(l_cplt_conf0.putScom_SET(i_target_chiplet), "Failed to set SDIS_N");
    }

    return FAPI2_RC_SUCCESS;

fapi_try_exit:
    return current_err;
}

static ReturnCode wait_opcg_done(
    const Target<TARGET_TYPE_PERV>& i_target_chiplet)
{
    uint32_t l_timeout = RUN_ABIST_TIMEOUT;

    while (--l_timeout)
    {
        CPLT_STAT0_t l_cplt_stat;
        FAPI_TRY(l_cplt_stat.getScom(i_target_chiplet), "Failed to read Chiplet Status 0 Register");

        if (l_cplt_stat.get_OPCG_DONE())
        {
            break;
        }

        delay(RUN_ABIST_DELAY_NS, RUN_ABIST_DELAY_SIM);
    }

    FAPI_ASSERT(l_timeout, FASTARRAY_CLOCK_TIMEOUT().set_TARGET(i_target_chiplet),
                "Timed out waiting for OPCG done");

fapi_try_exit:
    return current_err;
}

/**
 * @brief Run a number of ABIST cycles
 * @param[in] i_target_chiplet The chiplet target on where we need to run cycles
 * @param[in] i_clock_cycles   Number of cycles to run
 * @return FAPI2_RC_SUCCESS if success, else error code.
 */
static ReturnCode run_abist_cycles(
    const Target<TARGET_TYPE_PERV>& i_target_chiplet,
    const uint32_t i_clock_cycles,
    const fastarray_header& i_options)
{
    OPCG_REG0_t l_opcg_reg0 = 0;

    l_opcg_reg0.set_OPCG_GO(1);
    l_opcg_reg0.set_RUNN_MODE(i_options.flags & FA_FLAG_RUNN_MODE);
    l_opcg_reg0.set_LOOP_COUNT(i_clock_cycles - 1);
    FAPI_TRY(l_opcg_reg0.putScom(i_target_chiplet), "Failed to clock %d array cycle(s)",
             i_clock_cycles);

    /* If we clocked more than a single cycle, do due diligence and wait for OPCG_DONE */
    if( i_clock_cycles > 1 )
    {
        FAPI_TRY(wait_opcg_done(i_target_chiplet));
    }

    return FAPI2_RC_SUCCESS;

fapi_try_exit:
    return current_err;
}

/**
 * @brief Clean up a chiplet after a fast array dump
 * @param[in] i_target_chiplet The chiplet to clean up
 * @param[in] i_clock_regions  The clock regions that were previously enabled in fastarray_setup
 * @return FAPI2_RC_SUCCESS if success, else error code.
 */
static ReturnCode cleanup(
    const Target<TARGET_TYPE_PERV>& i_target_chiplet,
    const uint64_t i_clock_regions,
    const fastarray_header& i_options,
    const reg_save& i_reg_save,
    bool i_ignore_abist_done)
{
    buffer<uint64_t> l_buf;

    /* Let ABIST engines run to completion */
    if (i_options.flags & FA_FLAG_COMPLETE_ABIST)
    {
        CPLT_STAT0_t l_cplt_stat;
        uint32_t l_timeout = CLEANUP_ABIST_TIMEOUT;

        do
        {
            FAPI_TRY(run_abist_cycles(i_target_chiplet, CLEANUP_ABIST_CYCLES, i_options),
                     "Failed to clock ABIST to completion");
            FAPI_TRY(l_cplt_stat.getScom(i_target_chiplet), "Failed to read Chiplet Status 0 Register");
        }
        while (--l_timeout && !l_cplt_stat.get_ABIST_DONE());

        FAPI_ASSERT_NOEXIT(i_ignore_abist_done || l_timeout != 0,
                           FASTARRAY_CLEANUP_NOT_DONE().set_TARGET(i_target_chiplet),
                           "Warning: ABIST_DONE not seen! Your dump is probably fine, but this is unexpected.");
    }

    /* Disable ABIST and clock engines so they can cleanly reset */
    l_buf = 0;
    FAPI_TRY(putScom(i_target_chiplet, BIST,       l_buf), "Failed to clear BIST reg");
    FAPI_TRY(run_abist_cycles(i_target_chiplet, 0xFF, i_options), "Failed to clock ABIST engine reset");

    /* Clean up the clock controller */
    FAPI_TRY(putScom(i_target_chiplet, CLK_REGION, l_buf), "Failed to clear clock regions");
    FAPI_TRY(putScom(i_target_chiplet, OPCG_CAPT1, l_buf), "Failed to clear OPCG_CAPT1");
    FAPI_TRY(putScom(i_target_chiplet, OPCG_CAPT2, l_buf), "Failed to clear OPCG_CAPT2");
    FAPI_TRY(putScom(i_target_chiplet, OPCG_CAPT3, l_buf), "Failed to clear OPCG_CAPT3");

    /* Restore OPCG_ALIGN */
    FAPI_TRY(putScom(i_target_chiplet, OPCG_ALIGN, i_reg_save.opcg_align), "Failed to restore OPCG_ALIGN");

    /* Clear SDIS_N */
    {
        CPLT_CONF0_t l_cplt_conf0 = 0;
        l_cplt_conf0.set_SDIS_N(1);
        FAPI_TRY(l_cplt_conf0.putScom_CLEAR(i_target_chiplet), "Failed to clear SDIS_N");
    }

    /* Switch dual-clocked arrays back */
    {
        CPLT_CTRL0_t l_cplt_ctrl0 = 0;
        l_cplt_ctrl0.set_ABSTCLK_MUXSEL(1);
        FAPI_TRY(l_cplt_ctrl0.putScom_CLEAR(i_target_chiplet), "Failed to clear ABSTCLK_MUXSEL_DC");
    }

    return FAPI2_RC_SUCCESS;

fapi_try_exit:
    return current_err;
}

static ReturnCode do_dump(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_CORE > & i_target,
    const Target < TARGET_TYPE_PERV >& i_perv_target,
    hwp_data_istream&  i_instructions,
    hwp_data_ostream&  o_dump_data,
    const fastarray_header& i_options)
{
    uint32_t l_header, l_care_bits[MAX_CARE_WORDS];
    bool l_ignore_abist_done = i_options.bist_region_override;
    reg_save l_reg_save;

    // Open new scope so we can FAPI_TRY() across initializers at leisure
    {
        // Load the ring address from the stream and translate it
        FAPI_TRY(i_instructions.get(l_header));
        const uint64_t l_scan_region_type = svs::expand_ring_address(i_target, l_header);
        const uint64_t l_clock_region = l_scan_region_type & 0xFFFFFFFF00000000;
        const uint64_t l_abist_region = i_options.bist_region_override ? (uint64_t)i_options.bist_region_override << 28 :
                                        l_clock_region;

        FAPI_INF("Preparing for dump");
        // At this point there may still be pending write pulses stuck inside the array-internal
        // pipelines. Enabling ABIST mode will make sure no new writes enter the pipelines, but
        // any existing writes must be flushed.
        // At the lowest level the actual array writes pass through ARY clocked latches, so we
        // can drain those pipelines by clocking SL+NSL but not ARY. Thus, before we start dumping,
        // run some cycles without starting the ABIST engine and without enabling ARY clocks.
        FAPI_TRY(setup(i_perv_target, l_clock_region, l_abist_region, i_options, &l_reg_save, true));
        FAPI_TRY(run_abist_cycles(i_perv_target, NUM_FLUSH_CYCLES, i_options));

        // Now we can set up for the real thing, turn on the ABIST engines and enable ARY.
        FAPI_TRY(setup(i_perv_target, l_clock_region, l_abist_region, i_options, nullptr, false));

        uint32_t l_row = 0;

        // We have to run 1 extra cycle on the first skip operation to compensate an off-by-one bug
        // in the data preparation step. TODO: Fix this end to end.
        uint32_t l_start_extra = 1;

        while (true)
        {
            FAPI_INF("Progress: Row %d", l_row);

            // Get the next tuple header
            FAPI_TRY(i_instructions.get(l_header));
            const uint32_t l_ncycles = l_header >> 16, l_nwords = l_header & 0x03FF, l_stepsize = ((l_header >> 10) & 0x3F) + 1;
            l_row += l_ncycles * l_stepsize;

            // Copy the header to the output stream
            FAPI_TRY(o_dump_data.put(l_header));

            // No cycles -> we're done!
            if (!l_ncycles)
            {
                break;
            }

            // Cycles but no data -> skip cycles
            if (!l_nwords)
            {
                uint32_t l_skip_cycles = l_ncycles + l_start_extra;
                l_start_extra = 0;

                // If we skip a large amount of cycles, skip the majority with ARY clocks turned off
                // to work around potential ABIST bugs where arrays are accidentally written during
                // an outer loop switch.
                if (l_skip_cycles > ARY_PIPE_MAX_LEN)
                {
                    FAPI_TRY(setup_opcg_sequence(i_perv_target, OPCG_CAPT_SL_NSL, i_options));
                    FAPI_TRY(run_abist_cycles(i_perv_target, (l_skip_cycles - ARY_PIPE_MAX_LEN) * l_stepsize, i_options));
                    FAPI_TRY(setup_opcg_sequence(i_perv_target, OPCG_CAPT_SL_NSL_ARY, i_options));

                    // Do run the last few cycles with ARY back on to account for potential
                    // pipelining inside the arrays; the final clock cycle must yield data
                    // as if nothing ever changed.
                    l_skip_cycles = ARY_PIPE_MAX_LEN;
                }

                FAPI_TRY(run_abist_cycles(i_perv_target, l_skip_cycles * l_stepsize, i_options));
                continue;
            }

            // Copy the care data into a local buffer (for reuse) and into the output stream
            FAPI_ASSERT(l_nwords <= ARRAY_SIZE(l_care_bits),
                        FASTARRAY_CARE_BUFFER_OVERFLOW()
                        .set_BUFFER_SIZE_WORDS(ARRAY_SIZE(l_care_bits))
                        .set_DATA_SIZE_WORDS(l_nwords),
                        "Too many care data words for internal buffer");

            for (uint32_t i = 0; i < l_nwords; i++)
            {
                uint32_t l_data;
                FAPI_TRY(i_instructions.get(l_data));
                FAPI_TRY(o_dump_data.put(l_data));
                l_care_bits[i] = l_data;
            }

            // Dumped bits go into a bit stream
            hwp_bit_ostream l_dumped_bits(o_dump_data);

            for (uint32_t i = 0; i < l_ncycles; i++)
            {
                if (i != 0 && (i & 7) == 0)
                {
                    FAPI_INF("Progress: Row %d", l_row - (l_ncycles - i) * l_stepsize);
                }

                // Every cycle, reuse the care data we've been sent
                hwp_array_istream l_care_array(l_care_bits, l_nwords);
                hwp_bit_istream l_care_stream(l_care_array);

                FAPI_TRY(svs::sparse_getring(i_perv_target, l_scan_region_type, l_care_stream, l_dumped_bits));
                FAPI_TRY(run_abist_cycles(i_perv_target, l_stepsize, i_options));
            }

            FAPI_TRY(l_dumped_bits.flush());
        }

        FAPI_INF("Dump complete, cleaning up");
        FAPI_TRY(cleanup(i_perv_target, l_clock_region, i_options, l_reg_save, l_ignore_abist_done));
    }

fapi_try_exit:
    return current_err;
}

/**
 * @brief Scan0 all scan types in the selected region(s) except GPTR, TIME and REPR
 * @param[in] i_target_chiplet The target chiplet
 * @param[in] i_ring_address   Ring address of the fastarray ring; most scan types for its
 *                             clock region will be cleared.
 * @return FAPI2_RC_SUCCESS if success, else error code.
 */
static ReturnCode scan0(
    const Target<TARGET_TYPE_PERV>& i_target_chiplet,
    const uint32_t i_ring_address)
{
    const uint64_t l_scan_region_type = svs::expand_ring_address(i_target_chiplet, i_ring_address);
    const uint64_t l_clock_region = l_scan_region_type & 0xFFFFFFFF00000000;

    FAPI_TRY(putScom(i_target_chiplet, SCAN_REGION_TYPE, l_clock_region | SCAN_TYPES_ALL_BUT_RTG));
    FAPI_TRY(putScom(i_target_chiplet, CLK_REGION, 0));
    FAPI_TRY(putScom(i_target_chiplet, OPCG_REG0, 0x2000000000000000));
    FAPI_TRY(wait_opcg_done(i_target_chiplet));
    FAPI_TRY(putScom(i_target_chiplet, SCAN_REGION_TYPE, 0));

fapi_try_exit:
    return current_err;
}

ReturnCode poz_fastarray_get_control_info(
    const uint32_t* i_control_data,
    const uint32_t i_control_data_nwords,
    poz_fastarray_control_info& o_control_info)
{
    const uint32_t* l_ptr = i_control_data;
    const uint32_t* const l_limit = i_control_data + i_control_data_nwords;

    while (l_ptr < l_limit)
    {
        const uint32_t l_value = *l_ptr;

        if (poz_fastarray_chunk_tag(l_value) == POZ_FASTARRAY_TAG_CONTROL_INFO)
        {
            o_control_info = *reinterpret_cast<const poz_fastarray_control_info*>(l_ptr + 1);
            return FAPI2_RC_SUCCESS;
        }

        l_ptr += poz_fastarray_chunk_length(l_value) + 1;
    }

    return FAPI2_RC_FALSE;
}

ReturnCode poz_fastarray(
    const Target < TARGET_TYPE_PERV | TARGET_TYPE_CORE > & i_target,
    hwp_data_istream&  i_instructions,
    hwp_data_ostream&  o_dump_data)
{
    fastarray_header l_header;
    const auto l_perv_target = i_target.getParent<TARGET_TYPE_PERV>();
    char l_targetStr[fapi2::MAX_ECMD_STRING_LEN];
    fapi2::toString(i_target, l_targetStr, fapi2::MAX_ECMD_STRING_LEN);
    FAPI_INF("Entering poz_fastarray, i_target %s", l_targetStr);

    while (true)
    {
        // Read a single TLV header
        uint32_t l_tlv;
        FAPI_TRY(i_instructions.get(l_tlv));

        const uint8_t l_tag = l_tlv >> 24;
        const uint32_t l_length = l_tlv & 0x00FFFFFF;

        if (l_tag == FA_TAG_HEADER)
        {
            FAPI_INF("Header block found, importing");
            FAPI_TRY(l_header.load(i_instructions));
            FAPI_ASSERT(l_header.version == FA_FORMAT_VERSION,
                        FASTARRAY_VERSION_MISMATCH()
                        .set_HEADER_VERSION(l_header.version).set_HWP_VERSION(FA_FORMAT_VERSION),
                        "Control data version mismatch: got/exp = %d/%d",
                        l_header.version, FA_FORMAT_VERSION);
        }
        else if (l_tag == FA_TAG_FAD_VERSION)
        {
            FAPI_INF("FAD version block found, copying to output");
            FAPI_TRY(o_dump_data.put(l_tlv));

            for (uint32_t i = 0; i < l_length; i++)
            {
                uint32_t l_word;
                FAPI_TRY(i_instructions.get(l_word));
                FAPI_TRY(o_dump_data.put(l_word));
            }
        }
        else if (l_tag == FA_TAG_SCAN_IMAGE)
        {
            // Scan in setup ring image as directed by the control data
            union
            {
                char name[SETUP_IMAGE_NAME_MAXLEN];
                uint32_t words[SETUP_IMAGE_NAME_MAXLEN >> 2];
            } l_image_name;

            FAPI_ASSERT(l_length <= ARRAY_SIZE(l_image_name.words),
                        FASTARRAY_SCAN_ENTRY_OVERSIZE()
                        .set_LENGTH(l_length).set_MAX_LENGTH(ARRAY_SIZE(l_image_name.words)),
                        "Scan image name is longer (%d words) than maximum (%d words)",
                        l_length, ARRAY_SIZE(l_image_name.words));

            for (uint32_t i = 0; i < l_length; i++)
            {
                uint32_t input_word;
                FAPI_TRY(i_instructions.get(input_word));
                l_image_name.words[i] = htobe32(input_word);
            }

            FAPI_INF("Scanning in setup bits");
            FAPI_TRY(putRing(i_target, l_image_name.name));
        }
        else if (l_tag == FA_TAG_SCAN_ZERO)
        {
            uint32_t l_scanAddr;
            FAPI_TRY(i_instructions.get(l_scanAddr));
            FAPI_INF("Scan-zeroing region 0x%08x", l_scanAddr);
            FAPI_TRY(scan0(l_perv_target, l_scanAddr));
        }
        else if (l_tag == FA_TAG_CAREDATA)
        {
            FAPI_TRY(do_dump(i_target, l_perv_target, i_instructions, o_dump_data, l_header));
            break; // after the actual dump we're done and don't expect more TLV blocks
        }
        else
        {
            FAPI_DBG("Unknown tag 0x%02X - skipping", l_tag);

            for (uint32_t i = 0; i < l_length; i++)
            {
                uint32_t l_bitbucket;
                FAPI_TRY(i_instructions.get(l_bitbucket));
            }
        }
    }

fapi_try_exit:
    FAPI_INF("Exiting poz_fastarray, i_target %s", l_targetStr);
    return current_err;
}
