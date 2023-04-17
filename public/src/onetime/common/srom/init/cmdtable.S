/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/onetime/common/srom/init/cmdtable.S $              */
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

/*
The FSM interpreter code uses registers for specific purposes:
r0   timeout count
r1   insn
r2   param
r3   address
r4   scratch
r5   scratch
r6   mask
r7   mask
r8   data
r9   data
r10   cust code start
r13   other big limit
r28   this code location \
r39   this small data    | swap these with ctr/sprg0/edr on CALL/RETURN
r30   this big data      |
r31   this big limit     /

ctr   other code location
lr    -- reserve for subroutines, only use if necessary
sprg0 other small data
edr   other big data
*/

#include <ppe42_asm.h>
#include <errorcodes.H>
#include <initcommonconst.H>
        // rename registers for more intuitive code
#define timeout      %r0
#define insn         %r1
#define param        %r2        // also used as error code in _panic
#define addr         %r3
#define scr          %d4
#define scr0         %r4
#define scr1         %r5
#define mask         %d6
#define mask0        %r6
#define mask1        %r7
#define data         %d8
#define data0        %r8
#define data1        %r9
#define cust_start   %r10
#define code         %r28
#define small        %r29
#define big          %r30
#define limit        %r31
#define other_limit  %r13
#define FORMAT_MAGIC 0x434d5461    // 'CMTa'

#define SCAN_REGION_TYPE   0x01030005
#define BLOCKED_SCAN_TYPES 0x8820       // FUNC, LBIST, CMSK

// Poll count and delay together should take about 100 ms
// For fast response, let's pick a 100 microsecond delay loop, requiring 1k loops for 100 ms
#define POLL_COUNT 1000
// Assuming an operating frequency of 600 MHz (worst case) we need to spend 60000 cycles for 100 microseconds.
// The delay loop below is written to consume three cycles per iteration, so we need 20000 iterations.
// At refclock speed (133 MHz / 4) the timeout will take 1.8 seconds.
#define POLL_DELAY 20000

// Pak structure related macros
#define PAK_MAGIC_START         0x50414B21  // PAK!
#define PAK_MAGIC_PAD           0x50414B50  // PAKP
#define PAK_MAGIC_END           0x2F50414B  // /PAK
#define PAK_CORE_HE_SIZE_OFFSET 0x06
#define PAK_EXT_METHOD_OFFSET   0x09
#define PAK_EXT_N_SIZE_OFFSET   0x0A
#define PAK_EXT_D_SIZE_OFFSET   0x14
#define PAK_EXT_P_SIZE_OFFSET   0x18
#define PAK_EXT_NAME_OFFSET     0x1C

        // Trap with an error code in EDR and ERROR_REG
        .macro panic errorcode
        li      %r2, \errorcode
        b       _panic
        .endm

        .align 3
main_fsm_fname:
        .ascii "main.fsm"
        .set .Lmain_fsm_fname_len, $-main_fsm_fname

cust_fsm_fname:
        .ascii "cust.fsm"
        .set .Lcust_fsm_fname_len, $-cust_fsm_fname

////////////////////////////////////////////////////////////////////////////////
// Locate file in seeprom
////////////////////////////////////////////////////////////////////////////////
        // Locate a file in SEEPROM and return its starting address and size
        //
        // @param[in]    r1   file name start address
        // @param[in]    r2   file name length, excluding trailing null byte
        // @param[in]    r3   NOR partition start, this should not be modified
        // @param[in]    r4   NOR partition limit (first invalid address), this should not be modified
        //
        // @return       r1   start of file in PPE address space, or 0 if file not found
        // @return       r2   length of file
        //
        // May clobber any GPR except r28..r31
        .global locate_seeprom_file
locate_seeprom_file:
        cmpwibeq %r2, 8, 1f             # we're assuming that the file name size is 8 below
        panic   BOOT_RC_INVALID_FN_SIZE # so panic if not
1:
        mr      %r13, %r3               # Use r13 as the pointer into SEEPROM

check_magic:
        cmpwblt %r13, %r3, partition_overrun  # Check that we're still within the partition limits
        cmpwblt %r13, %r4, 1f           # in either direction

partition_overrun:
        panic   BOOT_RC_PARTITION_OVERRUN
1:
        lwz     %r5, 0(%r13)            # Load the magic word from pak file
        _liw    %r6, PAK_MAGIC_END      # Load end magic value
        cmpwbne %r5, %r6, 1f

return_empty_file:
        li      %r1, 0
        blr
1:
        _liw    %r6, PAK_MAGIC_START    # Load start magic value
        cmpwbeq %r5, %r6, process_file_entry

        _liw    %r6, PAK_MAGIC_PAD      # Not a file, check for padding
        cmpwbeq %r5, %r6, skip_padding
        panic   BOOT_RC_INVALID_PAK_MAGIC  # None of the above - panic

skip_padding:
        lwz     %r5, 4(%r13)            # Load amount of bytes to skip and advance the read pointer
        addi    %r13, %r13, 8
        add     %r13, %r13, %r5
        b       check_magic

process_file_entry:
        lhz     %r5, PAK_CORE_HE_SIZE_OFFSET(%r13)
        lwz     %r6, PAK_EXT_P_SIZE_OFFSET(%r13)
        lhz     %r7, PAK_EXT_N_SIZE_OFFSET(%r13)
        cmpwbne %r7, %r2, jump_to_next_file     # compare iv_nsize with r2 (input file name size)
        lvd     %d7, 0(%r1)                     # read the expected file name (assuming always 8 byte)
        lwz     %r9, PAK_EXT_NAME_OFFSET(%r13)  # read the name from NOR (1st 4-bytes, since address is not aligned)
        lwz     %r10, PAK_EXT_NAME_OFFSET + 4(%r13)      # 2nd 4-bytes
        cmpwbne %r7, %r9, jump_to_next_file     # compare MS-Word
        cmpwbne %r8, %r10, jump_to_next_file    # compare LS-Word

return_file_pointer:
        lbz     %r1, PAK_EXT_METHOD_OFFSET(%r13)
        cmpwibeq %r1, 1, 1f             # panic if the file is compressed
        panic   BOOT_RC_FILE_COMPRESSED

1:
        addi    %r1, %r13, 8            # add core header size
        add     %r1, %r1, %r5           # add iv_hesize
        lwz     %r2, PAK_EXT_D_SIZE_OFFSET(%r13)

        # check that the end of the file does not exceed the partition boundaries
        add     %r5, %r1, %r2
        cmpwblt %r5, %r3, partition_overrun
        cmpwbge %r5, %r4, partition_overrun
        blr

jump_to_next_file:
        # move r13 (pak pointer) to next file entry
        addi    %r13, %r13, 8           # add core header size
        add     %r13, %r13, %r5         # add iv_hesize
        add     %r13, %r13, %r6         # add iv_psize
        b       check_magic

////////////////////////////////////////////////////////////////////////////////
// UTILITY CODE
////////////////////////////////////////////////////////////////////////////////

_panic:
        lis     %r1, MBX_BASE
        li      %r3, 0
        stvd    %d2, ERROR_REG(%r1)
        mtedr   %r2
        trap

        // Swap the active table registers and the other table registers,
        // switching between main and custom FSM
swap_fsm:
        mr      scr0, code
        mfctr   code
        mtctr   scr0
        mr      scr0, small
        mfsprg0 small
        mtsprg0 scr0
        mr      scr0, big
        mfedr   big
        mtedr   scr0
        mr      scr0, limit
        mr      limit, other_limit
        mr      other_limit, scr0
        blr

        // Load the FSM starting at r1 into code, small, big, limit
        // and perform checks; trap if any check fails.
        //
        // @param[in]    r1   starting location of FSM
        // @param[in]    r2   length of FSM
        // @param[in]    r3   error code to use in case of failure
        //
        // Clobbers r4
load_fsm:
        // Check magic word
        lwz     %r4, 0(%r1)
        _liw    %r0, FORMAT_MAGIC
        cmpw    %r4, %r0
        beq     1f

invalid_fsm:
        mr      %r2, %r3
        b       _panic

1:
        // Decode size header
        lwz     %r4, 4(%r1)
        addi    code, %r1, 8
        rlwinm  small, %r4, 18, 14, 29  // extract size of instructions - bits 0:15 to 14:29 (multiplying by 4)
        addi    small, small, 4
        add     small, small, code
        rlwinm  big, %r4, 25, 23, 29    // extract size of small values - bits 16:22 to 23:29 (multiplying by 4)
        addi    big, big, 4
        add     big, big, small
        rlwinm  limit, %r4, 3, 20, 28   // extract size of big values - bits 23:31 to 20:28 (multiplying by 8)
        addi    limit, limit, 8
        add     limit, limit, big

        // Check that calculated size matches table size
        add     %r1, %r1, %r2
        cmpwbne %r1, limit, invalid_fsm
        blr

//////////////////////////////////////////////////////////////////////////////////////////
// ADDRESS FILTERING
//////////////////////////////////////////////////////////////////////////////////////////

        // Check address and data fields for validity, trap on disallowed addresses
        // and filter data for restricted ones
        //
        // @param[inout] addr  address; may be modified
        // @param[inout] data  data; may be modified
        //
        // May clobber scr*, data and mask, must preserve everything else
filter_address:
        // TP chiplet access?
        extrwi   scr0, addr, 8, 0
        cmpwibne scr0, 1, no_chiplet

        // it's the TP chiplet, which endpoint?
        extrwi.  scr0, addr, 8, 8
        beqlr                          // chiplet controls - ok
        cmpwibeq scr0, 1, addr_ok      // PSCOM - ok
        cmpwibne scr0, 3, addr_blocked // CC - continue filtering, else fail

        // Filter clock controller accesses... is this a scan?
        bb1wi    addr, 16, scan_addr

        // It's not, check for allowed addresses
        andi.    scr0, addr, 0xFFFF
        cmpwibeq scr0, 4, addr_blocked // 0x01030004 is VERBOTEN
        cmpwibeq scr0, 7, addr_blocked // and so is 0x01030007
addr_ok:
        blr                            // anything else is aok

scan_addr:
        // For scan access, check the value of the scan region type register
        _liw     scr0, SCAN_REGION_TYPE
        lvd      scr, 0(scr0)
        andi.    scr1, scr1, BLOCKED_SCAN_TYPES
        // None of the bad bits set? We're good.
        beqlr
        // Otherwise we very much aren't.
        panic    BOOT_RC_SCAN_TYPE_BLOCKED

no_chiplet:
        // Not TP chiplet... mailbox perhaps?
        extrwi   scr0, addr, 16, 0
        cmpwibeq scr0, 0x5, addr_ok

        // Nope... fall through to panic
addr_blocked:
        panic   BOOT_RC_ADDRESS_BLOCKED

//////////////////////////////////////////////////////////////////////////////////////////
// SETUP
//////////////////////////////////////////////////////////////////////////////////////////

        .global run_cmd_tables

run_cmd_tables:
        // locate main FSM and stow its location in r30/r31
        _liw    %r1, main_fsm_fname
        _liw    %r2, .Lmain_fsm_fname_len
        bl      locate_seeprom_file
        bwnz    %r1, 1f
        panic   BOOT_RC_MISSING_MAIN_FSM

1:
        // stow main FSM location in r30/r31 for now
        mr      %r30, %r1
        mr      %r31, %r2

        // locate cust FSM
        _liw    %r1, cust_fsm_fname
        _liw    %r2, .Lcust_fsm_fname_len
        bl      locate_seeprom_file

        // we're going to need r30/r31 now but r6/r7 are free
        mr      %r6, %r30
        mr      %r7, %r31

        // do we have a custom table?
        bwnz    %r1, 1f

        // No custom table -> clear custom table info
        li      cust_start, 0
        b       load_main_fsm

1:
        // otherwise load the custom table and swap it into the "other" slot
        li      %r3, BOOT_RC_INVALID_CUST_FSM
        bl      load_fsm
        bl      swap_fsm

        // record custom FSM first instruction for later
        mfctr   cust_start

        // Haxxy trick to save a register:
        //   main limit = real main limit
        //   cust limit = real cust limit - 1
        // This way comparisons for range checks still work as before
        // (since the addresses are 8 byte granular) and we can check
        // the LSB for cust vs. main.
        // Since the limit is never used for memory accesses the
        // unaligned value doesn't hurt us.
        addi    other_limit, other_limit, -1

load_main_fsm:
        // load the main FSM
        mr      %r1, %r6
        mr      %r2, %r7
        li      %r3, BOOT_RC_INVALID_MAIN_FSM
        bl      load_fsm

        // write progress code to SB_MSG
        // use aliased register names so we know we're not clobbering anything important
        lis     insn, MBX_BASE
        lvd     scr, SB_MSG(insn)
        rlwinm  scr0, scr0, 0x0, 0x0, 0x19         # Clear off the bits 26:27:28:29:30:31 (Old progress code)
        ori     scr0, scr0, COMMAND_TABLE_EXEC
        stvd    scr, SB_MSG(insn)

        // Ready to execute, lets-a go!

//////////////////////////////////////////////////////////////////////////////////////////
// MAIN LOOP
//////////////////////////////////////////////////////////////////////////////////////////

main_loop:
        // check instruction address for overrun
        cmpwblt code, small, 1f         // the start of small data is the limit for instructions
        panic   BOOT_RC_INSTRUCTION_OVERRUN
1:
        // load instruction; we don't use lwu because
        // it would make the address comparison above tricky
        lwz     insn, 0(code)
        addi    code, code, 4

        // decode param
        rlwinm  param, insn, 12, 24, 29  // extract bits 4:9 into bits 24:29 (multiplying by 4)
        add     param, param, small
        cmpwblt param, big, 1f           // the start of big data is the limit for small data

value_overrun:
        panic   BOOT_RC_VALUE_OVERRUN

1:
        lwz     param, 0(param)

        // handle NOP, CALL and RETURN now since they don't have address or data
        extrwi. scr0, insn, 3, 1        // extract opcode
        beq     main_loop               // 0 == NOP -- do nothing
        cmpwibge scr0, 3, scom_op       // 3 or above are SCOM ops
        cmpwibeq scr0, 2, return_op     // 2 == RETURN

        // otherwise 1 == CALL
call_op:
        // we must not be executing from the cust table already
        bb1wi   limit, 31, invalid_call
        // we should also have a cust table in the first place :)
        bwz     cust_start, invalid_call

        // otherwise swap states and set the new instruction pointer
        bl      swap_fsm
        slwi    scr0, param, 2
        add     code, cust_start, scr0

        // check for jump outside allowed range
        cmpwblt code, cust_start, invalid_call
        cmpwbge code, small, invalid_call

        b       main_loop

invalid_call:
        panic   BOOT_RC_INVALID_CALL

return_op:
        // are we returning from the cust table?
        bb1wi   limit, 31, return_from_cust
        // if we're returning from the main table, we're officially done
        b       __pk_boot

return_from_cust:
        // swap state back and continue
        bl      swap_fsm
        b       main_loop

scom_op:
        // decode address
        rlwinm  addr, insn, 19, 23, 29  // extract bits 10:16 into bits 23:29 (multiplying by 4)
        add     addr, addr, small
        cmpwbge addr, big, value_overrun
        lwz     addr, 0(addr)

        // factor out the address filter into its own function just for neatness
        bl      filter_address

        // decode data
        rlwinm  data0, insn, 3, 20, 28  // extract bits 23:31 into bits 20:28 (multiplying by 8)
        add     data0, data0, big
        cmpwbge data0, limit, value_overrun
        lvd     data, 0(data0)

        // decode mask
        rlwinm  mask0, insn, 26, 23, 28 // extract bits 17:22 into bits 23:28 (multiplying by 8)
        add     mask0, mask0, big
        cmpwbge mask0, limit, value_overrun
        lvd     mask, 0(mask0)

        // PUTSCOM is the last opcode below 4 left to check so we can look at bits
        bb1wi   insn, 1, test_ops

putscom_op:
        cmpwi   mask0, -1
        bne     putscom_with_mask
        cmpwi   mask1, -1
        bne     putscom_with_mask

        // pure PUTSCOM, no mask - go!
        stvd    data, 0(addr)
        b       main_loop

putscom_with_mask:
        // read-modify-write
        lvd     scr, 0(addr)
        not     mask0, mask0
        not     mask1, mask1
        and     scr0, scr0, mask0
        and     scr1, scr1, mask1
        or      scr0, scr0, data0
        or      scr1, scr1, data1
        stvd    scr, 0(addr)
        b       main_loop

test_ops:
        // init poll timeout for POLL operation
        li      timeout, POLL_COUNT

poll_loop:
        // for any of POLL, TEST, CMPBEQ and CMBBNE we need to read and compare under mask now, so let's do that
        lvd     scr, 0(addr)
        and     scr0, scr0, mask0
        and     scr1, scr1, mask1
        xor     scr0, scr0, data0
        xor     scr1, scr1, data1
        or      scr1, scr1, scr0    // scr1 is now zero only if the values match

        bb1wi   insn, 2, cmpbxx_op  // 0b11x == CMPBxx

        // Here we're at opcode 0b10x, so either POLL or TEST.
        // Both complete the instruction if the comparison matches
        bwz     scr1, main_loop

        // On TEST, we fail on mismatch
        bb0wi   insn, 3, _panic     // 0b100 == TEST

        // On POLL, we decrement the timeout, fail on zero or wait and repeat
        // Skip the timeout check if param (error code) is zero
        bwz     param, no_timeout
        subwibz timeout, 1, _panic

no_timeout:
        _liw    scr0, POLL_DELAY

        // The delay loop is a single instruction so that it won't incur any
        // PIBMEM reads and have a deterministic number of cycles per iteration
        // (three according to the PPE42 workbook).
poll_delay:
        subwibnz scr0, 1, poll_delay

        b       poll_loop

cmpbxx_op:
        // calculate branch target and check that it's within range
        slwi    param, param, 2
        add     scr0, code, param
        cmpwbge scr0, small, invalid_branch
        cmpwbge scr0, code, valid_branch

invalid_branch:
        panic   BOOT_RC_INVALID_BRANCH

valid_branch:
        bb1wi   insn, 3, cmpbne_op      // 0b111 == CMPBNE

        // 0b110 == CMPBEQ
        bwz     scr1, take_branch
        b       main_loop

cmpbne_op:
        bwnz    scr1, take_branch
        b       main_loop

take_branch:
        mr      code, scr0
        b       main_loop
