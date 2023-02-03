/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/poz_cmdtable_interpreter.C $ */
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
/// @file  poz_cmdtable_interpreter.C
/// @brief Interpreter for boot ROM command tables
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Joachim Fenkes <fenkes@de.ibm.com>
// *HWP FW Maintainer   : Kahn Evans <kahneva@us.ibm.com>
//------------------------------------------------------------------------------

#include <poz_cmdtable_interpreter.H>
#include <vector>
#include <endian.h>

using namespace fapi2;

enum POZ_CMDTABLE_INTERPRETER_Private_Constants
{
    FORMAT_MAGIC = 0x434d5461,    // 'CMTa'
    POLL_TIMEOUT = 100,
    SB_MSG  = 0x50009,
    NOP     = 0b000,
    CALL    = 0b001,
    RETURN  = 0b010,
    PUTSCOM = 0b011,
    TEST    = 0b100,
    POLL    = 0b101,
    CMPBEQ  = 0b110,
    CMPBNE  = 0b111,
};

/// @brief Helper class to execute a single command table
class Executor
{
    public:
        Executor(
            const char* i_type, const void* i_table,
            const size_t i_table_size, Executor* i_cust_executor) :
            iv_type(i_type), iv_table((uint32_t*)i_table),
            iv_table_size(i_table_size), iv_cust_executor(i_cust_executor)
        {}

        /// @brief Check the command table for consistency
        ReturnCode check();

        /// @brief Run the command table from i_start_address
        /// @return FAPI2_RC_FALSE if execution aborted, FAPI2_RC_SUCCESS on return, or another FAPI error
        ReturnCode run(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target, int i_start_address);

        /// @brief return true if the table is empty/nonexistent
        bool empty()
        {
            return iv_table == NULL;
        }

    private:
        const char* iv_type;
        const uint32_t* iv_table;
        const size_t iv_table_size;
        Executor* const iv_cust_executor;

        const uint32_t* iv_insn, *iv_small;
        const uint64_t* iv_big;
        uint32_t iv_ninsn, iv_nsmall, iv_nbig;
};

ReturnCode Executor::check()
{
    // ^ means XOR means "either one or the other"
    if ((iv_table == NULL) ^ (iv_table_size == 0))
    {
        FAPI_ERR("Inconsistent parameters, table pointer and size must be "
                 "either both null or both non-null: table=%p size=%d",
                 iv_table, iv_table_size);
        return FAPI2_RC_INVALID_PARAMETER;
    }

    if (iv_table == NULL)
    {
        return FAPI2_RC_SUCCESS;
    }

    if (iv_table_size < 8)
    {
        FAPI_ERR("Invalid %s table size (too small): %d",
                 iv_type, iv_table_size);
        return FAPI2_RC_INVALID_PARAMETER;
    }

    const uint32_t magic = be32toh(iv_table[0]);

    if (magic != FORMAT_MAGIC)
    {
        FAPI_ERR("Cannot recognize %s table file format, magic value mismatch: 0x%08X",
                 iv_type, magic);
        return FAPI2_RC_INVALID_PARAMETER;
    }

    const uint32_t header = be32toh(iv_table[1]);
    iv_ninsn = (header >> 16) + 1;
    iv_nsmall = ((header >> 9) & 0x7F) + 1;
    iv_nbig = (header & 0x1FF) + 1;
    FAPI_DBG("%s Command table: %d instructions, %d small values, %d big values",
             iv_type, iv_ninsn, iv_nsmall, iv_nbig);

    const size_t expect_size = 8 + 4 * iv_ninsn + 4 * iv_nsmall + 8 * iv_nbig;

    if (iv_table_size != expect_size)
    {
        FAPI_ERR("Incorrect %s command table size: expected %d bytes, got %d bytes",
                 iv_type, expect_size, iv_table_size);
        return FAPI2_RC_INVALID_PARAMETER;
    }

    iv_insn = iv_table + 2;
    iv_small = iv_insn + iv_ninsn;
    iv_big = (const uint64_t*)(iv_small + iv_nsmall);

    return FAPI2_RC_SUCCESS;
}

static const char* opcode_names[8] =
{
    "NOP",
    "CALL",
    "RETURN",
    "PUTSCOM",
    "TEST",
    "POLL",
    "CMPBEQ",
    "CMPBNE",
};

ReturnCode Executor::run(const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target, int i_start_address)
{
    fapi2::buffer<uint64_t> buf;
    uint32_t ip = i_start_address;

    while (true)
    {
        if (ip >= iv_ninsn)
        {
            FAPI_ERR("%s command table overrun", iv_type);
            return FAPI2_RC_FALSE;
        }

        // Decode instruction
        const uint32_t insn = be32toh(iv_insn[ip]);
        const uint32_t opcode = (insn >> 28),
                       _param = ((insn >> 22) & 0x3F),
                       _address = (insn >> 15) & 0x7F,
                       _mask = (insn >> 9) & 0x3F,
                       _data = insn & 0x1FF;

        FAPI_DBG("insn: %08X, opcode: %d, param idx: %d, addr idx: %d",
                 insn, opcode, _param, _address);
        FAPI_DBG("mask idx: %d, data idx: %d", _mask, _data);

        // Check value bounds
        if (opcode > ARRAY_SIZE(opcode_names))
        {
            FAPI_ERR("%s:%04d: Invalid opcode %d", iv_type, ip, opcode);
            return FAPI2_RC_FALSE;
        }

        if (_param > iv_nsmall || _address > iv_nsmall || _mask > iv_nbig || _data > iv_nbig)
        {
            FAPI_ERR("%s:%04d: Index exceeds table size! param=%d address=%d",
                     iv_type, ip, _param, _address);
            FAPI_ERR("nsmall=%d mask=%d data=%d nbig=%d",
                     iv_nsmall, _mask, _data, iv_nbig);
            return FAPI2_RC_FALSE;
        }

        // Load arguments from tables
        const uint32_t param = be32toh(iv_small[_param]),
                       address = be32toh(iv_small[_address]);
        const uint64_t mask = be64toh(iv_big[_mask]),
                       data = be64toh(iv_big[_data]);

        // Output instruction
        FAPI_DBG("%s:%04d|%-7s|%07X|",
                 iv_type, ip, opcode_names[opcode], param);
        FAPI_DBG("%08X|%016lX|%016lX|", address, mask, data);

        // Execute
        switch (opcode)
        {
            case NOP:
                break;

            case CALL:
                if (!iv_cust_executor)
                {
                    FAPI_ERR("%s:%d: CALL is not allowed from the custom table", iv_type, ip);
                    return FAPI2_RC_FALSE;
                }

                if (iv_cust_executor->empty())
                {
                    FAPI_ERR("%s:%d: CALL without a custom table", iv_type, ip);
                    return FAPI2_RC_FALSE;
                }

                FAPI_TRY(iv_cust_executor->run(i_target, param));
                break;

            case RETURN:
                return FAPI2_RC_SUCCESS;

            case PUTSCOM:
                if (mask == 0xFFFFFFFFFFFFFFFF)
                {
                    FAPI_TRY(putScom(i_target, address, data));
                }
                else
                {
                    FAPI_TRY(getScom(i_target, address, buf));
                    buf = (buf & ~mask) | data;
                    FAPI_TRY(putScom(i_target, address, buf));
                }

                break;

            case TEST:
            case POLL:
            case CMPBEQ:
            case CMPBNE:
                {
                    // TEST, POLL, CMP* all have the same basic structure:
                    // read, compare under mask, do something with the result.
                    // Thus we use the same code to handle them all.
                    int timeout = POLL_TIMEOUT;
                    bool match;

                    // The loop is only really used for POLL
                    do
                    {
                        FAPI_TRY(getScom(i_target, address, buf));
                        match = (buf & mask) == data;

                        switch (opcode)
                        {
                            case TEST:
                                if (!match)
                                {
                                    FAPI_ERR("%s:%d: TEST instruction failed: ",
                                             iv_type, ip);
                                    FAPI_ERR("got(masked)=0x%016X(0x%016X) expected=0x%016X",
                                             buf, buf & mask, data);
                                    FAPI_TRY(putScom(i_target, SB_MSG, param));
                                    return FAPI2_RC_FALSE;
                                }

                                break;

                            case POLL:

                                // We exit the loop below if we match, only check for timeout
                                // If param == 0, poll indefinitely
                                if (param)
                                {
                                    timeout--;
                                }

                                if (!timeout)
                                {
                                    FAPI_ERR("%s:%d: POLL instruction timed out", iv_type, ip);
                                    FAPI_TRY(putScom(i_target, SB_MSG, param));
                                    return FAPI2_RC_FALSE;
                                }

                                break;

                            case CMPBEQ:
                            case CMPBNE:

                                // ^ means XOR means "either one or the other"
                                if (match ^ (opcode == CMPBNE))
                                {
                                    // Note that the IP is always advanced by an additional 1
                                    // at the end of the execution loop, so even if param == 0
                                    // we still advance at least one instruction. The assembler
                                    // takes this into account when generating the param value.
                                    ip += param;
                                }

                                break;
                        }
                    }
                    while (opcode == POLL && !match);

                    break;
                }
        }

        // Advance to the next instruction
        ip++;
    }

fapi_try_exit:
    return current_err;
}

ReturnCode poz_cmdtable_interpreter(
    const Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const void* i_main_table, const size_t i_main_table_size,
    const void* i_cust_table, const size_t i_cust_table_size)
{
    Executor cust("cust", i_cust_table, i_cust_table_size, NULL);
    Executor main("main", i_main_table, i_main_table_size, &cust);

    FAPI_TRY(main.check(), "Malformed main command table");
    FAPI_TRY(cust.check(), "Malformed custom command table");

    FAPI_TRY(main.run(i_target, 0));

fapi_try_exit:
    return current_err;
}
