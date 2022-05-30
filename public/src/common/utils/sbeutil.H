/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/utils/sbeutil.H $                           */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2022                        */
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
#ifndef SBE_UTIL_H
#define SBE_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pk.h"
#include "pk_api.h"
#include "ppe42_scom.h"
#include "sha3.H"

#ifdef __cplusplus
}
#endif

#define MASK_ZERO_L32B_UINT64(x)    ((x) & 0xFFFFFFFF00000000ull)
#define MASK_ZERO_H32B_UINT64(x)    ((x) & 0x00000000FFFFFFFFull)
#define SHIFT_RIGHT(x, bits)        ((x) >> bits)
#define SHIFT_LEFT(x, bits)         ((x) << bits)

#define SBE_MESSAGE_REG_PROGRESS_CODE_BIT_MASK 0x3F00000000

// To handle unused variables compilation error
static inline void UNUSED(int dummy, ...) {}

// Magic instruction

/** @brief  This is a special assembler instruction that is a nop on
 *  regular hardware, but has special meaning to Simics.  Code that
 *  executes this instruction in Simics will cause a "hap," a
 *  Simics term.  If there is no hap handler registered, and magic
 *  breakpoints have not been enabled with
 *      simics> enable-magic-breakpoint
 *  then this instruction is also a nop in Simics.
 *
 *  If magic breakpoints are enabled, and there is no hap handler, then
 *  when SBE code executes this instruction in Simics, Simics will
 *  stop the simulation. (Prompt changes from running> to simics> )
 *
 *  If a hap is registered, then Simics will call the hap handler.  Hap
 *  handlers are written in Python, and the best place for them is
 *
 *     src/tools/debug//simics-debug-framework.py
 *
 *  Sample code to register the hap handler:
 *        # arg contains the integer parameter n passed to MAGIC_INSTRUCTION(n)
 *        def magic_instruction_callback(user_arg, cpu, arg):
 *            # print to console works...
 *            print "Hit magic instruction ", arg
 *            # Or else stop the simulation...
 *            SIM_break_simulation( "Stopped at magic instruction" )
 *
 *        # Register the magic instruction callback.
 *        SIM_hap_add_callback( "Core_Magic_Instruction",
 *                              magic_instruction_callback, None )
 *
 *        # Better to register the SBE range 8000-8190
 *        # so that PHYP and others won't be affected.
 *        SIM_hap_add_callback_range( "Core_Magic_Instruction",
 *                      magic_instruction_callback, None, 8000, 8190 )
 *
 *  The argument n is an integer from 0..8191 which Simics passes to the hap
 *  handler in parameter 3, or "arg" in the sample code above.
 */
__attribute__((always_inline))
inline void SBE_MAGIC_INSTRUCTION(int _n)
{
    register int n = _n;
    asm volatile("rlwimi %0,%0,0,%1,%2"       \
        :: "i" (((n) >> 8) & 0x1f),           \
        "i" (((n) >> 4) & 0xf),               \
        "i" ((((n) >> 0) & 0xf) | 16));
}

#define SBE_EXEC_HWP_NOARG(...) SBE_EXEC_HWP(__VA_ARGS__)
#define SBE_EXEC_HWP(fapiRc, hwp, ...) \
{ \
    fapiRc = hwp(__VA_ARGS__); \
}

namespace SBE
{
    // Nest to SBE frequency ratio
    static const uint8_t  SBE_TO_NEST_FREQ_FACTOR = 4;

    /* @brief - Check if the boot is on HRESET of SBE
     *
     * @return - True if HRESET, false otherwise
     */
     bool isHreset(void);


    // enum for magic instructions
    enum
    {
        MAGIC_SIMICS_CHECK = 8000, // check if code is running on simics
    };

    // Currently PK does not define start range for app
    // specifc panic code as enum. It is implicit understanding
    // through code comments. Expectation is 0x1cxx range is for
    // non-pk code.
    static const uint32_t PK_APP_OFFSET_SBE_START = 0x1c00;
    enum SH_LIST_RET_t
    {
        // For defining new panic codes refer to pk/ppe42/pk_panic_codes.h
        PANIC_ASSERT = PK_APP_OFFSET_SBE_START,
        PANIC_SCRATCH = PK_APP_OFFSET_SBE_START + 1,
    };

    /*@brief - checks if sbe is running on simics
     *
     * @return - True if sbe is running on simics , false otherwise
     */
    bool isSimicsRunning();

    typedef int HASH_LIST_RET_t;

    enum HASH_LIST_RETURN_CODES
    {
        HASH_COMPARE_PASS = 0x00,
        HASH_COMPARE_FAIL,
        FILE_NOT_FOUND,
    };

    /*@brief - checks if file calculated file hash returned during pak read
     *         matches with the one in hash list
     *
     * @param[in] - i_fname - name of the file with which hash has to be compared
     *
     * @param[in] - i_hash - Hash calculated during pak read
     *
     * @param[in] - i_hash_list - Hash list start offset
     *
     * @return - HASH_LIST_RET_t - If hash matches ARC_OPERATION_SUCCESSFUL else
     *                       respective error codes
     */
     HASH_LIST_RET_t check_file_hash(const char *i_fname, const sha3_t &i_hash, const uint8_t *i_hash_list);

    /*@brief - Copy memory from an unaligned address byte by byte into
     * destination address
     *
     * @param[in] - vdest - Destination Address
     *
     * @param[in] - vsrc - Source Address
     *
     * @param[in] - len - Length of the data to be copied from vsrc to vdest
     *
     * @return - None
     */
    void memcpy_byte(void* vdest, const void* vsrc, size_t len);

    /*@brief - Get higher 32bit number from uint64
     *
     * @param[in] - i_lWord - 64bit long word
     *
     * @return - uint32_t word
     */
    inline uint32_t higher32BWord(uint64_t i_lWord)
    {
        return (uint32_t)(SHIFT_RIGHT(MASK_ZERO_L32B_UINT64(i_lWord), 32));
    }

    /*@brief - Get lower 32bit number from uint64
     *
     * @param[in] - i_lWord - 64bit long word
     *
     * @return - uint32_t word
     */
    inline uint32_t lower32BWord(uint64_t i_lWord)
    {
        return (uint32_t)(MASK_ZERO_H32B_UINT64(i_lWord));
    }

    /*@brief - Read 8 Byte data from a unaligned address
     *
     * @param[in] - *src - Address
     *
     * @return - uint64_t data
     */
    inline uint64_t get64(void* src)
    {
        uint64_t dest;
        memcpy_byte(&dest, src,sizeof(uint64_t));
        return dest;
    }

    /*@brief - Read 4 Byte data from a unaligned address
     *
     * @param[in] - *src - Address
     *
     * @return - uint32_t data
     */
    inline uint32_t get32(void* src)
    {
        uint32_t dest;
        memcpy_byte(&dest, src,sizeof(uint32_t));
        return dest;
    }

    /*@brief - Read 2 Byte data from a unaligned address
     *
     * @param[in] - *src - Address
     *
     * @return - uint16_t data
     */
    inline uint16_t get16(void* src)
    {
        uint16_t dest;
        memcpy_byte(&dest, src,sizeof(uint16_t));
        return dest;
    }

    /*@brief - Read 1 Byte data from a address
     *
     * @param[in] - *src - Address
     *
     * @return - uint8_t data
     */
    inline uint8_t get8(void* src)
    {
        uint8_t dest;
        memcpy_byte(&dest, src,sizeof(uint8_t));
        return dest;
    }

    void updateProgressCode(uint64_t data, uint8_t value);

} // namespace SBE

#endif //SBE_UTIL_H
