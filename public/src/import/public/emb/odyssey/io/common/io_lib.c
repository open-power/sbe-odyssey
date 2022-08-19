/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/common/io_lib.c $     */
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
// *!---------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2016
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : io_lib.c
// *! TITLE       :
// *! DESCRIPTION : Common functions
// *!
// *! OWNER NAME  : Vikram Raj          Email: vbraj@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// gap22062200 |gap     | Moved per-gen fifo l2u settings to tx_fifo_init
// jjb22062700 |jjb     | Removed 5nm qualifiers from tx patgen code
// jjb22062100 |jjb     | Added 80 bit repeating pattern support for tx_write_4_bit_pat
// vbr22050900 |vbr     | Issue 278561: Updated the set_all_cal_lane_sel function to not use a broadcast
// mbs22041300 |mbs     | Issue 259276,259278: Added special tx_fifo_l2u_dly settings for zm_abus
// mbs22022500 |mbs     | Removed scom_ppe_fir mapping (logic no longer exists)
// vbr22011300 |vbr     | Added functions for updating lanes_pon_0_23 mem_reg
// jfg22011000 |jfg     | Update Log2 to use 32 based count. see text
// jfg22010701 |jfg     | Replace Log2 with a clz instruction
// jfg21121400 |jfg     | Added Log2
// vbr21101900 |vbr     | Update tx fifo init to get pcie rate from TX clock config instead of pipe interface
// jfg21102000 |jfg     | Fix mask compare bug in set_all_cal_lane_sel
// vbr21100700 |vbr     | Updated set_fir() to set ppe_error_type
// jfg21100600 |jfg     | Fix code error on last
// jfg21100500 |jfg     | Invert use of lane_mask in set_all_cal_lane for code flow
// jfg21091300 |jfg     | Add set_all_cal_lane_sel
// vbr21100500 |vbr     | Thread active time check disable now disables recording of time as well
// vbr21093000 |vbr     | Updates for new threading method
// vbr21092700 |vbr     | get_ptr (rmw) optimization.
// mwh21092300 |mwh     | Moved tx_write_4_bit_pat and in_tx_half_width_mode into libs because need be common for dcc iot and ioo
// vbr21082400 |vbr     | Changes to run supervisor thread 1/X loops. Handle thread active time check completely in io_sleep.
// vbr21081200 |vbr     | Rearrranged TX FIFO config writes.  Removed unnecesary writes to tx_fifo_load_count_max (writing to default value).
// mwh21007190 |mwh     | Moved functions eo_get_weight_ave and eo_round to io_lib since common across iot and ioo for me
// vbr21062500 |vbr     | Different abort masks for AXO and PCIe
// vbr21061700 |vbr     | DL recal_abort is abort_error, bump_ui_mode and pcie_cmd are abort_clean
// vbr21061000 |vbr     | Added bump_ui to recal abort check
// vbr21060800 |vbr     | Updates to checking for recal abort and improved run_servo_ops efficiency
// vbr21060200 |vbr     | Common function for checking servo queue empty
// mwh21052700 |mwh     | Moved eo_eoff_round eo_get_weight_ave function to eo_eoff.h
// mwh21043000 |mwh     | Added function used by eo_eoffs eo_eoff_round eo_get_weight_ave
// vbr21051300 |vbr     | Added checking of interval between io_sleep calls. Made ppe_error PG instead of PT.
// vbr21040200 |vbr     | Added function to set lane/reg_id in gcr_adr
// vbr21020500 |vbr     | Move common ioo/iot code to io_lib.
// vbr20121100 |vbr     | Removed manual servo op (P10 dd1) code
//-------------|P11     |-------------------------------------------------------
// mbs21041200 |mbs     | Renamed rx_lane_bad vector to rx_lane_fail, removed per-lane version, and added rx_lane_fail_cnt
// vbr21020901 |vbr     | HW552111: Set fir and debug log on io_wait escape
// vbr20111800 |vbr     | HW552111: Added escape to io_wait()
// bja20090900 |bja     | Use common is_p10_dd1() check
// mbs20073000 |mbs     | LAB - Added workaround hooks for run_servo_ops_base
// vbr20030900 |vbr     | HW525544: Make ppe_servo_status0/1 trap registers and clear the hw servo_status0/1 registers after copying
// vbr20012400 |vbr     | HW520453: Clear the scom_ppe_fir reg bit before setting it again since FW can't write to it on DD1
// vbr19111500 |vbr     | Initial implementation of debug levels
// jgr19102300 |jgr     | HW509784: updated put/get functions to wait for echo flag independent of read valid
// vbr19031300 |vbr     | Removed inlining on some set_gcr_addr_* functions.
// mbs19021900 |mbs     | Updated polling loop in ll_put
// mbs19021800 |mbs     | Replaced mmio functions with ll and msg functions for the dl ppe to use
// vbr18120400 |vbr     | Improved local put/get functions.
// vbr18111400 |vbr     | Updated set_fir function.
// vbr18101700 |vbr     | Added scaled_microsecond global.
// vbr18100200 |vbr     | set_fir no longer hangs the ppe but still doesn't set a FIR. Updated handling of servo_status.
// vbr18081701 |vbr     | Added some assumption checking
// vbr18062000 |vbr     | Updated set_fir to set the new ppe_error status bit.
// vbr18030500 |vbr     | Further performance enhancements based on changing the gcr packet format.
// vbr18030200 |vbr     | Performance enhancements for hw reg access functions.
// vbr18022200 |vbr     | HW435186: run_servo_ops checks (and saves) servo_status and has option to set FIR on error.
// mwh17110700 |mwh     | Add empty assembly instruction so this loop does not get optimized out
// mwh17101800 |mwh     | Put in endless loop inside fir function.  So we hang if fir is called
// vbr17110100 |vbr     | Removed gcr_port (only 1 port).
// vbr17092800 |vbr     | Removed abort checking from run_servo_ops.
// vbr17071801 |vbr     | Removed servo_queue_depth variable and using empty status bit instead
// vbr17062901 |vbr     | Switched from ServoResultToInt to TwosCompToInt.
// vbr17062801 |vbr     | Added img_addr() and switched mem_regs/img_regs to moving pointers and added _base pointers.
// vbr17062800 |vbr     | Moved last_thread_run from mem_regs into img_regs.
// vbr17062700 |vbr     | Increased to 8 IO threads and added img_regs.
// vbr17062100 |vbr     | Updated run_servo_ops and added wrappers to work with rx_servo_disable_result=1.
// vbr17062000 |vbr     | Corrected the servo_op queue empty check on an abort.
// vbr17061400 |vbr     | Added new return codes and updated comments.
// vbr17061200 |vbr     | Check abort in run_servo_ops; don't check for results in first loop.
// vbr17051801 |vbr     | Removed recal bit from servo op and added queue input.
// vbr17051800 |vbr     | Remove IOF code.
// vbr17042500 |vbr     | Added IOO thread support
// vbr17041700 |vbr     | Added correct registers to run_servo_ops.
// vbr17041200 |vbr     | Added recal_bit to run_servo_ops.
// vbr17030300 |vbr     | First pass at a function for using a servo_op queue.
// vbr17020600 |vbr     | Moved manual servo op into its own file
// vbr17011800 |vbr     | Fix for HW393271 - don't self-seed during servo if DFE2-12 is disabled.
// vbr17011200 |vbr     | Added gcr_wr_raw.
// vbr17010800 |vbr     | Manual servo op: no longer moving integ_fine_gain even/odd together
// vbr17010400 |vbr     | Manual servo op: filter depth and change count are configurable
// vbr16120600 |vbr     | Manual servo op: set integ_fine_gain to mid-scale before doing CM coarse
// vbr16111400 |vbr     | Added ap_1011, ap_0011 servos to manual_servo_op.
// vbr16102800 |vbr     | Added manual_servo_op functions to work around IOO DD1 issues.
// vbr16102500 |vbr     | Split recal filter and self-seed into seperate inputs to run_servo_op
// vbr16101900 |vbr     | Added global variable to indicate the final thread in the round robin chain.
// vbr16082500 |vbr     | Converted some functions to macros
// vbr16081800 |vbr     | Moved some functions from eo_common to io_lib (and renamed some).
// vbr16070700 |vbr     | Removed deprecated put/get; minor stack/code size optimizations
// vbr16070100 |vbr     | Removed RMW option from put_ptr_int (only does RMW).
// vbr16062800 |vbr     | Optimization
// mbs16062400 |mbs     | Added gcr_addr struct
// mbs16062400 |mbs     | Added put_ptr, put_ptr_fast, and get_ptr functions
// vbr16050500 |vbr     |- Moved mem_regs and various functions from eo_common to io_lib
// vbr16050300 |vbr     |- Added lcl_get()
// mbs16040800 |mbs     |- Added instance parm to mmio_put and mmio_get, and adjusted bit fields
// -----------------------------------------------------------------------------

#include "io_lib.h"
#include "pk.h"

#include "io_logger.h"
#include "servo_ops.h"

#include "ppe_img_reg_const_pkg.h"
#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"


////////////////////////////////////////////////////////////////////////////////////////////
// Mem Regs: 1KB section of memory allocated by the linker (link_ioo.cmd) in the RAM.
// It is actually C_MAX_THREADS x 1KB sections (1 per thread/bus).
// See link_ioo.cmd for actual offset addresses.
// Using a uint16_t* so indexed 2B at a time. The array _size could be calculated using the linker symbols, but hard code it instead.
// Marked as volatile so always accesses SRAM instead of local register copy.
////////////////////////////////////////////////////////////////////////////////////////////
volatile uint16_t* mem_regs_u16 = &_mem_regs0_start;
volatile uint16_t* mem_regs_u16_base = &_mem_regs0_start;

////////////////////////////////////////////////////////////////////////////////////////////
// FW Regs: 8B section of memory allocated by the linker (link_ioo.cmd) in the RAM.
// It is actually C_MAX_THREADS x 8B (1 per thread/bus).
// See link_ioo.cmd for actual offset addresses.
// Using a uint16_t* so indexed 2B at a time. The array _size could be calculated using the linker symbols, but hard code it instead.
// Marked as volatile so always accesses SRAM instead of local register copy.
////////////////////////////////////////////////////////////////////////////////////////////
volatile uint16_t* fw_regs_u16 = &_fw_regs0_start;
volatile uint16_t* fw_regs_u16_base = &_fw_regs0_start;

////////////////////////////////////////////////////////////////////////////////////////////
// Img Regs: 16B section of memory allocated by the linker (link_ioo.cmd) in the RAM.
// See link_ioo.cmd for actual offset address.
// Using a uint16_t* so indexed 2B at a time. The array _size could be calculated using the linker symbols, but hard code it instead.
// Marked as volatile so always accesses SRAM instead of local register copy.
////////////////////////////////////////////////////////////////////////////////////////////
volatile uint16_t* img_regs_u16 = &_img_regs_start;


////////////////////////////////////////////////////////////////////////////////////////////
// Thread Barrier Semaphore - 1 for all threads (method 0), 1 per thread (method 1)
////////////////////////////////////////////////////////////////////////////////////////////
#if PK_THREAD_SUPPORT

    // Simple Round Robin: Single thread barrier that all io threads use
    #if IO_THREADING_METHOD == 1
        PkSemaphore thread_barrier;
    #endif

    // Round Robin with Servo Pending: Each io thread has its own thread barrier
    #if IO_THREADING_METHOD == 2
        PkSemaphore thread_barrier[C_MAX_THREADS - 1];
    #endif

    unsigned int
    final_io_thread;  // Indicates the final IO thread (prior to the Supervisor thread) in the round robin chain
    unsigned int round_robin_loop_count; // Track the number of times through the round robin chain
#endif //PK_THREAD_SUPPORT


////////////////////////////////////////////////////////////////////////////////////////////
// SLEEP & WAIT FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

// Saved value of a scaled microsecond to reduce operations in some places.
// Use u32 instead of PkInterval (u64) since this will always be less than 2^32.
uint32_t scaled_microsecond; //PkInterval

#if PK_THREAD_SUPPORT
// Track the last time io_sleep() was called to check thread active time
PkTimebase last_io_sleep_call_time;

// Record and Check the amount of time that the current (going to sleep) thread was active
static inline void check_thread_active_time()
{
    if (mem_pg_field_get(ppe_disable_thread_active_time_check))
    {
        return;
    }

    // Record the amount of time that the sleeping thread was active
    PkTimebase current_time = pk_timebase_get();
    uint32_t active_time = (uint32_t)(current_time - last_io_sleep_call_time);
    uint16_t active_time_us = active_time / TIMER_US_DIVIDER;
    mem_regs_u16[pg_addr(ppe_thread_last_active_time_us_addr)] = active_time_us;
    last_io_sleep_call_time = current_time;

    // Check that the active time does not exceed the limit and set a FIR if it does (skip this check if it is disabled for sim or BIST)
    if (active_time_us > THREAD_ACTIVE_TIME_LIMIT_US)
    {
        ADD_LOG(DEBUG_THREAD_ACTIVE_TIME_ERROR, active_time_us);
        set_fir(fir_code_thread_active_time_exceeded);
    }
}//check_thread_active_time


#if IO_THREADING_METHOD == 1 // Simple Round Robin
// Sleep the thread by blocking on the semaphore - allows other threads to run. Takes current thread as a parameter.
void io_sleep(unsigned int thread)
{
    // Record and check the current thread active time
    check_thread_active_time();

    if (thread < final_io_thread)
    {
        // If not the final IO thread in the round robin chain, pend on the thread barrier semaphore.
        // This sleeps the current thread and lets the next highest priority non-pending thread to run.
        pk_semaphore_pend(&thread_barrier, PK_WAIT_FOREVER);
    }
    else if (thread == final_io_thread)
    {
        // If this is the final IO thread, determine if run the Supervisor thread (next thread in round robin chain)
        // or go back to the first thread of the round robin chain based on the configured rate.
        bool run_supervisor_thread = ((round_robin_loop_count % C_SUPERVISOR_THREAD_RATE) == 0);
        round_robin_loop_count = round_robin_loop_count + 1; // Increment the round robin loop count

        if (run_supervisor_thread)
        {
            // Pend on the thread barrier semaphore so that the next thread (Supervisor thread) runs.
            pk_semaphore_pend(&thread_barrier, PK_WAIT_FOREVER);
        }
        else     //!run_supervisor_thread
        {
            // Release the thread barrier semaphore so that round robin chain resets to the first thread.
            pk_semaphore_release_all(&thread_barrier);
        }
    }
    else     //thread > final_io_thread (thread == supervisor_thread)
    {
        // When the Supervisor thread runs, it is always the last thread in the round robin chain.
        // The final thread in the round robin chain does not pend, instead it releases the semaphore for the
        // higher priority threads; it gets preempted since it is lowest priority and runs when they are all blocking.
        // This also work (inefficiently) in the case where there is only a single thread.
        pk_semaphore_release_all(&thread_barrier);
    }

    // Set the pointers for the current thread before returning
    set_pointers(thread);

    // Debug info on the current thread running
    img_field_put(ppe_current_thread, thread);
}//io_sleep
#endif //Simple Round Robin


#if IO_THREADING_METHOD == 2 // Round Robin With Servo Pending
// Sleep the thread by blocking on a semaphore - allows other threads to run. Takes current thread as a parameter.
void io_sleep(unsigned int thread)
{
    // Record and check the current thread active time
    check_thread_active_time();

    // If this is the final IO thread, determine if run the Supervisor thread (next thread in round robin chain) based on the configured rate.
    if (thread == final_io_thread)
    {
        bool run_supervisor_thread = ((round_robin_loop_count % C_SUPERVISOR_THREAD_RATE) == 0);
        round_robin_loop_count = round_robin_loop_count + 1; // Increment the round robin loop count

        if (run_supervisor_thread)
        {
            // Pend on the thread barrier semaphore so that the next thread (Supervisor thread) runs.
            pk_semaphore_pend(&thread_barrier[thread], PK_WAIT_FOREVER);
            goto thread_wakeup;
        }
    } //final_io_thread

    // Read the Servo Interrupt local register to determine which threads are not pending on servos
    // This vector is aligned so that bit[0]=thread0, bit[1]=thread1, bit[2]=thread2, etc.
    //   0 = pending (wait to run)
    //   1 = not pending (runnable)
    uint64_t volatile* servo_intr_addr = (uint64_t*)(0xC0000000 | scom_ppe_intr_lcl_addr);
    uint32_t servo_intr_vec = (uint32_t)((*servo_intr_addr) >> 32);

    // This re-arranges the vector alignment to be bit[0]=thread+1, bit[1]=thread+2, ..., bit[num_io_threads-1]=thread.
    // Thus bit[0] is the next thread in a simple round robin and it continues in order with the current thread being the last valid bit.
    uint32_t num_io_threads     = final_io_thread + 1;
    uint32_t up_shift           = (thread == num_io_threads) ? thread : thread +
                                  1; // Supervisor thread calculates next_thread like it is the final_io_thread
    uint32_t down_shift         = num_io_threads - up_shift;
    uint32_t thread_sel_vec     = (servo_intr_vec << up_shift) | (servo_intr_vec >> down_shift);

    // Check which is the first bit in the vector that is 1 (indicating a runnable thread).
    // If this selects the current thread or no thread, just remain on the current thread.
    uint32_t next_thread_offset = __builtin_clz(thread_sel_vec);

    if ( (next_thread_offset > final_io_thread) ||                                    // No runnable IO thread
         ((next_thread_offset == final_io_thread)
          && (thread != num_io_threads)) )    // Selected current thread (or final_io_thread if this is the  Supervisor thread)
    {
        return;
    }

    // Release the semaphore on the thread selected to run next
    uint32_t next_thread = (up_shift +
                            next_thread_offset); //up_shift = thread + 1 (where thread is capped at final_io_thread)

    if (next_thread >= num_io_threads)
    {
        next_thread = next_thread - num_io_threads;    // This is less costly than a call to __umodsi3()
    }

    pk_semaphore_release_all(&thread_barrier[next_thread]);

    // If the next thread is lower priority, need to also pend.
    // Otherwise, do not need to do anything since a higher priority thread starts as soon as its semaphore is released.
    // The shortcoming of this method is that by not pending, this thread will run the next time it is the highest
    // priority thread regardless of the servo state.
    if (next_thread > thread)
    {
        pk_semaphore_pend(&thread_barrier[thread], PK_WAIT_FOREVER);
    }

thread_wakeup:
    // Set the pointers for the current thread before returning
    set_pointers(thread);

    // Debug info on the current thread running
    img_field_put(ppe_current_thread, thread);
}//io_sleep
#endif //Round Robin With Servo Pending


// Wait for at least X time by sleeping.
// Note that with threading we could just sleep the thread for the desired interval, but that would break our controlled
// round-robin scheduling since a high priority thread would immediately resume running when the timer expires.
// The timer on the PPE42 can be based on the DEC counter or an external counter. We use the external counter.
// The DEC is a 32-bit decrement counter that underflows from 0 to 0xfffffffful while
// the external counter is a 32-bit counter that overflows from 0xfffffffful to 0.
// However, we do not need to handle that since we use the pk_timebase_get() (which returns an incrementing u64 timebase)
// and the kernel manages the over/under-flow.
// To manage the DEC underflow, the MSR[EE] must be set to enable external and timer interrupts.
void io_wait(int thread, PkInterval wait_time)
{
    PkTimebase end_time = pk_timebase_get() + PK_INTERVAL_SCALE(wait_time);

    // Loop on a spin/sleep until pass the min time or hit a loop limit (in case timer is broke HW552111)
    int loop_count = 0;

    do
    {
        loop_count++;
        io_sleep(thread);
    }
    while ( (pk_timebase_get() < end_time)
            && (loop_count < 128) );   // 128 loops is min ~250us but can be much more based on thread sleep times

    // Set fir and debug_log if hit max loop count
    if (loop_count >= 128)
    {
        set_fir(fir_code_fatal_error);
        ADD_LOG(DEBUG_IO_WAIT_ESCAPE, 0x00);
    }
} //io_wait
#endif //PK_THREAD_SUPPORT


// Simple Log2 computation
// Mathematically accurate such that base2 number requires the nth bit to be represented
// i.e. 32 = 10000 rather than 11111 being 32 binary values
// Note 0 returns 0.
inline int Log2( uint32_t val )
{
    int log;
    log = 32 - __builtin_clz(val);
    return log;
}


////////////////////////////////////////////////////////////////////////////////////////////
// SERVO OP FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

// Run a list of servo ops and return the results in an array (2's complement integers).
// Return value indicates if there was an error reported by the servo status (which it clears after copying to the mem_regs).
// Can disable the pushing (and thus reading and returning) of results to improve speed.
// Can disable the setting of the FIR on a servo error if want to handle that in calling function.
PK_STATIC_ASSERT(rx_servo_op_queue0_startbit == rx_servo_op_queue1_startbit);
PK_STATIC_ASSERT(rx_servo_op_queue0_endbit == rx_servo_op_queue1_endbit);
PK_STATIC_ASSERT(rx_servo_queue_status_full_reg_alias_width == 16);
PK_STATIC_ASSERT(rx_servo_queue_status_full_reg_alias_addr == rx_servo_result_queue_full_space_addr);
PK_STATIC_ASSERT(rx_servo_queue_status_full_reg_alias_addr == rx_servo_op_queue_empty_space_addr);
PK_STATIC_ASSERT(rx_servo_queue_status_full_reg_alias_addr == rx_servo_op_queue_empty_addr);
#ifdef IOO
    PK_STATIC_ASSERT(rx_servo_queue_status_full_reg_alias_addr == rx_pipe_ifc_recal_abort_addr);
    PK_STATIC_ASSERT(rx_servo_queue_status_full_reg_alias_addr == rx_dl_phy_cal_lane_recal_abort_addr);
    PK_STATIC_ASSERT(rx_servo_queue_status_full_reg_alias_addr == rx_dl_phy_cal_lane_recal_abort_sticky_addr);
    PK_STATIC_ASSERT(rx_servo_queue_status_full_reg_alias_addr == rx_dl_phy_cal_lane_bump_recal_abort_sticky_addr);
#endif
PK_STATIC_ASSERT(rx_datasm_cntl1_wo_full_reg_alias_width == 16);
PK_STATIC_ASSERT(rx_datasm_cntl1_wo_full_reg_alias_addr == rx_clear_servo_queues_addr);
PK_STATIC_ASSERT(rx_datasm_cntl1_wo_full_reg_alias_addr == rx_reset_servo_status_addr);
int run_servo_ops_base(t_gcr_addr* gcr_addr, unsigned int queue, unsigned int num_ops, uint16_t* servo_ops,
                       int32_t* results, bool results_enabled, bool set_fir_on_error)
{
    unsigned int i;
    unsigned int ops_submitted = 0;
    unsigned int ops_done = 0;
    unsigned int servo_op_queue_addr = (queue == c_servo_queue_general) ? rx_servo_op_queue0_addr : rx_servo_op_queue1_addr;

    int recal = mem_pg_field_get(rx_running_recal);
#ifdef IOO
    // Different Aborts for PCIe and AXO
    int pcie_mode = fw_field_get(fw_pcie_mode);
    const uint32_t abort_mask       = pcie_mode ? rx_pipe_ifc_recal_abort_mask : (rx_dl_phy_cal_lane_recal_abort_sticky_mask
                                      | rx_dl_phy_cal_lane_bump_recal_abort_sticky_mask);
    const uint32_t abort_error_mask = pcie_mode ? 0 : rx_dl_phy_cal_lane_recal_abort_sticky_mask;
#else
    const uint32_t abort_mask       = 0;
    const uint32_t abort_error_mask = 0;
#endif

    // In recal, need to initially read the status to get abort status.
    // In init, don't care about the abort and can assume the queues are empty with a depths of 8.
    uint32_t servo_queue_status_full_reg;

    if(recal)
    {
        servo_queue_status_full_reg = get_ptr_field(gcr_addr, rx_servo_queue_status_full_reg_alias);
    }
    else
    {
        servo_queue_status_full_reg = rx_servo_op_queue_empty_mask | rx_servo_result_queue_empty_mask |
                                      (8 << rx_servo_op_queue_empty_space_shift);
    }

    // Loop to submit servo ops and read results
    bool read_results = false; // Don't want to read results on first loop since haven't submitted anything as yet.

    do   //while (ops_done < num_ops);
    {
        // In recal, check for recal abort
        if (recal)
        {
            uint32_t recal_abort = servo_queue_status_full_reg & abort_mask;

            if (recal_abort)
            {
                // Abort the currently running servo and clear the queue.  Also clear (and ignore) any servo errors.
                put_ptr_field_fast(gcr_addr, rx_datasm_cntl1_wo_full_reg_alias,
                                   (rx_clear_servo_queues_mask | rx_reset_servo_status_mask)); // strobe bits

                // Return abort_error for an AXO DL recal_abort; return abort_clean for any other abort source
                int ret_val = (servo_queue_status_full_reg & abort_error_mask) ? abort_error_code : abort_clean_code;
                return ret_val;
            } //if(abort)
        } //if(recal)

        // Read results when available in the result queue (if enabled); skip this the first loop iteration since haven't submitted anything as yet.
        if (read_results)
        {
            unsigned int results_available = bitfield_get(servo_queue_status_full_reg, rx_servo_result_queue_full_space_mask,
                                             rx_servo_result_queue_full_space_shift); //get_ptr_field(gcr_addr, rx_servo_result_queue_full_space);

            if (results_available)
            {
                for (i = 0; i < results_available; i++)
                {
                    int32_t raw_result = get_ptr_field(gcr_addr, rx_servo_result_queue);
                    results[ops_done] = TwosCompToInt(raw_result, rx_servo_result_queue_width);
                    ops_done = ops_done + 1;
                }
            }
        }
        else     //!read_results (first_loop)
        {
            // After first loop, only read results if enabled.
            read_results = results_enabled;
        }

        // Submit ops (if needed) when space in the servo op queue
        if (ops_submitted < num_ops)
        {
            unsigned int servo_queue_space = bitfield_get(servo_queue_status_full_reg, rx_servo_op_queue_empty_space_mask,
                                             rx_servo_op_queue_empty_space_shift); //get_ptr_field(gcr_addr, rx_servo_op_queue_empty_space);

            while ( (ops_submitted < num_ops) && (servo_queue_space != 0) )
            {
                put_ptr(gcr_addr, servo_op_queue_addr, rx_servo_op_queue0_startbit, rx_servo_op_queue0_endbit, servo_ops[ops_submitted],
                        fast_write);
                ops_submitted = ops_submitted + 1;
                servo_queue_space = servo_queue_space - 1;
            }
        }
        else if (!results_enabled)
        {
            // If results aren't enabled, we start checking for the servo op queue to be empty only after submitting all servo ops.
            // Once it is empty, all the servos are done running and we can set ops_done=num_ops so that the loop exits.
            int servo_queue_empty = servo_queue_status_full_reg &
                                    rx_servo_op_queue_empty_mask; //get_ptr_field(gcr_addr, rx_servo_op_queue_empty);

            if (servo_queue_empty)
            {
                ops_done = num_ops;
            }
        }

        // If still waiting for servo ops to finish, sleep and then check the status
        if (ops_done < num_ops)
        {
#if PK_THREAD_SUPPORT
            io_sleep(get_gcr_addr_thread(gcr_addr));
#else
            io_spin_us(polling_interval_us);
#endif

            servo_queue_status_full_reg = get_ptr_field(gcr_addr, rx_servo_queue_status_full_reg_alias);
        }
    }
    while (ops_done < num_ops);

    // Once done running all the servo ops, check and handle the servo status and return rc_warning or rc_no_error.
    // status0 is the first servo_op that had an error.
    // status1 is the information about the error.
    int servo_status1 = get_ptr_field(gcr_addr, rx_servo_status1);

    if (servo_status1 != 0)   // Error reported by servo status.
    {
        // Trap the servo status into the mem_regs if ppe_servo_status1 nothing previously trapped.
        // For simplicity, checking that the full reg is 0 and not just the valid bit.
        if (mem_pg_field_get(ppe_servo_status1) == 0)
        {
            int servo_status0 = get_ptr_field(gcr_addr, rx_servo_status0);
            mem_pg_field_put(ppe_servo_status0, servo_status0);
            mem_pg_field_put(ppe_servo_status1, servo_status1);
        }

        // Set the FIR.
        if (set_fir_on_error)
        {
            set_fir(fir_code_bad_lane_warning);
        }

        // HW525544: Clear the servo status in HW so don't react to the same error again
        put_ptr_field_fast(gcr_addr, rx_reset_servo_status, 0b1); // strobe bit

        return rc_warning;
    }
    else
    {
        // No error reported by servo status
        return rc_no_error;
    }
}//run_servo_ops_base


// Wrapper function for when do not want results to be pushed.
// Includes the register writes to disable results and then re-enable them when done (since that is the default setting).
// Can disable the setting of the FIR on a servo error if want to handle that in calling function.
int run_servo_ops_with_results_disabled_base(t_gcr_addr* gcr_addr, unsigned int queue, unsigned int num_ops,
        uint16_t* servo_ops, bool set_fir_on_error)
{
    bool results_enabled = false;
    int32_t* results = NULL;

    put_ptr_field(gcr_addr, rx_servo_disable_result_set, 0b1, fast_write);
    int status = run_servo_ops_base(gcr_addr, queue, num_ops, servo_ops, results, results_enabled, set_fir_on_error);
    put_ptr_field(gcr_addr, rx_servo_disable_result_clr, 0b1, fast_write);

    return status;
} //run_servo_ops_with_results_disabled_base


// Return if the servo op and servo result queues are empty (for error checking in calibration steps)
PK_STATIC_ASSERT(rx_servo_queue_status_full_reg_alias_width == 16);
bool get_servo_queues_empty(t_gcr_addr* gcr_addr)
{
    int  servo_queue_status = get_ptr_field(gcr_addr, rx_servo_queue_status_full_reg_alias);
    int  op_queue_empty     = bitfield_get(servo_queue_status, rx_servo_op_queue_empty_mask,
                                           rx_servo_op_queue_empty_shift);
    int  result_queue_empty = bitfield_get(servo_queue_status, rx_servo_result_queue_empty_mask,
                                           rx_servo_result_queue_empty_shift);
    bool both_queues_empty  = (op_queue_empty & result_queue_empty);
    return both_queues_empty;
}

// Common function for checking and handling the servo queues being empty at the beginning of cal
int check_servo_queues_empty(t_gcr_addr* gcr_addr, const uint16_t error_state, const uint8_t error_log_type)
{
    int status = rc_no_error;
    bool servo_queues_empty = get_servo_queues_empty(gcr_addr);

    if (!servo_queues_empty)
    {
        set_debug_state(error_state); // Debug - Servo Queue Empty Error
        set_fir(fir_code_fatal_error);
        ADD_LOG(error_log_type, gcr_addr, 0x0);
        status = rc_error;
    }

    return status;
}


////////////////////////////////////////////////////////////////////////////////////////////
// IOO / IOT COMMON FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

// Broadcast write pulse to rx_clr_cal_lane_sel.
// Activates data pipe clocks, allowing lingering data to clear.
void clear_all_cal_lane_sel(t_gcr_addr* gcr_addr)
{
    // save currently active lane
    int saved_lane = get_gcr_addr_lane(gcr_addr);
    // use broadcast lane num
    set_gcr_addr_lane(gcr_addr, bcast_all_lanes);
    // broadcast write pulse to clr reg
    put_ptr_field(gcr_addr, rx_clr_cal_lane_sel, 0b1, fast_write);
    // restore active lane
    set_gcr_addr_lane(gcr_addr, saved_lane);
    return;
}

// Broadcast write pulse to rx_set_cal_lane_sel.
// Activates data pipe clocks, allowing lingering data to clear.
// Lane Mask Aware: If a mask is set then individually disable specified lanes
void set_all_cal_lane_sel(t_gcr_addr* gcr_addr, const uint32_t i_lane_mask_rx)
{
    // save currently active lane
    int saved_lane = get_gcr_addr_lane(gcr_addr);
    // It would be faster to use a broadcast set and then clear on the non-enabled/bad lanes,
    // but it is possible for the configured lanes (max_lanes) to be less than the actual lanes in the chiplet.
    int max_lanes = get_num_rx_lane_slices();
    uint32_t shifted_mask = i_lane_mask_rx;
    int lane = 0;

    while (lane < max_lanes)
    {
        if ( ((0x80000000 & shifted_mask) == 0x80000000) )
        {
            set_gcr_addr_lane(gcr_addr, lane);
            put_ptr_field(gcr_addr, rx_set_cal_lane_sel, 0b1, fast_write);
        }

        shifted_mask = shifted_mask << 1;
        lane++;
    }

    // restore active lane
    set_gcr_addr_lane(gcr_addr, saved_lane);
    return;
}

void set_rx_lane_fail_cnt()
{
    uint32_t fail_cnt = 0;
    uint32_t lane_fail = (mem_pg_field_get(rx_lane_fail_0_15) << 8) | mem_pg_field_get(rx_lane_fail_16_23);

    while ( lane_fail != 0 )
    {
        if ( fail_cnt < ((1 << rx_lane_fail_cnt_width) - 1) )   // Increase the fail count if the bit width allows
        {
            fail_cnt += (lane_fail & 0x1);
        }

        lane_fail = (lane_fail >> 1); // Shifting will eventually go to 0
    }

    mem_pg_field_put(rx_lane_fail_cnt, fail_cnt);
}

// Functions to set or clear a lane's status in rx_lane_fail_0_23 (pg)
PK_STATIC_ASSERT(rx_lane_fail_0_15_width == 16);
PK_STATIC_ASSERT(rx_lane_fail_16_23_width == 8);
PK_STATIC_ASSERT(rx_lane_fail_16_23_startbit == 0);
void set_rx_lane_fail(unsigned int lane)
{
    uint32_t lane_mask = 0x80000000 >> lane;

    if (lane < 16)
    {
        mem_regs_u16_bit_set(pg_addr(rx_lane_fail_0_15_addr), (lane_mask >> 16));
    }
    else     // lane>=16
    {
        mem_regs_u16_bit_set(pg_addr(rx_lane_fail_16_23_addr), lane_mask);
    }

    set_rx_lane_fail_cnt();
} //set_rx_lane_fail()

void clr_rx_lane_fail(unsigned int lane)
{
    uint32_t lane_mask = 0x80000000 >> lane;

    if (lane < 16)
    {
        mem_regs_u16_bit_clr(pg_addr(rx_lane_fail_0_15_addr), (lane_mask >> 16));
    }
    else     // lane>=16
    {
        mem_regs_u16_bit_clr(pg_addr(rx_lane_fail_16_23_addr), lane_mask);
    }

    set_rx_lane_fail_cnt();
} //clr_rx_lane_fail()


// Set/Clear the mem_reg bit for tracking the lanes that are powered on
void set_rx_lanes_pon_0_23(unsigned int lane)
{
    if (lane < 16)
    {
        int mask = (0x8000 >> lane);
        int reg_val = mem_pg_field_get(rx_lanes_pon_00_15);
        reg_val |= mask;
        mem_pg_field_put(rx_lanes_pon_00_15, reg_val);
    }
    else     //16-23
    {
        int mask = (0x80 >> (lane - 16));
        int reg_val = mem_pg_field_get(rx_lanes_pon_16_23);
        reg_val |= mask;
        mem_pg_field_put(rx_lanes_pon_16_23, reg_val);
    }
} //set_rx_lanes_pon_0_23

void clr_rx_lanes_pon_0_23(unsigned int lane)
{
    if (lane < 16)
    {
        int mask = (0x8000 >> lane);
        int reg_val = mem_pg_field_get(rx_lanes_pon_00_15);
        reg_val &= ~mask;
        mem_pg_field_put(rx_lanes_pon_00_15, reg_val);
    }
    else     //16-23
    {
        int mask = (0x80 >> (lane - 16));
        int reg_val = mem_pg_field_get(rx_lanes_pon_16_23);
        reg_val &= ~mask;
        mem_pg_field_put(rx_lanes_pon_16_23, reg_val);
    }
} //clr_rx_lanes_pon_0_23

void set_tx_lanes_pon_0_23(unsigned int lane)
{
    if (lane < 16)
    {
        int mask = (0x8000 >> lane);
        int reg_val = mem_pg_field_get(tx_lanes_pon_00_15);
        reg_val |= mask;
        mem_pg_field_put(tx_lanes_pon_00_15, reg_val);
    }
    else     //16-23
    {
        int mask = (0x80 >> (lane - 16));
        int reg_val = mem_pg_field_get(tx_lanes_pon_16_23);
        reg_val |= mask;
        mem_pg_field_put(tx_lanes_pon_16_23, reg_val);
    }
} //set_tx_lanes_pon_0_23

void clr_tx_lanes_pon_0_23(unsigned int lane)
{
    if (lane < 16)
    {
        int mask = (0x8000 >> lane);
        int reg_val = mem_pg_field_get(tx_lanes_pon_00_15);
        reg_val &= ~mask;
        mem_pg_field_put(tx_lanes_pon_00_15, reg_val);
    }
    else     //16-23
    {
        int mask = (0x80 >> (lane - 16));
        int reg_val = mem_pg_field_get(tx_lanes_pon_16_23);
        reg_val &= ~mask;
        mem_pg_field_put(tx_lanes_pon_16_23, reg_val);
    }
} //clr_tx_lanes_pon_0_23


// apply (un)load settings and synchronize
void tx_fifo_init(t_gcr_addr* gcr_addr)
{
#ifdef IOO
    int pcie_mode = fw_field_get(fw_pcie_mode);

    if (pcie_mode)
    {
        //uint32_t l_rate = get_ptr_field(gcr_addr, pipe_state_rate); // gen1=0, gen2=1, gen3=2, gen4=3, gen5=4
        uint32_t l_rate_one_hot = get_ptr_field(gcr_addr,
                                                tx_pcie_clk_sel); // gen1=0b00001, gen2=0b00010, gen3=0b00100, gen4=0b01000, gen5=0b10000
        uint32_t l_rate = 31 - __builtin_clz(l_rate_one_hot); // gen1=0, gen2=1, gen3=2, gen4=3, gen5=4

        if (l_rate < 2)   // GEN 1 and GEN 2
        {
            put_ptr_field(gcr_addr, tx_fifo_l2u_dly, 0b010, read_modify_write);
            put_ptr_field(gcr_addr, tx_fifo_load_width_sel,   1, read_modify_write);
            put_ptr_field(gcr_addr, tx_fifo_unload_count_max, 4, read_modify_write);
        }
        else
        {
            put_ptr_field(gcr_addr, tx_fifo_l2u_dly, 0b001, read_modify_write);
            put_ptr_field(gcr_addr, tx_fifo_load_width_sel,   0, read_modify_write);
            put_ptr_field(gcr_addr, tx_fifo_unload_count_max, 3, read_modify_write);
        }
    }

    // TX FIFO L2U settings for zMetis Abus
    if ( is_zm_abus() )
    {
        // l2u_dly=3, max_load_cnt=7, max_unload_cnt=7
        put_ptr_field_fast(gcr_addr, tx_fifo_l2u_alias, 0x377); //pl
    }

#endif

    put_ptr_field_fast(gcr_addr, tx_clr_unload_clk_disable,   0b1);
    put_ptr_field_fast(gcr_addr, tx_fifo_init,   0b1);
    put_ptr_field_fast(gcr_addr, tx_set_unload_clk_disable,   0b1);
    return;
}


//Used by eo_eoff and eo_loff_iot.c
int eo_get_weight_ave( int i_e_after, int i_e_before)
{
    //shift by 2 is divide by 4
    int eoffn_next = (1 * i_e_after + 3 * i_e_before);
    int weight_average = eo_round(eoffn_next);
    return weight_average;
}


// write a repeating 4-bit pattern to the tx pattern register
//shared across iot and paxo
void tx_write_4_bit_pat(t_gcr_addr* gcr_addr, unsigned int pat_4)
{
    unsigned int pat_8 = 0;
    unsigned int pat_16 = 0;

    // this copies the 4-bits to the even bits of an 8-bit word,
    // considering msb as 0
    // two steps to move to odd; then, shift 1 to move to even
    if (in_tx_half_width_mode())
    {
        pat_8 = ((pat_4 << 2) | pat_4) & 0x33; // move 2 msb over 2; odd bits are in correct relative place now
        pat_8 = ((pat_8 << 1) | pat_8) & 0x55; // move even bits over 1;
        pat_8 = pat_8 << 1; // shift to move odd to even
    }
    else
    {
        pat_8 = pat_4 | (pat_4 << 4);
    }

    pat_16 = pat_8 | (pat_8 << 8);

    put_ptr_field(gcr_addr, tx_pattern_0_15,   pat_16,  fast_write);
    put_ptr_field(gcr_addr, tx_pattern_16_31,  pat_16,  fast_write);
    put_ptr_field(gcr_addr, tx_pattern_32_47,  pat_16,  fast_write);
    put_ptr_field(gcr_addr, tx_pattern_48_63,  pat_16,  fast_write);
//  // if IOO and 5nm design then initialize 80 pat repeating pattern
#ifdef IOO
//  bool is_5nm_l = !is_odyssey();
//  if (is_5nm_l) {
    put_ptr_field(gcr_addr, tx_pattern_64_79,  pat_16,  fast_write);
//  }
#endif
} // tx_write_4_bit_pat


// Determine if in tx_half_width_mode; stub returning false til P11 defined
bool in_tx_half_width_mode()
{
    return false;
} // in_tx_half_widt_mode




////////////////////////////////////////////////////////////////////////////////////////////
// GCR ADDRESS FORMING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////

// These functions are setup to interace with a "t_gcr_addr" structure which is only infrequently updated.
// A pointer to this structure can be passed down the call stack and provide all the necessary information
// to put and get functions as well as precalculated masks

// This is called once at the start of a thread to set up the gcr_addr struct
#if PK_THREAD_SUPPORT
    void set_gcr_addr(t_gcr_addr* gcr_addr, int thread, int bus_id, int reg_id, int lane)
#else
    void set_gcr_addr(t_gcr_addr* gcr_addr,             int bus_id, int reg_id, int lane)
#endif
{
#if PK_THREAD_SUPPORT
    gcr_addr->thread = thread;
#endif
    gcr_addr->bus_id = bus_id;
    gcr_addr->reg_id = reg_id;
    gcr_addr->lane = lane;

    gcr_addr->gcr_read_packet =
        0x80002000 // base_gcr_msg: StartBit=1, UserProgrammable=1
        | (bus_id    << (31 - 10))
        | (reg_id    << (31 - 16))
        | (lane      << (31 - 31));

    gcr_addr->gcr_write_packet =  gcr_addr->gcr_read_packet | 0x08000000;
};

// Limited use in interrupt handler and supervisor thread
void set_gcr_addr_bus_id(t_gcr_addr* gcr_addr, int bus_id)
{
    gcr_addr->bus_id = bus_id;
    gcr_addr->gcr_read_packet  =  (gcr_addr->gcr_read_packet & 0xf81fffff) |
                                  (bus_id    << (31 - 10));
    gcr_addr->gcr_write_packet =  gcr_addr->gcr_read_packet | 0x08000000;
}

// For switching between rx_group, tx_group, etc
void set_gcr_addr_reg_id(t_gcr_addr* gcr_addr, int reg_id)
{
    gcr_addr->reg_id = reg_id;
    gcr_addr->gcr_read_packet  =  (gcr_addr->gcr_read_packet & 0xffe07fff) |
                                  (reg_id    << (31 - 16));
    gcr_addr->gcr_write_packet =  gcr_addr->gcr_read_packet | 0x08000000;
}

// For setting the addressed lane in per-lane accesses
void set_gcr_addr_lane(t_gcr_addr* gcr_addr, int lane)
{
    gcr_addr->lane = lane;
    gcr_addr->gcr_read_packet  =  (gcr_addr->gcr_read_packet & 0xffffffe0) |
                                  (lane << (31 - 31));
    gcr_addr->gcr_write_packet =  gcr_addr->gcr_read_packet | 0x08000000;
}

// For switching between rx_group, tx_group, etc and lane
void set_gcr_addr_reg_id_lane(t_gcr_addr* gcr_addr, int reg_id, int lane)
{
    gcr_addr->reg_id = reg_id;
    gcr_addr->lane = lane;
    gcr_addr->gcr_read_packet  =  (gcr_addr->gcr_read_packet & 0xffe07fe0) |
                                  (lane      << (31 - 31))                 |
                                  (reg_id    << (31 - 16));
    gcr_addr->gcr_write_packet =  gcr_addr->gcr_read_packet | 0x08000000;
}


////////////////////////////////////////////////////////////////////////////////////////////
// HW I/O FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////
// !!TODO!! - Assure that the I/O functions are atomic with interrupts

void ll_put(uint32_t addr, uint32_t data)
{
    uint64_t volatile* lclmem_addr = (uint64_t*)(0xC0000210);

    (*lclmem_addr) =
        (uint64_t)addr << 32   //  0:31
        |  (uint64_t)data;        // 32:63

    // poll for clearing of address bits to determine completion
    uint64_t result = -1;

    do
    {
        result = (*lclmem_addr);
    }
    while ( (result & 0xffffffff00000000) != 0x0 );


    return;
}

uint32_t ll_get(uint32_t addr)
{
    uint64_t volatile* lclmem_addr = (uint64_t*)(0xC0000210);

    (*lclmem_addr) =
        (uint64_t)addr << 32;  //  0:31

    // poll for clearing of address bits to determine completion
    uint64_t result = -1;

    do
    {
        result = (*lclmem_addr);
    }
    while ( (result & 0xffffffff00000000) != 0x0 );


    uint32_t r_data = (uint32_t)(result & 0xffffffff);
    return r_data; // 32 bits
    //return;
}

void msg_put(uint64_t data)
{
    uint64_t volatile* lclmem_addr = (uint64_t*)(0xC0000220);

    (*lclmem_addr) = data;

    return;
}

uint64_t msg_get()
{
    uint64_t volatile* lclmem_addr = (uint64_t*)(0xC0000220);

    return (*lclmem_addr);
}





// This allows writing a raw GCR packet to the bus.
// Primarily meant as a workaround for the GCR interrupt packet not being ack'd.
void gcr_wr_raw(t_gcr_addr* gcr_addr, uint64_t packet)
{
    // Setup the pointer
    uint64_t volatile* lclmem_addr = (uint64_t*)gcr_port_addr;

    // wait on previous op while start bit is set but echo flag is not set
    while (((*lclmem_addr) & 0x9000000000000000) == 0x8000000000000000 );

    // send write op and exit
    (*lclmem_addr) = packet;
} //gcr_wr_raw


// Fast put where the packet is preformed and the data field uses all 16 bits so we just need to fill in the reg addr and 16-bit data.
// Orderng of this fuction is important to its performance. Modifying the function in anyway will change how it is compiled and performs.
void put_ptr_fast_int(t_gcr_addr* gcr_addr, uint32_t reg_addr, uint32_t data)
{
    // Setup the gcr peripheral pointer
    uint64_t volatile* lclmem_addr64 = (uint64_t*)gcr_port_addr;
    uint32_t volatile* lclmem_addr32 = (uint32_t*)gcr_port_addr;

    // Wait on previous op while start bit is set but echo flag is not set. Status bits are in upper word.
    while (((*lclmem_addr32) & 0x90000000) == 0x80000000); // Bit 0 is start, bit 3 is echo

    // Data field is already shifted into correct bit position by wrapper function.
    uint32_t w_addr_data = reg_addr | data;

    uint64_t packet =
        ((uint64_t)gcr_addr->gcr_write_packet << 32) |
        w_addr_data;

    // send write op and exit
    (*lclmem_addr64) = packet;
} //put_ptr_fast_int


// Put where the packet is preformed. always does read-modify-write.
// Orderng of this fuction is important to its performance. Modifying the function in anyway will change how it is compiled and performs.
void put_ptr_int(t_gcr_addr* gcr_addr, uint32_t reg_addr, uint32_t and_mask, uint32_t data)
{
    // For a read-modify-write, first do a read of the register in question.
    // However we don't need to do a full get_ptr() because we don't care about shifting the
    // data around.
    uint64_t rd_packet =
        ((uint64_t)gcr_addr->gcr_read_packet << 32) |
        reg_addr;

    // Setup the gcr peripheral pointer
    uint64_t volatile* lclmem_addr64 = (uint64_t*)gcr_port_addr;
    uint32_t volatile* lclmem_addr32 = (uint32_t*)gcr_port_addr;

    // Wait on previous op while start bit is set but echo flag is not set. Status bits are in upper word.
    while (((*lclmem_addr32) & 0x90000000) == 0x80000000); // Bit 0 is start, bit 3 is echo

    // Send Read Req packet
    (*lclmem_addr64) = rd_packet;

    // Partially prepare wr_packet while the read command is running.
    // This takes advantage of the read and write packets being the same except for the data field and read/write bit.
    uint64_t wr_packet = rd_packet | 0x0800000000000000;

    // Poll for read response packet flag to indicate returned data and echo flag to indicate op completed.
    // The read response flag is set ~3 cycles before the echo flag, so there are two options:
    //   1) Poll on read response flag and hope that the subsequent instructions take more cycles than the op to complete (risky).
    //   2) Poll on the echo flag (and optionally the read response flag) before proceeding (echo flag only implemented).
    uint64_t resp_packet = 0;

    do
    {
        resp_packet = (*lclmem_addr64);
    }
    while ((resp_packet & 0x1000000000000000) == 0);   // Bit 2 is read data valid, bit 3 is echo

    // Mask out old data field and or in new data, then finish forming the packet.
    // Data field is already shifted into correct bit position by wrapper function.
    uint32_t w_data = (resp_packet & and_mask) | data;
    wr_packet = wr_packet | w_data;

    // Wait on previous op while start bit is set but echo flag is not set. Status bits are in upper word.
    // This is not needed due to previous read response poll covering this (see notes above).
    //while (((*lclmem_addr32) & 0x90000000) == 0x80000000);

    // write data back and exit
    (*lclmem_addr64) = wr_packet;
} //put_ptr_int


// Get where the packet is preformed.
// Orderng of this fuction is important to its performance. Modifying the function in anyway will change how it is compiled and performs.
uint32_t get_ptr_int(t_gcr_addr* gcr_addr, uint32_t reg_addr, uint32_t and_mask, uint32_t shift_amt)
{
    // Setup the gcr peripheral pointers
    uint64_t volatile* lclmem_addr64 = (uint64_t*)gcr_port_addr;
    uint32_t volatile* lclmem_addr32 = (uint32_t*)gcr_port_addr;

    // Wait on previous op while start bit is set but echo flag is not set. Status bits are in upper word.
    while (((*lclmem_addr32) & 0x90000000) == 0x80000000);

    // Form the Read Req packet
    uint64_t packet =
        ((uint64_t)gcr_addr->gcr_read_packet << 32) |
        reg_addr;

    // Send Read Req packet
    (*lclmem_addr64) =  packet;

    // poll for read response packet flag to indicate returned data
    uint64_t result = 0;

    do
    {
        result = (*lclmem_addr64);
    }
    while ( (result & 0x2000000000000000) == 0x0);   // Bit 2 is read data valid

    uint32_t r_data = ((uint32_t)(result & and_mask)) >> shift_amt;

    return r_data; // 16 bits
} //get_ptr_int


// PPE Local Register Access
void lcl_put_int(uint32_t reg_addr, uint32_t data)
{
    uint64_t volatile* lclmem_addr = (uint64_t*)(0xC0000000 | reg_addr);
    (*lclmem_addr) = (uint64_t)data << 32;
}

uint32_t lcl_get_int(uint32_t reg_addr, uint32_t shift)
{
    uint64_t volatile* lclmem_addr = (uint64_t*)(0xC0000000 | reg_addr);
    uint32_t r_data = (uint32_t)((*lclmem_addr) >> 32);
    r_data = r_data >> shift;
    return r_data;
}


// Set the FIR bits and mem_reg ppe_error status
// Returns if ppe_error_valid was already set
int set_fir(uint32_t fir_code)
{
#if IO_DEBUG_LEVEL >= 1
    // Trap the error info if there isn't already a trapped error
    int error_valid = mem_pg_field_get(ppe_error_valid);

    if (!error_valid)
    {
        int state = mem_pg_field_get(ppe_debug_state);
        mem_pg_field_put(ppe_error_state, state);

        int thread = img_field_get(ppe_current_thread);
        mem_pg_field_put(ppe_error_thread, thread);

        int lane = mem_pg_field_get(rx_current_cal_lane);
        mem_pg_field_put(ppe_error_lane, lane);

        int type = __builtin_clz(fir_code);
        mem_pg_field_put(ppe_error_type, type);

        mem_pg_bit_set(ppe_error_valid);
    }

#else
    int error_valid = 1;
#endif

    // Clear the bit in the PPE FIR register so it can be set again (FIR is edge triggered)
    //uint64_t volatile *fir_clr_addr = (uint64_t*)(0xC0000000 | scom_ppe_fir_clr_lcl_addr);
    //(*fir_clr_addr) = (uint64_t)fir_code << 32;

    // Set the bit in the PPE FIR register to trigger the FIR
    uint64_t volatile* fir_set_addr = (uint64_t*)(0xC0000000 | scom_ppe_fir_set_lcl_addr);
    (*fir_set_addr) = (uint64_t)fir_code << 32;

    //PK_PANIC(0);
    return error_valid;
} //set_fir
