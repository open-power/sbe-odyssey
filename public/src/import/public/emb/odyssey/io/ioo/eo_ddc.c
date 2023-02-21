/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_ddc.c $        */
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
// *!---------------------------------------------------------------------------
// *! (C) Copyright International Business Machines Corp. 2016
// *! All Rights Reserved -- Property of IBM
// *! *** IBM Confidential ***
// *!---------------------------------------------------------------------------
// *! FILENAME    : eo_ddc.c
// *! TITLE       :
// *! DESCRIPTION : Run DDC and update the historical eye width
// *|                 The Quad Phase Adjust (QPA) is also calculated independently for data phases.
// *!
// *! OWNER NAME  : John Gullickson     Email: gullicks@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// vbr23010300 |vbr     | EWM296171: Added sleep at end of ddc_seek_loop (recal only)
// jfg22111701 |jfg     | Replace PR reads with read_active_pr
// vbr22111600 |vbr     | Skip sleeps when in RxEqEval
// mbs22082601 |mbs     | Updated with PSL comments
// jfg22072200 |jfg     | EWM278876: Relegate the pr_recenter debug_state to Level 3
// jfg22062102 |jfg     | EWM265038: Remove all ddc_fail and set_fir actions outside of the rx_ddc_check_en conditional.
// jfg22032300 |jfg     | EWM275896: in ddc_seek_loop, st 0x8020, reduce edge preset in direction of edge QPA (previously ignored)
// jfg22031401 |jfg     | Change pr_recenter debug state to 0xF8yy
// jfg22030800 |jfg     | repair rx_ddc_measure_limited in main seek.
// jfg22030500 |jfg     | repair rx_ddc_measure_limited. Remove excessive sleep logic.
// jfg22021400 |jfg     | Remove CDR Lock check entirely. Convert all primary GCR writes to fast.
// jfg22020900 |jfg     | Tweak sleep logic and make QPA analysis conditional on recal mode
// jfg22012600 |jfg     | Add rx_ddc_ber_period_sel modifier to nominal BER sampler timer selection
// jfg22012600 |jfg     | Initial eye seek error condition repair. Mainly now only handle non-error-free starting sample
// jfg22012000 |jfg     | in pr_seek_ber add in additional io_sleep after 10 IOreg queries for leftover delay of game
// jfg22011800 |jfg     | Recode ddc_seek_loop variables for NS/EW consistency
// jfg22011700 |jfg     | Fix ns/ew swap in quad phase seek request
// jfg22011400 |jfg     | Fix comments and debug state
// jfg22011200 |jfg     | Tweak debug states for abort test
// jfg22011000 |jfg     | Update sleep loop for log2 function change
// jfg22010500 |jfg     | Force a io_sleep in pr_ber_seek if Dstep=0 for basic timer wait
// jfg22010500 |jfg     | Replace fixed 14 initial seek (in pr_ber_seek) with a limiting calc for ds_max
// jfg21121700 |jfg     | Eliminate seek loop in lieu of single pr_seek_ber function call. Resolve Edge PR math underflow
// jfg21121600 |jfg     | Temporarily remove Dstep even correction.
// jfg21121400 |jfg     | Add io_sleep evaluation logic.
// jfg21121400 |jfg     | Remove initial BER check completely and instead rely on initial Right Hand Seek.
// jfg21120800 |jfg     | Speedups: Combine NS/EW PR seeks when adj val is equal. Reduce excees seeking steps and roll edge leftovers to quad compares.
// jfg21120700 |jfg     | Speedups: eliminate backoff paranoia and decrease; increase initial edge seek from 6 to 16, decrease short seek timer
// jfg21120600 |jfg     | Correctly use abs of final adj compare or completeness check in pr_seek_ber
// jfg21113000 |jfg     | Repair Estep code in pr_seek_ber to limit steps in a phase-matched fashion
// jfg21113000 |jfg     | Fix initial search small eye warning (8015) by using es count for loop limit and max_eye for additional comparison.
// jfg21111800 |jfg     | Simplify pr_seek_ber, remove redundant code.
// jfg21110900 |jfg     | Refactor for P11 HW accellerators
// -----------------------------------------------------------------------------
// vbr21051800 |vbr     | Added sleeps to reduce thread blocking
// jfg21040501 |jfg     | EWM228233 Be explicit about signed comparison to left/right_edge_reg variables.
// vbr21012800 |vbr     | Skip historical eye width logging when clr_eye_height_width is set
// mbs21041200 |mbs     | Renamed rx_lane_bad vector to rx_lane_fail, removed per-lane version, and added rx_lane_fail_cnt
// jfg20120301 |jfg     | HW552377 Add rx_ddc_small_eye_warning indicator in ddc_seek_loop
// jfg20120200 |jfg     | HW552377 Increase safe near-fuzz sample time and modify max standoff for small eyes.
// jfg20090100 |jfg     | HW532333 Move offset changes to eo_main
// jfg20081400 |jfg     | HW532333 Add new static PR Offset feature
// jfg20091600 |jfg     | HW544800 Remove max-eye branch from 0x8013 to continue straight 0x8003 edge seek. Increase hyst per-measured-EW
// jfg20042000 |jfg     | HW523199 Remove ddc_last_left & ddc_last_right and add per-lane history recording.
// jfg20042000 |jfg     | HW523199 Replace ddc_hist_left/right with ddc_hyst_left_right
// jfg20041400 |jfg     | HW518382 Regression exposed a possible error in QPA offset comparison using signed math on a signed value instead of unsigned
// jfg20031101 |jfg     | HW518382 Fix use of ber_long on initial N quad search to match ber_short as on E
// jfg20031100 |jfg     | HW525882 Fix parameter swap in mem_pl_field_put for ddc_measure_limited
// mwh20022400 |mwh     | Add in warning fir to DFT fir so both get set if DFT check triggers
// vbr20020600 |vbr     | Use new common set_rx_lane_bad() function which also sets per-lane bit
// jfg19102300 |jfg     | HW508732 Remove excess conditionals in abort check
// jfg19090900 |jfg     | HW499875 As described in Verif Forum All Eye Opt remove the max_eye condition on the
//             |        | initial edge search in 8002 (still relies on the one in 8023).
//             |        | In state 8002 the step jump-back of data/edge will now use all data steps to avoid running out.
// jfg19071800 |jfg     | HW502715 Restore quads to original position in 8023 pre edge tune
// jfg19071800 |jfg     | HW497599 prevent async hazard when changing berpl_thresh
// jfg19071800 |jfg     | HW496621 Add status indicator for lack of mini-PR steps when seeking edges.
// jfg19062400 |jfg     | moved single stepping wrapper function down into nedge_seek_step so use original
// vbr19061200 |vbr     | HW492354/HW492202 - With Mini PR gray coding, can only move Mini PR 1 step at at time.
// jfg19061700 |jfg     | Enhance Eye search with extra double step correction into error free window if too many errors
// jfg19061500 |jfg     | Repair lefthand error recenter of negative edge. Attempt to detect direction change in hysteresis accounting.
// jfg19060600 |jfg     | Enhance error-based recentering for wierd data & edge relationships
// jfg19060600 |jfg     | Prevent lefthand seek hang
// jfg19052203 |jfg     | Remove commented code
// jfg19052000 |jfg     | Remove lock sticky check as not intended.
// jfg19050801 |jfg     | HW475341 Split rx_ddc_hist_left_edge for a & b banks. Replace ppe_ddc_failed_status with rx_ddc_fail
// jfg19050800 |jfg     | Fix endless loop in leftward seek due to data step == 0
// jfg19043000 |jfg     | Missing else on max_eye conditional in seek_loop causing EW eye tune to be skipped
// jfg19043000 |jfg     | Minor change to move BER threshold reset after fuzz is found.
// jfg19042500 |jfg     | Fix threshold limit set when ber_count == 1. No dividing.
// jfg19041900 |jfg     | Remove old seek_edge function. Using qpa version now. Add error state PR step return.
// jfg19041601 |jfg     | Fix leftward search for a valid 0 error rate. go figure.
// jfg19041601 |jfg     | Add a step to reduce the dual seek upon first error to allow better non-error margin for phase seek
// jfg19040400 |jfg     | Readjust BER goals to +1 from -1 as typo. Added ber_sel_done for final eye check.
// jfg19040400 |jfg     | Redesign split phase fine tune to ensure both phases are inside eye to prevent breaking DFE speculation
// jfg19040200 |jfg     | Skip extra edge in st 8002 and 8006 so that data doesn't travel as far during fine tune
// jfg19040200 |jfg     | Discussed in PR#439 to make the L/R history update dependent on either phase moving.
// jfg19040200 |jfg     | Discussed in PR#439 cutting BER goals /2 for phase-based search to maintain same joint error rate.
// jfg19032100 |jfg     | Fix comparison of left and right edges against hysteresis. Previously ignoring EW value.
// jfg19030800 |jfg     | Incorporate QPA architecture updates into DDC behavior
// mwh19043000 |mwh     | add in set_fir(fir_code_dft_error);
// mwh19040119 |mwh     | removed the rx_fail_flag code -- code size hit and taking out reduce sim
// mwh19012100 |mwh     | Added in rxbist fail flag
// vbr19012200 |vbr     | Updated function calls
// mwh19011000 |mwh     | Added in rxbist check stuff
// jfg18122900 |jfg     | HW474827 Marginal low error rates broke left edge seek. Added mid-range BER reset. Fix jfg18122600 range check.
// jfg18122800 |jfg     | Fix hang due to edge step by disabling check with stepsize=0
// jfg18122800 |jfg     | HW474827 Remove if check for ber_reported before attempting final search to force entry into fine staging.
// jfg18122600 |jfg     | HW474827 Increase range of max_eye by decopling data&edge checks. Perform range check before writing mem_reg
// jfg18122200 |jfg     | Switch first center loop from do to while to avoid 0 data adjust error. Add two extra error escapes.
// jfg18122100 |jfg     | Fixing missing bounds check on PR position inside edge_seek
// jfg18121700 |jfg     | Clear Edge offset adjust to 0 to preserve QPA. Clean up some coverage conditionals.
// jfg18121600 |jfg     | And one more typo for not fixing all the offset equations.
// jfg18121500 |jfg     | Resolve a number of marginal sneak paths around low error boundaries and save offset truncation bit
// jfg18121301 |jfg     | Reduce outward search by 1 for a more conservative 2-step edge margin and fix sign on Eleft recenter.
// jfg18121300 |jfg     | HW474503 Replace the overcomplicated conditionals with while loops for repositioning.
// jfg18121300 |jfg     | HW474503 Replace final reposition math with conditionals for + and - offset
// jfg18121300 |jfg     | Moved ber_reset in seek function ahead of wait to improve excess shift inward. Also cleaned up comments.
// jfg18121201 |jfg     | Moved error handling code to a function and added BER default restores there as well.
// jfg18121200 |jfg     | Needed to restore berpl_exp_data_sel to default and set initial ber check duration to match final
// jfg18121102 |jfg     | Needed to restore berpl_sat_thresh to default
// jfg18121101 |jfg     | HW472436/HW474499 Revert last change and rewrite error limit flows for less overhead
// jfg18121100 |jfg     | HW472436/HW474499 Reduce Initial BER measurement limit to 1 error out of a short window
// jfg18121001 |jfg     | Fix ber threshold disable from to max from 0 (since 0 inhibits count)
// jfg18121000 |jfg     | HW474503/HW474501 Fix reposition math for final edge adjust.
// jfg18121000 |jfg     | HW472436/HW474499 Initial BER measurement fails due to missing reset. Also Added additional paranoia checks for eyes that are too small to measure with this algorithm.
// jfg18120500 |jfg     | HW472825 Add some missing BER resets and add additional ber_running reads to slow things down. See HW474100
// jfg18120400 |jfg     | HW472825 Fix edge_seek ber_lim >= check.
// jfg18120400 |jfg     | HW472825 Fix direction search in state 8007 and add edge shift balancing to prevent false pass prior to middle
// jfg18112001 |jfg     | HW472825 Increase/enhance BER duration and choices for 10^5 target. See Workbook.
// vbr18111400 |vbr     | Added calls to set_fir
// jfg18102300 |jfg     | Replace initial lock b check with alias / bank aware
// jfg18110200 |jfg     | HW471171 Update hist_min_eye_width addr to lane version for mem_reg read during valid=1 compare
// jfg18101700 |jfg     | A comment update on ber_sel_long to match code
// jfg18101600 |jfg     | HW467166 Add rx_berpl_sat_thresh for reading count value while BER runs
// jfg18100200 |jfg     | Revert long_ber_count back to 7
// jfg18093000 |jfg     | Increase BER sel setting by 1 per request of Rell due to design change.
// jfg18092700 |jfg     | Refactor ber_running check for accurate ber count reporting
// jfg18092700 |jfg     | Fixed error in fast write of ber_timer_sel field causing hang and general ber errors
// jfg18092500 |jfg     | Identified a bad lane setting bug which flipped the 0_15 and 16_23 portions
// jfg18092000 |jfg     | HW461866 increase BER sel width and error check duration for pre and post edge error rates
// jfg18091300 |jfg     | HW461949 Replace loop-based cdr lock check with end of measure check
// jfg18082900 |jfg     | HW461947 add rx_abort recenter and fix edge on non-seek move to allow non-zero move
// jfg18082200 |jfg     | Change  rx_bad_lane_16_19 to  rx_bad_lane_16_23
// jfg18082200 |jfg     | Change  seek edge stepsize to 2:1 decision based on distance away from center
// jfg18082200 |jfg     | Change  recal_dac_change term into || to allow a future non-hysteresis based override
// jfg18082000 |jfg     | Switch last & hist edge regs to per-lane
// jfg18082000 |jfg     | Fix offset math by /2 and negative data travel
// jfg18081500 |jfg     | Replace L/R edge puts with individual functions
// jfg18081500 |jfg     | Fix sum of max adjusted step math to difference
// jfg18081400 |jfg     | Fix Eew comparison to prmini (>) in edge_seek function
// jfg18081400 |jfg     | Fix prmask shifting to match mask values. Add ber_reported=0 after first search to eliminate flythrough on max extent
// jfg18081400 |jfg     | Replace last and hist edge writes with mem_put
// jfg18081300 |jfg     | Replace ppe_ddc_failed_status set with clr on abort pass
// jfg18081300 |jfg     | Add missing ff and 1f debug states
// jfg18081300 |jfg     | Move re-center shift outside hysteresis check and clear offset on fail.
// jfg18081300 |jfg     | Fix L/R edge reg data packing shift position
// jfg18081300 |jfg     | Reposition hist_eye_width reg accessess per regdef.
// jfg18081300 |jfg     | Fix prmask macros to match post-function data
// jfg18073100 |jfg     | Converted file to ppe-based DDC flow
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ioo_common.h"
#include "eo_ddc.h"
#include "eo_qpa.h"

#include "ppe_com_reg_const_pkg.h"
#include "ppe_mem_reg_const_pkg.h"
#include "ppe_img_reg_const_pkg.h"
#include "io_config.h"

#define ber_too_long 1023

#define edge_half_left (pr_mini_cen-((pr_mini_cen - pr_mini_min)>>1)-4)
#define edge_half_right (pr_mini_cen+((pr_mini_max - pr_mini_cen)>>1)+4)

#define edge_size(cnt,ref) ((cnt <= (ref<<1))?2:1)

#define dirRnoseek true
#define dirLnoseek false
#define dirRseek   false
#define dirLseek   true

// Training Time Benchmarks
// Time from put of Stepper control and step val until first Step occurs
#define C_PR_STEPPER_START_CYC 20  // normalized to STEP_CYC: 2**15 > 28000 actual measured time
// Time BTW Each stepper step
#define C_PR_STEPPER_PER_STEP_CYC 1024 // 2**10 > 768 actual measured 
// Time for 2 GCR writes and one io_sleep
#define C_THREAD_SLEEP_P_2GCR_CYC 128 // normalized to STEP_CYC: 2**17 > 110080 actual measured
// Time For a Hi BER Measurement as executed by BER Sequencer (post GCR; operation only)
#define C_BER_LO_SLEEP_CNT 1  // Normalized to C_THREAD_SLEEP_P_2GCR_CYC: 2**(7+3) >= 262K
// Time For a Hi BER Measurement as executed by BER Sequencer (post GCR; operation only)
#define C_BER_HI_SLEEP_CNT 4  // Normalized to C_THREAD_SLEEP_P_2GCR_CYC: 2**(7+5) >= 1048K
// Time For a Hi BER Measurement as executed by BER Sequencer (post GCR; operation only)
#define C_BER_SEEK_SLEEP_CNT 1  // Normalized to C_THREAD_SLEEP_P_2GCR_CYC: 2**(7+2) >= 131K

uint32_t G_SLEEP_QUAD;
uint32_t G_SLEEP_MIN;

// Array Definitions
//static const int Didx[2] = {prDns_i, prDew_i};
//static const int Eidx[2] = {prEns_i, prEew_i};
//static const t_seek seekidx[2] = {noseekNS, noseekEW};

// rx_berpl_sample_sel selection values
// total UI count = 16K * 2^<select>
const int ber_sel_done = 0x3; // Post-DDC error check period select. Equal or greater than hiBE version.
const int ber_sel_short = 0x1;//2; // Selects 64K (xxx132K) UI. Divided for split phase
const int ber_sel_loBE_final = 0x2;//3; // Selects 128K UI. Divided for split phase
const int ber_sel_hiBE_final = 0x5; // Selects 512K UI. Divided for split phase
int G_ber_sel_short;
int G_ber_sel_final;
int G_io_threads;

/////////////////////////////////////////////////////////////////////////////////
// FUNCDOC: pr_seek_ber
// multi-purpose eye sample position shifting and testing
// Implements single step iterations to satisfy phase rotator gray code
// Note: There's an engineering decision here to check the BER count *after* the PR movement. Yes after.
//       The motivation is to return a value which encompases the post-movement position. It should be understood
//       that this value may include a longer term measurement from the last PR position depending on the time
//       elapsed outside the function before a repeat call.
//       This is deemed more conservative because it 1) captures the newly positioned sample and
//       2) provides hysteresis of diminishing error when a sub-threshold rate is sought
// parms:
// - Estep   : Step size for edge mini-PR to take
// - Dstep   : Step size for data mini-PR to take
// - pr_vals: 4 int array containing values of MINI-PR in order of NS Data; NS Edge; EW Data; EW Edge
//--- dirL1R0 -- noBER -- seek_edge --- ACTION --- (IF PR MAX or PR MIN exceeded return max_eye) -------------------
//    FALSE      FALSE    noseek{ns/ew} Subtract D from selected DATA PR and add E to EDGE; Reset & Perform BER check
//    FALSE      FALSE    doseek        Add D to both DATA PR and subtract E from EDGE PR; Perform BER check
//    FALSE      TRUE     noseek{ns/ew} Subtract D from selected DATA PR and add E to EDGE; Do NOT run BER check
//    FALSE      TRUE     doseek        Add D to both DATA PR and subtract E from EDGE PR; Do NOT run BER check
//    TRUE       FALSE    noseek{ns/ew} Add D to selected DATA PR and subtract E from EDGE; Reset & Perform BER check
//    TRUE       FALSE    doseek        Subtract D from both DATA PR and add E to EDGE PR; Perform BER check
//    TRUE       TRUE     noseek{ns/ew} Add D to selected DATA PR and subtract E from EDGE ; Do NOT run BER check
//    TRUE       TRUE     doseek        Subtract D from both DATA PR and add E to EDGE PR; Do NOT run BER check
// Return value:
//   -- An error count value if noBER = false else seek_error
//   -- A constant == seek_error if the mini-PR extents will be violated
int pr_seek_ber (t_gcr_addr* gcr_addr, t_bank bank, unsigned int Dstep, unsigned int Estep, bool dirL1R0, bool noBER,
                 t_seek seek_edge, int* pr_vals)
{

    int ber_count = 0;
    int pr_temp[4];
    int pr_sel_both = 0b11; // Enable both phases by default
    bool dirL = dirL1R0 ^ (seek_edge != doseek);

    int ns_edge_go = 0;
    int ns_data_go = 0;
    int ew_edge_go = 0;
    int ew_data_go = 0;

    // BERPL Sequencer requires an even starting value since it truncates odd remainders
    int Dstep_val = Dstep;// + step_val_plus;
    int Estep_val = Estep;

    // Disable NS Phase (0) move or EW Phase (1) move
    // PSL seek_edge_noseekNS
    if (seek_edge == noseekNS)
    {
        pr_sel_both = pr_sel_both ^ 0b01;
    }

    // PSL seek_edge_noseekEW
    if (seek_edge == noseekEW)
    {
        pr_sel_both = pr_sel_both ^ 0b10;
    }

    if (seek_edge != noseekEW)
    {
        ns_data_go = Dstep_val;
        ns_edge_go = Estep_val;
    }

    if (seek_edge != noseekNS)
    {
        ew_data_go = Dstep_val ;
        ew_edge_go = Estep_val;
    }

#if IO_DEBUG_LEVEL > 1
    set_debug_state(0x800A | (Dstep_val << 4)); // DEBUG - DDC Setup pr_seek_br
#endif

    // PSL dirL
    if (dirL)
    {
        // if dir LEFT
        pr_temp[prDns_i] = pr_vals[prDns_i] - ns_data_go;//
        pr_temp[prDew_i] = pr_vals[prDew_i] - ew_data_go;//
        pr_temp[prEns_i] = pr_vals[prEns_i] + ns_edge_go;//
        pr_temp[prEew_i] = pr_vals[prEew_i] + ew_edge_go;//
        Dstep_val = 0 - Dstep_val;
    }
    else
    {
        pr_temp[prDns_i] = pr_vals[prDns_i] + ns_data_go;//
        pr_temp[prDew_i] = pr_vals[prDew_i] + ew_data_go;//
        pr_temp[prEns_i] = pr_vals[prEns_i] - ns_edge_go;//
        pr_temp[prEew_i] = pr_vals[prEew_i] - ew_edge_go;//
        Estep_val = 0 - Estep_val;
    } // else dir RIGHT

    // Bounds Checking
    //TODO JG: Consider removing this
    //  if ((((pr_temp[prDns_i] > pr_mini_max) || (pr_temp[prDew_i] > pr_mini_max) || (Dstep==0)) &&
    //       ((pr_temp[prEns_i] < pr_mini_min) || (pr_temp[prEew_i] < pr_mini_min) || (Estep==0))) ||
    //      (((pr_temp[prDns_i] < pr_mini_min) || (pr_temp[prDew_i] < pr_mini_min) || (Dstep==0)) &&
    //       ((pr_temp[prEns_i] > pr_mini_max) || (pr_temp[prEew_i] > pr_mini_max) || (Estep==0)))) { // Coverage may require an initial pr_*_edge offset > 1/2 max
    //    return seek_error; // New position has exceeded the PR max/min and is a bad request. A measurement CANNOT be taken. Return error value.
    // NOTES:
    // This looks like it prevents full utilization of PR space by combining NS and EW checks. It works with these two case
    // A - When stepping by >1 step then seek is expected and both data phases must move in sync or risk wiping out prior alignment work.
    // B - When a non-seek phase mode is enabled then the conditionals for *_go variable will truncate an unmovable phase step so the other can reach max
    //}

    // Write the target phases for BER Seq to manipulate.
    put_ptr_fast(gcr_addr, rx_berpl_seq_pr_sel_addr, rx_berpl_seq_pr_sel_endbit, pr_sel_both);
    uint32_t Dstep_2c = IntToTwosComp(Dstep_val, rx_mini_pr_step_data_adj_width);

    // PSL noBER
    if (!noBER)
    {
        uint32_t ber_status;

        // PSL seek_edge
        if (((seek_edge == doseek) || (seek_edge == noseek)) && (Estep > 0))
        {
            // During doseek or noseek, it is assumed that this is DDC and edges are already aligned. Make sure that alignment does not change
            // by inadvertently stopping one of them at an extent.
            // Calculate the overage
            int Estep_limit_ns = limit(pr_temp[prEns_i], pr_mini_min, pr_mini_max);
            Estep_limit_ns -= pr_temp[prEns_i];
            int Estep_limit_ew = limit(pr_temp[prEew_i], pr_mini_min, pr_mini_max);
            Estep_limit_ew -= pr_temp[prEew_i];
            bool limit_ns = abs(Estep_limit_ns) > abs(Estep_limit_ew);
            // Reduce the DIRECTIONAL Estep_val by the largest diff
            Estep_val += (limit_ns) ? Estep_limit_ns : Estep_limit_ew;
            uint32_t Estep_2c = IntToTwosComp(Estep_val, rx_mini_pr_step_edge_adj_width);

            // TODO JG?: Need to expose the berpl_lane_mode to regs for random counting use.
            uint32_t adj_val = Dstep_2c << rx_mini_pr_step_data_adj_shift | Estep_2c << rx_mini_pr_step_edge_adj_shift;
            put_ptr_fast(gcr_addr, rx_mini_pr_step_data_edge_adj_full_reg_addr, rx_mini_pr_step_data_edge_adj_full_reg_endbit,
                         adj_val);
            uint32_t runid = (rx_mini_pr_step_a_ns_edge_run_mask | rx_mini_pr_step_a_ew_edge_run_mask);

            // PSL seek_edge_bank_a
            if (bank != bank_a)
            {
                runid = (rx_mini_pr_step_b_ns_edge_run_mask | rx_mini_pr_step_b_ew_edge_run_mask);
            }

            put_ptr_fast(gcr_addr, rx_mini_pr_step_run_done_full_reg_addr, rx_mini_pr_step_run_done_full_reg_endbit, runid);
            pr_vals[prEew_i] += Estep_val;
            pr_vals[prEns_i] += Estep_val;
        }
        else
        {
            uint32_t adj_val = Dstep_2c << rx_mini_pr_step_data_adj_shift;
            put_ptr_fast(gcr_addr, rx_mini_pr_step_data_edge_adj_full_reg_addr, rx_mini_pr_step_data_edge_adj_full_reg_endbit,
                         adj_val);
        }

#if IO_DEBUG_LEVEL > 1
        set_debug_state(0x8002 ); // DEBUG - DDC pr_seek sleep
#endif

        // Run BER-PL PR Control Sequencer. It independently resolves the enabled bank and returns the final data adjustment
        // A BER Count == 0 means no error threshold was detected within the search window: A likely max_eye condition
        put_ptr_fast(gcr_addr, rx_berpl_seq_run_addr, rx_berpl_seq_run_endbit, 1);
        bool single_sleep = true;

        do
        {
            // PSL single_sleep
            if (single_sleep )
            {
                if (mem_pg_field_get(rx_running_eq_eval) == 0)
                {
                    io_sleep(get_gcr_addr_thread(gcr_addr));
                }
            }

            single_sleep = (G_io_threads < 2);

            ber_status = get_ptr(gcr_addr, rx_berpl_final_adj_done_alias_addr, rx_berpl_final_adj_done_alias_startbit,
                                 rx_berpl_final_adj_done_alias_endbit);
        }
        while ((ber_status & rx_berpl_seq_done_mask) == 0);

        int pr_adj = TwosCompToInt( ((ber_status & rx_berpl_final_adj_mask) >> rx_berpl_final_adj_shift),
                                    rx_berpl_final_adj_width);
#if IO_DEBUG_LEVEL > 1
        set_debug_state(0x8003 ); // DEBUG - DDC pr_seek status
#endif

        if (seek_edge != noseekNS)
        {
            pr_vals[prDew_i] += pr_adj;
        }

        if (seek_edge != noseekEW)
        {
            pr_vals[prDns_i] += pr_adj;
        }

        ber_count = get_ptr(gcr_addr, rx_berpl_count_addr, rx_berpl_count_startbit, rx_berpl_count_endbit);

        // PSL max_eye
        if (  (abs(pr_adj) >= (abs(Dstep_val))) && (Dstep != 0) && (ber_count == 0))
        {
            // Traversed requested range and no error reversal detected
            ber_count = max_eye;
        }

        // PSL ber_seek_error
        if ((pr_adj == 0) && (ber_count != 0))
        {
            // The BER Sequencer algorithm will only return 0 when no other position will satisfy the BER Error Threshold
            ber_count = seek_error;
        }
    }
    else
    {
        ber_count = seek_error; // BER is not requested just return bad value
        // PR Stepping out BER context is no longer implemented
    }

    return ber_count;
} //pr_seek_ber()


/////////////////////////////////////////////////////////////////////////////////
// FUNCDOC: pr_recenter
// multi-purpose mini phase rotator position recentering
// Returns Data and Edge PR positions independently for NS and EW to the original position plus a signed offset.
// If the NS and EW values are equal then both sets will be batched into a single operation
// parms:
// - pr_vals: 4 uint array containing values of MINI-PR in order of NS Data; NS Edge; EW Data; EW Edge
// - Esave  : 2 uint array containing original PR position packed into NS / EW format
// - Dsave  : 2 uint array containing original PR position packed into NS / EW format
// - Doff   : 2 int array containing signed offset from orignal start if desired. Otherwise set = 0
// - Eoff   : 1 int provided for implementing PR Bias. Must be common between both edges. Split values not supported.
// Return value:
//  rc_pass    : No errors on recenter
//  rc_error   : Bad recenter request
int  pr_recenter(t_gcr_addr* gcr_addr, t_bank bank, int* pr_vals, uint32_t* Esave, uint32_t* Dsave, int* Doffset,
                 int Eoffset)
{
    int i;
    int status = rc_pass;
    int Eleft[2], Dleft[2];
    uint32_t adj_val[2];

    for (i = 0; i < 2; i++)
    {
        int Eidx, Didx;

        if (i == 0 )
        {
            Didx = prDns_i;
            Eidx = prEns_i;
        }
        else
        {
            Didx = prDew_i;
            Eidx = prEew_i;
        }

        Eleft[i] = (Esave[i] - pr_vals[Eidx]) + (Eoffset);
        Dleft[i] = (Dsave[i] - pr_vals[Didx]) + (Doffset[i]);
        int lim_chk = pr_vals[Eidx] + Eleft[i];
        lim_chk -= limit(lim_chk, pr_mini_min, pr_mini_max);

        if (lim_chk != 0)
        {
            status = rc_warning;
        }

        Eleft[i] -= lim_chk;
        pr_vals[Eidx] += Eleft[i];
        lim_chk = pr_vals[Didx] + Dleft[i];
        lim_chk -= limit(lim_chk, pr_mini_min, pr_mini_max);

        if (lim_chk != 0)
        {
            status = rc_warning;
        }

        Dleft[i] -= lim_chk;
        pr_vals[Didx] += Dleft[i];
        uint32_t Eleft_2c = IntToTwosComp(Eleft[i], rx_mini_pr_step_edge_adj_width);
        uint32_t Dleft_2c = IntToTwosComp(Dleft[i], rx_mini_pr_step_data_adj_width);
        set_debug_state(0xF800 | ((Dleft[i] & 0xF) << 4 | (Eleft[i] & 0xF)), 3); // DEBUG - pr_recenter step target
        adj_val[i] = Dleft_2c << rx_mini_pr_step_data_adj_shift | Eleft_2c;
    }

    int passes = 2;
    uint32_t runid;
    runid = (rx_mini_pr_step_a_ns_data_run_mask | rx_mini_pr_step_a_ns_edge_run_mask) |
            (rx_mini_pr_step_a_ew_data_run_mask | rx_mini_pr_step_a_ew_edge_run_mask) ;

    // PSL bank_a
    if (bank != bank_a)
    {
        runid = (rx_mini_pr_step_b_ns_data_run_mask | rx_mini_pr_step_b_ns_edge_run_mask) |
                (rx_mini_pr_step_b_ew_data_run_mask | rx_mini_pr_step_b_ew_edge_run_mask) ;
    }

    // PSL adj_val_compare
    if (adj_val[0] == adj_val[1])
    {
        passes = 1;
    }

    for (i = 0; i < passes; i++)
    {
        put_ptr_fast(gcr_addr, rx_mini_pr_step_data_edge_adj_full_reg_addr, rx_mini_pr_step_data_edge_adj_full_reg_endbit,
                     adj_val[i]);

        // PSL passes_gt_1
        if (passes > 1)
        {
            runid = (rx_mini_pr_step_a_ns_data_run_mask | rx_mini_pr_step_a_ns_edge_run_mask) >> i;

            // PSL passes_gt_1_bank_a
            if (bank != bank_a)
            {
                runid = (rx_mini_pr_step_b_ns_data_run_mask | rx_mini_pr_step_b_ns_edge_run_mask) >> i;
            }
        }

        put_ptr_fast(gcr_addr, rx_mini_pr_step_run_done_full_reg_addr, rx_mini_pr_step_run_done_full_reg_endbit, runid);
        // Run status check if stepcount is large
        uint32_t longest = max(abs(Eleft[i]), abs(Dleft[i]));
        bool single_sleep = (G_io_threads < 2);

        // PSL pr_stepper_start_cyc_compare
        if ( C_PR_STEPPER_START_CYC < (longest << 1) )
        {
            uint32_t donestat;
            uint32_t donemask = rx_mini_pr_step_a_done_alias_mask | rx_mini_pr_step_b_done_alias_mask;
            uint32_t loop_count = 0;

            // PSL pr_stepper_start_cyc_i_eq_1
            if (i == 1)
            {
                //sleep on 2nd pass only
                if (single_sleep )
                {
                    if (mem_pg_field_get(rx_running_eq_eval) == 0)
                    {
                        io_sleep(get_gcr_addr_thread(gcr_addr));
                    }
                }
            }

            // PSL donestat
            do
            {
                donestat = get_ptr(gcr_addr, rx_mini_pr_step_run_done_full_reg_addr, rx_mini_pr_step_run_done_full_reg_startbit,
                                   rx_mini_pr_step_run_done_full_reg_endbit);
                loop_count++;
            }
            while ((donestat & donemask) == 0);
        }
    }

    return status;
}


void set_ddc_err (t_gcr_addr* gcr_addr, t_bank bank, int lane, int* pr_vals, uint32_t* Esave, uint32_t* Dsave)
{
    mem_pl_bit_set(rx_bad_eye_opt_width, lane);
    //EWM265038: This fail is for rx_bist not abort or ddc internal error: mem_pl_bit_set(rx_ddc_fail, lane);
    //Disable BER
    put_ptr(gcr_addr, rx_ber_en_addr, rx_ber_en_startbit, rx_ber_en_endbit, 0, read_modify_write);
    // Disable the per-lane counter
    put_ptr(gcr_addr, rx_berpl_count_en_addr, rx_berpl_count_en_startbit, rx_berpl_count_en_endbit, 0, read_modify_write);
    // Reset BER compare A/B data banks to PRBS
    put_ptr(gcr_addr, rx_berpl_exp_data_sel_addr, rx_berpl_exp_data_sel_startbit, rx_berpl_exp_data_sel_endbit, 0,
            read_modify_write);
    put_ptr(gcr_addr, rx_berpl_mask_mode_addr, rx_berpl_mask_mode_startbit, rx_berpl_mask_mode_endbit, 0,
            read_modify_write);
    put_ptr(gcr_addr, rx_err_trap_mask_addr, rx_err_trap_mask_startbit, rx_err_trap_mask_endbit, 0 , read_modify_write);
    /* int ds = 1; */
    /* int es = 0; */
    /* // make 1 data step */
    /* ber_reported = pr_seek_ber(gcr_addr, bank, ds, es, false, false, noseek, pr_active, ber_count); */
    int offsets[2] = {0, 0};

    // PSL pr_recenter
    pr_recenter(gcr_addr, bank, pr_vals, Esave, Dsave, offsets, 0);
    // Run this twice in case D/E get out of whack and need to restore in opposite directions
    pr_recenter(gcr_addr, bank, pr_vals, Esave, Dsave, offsets, 0);

}

/////////////////////////////////////////////////////////////////////////////////
// FUNCDOC: ddc_seek_loop
// Implements DDC algoritm state transitions 8020 thru 8026
// Returns pass/fail
// Stores Data and Edge PR positions and the reported BER in original parameter pointers
//   i.e. when the function exits, the PRs must be left in their identified edge position
// Function expects that starting PR position is defined as a solid low/no-error sample position
// parms:
// - pr_vals: 4 uint array containing values of MINI-PR in order of NS Data; NS Edge; EW Data; EW Edge
// - seekdir: direction a defined in pr_seek_ber
// - ber_count: target error count for valid edge threshold
// - ber_reported: direct response from BER check hardware
// - recal: Used to disable the quad phase handling for Initial Training execution speed
// Return value:
//  False   : One of the defined errors occurred.
int ddc_seek_loop (t_gcr_addr* gcr_addr, t_bank bank, int* pr_vals, bool seekdir, int* ber_reported, int ber_count,
                   bool recal)
{
    uint32_t ds;
    uint32_t es;//edge_size(*ber_reported,ber_count);
    bool revSeekDir = !seekdir;
    int ber_lim = (ber_count == 1) ? 1 : ber_count >> 1;
    int lane = get_gcr_addr_lane(gcr_addr);
    uint32_t Dsave[2];
    uint32_t Esave[2];

    // Store starting position for search. This implies, and is defined as, the starting sample prior to DDC
    Esave[0] = pr_vals[prEns_i];
    Dsave[0] = pr_vals[prDns_i];
    Esave[1] = pr_vals[prEew_i];
    Dsave[1] = pr_vals[prDew_i];

    set_debug_state(0x8020 ); // DEBUG DDC Start Seek Edge

    *ber_reported = 0;
    //Initialize sat_thresh to max value to begin error search
    uint32_t ber_sel_val = ber_lim << (rx_berpl_sat_thresh_shift - (16 - rx_berpl_sat_sample_sel_alias_width))
                           | (G_ber_sel_short << (rx_berpl_sample_sel_shift - (16 - rx_berpl_sat_sample_sel_alias_width)));
    // Reset the timer to short timeout fir quick position search and start edge search
    put_ptr_fast(gcr_addr, rx_berpl_sat_sample_sel_alias_addr, rx_berpl_sat_sample_sel_alias_endbit, ber_sel_val);

    // 3: Search RIGHT Edge //////////////////////////////////////////////////
    // Leapfrog Edge and Data PRs to find initial noise.
    // In FAST (Gen1-3) mode, this is the only operation and Quadrant Phase steps are skipped
    // First move Data out and search
    // Then move Edge PR to last ds then iterate to next ds width.
    // Plan to use all available edge steps by using initial data adj=15+edge but limit based on remaining space.
    // Pre-seek Calculate the edge steps available
    int es_diff = (Esave[0] - Esave[1]);
    es = (seekdir == dirRseek) ? Esave[0] : (pr_mini_max - Esave[0]); // NS is used as a ballpark. Refined later.
    es_diff = (seekdir == dirRseek) ? es_diff : 0 - es_diff;
    es_diff = max(es_diff, 0);
    es -= es_diff;
    ds = 15 + es;
    int ds_dir = (seekdir == dirRseek) ?  ds : 0 - ds;
    //Calculate next data step value of data + edge from current Data
    int lim_chkN = pr_vals[prDns_i] + ds_dir;
    int lim_chkE = pr_vals[prDew_i] + ds_dir;
    int lim_diff = lim_chkN - lim_chkE;
    lim_diff = (seekdir == dirLseek) ?  lim_diff : 0 - lim_diff;
    lim_diff = max(lim_diff, 0);
    // Subtract data skew
    ds -= lim_diff;
    //Calculate excess steps of next data
    int lim_max = limit(lim_chkN, pr_mini_min, pr_mini_max);
    // Reduce the Data overage by the limits and use it to shrink the seek range
    lim_chkN -= lim_max;
    int overage = abs(lim_chkN);

    // PSL es_overage
    if (es >= overage)
    {
        es = overage;
    }
    else
    {
        ds -= (overage - es);
    }

    //Calculate Approximate margin for data steps, used later.
    int ds_max = ds - es;
    //Now set directional edge and data Pre-position values
    // PSL es_seekdir
    es = (seekdir == dirRseek) ?  0 - es : es;
    int noffset[3] = {es, es, es};
    pr_recenter(gcr_addr, bank, pr_vals, Esave, Dsave, noffset, noffset[2]);
    //int seek_status;
    G_SLEEP_MIN = C_BER_SEEK_SLEEP_CNT;
    //  do {
    *ber_reported = pr_seek_ber(gcr_addr, bank, ds, 0, seekdir, false, doseek, pr_vals);
    // If MAX EYE is reached while < ber_count it means that quad phase balancing is not possible nor required due to huge eye or other error.

    // PSL max_eye
    if ((*ber_reported == max_eye) )
    {
        mem_pl_field_put(rx_ddc_measure_limited, lane, 1);
        set_debug_state(0x8012 ); // DEBUG max eye check
        *ber_reported = 0;
    }

//  while ((*ber_reported == max_eye) && (seek_status==rc_pass) && (*ber_reported != seek_error));
#if IO_DEBUG_LEVEL > 3
    set_debug_state(0x1000 | ((*ber_reported & 0xFF) << 4 | (seek_status & 0xF))); // DEBUG - DDC pr_seek status
#endif

    // PSL recal
    if (recal)   // Recal mode Conditional - Skip Quad Phase until after init
    {

        ber_sel_val = ber_lim << (rx_berpl_sat_thresh_shift - (16 - rx_berpl_sat_sample_sel_alias_width))
                      | (G_ber_sel_final << (rx_berpl_sample_sel_shift - (16 - rx_berpl_sat_sample_sel_alias_width)));
        // Reset the timer to short timeout fir quick position search and start edge search
        put_ptr_fast(gcr_addr, rx_berpl_sat_sample_sel_alias_addr, rx_berpl_sat_sample_sel_alias_endbit, ber_sel_val);

        set_debug_state(0x8021); // DEBUG - 3: Prepare Quadrant Search
        // Reduce last step movement for margin before continuing seek.
        // The ds_max term is a *rough* calculation to gauge how much range is left for noise standoff or min-EW
        // This is to avoid blindly moving into the opposing edge noise on small eyes.
        //  int ds_max = abs(pr_vals[prDns_i] - Dsave[0]) + abs(pr_vals[prEns_i] - Esave[0]);// DS-Dsave is always 0 here
        es = 0;

        // PSL ds_max_gt_five
        if ((ds_max > 5))
        {
            ds = 4;//+es;
        }
        else
        {
            ds = 2;//+es;
        }

        // PSL ds_seekdir
        ds = (seekdir == dirLseek) ?  ds : 0 - ds;
        //es = (seekdir == dirLseek)?  es : 0-es;
        uint32_t Dint[2];
        uint32_t Eint[2];
        Eint[0] = pr_vals[prEns_i];
        Dint[0] = pr_vals[prDns_i];
        Eint[1] = pr_vals[prEew_i];
        Dint[1] = pr_vals[prDew_i];
        int Doffset[2] = {ds, ds};
        pr_recenter(gcr_addr, bank, pr_vals, Eint, Dint, Doffset, es);
        ds = 0;
        /* Skip BACKOFF Paranoia Test for Evaluation
        *****************************
          *ber_reported = pr_seek_ber(gcr_addr, bank, ds, es, seekdir, false, noseek, pr_vals);
          if ((*ber_reported > ber_lim)){ //This is pure paranoia and may not be coverable except on really bad channel
              //HW552377 After increased 5 step standoff...skip paranoia completely if we reach starting position.
              set_debug_state(0x8015 ); // DEBUG DDC Large Edge Noise Warning
              if (ds_max < 5) {
               mem_pl_field_put(rx_ddc_small_eye_warning, lane, 1);
               ds = 1;
              }
              else { ds = 2; }
              Eint[0] = pr_vals[prEns_i];
              Dint[0] = pr_vals[prDns_i];
              Eint[1] = pr_vals[prEew_i];
              Dint[1] = pr_vals[prDew_i];
              ds = (seekdir == dirLseek)?  ds : 0-ds;
              noffset[0]=ds;
              noffset[1]=ds;
              pr_recenter(gcr_addr, bank, pr_vals, Eint, Dint, noffset, noffset[2]);
          }
        ****************************
        */


        // The threshold reset is placed here to allow for the previous step reduction to move the sample out of the fuzz
        // It also keeps the register write outside of the phase loop
        // put_ptr(gcr_addr, rx_berpl_sat_thresh_addr, rx_berpl_sat_thresh_startbit, rx_berpl_sat_thresh_endbit, (0x1<<rx_berpl_sat_thresh_width) - 1, read_modify_write );
        put_ptr(gcr_addr, rx_berpl_mask_mode_addr, rx_berpl_mask_mode_startbit, rx_berpl_mask_mode_endbit, 1,
                read_modify_write);
        // Save Safe Sample position for returning a quad to ensure same edge dynamics for all quad searches
        // ALL ARRAYS are index with 0 = NS-based values, 1 = EW-based values
        Esave[0] = pr_vals[prEns_i];
        Esave[1] = pr_vals[prEew_i];
        Dsave[0] = pr_vals[prDns_i];
        Dsave[1] = pr_vals[prDew_i];
        uint32_t Dedge[2];
        Doffset[0] = 0;
        Doffset[1] = 0;
        uint32_t quad_mask[2] = {0x5, 0xA};
        t_seek   seek_quad[2] = {noseekNS, noseekEW};

        G_SLEEP_MIN = G_SLEEP_QUAD;
        int i;

        for (i = 1; i >= 0; i--)
        {
            // quad_mask assumes N samples aligned to data_pipe(0) and counts N and S together. Smaller granularity is not practical since they can't be aligned independently
            put_ptr(gcr_addr, rx_err_trap_mask_addr, rx_err_trap_mask_startbit, rx_err_trap_mask_endbit, quad_mask[i] ,
                    read_modify_write);
            //put_ptr(gcr_addr, rx_berpl_seq_pr_sel_addr, rx_berpl_seq_pr_sel_startbit, rx_berpl_seq_pr_sel_endbit, quad_mask & 0x1, read_modify_write );
            set_debug_state(0x8023); // DEBUG - Set Quadrant mask

            // 4: Search RIGHT until above threshold ///////////////////////////////////////////////////
            // Set target search width = remaining Data PR Steps
            ds = (seekdir == dirLseek) ? Dsave[i] : (pr_mini_max - Dsave[i]);
            es = 0;
            *ber_reported = pr_seek_ber(gcr_addr, bank, ds, es, revSeekDir, false, seek_quad[i], pr_vals);

            // If MAX EYE is reached while < ber_count it means that quad phase balancing is not possible nor required due to huge eye or other error.
            // PSL max_eye
            if ((*ber_reported == max_eye) )
            {
                mem_pl_field_put(rx_ddc_measure_limited, lane, 1);
                set_debug_state(0x8012 ); // DEBUG max eye check
                *ber_reported = 0;
            }

            set_debug_state(0x8026); // DEBUG - 4: Search LEFT

            if (i == 1)
            {
                // Save identified edge location
                Dedge[1] = pr_vals[prDew_i];
                // Reposition to safe sample for quad search ONLY for first quadrant (EW)
                pr_recenter(gcr_addr, bank, pr_vals, Esave, Dsave, Doffset, 0);
            }
            else
            {
                Dedge[0] = pr_vals[prDns_i];
            }
        }

        // A final repositioning of the EW PRs to their actual edge value is required in order to
        //   A) Return them to the parent
        //   B) retain code fidelity that pr_vals always matches hardware
        // Only EW needs to change here because NS finished on it's edge but EW had been return to "Safe Sample" during NS search.
        pr_recenter(gcr_addr, bank, pr_vals, Esave, Dedge, Doffset, 0);
        put_ptr(gcr_addr, rx_berpl_mask_mode_addr, rx_berpl_mask_mode_startbit, rx_berpl_mask_mode_endbit, 0,
                read_modify_write);
        put_ptr(gcr_addr, rx_err_trap_mask_addr, rx_err_trap_mask_startbit, rx_err_trap_mask_endbit, 0 , read_modify_write);
        io_sleep(get_gcr_addr_thread(gcr_addr));
    } // Recal mode Conditional - Skip Quad Phase

    return pass_code;
}

////////////////////////////////////////////////////////////////////////////
// Run DDC on a lane and update historical width
// Adjust *DATA* NS and EW mini-rotators independently to accomplish quad phase adjustment
// PHY state prior to DDC adjustment
//   Lane is fully powered on
//   CDR tracking alt bank in external data mode (bank_sel = B on initial cal), all quadrant
//   Eye on the calibrated bank has been improved by initial VGA, Peaking, Edge Offset, LTE, DFE, and various latch offset Calibrations.
//   Quad Phase Adjust has adjusted *EDGE* mini rotators to fix phase error
//   Base line data sample error rate must be lower than rx_ddc_min_err_lim or function will exit with error..
//   All register modes are pre-set and stable
// PHY state at the completion of DDC adjustment
//   Mini-PR offset positions are applied by shifting via rx_{ab}_pr_{ns/ew}_data.
//   Historic minimums are written to rx_ddc_hyst_left_edge and rx_ddc_hyst_right_edge.
//   CDR tracking is left as is.
int eo_ddc(t_gcr_addr* gcr_addr, t_bank bank, bool recal, bool recal_dac_changed)
{
    set_debug_state(0x8000); // DEBUG - DDC Start
    int abort_status = pass_code;
    int pr_active[4]; // All four PR positions packed in as: {Data NS, Edge NS, Data EW, Edge EW}
    int cdr_status = 1;
    G_io_threads = img_field_get(ppe_num_threads);

    // 2: Set initial values
    read_active_pr(gcr_addr, bank, pr_active);

    uint32_t Dsave[2];
    Dsave[0] = pr_active[prDns_i];
    Dsave[1] = pr_active[prDew_i];
    uint32_t Esave[2];
    Esave[0] = pr_active[prEns_i];
    Esave[1] = pr_active[prEew_i];

    int lane = get_gcr_addr_lane(gcr_addr);

    // Removed: Check CDR lock status before clearing sticky bit to start test.
    // CDR lock check is not relevant

    // See Workbook Table x.x.1.1 "rx_ddc_min_err_lim Register Decode" in DDC Section
    int ddc_min_err_lim = mem_pg_field_get(rx_ddc_min_err_lim);
    int ddc_ber_period_sel = mem_pg_field_get(rx_ddc_ber_period_sel);
    int ber_count;
    // While edge searching: This upper limit allows for roughly 3x10^-3 error rate between PR shifts occurring every 50K UI or so
    // The official edge point: When used in conjuction with the ber_sel_short it saturates the count at a rate of 9x10^-3 errors
    int ber_sel_modifier = 0;

    // PSL ddc_ber_period_sel
    if (ddc_ber_period_sel == 1)
    {
        ber_sel_modifier = 1;
    }
    else if (ddc_ber_period_sel == 2)
    {
        ber_sel_modifier = 2;
    }

    G_ber_sel_short = ber_sel_short + ber_sel_modifier;
    G_ber_sel_final = ber_sel_loBE_final + ber_sel_modifier;
    G_SLEEP_QUAD = C_BER_LO_SLEEP_CNT;

    // PSL ddc_min_err_lim
    switch (ddc_min_err_lim)
    {
        case 0:
            ber_count = 1;
            break;

        case 1:
            ber_count = 2;
            break;

        case 2:
            ber_count = 4;
            break;

        case 3:
            ber_count = 8;
            G_ber_sel_final = ber_sel_hiBE_final;
            G_SLEEP_QUAD = C_BER_HI_SLEEP_CNT;
            break;

        case 4:
            ber_count = 16;
            G_ber_sel_final = ber_sel_hiBE_final;
            G_SLEEP_QUAD = C_BER_HI_SLEEP_CNT;
            break;

        case 5:
            ber_count = 24;
            G_ber_sel_final = ber_sel_hiBE_final;
            G_SLEEP_QUAD = C_BER_HI_SLEEP_CNT;
            break;

        case 6:
            ber_count = 32;
            G_SLEEP_QUAD = C_BER_HI_SLEEP_CNT;
            G_ber_sel_final = ber_sel_hiBE_final;
            break;

        default: // This setting should achieve a 33% confidence at 1/3 the checking duration
            ber_count = 2;
            break;
    }

    //int ber_lim=ber_count>>1; //NOTE: This must match function usage of ber_count>>1

// Set BER to compare A/B data banks instead of PRBS
    //Replaced with FAST: put_ptr(gcr_addr, rx_berpl_exp_data_sel_addr, rx_berpl_exp_data_sel_startbit, rx_berpl_exp_data_sel_endbit, 0, read_modify_write);
    // Enable the per-lane counter and reset the main count.
    //Replaced with FAST: put_ptr(gcr_addr, rx_berpl_count_en_addr, rx_berpl_count_en_startbit, rx_berpl_count_en_endbit, 1, read_modify_write);
    // This FAST right clears out the berpl_exp_data to 0 as stated
    uint32_t ber_enable_val = 1 << (rx_berpl_count_en_shift - (13 - rx_berpl_cnt_en_exp_sel_alias_width)) ;
    put_ptr_fast(gcr_addr, rx_berpl_cnt_en_exp_sel_alias_addr, rx_berpl_cnt_en_exp_sel_alias_endbit, ber_enable_val);


    //Clear out any prior bad measurement indicator.
    mem_pl_field_put(rx_ddc_measure_limited, lane, 0);
    int ber_reported;
    /* //We're Skipping initial search since BER Sequencer will return 0 in event of a fail
    uint32_t ber_sel_val = (ber_count << (rx_berpl_sat_thresh_shift-(16-rx_berpl_sat_sample_sel_alias_width)))
      | (G_ber_sel_final << (rx_berpl_sample_sel_shift-(16-rx_berpl_sat_sample_sel_alias_width)));
    set_debug_state(0x8001 | (ber_sel_val << 4)); // DEBUG - DDC Setup

    uint32_t ds = 0;

    //Initialize sat_thresh to max value to begin error search
    put_ptr_fast(gcr_addr, rx_berpl_sat_sample_sel_alias_addr, rx_berpl_sat_sample_sel_alias_endbit, ber_sel_val);
    // Initial error sense provided by pr_seek_ber because it is convenient with a ds = 0
    int ber_reported = pr_seek_ber(gcr_addr, bank, ds, ds, dirRseek, false, noseek, pr_active);

    // NOTE: Basic stipulation that the opening eye position must be error free.
    // NOTE: DDC Does not implement a search algorithm to locate an arbitrary starting eye center position
    //       but the BER Sequencer will identify the lack of an error free sampling position during intial seek.
    if ((ber_reported > ber_count) && (ber_reported != max_eye)) {
      set_ddc_err (gcr_addr, bank, lane, pr_active, Esave, Dsave);
      set_debug_state(0x8081 | (ber_reported << 8)); // DEBUG: Algorithm error. Problem with data eye.
      set_fir(fir_code_bad_lane_warning);
      return warning_code;
    } // ber_reported > ber_count
    */

    // 3: Search RIGHT Edge //////////////////////////////////////////////////
    abort_status |= ddc_seek_loop (gcr_addr, bank, pr_active, dirRseek, &ber_reported, ber_count, recal);

    if ((ber_reported == max_eye) && (abort_status != pass_code))
    {
        // PSL set_ddc_err
        set_ddc_err (gcr_addr, bank, lane, pr_active, Esave, Dsave);
        set_debug_state(0x8087); // DEBUG: Algorithm error. Lane broke during measure
        //EWM265038: This fail is for rx_bist not abort or ddc internal error: set_fir(fir_code_bad_lane_warning);
        return warning_code;
    }

    // 5: Calculate mini-pr and total right-side edge width
    // First each quadrant pair data and edge position is saved
    // Then find the distance traveled from the last saved center to the new edge
    // distance = data - last center + last edge - edge
    // If the algorithm is working correctly this should never be negative.
    int Dright[2];
    Dright[0] = (pr_active[prDns_i]) - Dsave[0] ;
    Dright[1] = (pr_active[prDew_i]) - Dsave[1] ;
    int Eright[2];
    Eright[0] = Esave[0] - (pr_active[prEns_i]);
    Eright[1] = Esave[1] - (pr_active[prEew_i]);
    // TODO-JG: Is error detection of negative values here neccessary? While a negative here would screw stuff up is it possible?
    int last_right_edge[2];
    last_right_edge[0] = Dright[0] + Eright[0]; // Defined as positive distance of edge right of center
    last_right_edge[1] = Dright[1] + Eright[1]; // Defined as positive distance of edge right of center

    // 6: Search LEFT first error

    set_debug_state(0x800B); // DEBUG - X: Return to center. Begin LEFT search

    // Do not enable BER until we've moved past the original start point to avoid false passes
    if ((pr_active[prDns_i] > Dsave[0]) || (pr_active[prEns_i] < Esave[0]) ||
        (pr_active[prDew_i] > Dsave[1]) || (pr_active[prEew_i] < Esave[1]))
    {
        // The left seek continually gets fooled by marginal error rates lower than 10^5 that sneak in
        // prior to advancing far enough from the right edge to avoid corrupting the error read
        int offsets[2] = {0, 0};
        // PSL pr_recenter_left_first_error
        pr_recenter(gcr_addr, bank, pr_active, Esave, Dsave, offsets, 0);
    }

    abort_status |= ddc_seek_loop (gcr_addr, bank, pr_active, dirLseek, &ber_reported, ber_count, recal);

    if ((ber_reported == seek_error) && (abort_status != pass_code))
    {
        // abort_status of rc_error on either side coupled with a seek_error on the left indicates undetectable eye.
        // PSL set_ddc_err
        set_ddc_err (gcr_addr, bank, lane, pr_active, Esave, Dsave);
        set_debug_state(0x8088); // DEBUG: Algorithm error. Lane broke during measure
        //EWM265038: This fail is for rx_bist not abort or ddc internal error: set_fir(fir_code_bad_lane_warning);
        return warning_code;
    }

    // 8: Calculate mini-pr and total left-side edge width
    // Then find the distance traveled from the last saved center to the new edge
    // distance = last center - (data) + (edge) - last edge
    // If the algorithm is working correctly this should never be negative.
    int Dleft[2];
    Dleft[0] = Dsave[0] - (pr_active[prDns_i]);
    Dleft[1] = Dsave[1] - (pr_active[prDew_i]);
    int Eleft[2];
    Eleft[0] = (pr_active[prEns_i]) - Esave[0];
    Eleft[1] = (pr_active[prEew_i]) - Esave[1];

    int last_left_edge[2];
    last_left_edge[0] = Dleft[0] + Eleft[0];
    last_left_edge[1] = Dleft[1] + Eleft[1]; // Defined as positive distance of edge left of center
    int last_left_edge_reg = (last_left_edge[0] + last_left_edge[1]) >> 1;
    int last_right_edge_reg = (last_right_edge[0] + last_right_edge[1]) >> 1;

    abort_status |= check_rx_abort(gcr_addr);
    int ddc_hyst_val = mem_pg_field_get(rx_ddc_hysteresis);
    // Defined as a positive value which shifts to the right or left if negative
    int ddc_offset_w_hyst[2];
    // Must use int type to store comparison value since signed comparison does not work against the binary literal.
    const int max_PR_val = 0b1 << rx_a_ddc_hyst_left_edge_width;
    ddc_offset_w_hyst[0] = (last_right_edge[0] - last_left_edge[0]) >> 1;
    ddc_offset_w_hyst[1] = (last_right_edge[1] - last_left_edge[1]) >> 1;

    // PSL left_edge_ge_max_pr_val
    if (last_left_edge_reg >= max_PR_val)
    {
        last_left_edge_reg = max_PR_val - 1;
    }

    // PSL right_edge_ge_max_pr_val
    if (last_right_edge_reg >= max_PR_val)
    {
        last_right_edge_reg = max_PR_val - 1;
    }

    // These registers can't have a negative value so 0 is best alternative for status reporting.
    // PSL left_edge_lt_0
    if (last_left_edge_reg < 0)
    {
        last_left_edge_reg = 0;
    }

    // PSL right_edge_lt_0
    if (last_right_edge_reg < 0 )
    {
        last_right_edge_reg = 0;
    }

    //DD1  mem_pl_field_put(rx_ddc_last_left_edge, lane, last_left_edge_reg);
    //DD1 mem_pl_field_put(rx_ddc_last_right_edge, lane, last_right_edge_reg);

    int old_offset = Dsave[0] - Dsave[1];
    int new_offset = ddc_offset_w_hyst[0] - ddc_offset_w_hyst[1];
    int width = last_right_edge_reg + last_left_edge_reg;
    ddc_hyst_val += (width >> 4);

    // If the difference between the left and right edges exceeds the hysteresis then shift the offset and save the new measurements.
    // PSL hysteresis
    if (((((abs(ddc_offset_w_hyst[0]) > ddc_hyst_val) || (abs(ddc_offset_w_hyst[1]) > ddc_hyst_val)
           || (abs(old_offset + new_offset) > (ddc_hyst_val << 1)))
          || recal_dac_changed)
         || !recal) &&
        (abort_status == pass_code))
    {
        // PSL bank_a
        if (bank == bank_a)
        {
            mem_pl_field_put(rx_a_ddc_hyst_left_edge, lane, last_left_edge_reg);
            mem_pl_field_put(rx_a_ddc_hyst_right_edge, lane, last_right_edge_reg);
        }
        else
        {
            mem_pl_field_put(rx_b_ddc_hyst_left_edge, lane, last_left_edge_reg);
            mem_pl_field_put(rx_b_ddc_hyst_right_edge, lane, last_right_edge_reg);
        }
    }
    else
    {
        // This returns us to prior sample position.
        ddc_offset_w_hyst[0] = 0;
        ddc_offset_w_hyst[1] = 0;
    }

    set_debug_state(0x800C); // DEBUG - 8: DDC Check Width and update historic eye values.

    pr_recenter(gcr_addr, bank, pr_active, Esave, Dsave, ddc_offset_w_hyst, 0);

    set_debug_state(0x801F); // DEBUG: 8: Shift back to sample position done

    // Note: If failed, return warning_code to skip result_check and ber_test and don't switch banks.
    // Note: Do the same thing if aborted, but return abort_code instead; still set ddc_failed_status so eo_main does not switch back to the DDC clock.
    // PSL abort_ne_pass
    if (abort_status != pass_code)
    {
        //Disable BER
        // Disable the per-lane counter
        //Replaced with FAST: put_ptr(gcr_addr, rx_berpl_count_en_addr, rx_berpl_count_en_startbit, rx_berpl_count_en_endbit, 0, read_modify_write);
        put_ptr_fast(gcr_addr, rx_berpl_sat_thresh_addr, rx_berpl_sat_thresh_endbit, 0xFFF);
        // Reset BER compare A/B data banks to PRBS
        //Replaced with FAST: put_ptr(gcr_addr, rx_berpl_exp_data_sel_addr, rx_berpl_exp_data_sel_startbit, rx_berpl_exp_data_sel_endbit, 0, read_modify_write);
        put_ptr_fast(gcr_addr, rx_berpl_cnt_en_exp_sel_alias_addr, rx_berpl_cnt_en_exp_sel_alias_endbit, 0);
        //EWM265038: This fail is for rx_bist not abort or ddc internal error: mem_pl_bit_set(rx_ddc_fail, lane);
        set_debug_state(0x8080); //DEBUG: Main RX abort
        return abort_status;
    }

    cdr_status = wait_for_cdr_lock(gcr_addr, true);

    if (cdr_status != pass_code)
    {
        // PSL set_ddc_error
        set_ddc_err (gcr_addr, bank, lane, pr_active, Esave, Dsave);
        set_debug_state(0x8083); // DEBUG: Algorithm error. Final lock error
        //EWM265038: This fail is for rx_bist not abort or ddc internal error: set_fir(fir_code_bad_lane_warning);
        return warning_code;
    }

    /*
      // No Longer running this section of BER Test code. Deemed OBSOLETE/UNECESSARY
    else {
      //Initialize sat_thresh to Done value to begin error search
      uint32_t ber_sel_val = ber_count << (rx_berpl_sat_thresh_shift-(16-rx_berpl_sat_sample_sel_alias_width))
        | ber_sel_done;
      uint32_t ds = 0;
      put_ptr_fast(gcr_addr, rx_berpl_sat_sample_sel_alias_addr, rx_berpl_sat_sample_sel_alias_endbit, ber_sel_val);
      // Final error sense provided by pr_seek_ber because it is convenient with a ds = 0
      ber_reported = pr_seek_ber(gcr_addr, bank, ds, ds, noseek, false, noseek, pr_active);
      if (ber_reported >  ber_count) {
        set_ddc_err (gcr_addr, bank, lane, pr_active, Esave, Dsave);
        set_debug_state(0x8084); // DEBUG: Problem with data eye: final error check failed.
        set_fir(fir_code_bad_lane_warning);
        return warning_code;
      } // ber_reported > ber_count
    }
    */

    // ddc passed
    //EWM265038: This fail is for rx_bist not abort or ddc internal error: mem_pl_bit_clr(rx_ddc_fail, lane);
    //Disable BER
    // Disable the per-lane counter
    //Replaced with FAST: put_ptr(gcr_addr, rx_berpl_count_en_addr, rx_berpl_count_en_startbit, rx_berpl_count_en_endbit, 0, read_modify_write);
    put_ptr_fast(gcr_addr, rx_berpl_sat_thresh_addr, rx_berpl_sat_thresh_endbit, 0xFFF);
    // Reset BER compare A/B data banks to PRBS
    //Replaced with FAST: put_ptr(gcr_addr, rx_berpl_exp_data_sel_addr, rx_berpl_exp_data_sel_startbit, rx_berpl_exp_data_sel_endbit, 0, read_modify_write);
    put_ptr_fast(gcr_addr, rx_berpl_cnt_en_exp_sel_alias_addr, rx_berpl_cnt_en_exp_sel_alias_endbit, 0);

    // Log historical width when clear is not asserted
    int clr_eye_height_width  = mem_pg_field_get(rx_clr_eye_height_width);

    // PSL clr_eye_height_width
    if (!clr_eye_height_width)
    {

        // Group historical
        // Get Left/Right Edges and compare against historical, record if new min.
        // Only read the current lane's width and compare if all lane mode (0), or the correct lane in single lane mode (1)
        int update_mode =  mem_pg_field_get(rx_hist_min_eye_width_mode);
        int hist_width_reg   = mem_regs_u16[pg_addr(
                                                rx_hist_min_eye_width_lane_addr)]; //Only works if lane & valid occupy same addr.
        int hist_width_lane  = bitfield_get(hist_width_reg, rx_hist_min_eye_width_lane_mask, rx_hist_min_eye_width_lane_shift);

        // Read width of lane
        if ( update_mode == 0 || ((update_mode == 1) && (lane == hist_width_lane)) )
        {
            int hist_width_valid = bitfield_get(hist_width_reg, rx_hist_min_eye_width_valid_mask,
                                                rx_hist_min_eye_width_valid_shift);
            int hist_width       = mem_pg_field_get(rx_hist_min_eye_width);

            // Write if no previous recording or if the width is a new min. Requirement: rx_hist_min_eye_width_valid and rx_hist_min_eye_width_lane occupy same reg
            if ( (hist_width_valid == 0) || (width < hist_width) )
            {
                hist_width_reg = (0b1 << (rx_hist_min_eye_width_valid_shift - rx_hist_min_eye_width_lane_shift)) | (lane);
                mem_regs_u16_put(pg_addr(rx_hist_min_eye_width_valid_addr),
                                 rx_hist_min_eye_width_valid_mask | rx_hist_min_eye_width_lane_mask, rx_hist_min_eye_width_lane_shift, hist_width_reg);
                mem_pg_field_put(rx_hist_min_eye_width, width);
            }
        } //width_mode

        // Lane historical
        int lane_hist_width_valid = mem_pl_field_get(rx_lane_hist_min_eye_width_valid, lane);

        if (bank == bank_a)
        {
            int lane_hist_width  = mem_pl_field_get(rx_a_lane_hist_min_eye_width, lane);

            if ((width < lane_hist_width) || (lane_hist_width_valid == 0))
            {
                mem_pl_field_put(rx_a_lane_hist_min_eye_width, lane, width);
                mem_pl_field_put(rx_lane_hist_min_eye_width_valid, lane, 1);
            }
        }
        else
        {
            int lane_hist_width  = mem_pl_field_get(rx_b_lane_hist_min_eye_width, lane);

            if ((width < lane_hist_width) || (lane_hist_width_valid == 0))
            {
                mem_pl_field_put(rx_b_lane_hist_min_eye_width, lane, width);
                mem_pl_field_put(rx_lane_hist_min_eye_width_valid, lane, 1);
            }
        }

    } //if(!clr_eye_height_width)

    //Rxbist check min eye width
    //After DDC runs should have min eye width of 40 -- could change based on hardware
    //Perfect eye would be 63 -- width will be postive only.
    int rx_ddc_check_en_int =  get_ptr(gcr_addr, rx_ddc_check_en_addr , rx_ddc_check_en_startbit  ,
                                       rx_ddc_check_en_endbit); //pg

    // PSL rx_ddc_check_en
    if (rx_ddc_check_en_int)
    {
        //begin 1
        int check_eye_width_min =  mem_pg_field_get(rx_eye_width_min_check);//ppe pg
        int rx_bist_eye_width   =  last_right_edge_reg + last_left_edge_reg;

        if ( rx_bist_eye_width  < check_eye_width_min)
        {
            mem_pl_field_put(rx_ddc_fail, lane, 0b1 );
            set_fir(fir_code_dft_error | fir_code_bad_lane_warning);
        }
    }//end 1

    //Checking done
    if (bank == bank_a)
    {
        mem_pl_field_put(rx_a_ddc_done, lane, 0b1);   //ppe pl
    }
    else
    {
        mem_pl_field_put(rx_b_ddc_done, lane, 0b1);
    }

    set_debug_state(0x80FF); // DEBUG: 10: exit pass

    // Return Success
    return abort_status;
} //eo_ddc
