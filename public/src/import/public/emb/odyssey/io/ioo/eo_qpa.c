/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/eo_qpa.c $        */
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
// *! FILENAME    : eo_qpa.c
// *! TITLE       :
// *! DESCRIPTION : Run QPA servos
// *!
// *! OWNER NAME  : John Gullickson     Email: gullicks@us.ibm.com
// *! BACKUP NAME : Mike Spear          Email: mspear@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
// ------------|--------|-------------------------------------------------------
// mbs22082601 |mbs     | Updated with PSL comments
// jfg22022401 |jfg     | Remove CDR locked check as obsolete behavior
// jfg22012701 |jfg     | Remove nedge_seek_step (behavior replaced by pr_seek_ber in eo_ddc.
// jfg22012701 |jfg     | Refactor PR movement loops to use pr_recenter instead of nedge_seek_step
// vbr21092200 |vbr     | Use shared functions for setting servo status error enable.
// vbr21060200 |vbr     | Moved servo queue check to once at beginning of training.
// vbr21051800 |vbr     | Added sleeps to reduce thread blocking
// jfg21021800 |jfg     | P11IOOLOG-266 remove set_qpa_err primarily for code reduction
// vbr21011901 |vbr     | Removed or changed to level 3 debug states that do not seem to be useful
// vbr20043000 |vbr     | Switched to common function for servo queues empty check
//-------------|P11 ----|-------------------------------------------------------
// mwh20012100 |mwh     | Add in code to set rx_quad_phase_fail if servo op has issue
// mbs20090201 |mbs     | HW528360 - Added rx_qpa_hysteresis and set to 3 (was hardcoded to 2 in eo_qpa previously)
// mbs20081900 |mbs     | HW542599 - Moved rx_vga_amax to per lane mem regs, and removed qpa obs regs to make room
// jfg20061500 |jfg     | HW527761 Servos have a clear resistance when > 3 or biased to EW: Add 1 to compensate
// jfg20022000 |jfg     | HW522672 Also had to change the ns/ew offset remainder conditional from && to ^
// jfg20021800 |jfg     | HW522672 Somehow after all this time centerskew_ns bool was found to be inverted due to it's interaction with centerdir_ns
// cws20011400 |cws     | Added Debug Logs
// jfg19090900 |jfg     | HW503330 Remove default FIR action (true parm) in wait_for_cdr
// jfg19070801 |jfg     | typo
// jfg19070800 |jfg     | fixed prior bug in edge_seek loop which skipped 0 step check for ending while loop
// jfg19070200 |jfg     | Error in step movement caused by switch to multi-step. Replace hardcoded 1 with variable.
// jfg19062400 |jfg     | finalize prior bad commit
// jfg19061701 |jfg     | moved single stepping wrapper function down into nedge_seek_step
// vbr19061200 |vbr     | Added wrapper around nedge_seek_step() to limit step size
// jfg19062400 |jfg     | Replace final calculations and windage adjust with a simpler difference pre-absolute value
// jfg19061900 |jfg     | Remove overpeaking since it is too aggressive.
// jfg19061600 |jfg     | Add rx_qpa_overpeak_disable and peak override during servo searches. Rename windage.
// jfg19061300 |jfg     | QPA servos are identified as biased early due to lower count of late edges compared to CDR. Move rx_qpa_ns_windage to raw results array.
// jfg19060100 |jfg     | Correct same sign offset bias adjust by averaging both results and taking a 1/4
// jfg19060600 |jfg     | Update doseek behavior of nedge_seek so that aligned edges are not disturbed
// jfg19053001 |jfg     | HW490617 & HW490631 wide ranging servo results showed the NS has a 1 step bias compared to EW when measuring. Added rx_qpa_ns_windage as a correction.
// jfg19052200 |jfg     | HW490617 & HW490631 Remove 1 of 2 1/2-step rounding in final divisions due to error accumulating
// jfg19052000 |jfg     | Remove lock sticky check as not intended. Simplify division bias for smaller code size.
// jfg19051703 |jfg     | typo2. no coffee today.
// jfg19051702 |jfg     | typo
// jfg19051701 |jfg     | HW490409 Average measurement of 3 needs special treatment to give better average.
// jfg19051700 |jfg     | HW491645 Add config to disable CDR lock check (default on)
// jfg19051600 |jfg     | HW490631 HW491945 Had inverted direction fix for same sign offset
// jfg19051300 |jfg     | HW490617 Simplify prior change for large offset bias by using /4 of remaining offset as bias correction. Fix error in sign correction.
// jfg19051000 |jfg     | Smoke test found condition of Offset between two same-sign results needing CDR track correction added
// jfg19050700 |jfg     | HW490631 Add a center offset detection to switch direction of odd-offset remainder
// jfg19040800 |jfg     | Comment updates and remove ber_lim from edge_seek
// jfg19031400 |jfg     | HW485870 Add same-sign bias detection / correction
// jfg19042900 |jfg     | HW489995 Replace per-phase hysteresis with total phase hystersis (any movement > 1)
// jfg19042400 |jfg     | HW485870 Remove phase independence in split/balanced offset result
// jfg19041801 |jfg     | Add single stepping error state repositioning.
// jfg19041600 |jfg     | HW485870 Repair bias errors and fix hysteresis to cancel a change unless both phases need to move.
// jfg19031900 |jfg     | HW485870 Add same-sign bias detection / correction
// jfg19031400 |jfg     | HW486026 fix bank select conditional and separate error E087 state
// jfg19030500 |jfg     | Rename recal_1strun as recal_2ndrun to match usage in main
// jfg19022800 |jfg     | Add result hysteresis and restore
// jfg19022200 |jfg     | Add error checking for vote_ratio and qpa_pattern. Add hysteresis for recal
// jfg19021501 |jfg     | fix rx_abort call
// jfg19021500 |jfg     | Remove debug statements
// jfg19021400 |jfg     | Many bugfixes to get loops and math working.
// jfg19020600 |jfg     | Initial QPA flow leveraging DDC edge functions
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ioo_common.h"
#include "servo_ops.h"
#include "eo_qpa.h"
#include "eo_ddc.h"

#include "ppe_com_reg_const_pkg.h"
#include "ppe_mem_reg_const_pkg.h"
#include "io_config.h"
#include "io_logger.h"


#define edge_max_left -10
#define edge_max_right 10

#define prmask_Dns(a) ( ((a) & (rx_a_pr_ns_data_mask >> rx_a_pr_ns_edge_shift)) >> (rx_a_pr_ns_data_shift - rx_a_pr_ns_edge_shift) )
#define prmask_Ens(a) ( ((a) & (rx_a_pr_ns_edge_mask >> rx_a_pr_ns_edge_shift)) )
#define prmask_Dew(a) ( ((a) & (rx_a_pr_ew_data_mask >> rx_a_pr_ew_edge_shift)) >> (rx_a_pr_ew_data_shift - rx_a_pr_ew_edge_shift) )
#define prmask_Eew(a) ( ((a) & (rx_a_pr_ew_edge_mask >> rx_a_pr_ew_edge_shift)) )

#define mk_opcode(op,opcode) ( op | ((opcode & ((0b1 << (c_op_code_width-1)) - 1)) << (16-c_op_code_startbit-c_op_code_width)))



////////////////////////////////////////////////////////////////////////////
// Run QPA on a lane and update historical width
// PHY state prior to DDC adjustment
//  Lane is fully powered on
//  CDR tracking main bank (bank_sel = A on initial cal), All quadrant.
//  DC Cal and edge offset complete on latches, along with vga gain
//  Servo queue is empty
//  All register modes are pre-set and stable
// PHY state at the completion of DDC adjustment
//   CDR tracking main bank (bank_sel = A on initial cal), All quadrant.
//   Mini PR offsets written with measured phase offset correction
int eo_qpa(t_gcr_addr* gcr_addr, t_bank bank, bool recal_2ndrun, bool* pr_changed)
{
    set_debug_state(0xE000); // DEBUG - QPA Start
    int abort_status = pass_code;
    uint32_t bank_pr_save[2];
    int pr_active[4]; // All four PR positions packed in as: {Data NS, Edge NS, Data EW, Edge EW}
    //uint32_t peak1_restore;
    //uint32_t peak2_restore;
    //bool disable_overpeak = mem_pg_field_get(rx_qpa_overpeak_disable) == 1;

    // 2: Set initial values
    // Load ****both**** data and edge values on read. Assumes in same reg address in data + edge order
    // PSL bank_a
    if (bank == bank_a)
    {
        bank_pr_save[0] = get_ptr(gcr_addr, rx_a_pr_ns_data_addr,  rx_a_pr_ns_data_startbit, rx_a_pr_ns_edge_endbit);
        bank_pr_save[1] = get_ptr(gcr_addr, rx_a_pr_ew_data_addr,  rx_a_pr_ew_data_startbit, rx_a_pr_ew_edge_endbit);
        /*
        peak1_restore = get_ptr(gcr_addr, rx_a_ctle_peak1_addr,  rx_a_ctle_peak1_startbit, rx_a_ctle_peak1_endbit);
        peak2_restore = get_ptr(gcr_addr, rx_a_ctle_peak2_addr,  rx_a_ctle_peak2_startbit, rx_a_ctle_peak2_endbit);
        if (!disable_overpeak) {
          put_ptr(gcr_addr, rx_a_ctle_peak1_addr,  rx_a_ctle_peak1_startbit, rx_a_ctle_peak1_endbit, (1<<rx_a_ctle_peak1_width)-1, read_modify_write);
          put_ptr(gcr_addr, rx_a_ctle_peak2_addr,  rx_a_ctle_peak2_startbit, rx_a_ctle_peak2_endbit, (1<<rx_a_ctle_peak2_width)-1, read_modify_write);
        }
        */
    }
    else
    {
        bank_pr_save[0] = get_ptr(gcr_addr, rx_b_pr_ns_data_addr,  rx_b_pr_ns_data_startbit, rx_b_pr_ns_edge_endbit);
        bank_pr_save[1] = get_ptr(gcr_addr, rx_b_pr_ew_data_addr,  rx_b_pr_ew_data_startbit, rx_b_pr_ew_edge_endbit);
        /*
        peak1_restore = get_ptr(gcr_addr, rx_b_ctle_peak1_addr,  rx_b_ctle_peak1_startbit, rx_b_ctle_peak1_endbit);
        peak2_restore = get_ptr(gcr_addr, rx_b_ctle_peak2_addr,  rx_b_ctle_peak2_startbit, rx_b_ctle_peak2_endbit);;
        if (!disable_overpeak) {
          put_ptr(gcr_addr, rx_b_ctle_peak1_addr,  rx_b_ctle_peak1_startbit, rx_b_ctle_peak1_endbit, (1<<rx_b_ctle_peak1_width)-1, read_modify_write);
          put_ptr(gcr_addr, rx_b_ctle_peak2_addr,  rx_b_ctle_peak2_startbit, rx_b_ctle_peak2_endbit, (1<<rx_b_ctle_peak2_width)-1, read_modify_write);
        }
        */
    }

    pr_active[prDns_i] = prmask_Dns(bank_pr_save[0]);
    pr_active[prEns_i] = prmask_Ens(bank_pr_save[0]);
    pr_active[prDew_i] = prmask_Dew(bank_pr_save[1]);
    pr_active[prEew_i] = prmask_Eew(bank_pr_save[1]);

    // Calculate Center offset
    bool centerskew_ns = abs(16 - pr_active[prEns_i]) < abs(16 - pr_active[prEew_i]);
    bool centerdir_ns = pr_active[prEns_i] > pr_active[prEew_i];

    uint32_t vote_ratio = get_ptr(gcr_addr, rx_qpa_vote_ratio_cfg_addr,  rx_qpa_vote_ratio_cfg_startbit,
                                  rx_qpa_vote_ratio_cfg_endbit);

    // PSL vote_ratio_eq_0
    if (vote_ratio == 0)
    {
        // Error case: Fix the users transgressions to prevent servo breakage.
        // If 0 vote_ratio is requested, force setting of 1 since 0 will eventually be used to end the vote after a direction change.
        put_ptr(gcr_addr, rx_qpa_vote_ratio_cfg_addr,  rx_qpa_vote_ratio_cfg_startbit, rx_qpa_vote_ratio_cfg_endbit, 1,
                read_modify_write);
    }

    int lane = get_gcr_addr_lane(gcr_addr);


    // See Workbook Table x.x.1.1 "rx_ddc_min_err_lim Register Decode" in QPA Section
    int servo_pattern_enable = mem_pg_field_get(rx_qpa_pattern_enable);
    int servo_pattern = mem_pg_field_get(rx_qpa_pattern);
    //const int ber_lim=0;

    uint16_t servo_code_a[4] = {c_qpa_ae_n000, c_qpa_ae_e000, c_qpa_ae_s000, c_qpa_ae_w000};
    uint16_t servo_code_b[4] = {c_qpa_be_n000, c_qpa_be_e000, c_qpa_be_s000, c_qpa_be_w000};

    // PSL servo_pattern_enable_eq_1
    if (servo_pattern_enable == 0b1)
    {
        bool noTXatH1H0 = ((servo_pattern & (0x4)) ^ (servo_pattern & (0x2))) == 0b0;

        // PSL noTXatH1H0
        if (noTXatH1H0)
        {
            // This is a user error condition where the custom pattern has no transition at H0 and therefore cannot measure an edge position.
            // In this case, fail the servo and set all QPA OBS values to 7 which is an impossible normal result.
            // MBS reclaim these reg bits // mem_pl_field_put(rx_qpa_ne_obs, lane, 7);
            // MBS reclaim these reg bits // mem_pl_field_put(rx_qpa_ee_obs, lane, 7);
            // MBS reclaim these reg bits // mem_pl_field_put(rx_qpa_se_obs, lane, 7);
            // MBS reclaim these reg bits // mem_pl_field_put(rx_qpa_we_obs, lane, 7);
            abort_status |= error_code;
            mem_pl_bit_set(rx_quad_phase_fail, lane);
            set_debug_state(0xE08F); //DEBUG: Main RX abort
        }

        servo_code_a[0] = mk_opcode(c_qpa_prg_ae_n000, servo_pattern);
        servo_code_a[1] = mk_opcode(c_qpa_prg_ae_e000, servo_pattern);
        servo_code_a[2] = mk_opcode(c_qpa_prg_ae_s000, servo_pattern);
        servo_code_a[3] = mk_opcode(c_qpa_prg_ae_w000, servo_pattern);
        servo_code_b[0] = mk_opcode(c_qpa_prg_be_n000, servo_pattern);
        servo_code_b[1] = mk_opcode(c_qpa_prg_be_e000, servo_pattern);
        servo_code_b[2] = mk_opcode(c_qpa_prg_be_s000, servo_pattern);
        servo_code_b[3] = mk_opcode(c_qpa_prg_be_w000, servo_pattern);
    }

    //set_debug_state(0xE001); // DEBUG - QPA Setup



    // Assume already switched to the correct bank prior to this being called
    /////// Initial Setup
    // Read existing ctle_coarse preset for use in abort sequence and select the correct servo ops
    uint16_t servo_ops[1];
    uint16_t quad = 4;
    int32_t servo_results[4];

    // Disable servo status for result at min/max
    servo_errors_disable_only_result_at_min_or_max(gcr_addr);

    //int qpa_windage = SignedMagToInt(mem_pg_field_get(rx_qpa_windage), rx_qpa_windage_width);
    uint32_t Dsave[2];
    uint32_t Esave[2];
    int Doffset[2] = {0, 0};

    // Store starting position for search. This implies, and is defined as, the starting sample prior to DDC
    Esave[0] = pr_active[prEns_i];
    Dsave[0] = pr_active[prDns_i];
    Esave[1] = pr_active[prEew_i];
    Dsave[1] = pr_active[prDew_i];

    do
    {
        quad--;
        servo_ops[0] = (bank == bank_a) ? servo_code_a[quad] : servo_code_b[quad];

        // Run the servo ops
        set_debug_state(0xE010 | quad); // DEBUG - CTLE Run Servo Op
        abort_status = run_servo_ops_and_get_results(gcr_addr, c_servo_queue_general, 1, servo_ops, &servo_results[quad]);
        abort_status |= check_rx_abort(gcr_addr);

        //for bist if there is a servo error this get set
        // PSL set_fail
        if (abort_status & rc_warning )
        {
            mem_pl_field_put(rx_quad_phase_fail, lane, 0b1);    //ppe pl
            set_debug_state(0xE0DD);
        }


        int pridx;

        //t_seek seek_quad;
        if ((quad == 2 || quad == 0))
        {
            pridx = prEns_i;
            //seek_quad=noseekNS;
        }
        else
        {
            pridx = prEew_i;
            //seek_quad=noseekEW;
        }

        int Ediff = servo_results[quad];
        //servo_results[quad] = servo_results[quad]  +  qpa_windage;

        // Range checking is not neccessary here because the servo is limited in it's excursions and will only return the amount of change actually applied to the PR
        pr_active[pridx] += Ediff;

        // The undo direction is the opposite of where the servo moved the PRs
        //bool dirL1R0 = !(Ediff < 0);
        //Ediff = (dirL1R0)? Ediff : (~Ediff + 1);
        // NO. Divide wrecks the bias correction: Now divide original Ediff by two as the new PR result.
        //int Ediff_half = ((Ediff)  >> 1) + (Ediff & 0b1);
        //servo_results[quad] = dirL1R0?(Ediff_half) : (~Ediff_half + 1);

        //DEBUG set_debug_state(Ediff & 0xFFFF);
        //int loop_count = 0;
        //calling nedge_seek_step with L1 move, using noBER, and no seek with step=2 to move Edge/Data back to new center
        pr_recenter(gcr_addr, bank, pr_active, Esave, Dsave, Doffset, 0);
        //nedge_seek_step(gcr_addr, bank, 0, 1, dirL1R0, true, seek_quad, pr_active);//, ber_lim
        //Ediff--;

        set_debug_state(0xE020, 3); // DEBUG - QPA Run Servo Op
    }
    while ((quad != 0) && (abort_status == 0));

    // Re-enable servo status for result at min/max
    servo_errors_enable_all(gcr_addr);

    // Note: If failed, return warning_code  don't switch banks.
    // Note: still set failed status
    // PSL set_fail
    if (abort_status != pass_code)
    {
        mem_pl_bit_set(rx_quad_phase_fail, lane);
        set_debug_state(0xE080); //DEBUG: Main RX abort
        return abort_status;
    }


    // 5: Calculate average of mini-pr offsets
    // First each quadrant pair edge offset is averaged
    // Equally split offset
    // If the original values of *both* quadrant pairs have the same sign then there is a bias which must be subtracted.
    //   -- Bias easily occurs due to pattern selection on random idiosyncracy
    // Then move PR into new position. Negative number is a leftward shift.
    int EsumNS = servo_results[0] + servo_results[2] ;
    int EsumEW = servo_results[1] + servo_results[3] ;
    bool dirL1R0NS = (EsumNS) < 0;
    bool dirL1R0EW = (EsumEW) < 0;

    /* uint32_t EsumNS_abs = abs(EsumNS); */
    /* uint32_t EsumEW_abs = abs(EsumEW); */
    /* EsumNS = ((EsumNS_abs)  >> 1); */
    /* EsumEW = (EsumEW_abs)  >> 1; */

    int OffsetNS;
    int OffsetEW;
    bool hysteresis_en = mem_pg_field_get(rx_qpa_hysteresis_enable) == 1;
    int qpa_hysteresis = mem_pg_field_get(rx_qpa_hysteresis);

    /*
    // Eliminate potential bias.
    // If the average value of each pair appear skewed to one side
    // then find their difference as the phase offset.
    // However, if the center is still between the pairs then treat as if opposite to correct CDR track offset
    if (! (dirL1R0NS ^ dirL1R0EW) ) {
      OffsetNS = abs(EsumNS - EsumEW); // Divide offset between the pairs later on.
      //int offset_half_temp = OffsetNS >> 1;
      // Since a larger bias expands the offset remove it. Smaller bias's don't affect it.
      int offset_bias_temp = (EsumNS + EsumEW) >> 3;
      OffsetNS = OffsetNS - offset_bias_temp;
      // Now correct the signs for a balanced movement
      // Same sign results worked servo harder to move out of a skewed eye
      // The quadrant closest to that skewed eye moved more and will be the larger.
      // ---> Therefore the smaller quadrant measurement moves against it's measured sign. <---
      dirL1R0NS = dirL1R0NS ^ (EsumNS < EsumEW);
      dirL1R0EW = !dirL1R0NS;
      //// If predicted offset is still between pairs, correct CDR dead space.
      //if ((EsumNS < offset_half_temp) || (EsumEW < offset_half_temp)) {
      //  OffsetNS = (OffsetNS >> 1) + (OffsetNS & 0b1);
      //}
    }
    else {
      // Otherwise EW and NS are opposite in sign (previously sign stripped of course)
      OffsetNS = EsumNS + EsumEW;
    }
    */

    OffsetNS = EsumEW - EsumNS;
    // Divide to compensate CDR track offset (dead space) but only when signs differ.
    //OffsetNS = (OffsetNS >> 1) + (OffsetNS & 0b1);
    dirL1R0NS = (OffsetNS) > 0;
    dirL1R0EW = !dirL1R0NS;
    int OffsetNS_mag = abs(OffsetNS);
    // OffsetNS is tweaked by two terms
    // A- +2 is added to abs(OffsetNS) to bias the ceiling of the div 4 magnitude to not lose 0.5 step changes
    // B- +1 is added to counter an emprically observed negative CDR inertia that becomes relevant over 3 steps in a servo result
    int neg_bias = 0;

    // PSL neg_bias
    if ((OffsetNS_mag >= 6) && dirL1R0NS)
    {
        neg_bias = 1;
    }

    OffsetNS = abs(OffsetNS + neg_bias);
    OffsetNS = (OffsetNS + 2) >> 2;
    // Split difference in shifts evenly between phases
    OffsetEW = OffsetNS;
    uint32_t ENavg_tgt;
    uint32_t EEavg_tgt;
    set_debug_state(0xE800 | (0x07FF & OffsetNS));

    // PSL centerskew_dir
    if (centerskew_ns ^  (centerdir_ns ^ dirL1R0NS))
    {
        ENavg_tgt = OffsetNS >> 1;
        EEavg_tgt = OffsetEW - ENavg_tgt;
    }
    else
    {
        EEavg_tgt = OffsetEW >> 1;
        ENavg_tgt = OffsetNS - EEavg_tgt;
    }


    // Post result averaging hysteresis applied to help reduce chances of result oscillation during Main VGA loop
    // Correct for fixed 1-step hysteresis during recal or post 1st iteration and when manually enabled
    // If hysteresis stability is triggered then neither phase should move.
    // PSL hysteresis
    if (!((recal_2ndrun || hysteresis_en) && (OffsetNS < qpa_hysteresis)))
    {

        set_debug_state(0xE030, 3); // DEBUG - Phase adjust exceeds hysteresis

        // Indicate if the PR value changed
        *pr_changed = (EEavg_tgt != 0) || (ENavg_tgt != 0);

        //set_debug_state(0xE800 | (0x07FF & ENavg_tgt));

        Esave[0] = (dirL1R0NS) ? Esave[0] - ENavg_tgt : Esave[0] + ENavg_tgt;
        Esave[1] = (dirL1R0EW) ? Esave[1] - EEavg_tgt : Esave[1] + EEavg_tgt;
        pr_recenter(gcr_addr, bank, pr_active, Esave, Dsave, Doffset, 0);
        /*    int loop_count = 0;
        while ((ENavg_tgt != 0)) {
        //calling nedge_seek_step with variable L1/R0, using noBER, and no seek with step=1 to move Edge to new center
        nedge_seek_step(gcr_addr, bank, 0, 1, dirL1R0NS, true, noseekNS, pr_active);
        ENavg_tgt--;

        loop_count++;
        if ((loop_count % 2) == 0) { io_sleep(get_gcr_addr_thread(gcr_addr)); }
        }

        while ((EEavg_tgt != 0)) {
        //calling nedge_seek_step with variable L1/R0, using noBER, and no seek with step=1 to move Edge to new center
        nedge_seek_step(gcr_addr, bank, 0, 1, dirL1R0EW, true, noseekEW, pr_active);
        EEavg_tgt--;

        loop_count++;
        if ((loop_count % 2) == 0) { io_sleep(get_gcr_addr_thread(gcr_addr)); }
        } */
    }

    // Write results but mask off leading bits to prevent corruption
    // MBS reclaim these reg bits // mem_pl_field_put(rx_qpa_ne_obs, lane, servo_results[0] & ((0b1 << rx_qpa_ne_obs_width) - 1));
    // MBS reclaim these reg bits // mem_pl_field_put(rx_qpa_ee_obs, lane, servo_results[1] & ((0b1 << rx_qpa_ee_obs_width) - 1));
    // MBS reclaim these reg bits // mem_pl_field_put(rx_qpa_se_obs, lane, servo_results[2] & ((0b1 << rx_qpa_se_obs_width) - 1));
    // MBS reclaim these reg bits // mem_pl_field_put(rx_qpa_we_obs, lane, servo_results[3] & ((0b1 << rx_qpa_we_obs_width) - 1));

    set_debug_state(0xE031, 3); // DEBUG - Phase adjust done

    // qpa passed
    mem_pl_bit_clr(rx_quad_phase_fail, lane);

    //Checking done
    if (bank == bank_a)
    {
        mem_pl_field_put(rx_a_quad_phase_done , lane, 0b1);
        //put_ptr(gcr_addr, rx_a_ctle_peak1_addr,  rx_a_ctle_peak1_startbit, rx_a_ctle_peak1_endbit, peak1_restore, read_modify_write);
        //put_ptr(gcr_addr, rx_a_ctle_peak2_addr,  rx_a_ctle_peak2_startbit, rx_a_ctle_peak2_endbit, peak2_restore, read_modify_write);
    }
    else
    {
        mem_pl_field_put(rx_b_quad_phase_done , lane, 0b1);
        //put_ptr(gcr_addr, rx_b_ctle_peak1_addr,  rx_b_ctle_peak1_startbit, rx_b_ctle_peak1_endbit, peak1_restore, read_modify_write);
        //put_ptr(gcr_addr, rx_b_ctle_peak2_addr,  rx_b_ctle_peak2_startbit, rx_b_ctle_peak2_endbit, peak2_restore, read_modify_write);
    }//ppe pl

    set_debug_state(0xE0FF); // DEBUG: 10: exit pass

    // Return Success
    return abort_status;
} //eo_qpa
