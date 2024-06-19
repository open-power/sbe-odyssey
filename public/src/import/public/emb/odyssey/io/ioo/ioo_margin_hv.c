/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/odyssey/io/ioo/ioo_margin_hv.c $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
// *! (C) Copyright International Business Machines Corp. 2022
// *! All Rights Reserved -- Property of IBM
// *!---------------------------------------------------------------------------
// *! FILENAME    : ioo_margin_hv.c
// *! TITLE       :
// *! DESCRIPTION : loff_offset_wrap for vertical offset manual and auto-margining capability
// *|               pr_offset_wrap for horizontal offset manual and auto-margining capability
// *!
// *! OWNER NAME  : John Gullickson     Email: gullicks@us.ibm.com
// *! BACKUP NAME : Vikram Raj          Email: vbraj@us.ibm.com
// *!
// *!---------------------------------------------------------------------------
// CHANGE HISTORY:
//------------------------------------------------------------------------------
// Version ID: |Author: | Comment:
//-------------|--------|-------------------------------------------------------
// jfg23061900 |jfg     | Add back H_end conditional for dual mode that was accidentally removed. Fix some escapes.
// jfg23061300 |jfg     | Prevent double V advance in V-only mode from state 0
// jfg23061000 |jfg     | Add PR range checking and optimized start/finish conditionals
// jfg23052500 |jfg     | Issue 305100 and 305102 Missed state transitions into and out of 1/2 phase mode setting
// jfg23052500 |jfg     | Restructure state transitions for clarity & consistency separate for Loff and PR
// jfg23022400 |jfg     | Move pr_offset_wrap and loff_offset_wrap AS-IS from eo_main.c
// -----------------------------------------------------------------------------

#include <stdbool.h>

#include "io_lib.h"
#include "pk.h"

#include "ioo_common.h"
#include "ioo_margin_hv.h"
#include "eo_ddc.h"

#include "ppe_mem_reg_const_pkg.h"
#include "ppe_com_reg_const_pkg.h"


void loff_offset_wrap (t_gcr_addr* gcr_addr, t_bank bank, bool apply)
{
    // Remove any applied static offset. If this is not set before initial training it could get tied up in hysteresis and act strangely.
    int lane = get_gcr_addr_lane(gcr_addr);
    bool loff_offset_pause = (0 != mem_pg_field_get(ppe_loff_offset_pause));
    int margin_mode = mem_pg_field_get(ppe_offset_margin_mode);
    int loff_offset_static_d = TwosCompToInt(mem_pg_field_get(ppe_loff_offset_d_override),
                               ppe_loff_offset_d_override_width);
    int loff_offset_static_e = TwosCompToInt(mem_pg_field_get(ppe_loff_offset_e_override),
                               ppe_loff_offset_e_override_width);

    int margin_st = mem_pl_field_get(ppe_margin_offset_overlay, lane);
    int applied_tgl = 1;
    int margin_st_next;

    // PSL skip_margin_on_overflow_or_bank_mismatch
    if (margin_mode != 0)
    {
        // PSL margin_mode_exit_conditions
        if ((margin_st == offset_st_overfl) ||
            ((margin_st & offset_mask_Bnk) ^ (bank & offset_mask_Bnk)))
        {
            return;
        }
    }

    // PSL setup_remove_pr_offset
    if (!apply)
    {
#if IO_DEBUG_LEVEL >4
        set_debug_state(0xFFF1); // Debug - Loffset Remove
#endif
        // Dual Mode behavior:
        // Non Margin mode - Normally both banks must have the offset applied before it can be removed. However, a simulation user may override one or the other to short circuit the removal when only one bank is under test. Use with caution
        // Margin mode - Toggling behavior requires coordination of all banks and lanes to use the same value and inversion. Therefore only invert one all offsets are removed


        //PSL remove_margin0
        if (margin_mode == 0)
        {
            loff_offset_static_d = 0 - loff_offset_static_d;
            loff_offset_static_e = 0 - loff_offset_static_e;
            applied_tgl = 0;

            // PSL non_margin_mode_bank_check
            if (bank == bank_a)
            {
                if ((margin_st & (ppe_loff_offset_applied_a_mask >> ppe_margin_offset_overlay_shift)) == 0)
                {
                    return;
                }
            }
            else
            {
                if ((margin_st & (ppe_loff_offset_applied_b_mask >> ppe_margin_offset_overlay_shift)) == 0)
                {
                    return;
                }
            }
        }
        else
        {
            // PSL skip_V_unapply_on_nonneg_and_nonoff
            if (((margin_st & offset_mask_PN) == 0) &&
                ((margin_st & offset_mask_HV) != offset_st_V_off) )
            {
                return;
            }
        }

    } // IF !apply
    else
    {
        //PSL pause_on_apply
        if (loff_offset_pause)
        {
            return;
        }

#if IO_DEBUG_LEVEL >4
        set_debug_state(0xFFF2); // Debug - Loffset Apply/Remove
#endif

        // Perform some preliminary state transitions
        // Veritcal and Horizontal margin modes must be mutually exclusive.
        // PSL offset_st_V_end
        if ((((margin_st & offset_mask_end) == offset_mask_end)  || (margin_st == offset_st_H_on) ) &&
            ((margin_mode & 0x2) == 0x2))
        {
            // Only return to on if in V standalone mode or dual mode. mode 1 is checked by PR Apply
            // This branch also handles the case where pr_offset_pause = 1
            margin_st_next = offset_st_V_on;
        }
        // Going to the next phase is handled by PR Apply as well as the normal st_H advance
        //PSL st_V_advance
        else if (((margin_st & offset_mask_HV) == offset_st_V_on) ||
                 ((margin_st & offset_mask_HV) == offset_st_V_off)   )
        {
            margin_st_next = margin_st + 1;
        }
        else
        {
            // No longer in Vertical mode
            return;
        }

        // PSL margin_mode_apply
        if (margin_mode != 0x0)
        {
            mem_pl_field_put(ppe_margin_offset_overlay, lane, margin_st_next);
        }

        // PSL skip_apply_on_st_V_off
        if (((margin_st & offset_mask_HV) == offset_st_V_off))
        {
            return;
        }
    } // ELSE apply

    // Due to multistate masking in the state flow above some shared V/H states need extra exclusion as shown here.
    // PSL not_in_V_mode
    if (((margin_st & offset_mask_HV) == offset_st_H_on) ||
        ((margin_st & offset_mask_HV) == offset_st_H_off))
    {
        return;
    }

    // NOTE: All flow control is determined in above apply or !apply conditional
    // PSL vertical_or_manual_mode
    if ((margin_mode) == 0)
    {
        // PSL banksel
        if (bank == bank_a)
        {
            mem_pl_field_put(ppe_loff_offset_applied_a, lane, applied_tgl);
        }
        else
        {
            mem_pl_field_put(ppe_loff_offset_applied_b, lane, applied_tgl);
        }
    }
    // PSL offset_mask_PN
    else if ((margin_st & offset_mask_PN) != 0)
    {
        // Note: This condition relies on margin_st steering conditionals above to exit the function
        // if no longer in Horizontal mode during the apply phase.
        // It is purposely sparse.
#if IO_DEBUG_LEVEL >4
        set_debug_state(0xFFF3, 4); // Debug - Loff offset apply - Offset Applied Margin Mode Invert
#endif
        // An offset had been applied and is now completely removed...so if it is margin_mode then invert the offset.
        loff_offset_static_d = 0 - loff_offset_static_d;
        loff_offset_static_e = 0 - loff_offset_static_e;
    }

    apply_rx_dac_offset(gcr_addr, data_only, bank, loff_offset_static_d);
    apply_rx_dac_offset(gcr_addr, edge_only, bank, loff_offset_static_e);
    io_sleep(get_gcr_addr_thread(gcr_addr));
    return;
}

void pr_offset_wrap (t_gcr_addr* gcr_addr, t_bank bank, bool apply)
{
    int lane = get_gcr_addr_lane(gcr_addr);
    int margin_mode = mem_pg_field_get(ppe_offset_margin_mode);

    bool pr_offset_pause = (0 != mem_pg_field_get(ppe_pr_offset_pause));

    //Some overhead for using pr_recenter
    int pr_active[4]; // All four PR positions packed in as: {Data NS, Edge NS, Data EW, Edge EW}
    read_active_pr(gcr_addr, bank, pr_active);
    uint32_t Dsave[2];
    Dsave[0] = pr_active[prDns_i];
    Dsave[1] = pr_active[prDew_i];
    uint32_t Esave[2];
    Esave[0] = pr_active[prEns_i];
    Esave[1] = pr_active[prEew_i];

    int pr_offset_static_d = TwosCompToInt(mem_pg_field_get(ppe_pr_offset_d_override), ppe_pr_offset_d_override_width);
    int pr_offset_static_e = TwosCompToInt(mem_pg_field_get(ppe_pr_offset_e_override), ppe_pr_offset_e_override_width);
    int margin_st = mem_pl_field_get(ppe_margin_offset_overlay, lane);
    int applied_tgl = 1;
    int margin_st_next;

    // PSL skip_margin_on_overflow_or_bank_mismatch
    if (margin_mode != 0)
    {
        // PSL margin_mode_exit_conditions
        if ((margin_st == offset_st_overfl) ||
            ((margin_st & offset_mask_Bnk) ^ (bank & offset_mask_Bnk)))
        {
            return;
        }
    }

    // PSL setup_remove_pr_offset
    if (!apply)
    {
#if IO_DEBUG_LEVEL >4
        set_debug_state(0xFFF4); // Debug - PRoffset Remove
#endif
        // Dual Mode behavior:
        // Non Margin mode - Normally both banks must have the offset applied before it can be removed. However, a simulation user may override one or the other to short circuit the removal when only one bank is under test. Use with caution
        // Margin mode - Toggling behavior requires coordination of all banks and lanes to use the same value and inversion. Therefore only invert one all offsets are removed


        //PSL remove_margin0
        if (margin_mode == 0)
        {
            pr_offset_static_d = 0 - pr_offset_static_d;
            pr_offset_static_e = 0 - pr_offset_static_e;
            applied_tgl = 0;

            //PSL non_margin_mode_bank_check
            if (bank == bank_a)
            {
                if ((margin_st & (ppe_pr_offset_applied_a_mask >> ppe_margin_offset_overlay_shift)) == 0)
                {
                    return;
                }
            }
            else
            {
                if ((margin_st & (ppe_pr_offset_applied_b_mask >> ppe_margin_offset_overlay_shift)) == 0)
                {
                    return;
                }
            }
        }
        else
        {
            // PSL skip_H_unapply_on_nonneg_and_nonoff
            if (((margin_st & offset_mask_PN) == 0) &&
                ((margin_st & offset_mask_HV) != offset_st_H_off) )
            {
                return;
            }
        }
    } // !apply
    else
    {
        //PSL pause_on_apply
        if (pr_offset_pause)
        {
            return;
        }

        // Perform some preliminary state transitions
        // PR Apply visits first so it handles the initial and ending dual mode state transitions
        // Veritcal and Horizontal margin modes must be mutually exclusive.
        // PSL offset_st_H_end
        if (((margin_st == offset_st_H_end) && (margin_mode == 0x1)) ||
            ((margin_st == offset_st_V_end) && (margin_mode != 0x2))   )
        {
            margin_st_next = offset_st_H_on;
        }
        // PSL st_H_advance
        else if ((((margin_st & offset_mask_HV) == offset_st_H_on) ||
                  ((margin_st & offset_mask_HV) == offset_st_H_off )) &&
                 (margin_mode != 0x2) && // Here the &0x2 mask compare could be eliminated with a more unique H_on state value
                 (margin_st != offset_st_H_end) )
        {
            margin_st_next = margin_st + 1;
        }
        else
        {
            // No longer in Horizontal mode
            return;
        }

        // PSL margin_mode_apply
        if (margin_mode != 0x0)
        {
            mem_pl_field_put(ppe_margin_offset_overlay, lane, margin_st_next);
        }

        // PSL skip_apply_on_st_H_off
        if ((margin_st & offset_mask_HV) == offset_st_H_off)
        {
            return;
        }
    } // ELSE apply

    // Due to multistate masking in the state flow above some shared V/H states need extra exclusion as shown here.
    // PSL not_in_H_mode
    if (((margin_st & offset_mask_HV) == offset_st_V_on) ||
        ((margin_st & offset_mask_HV) == offset_st_V_off))
    {
        return;
    }

    // NOTE: All flow control is determined in above apply or !apply conditional
#if IO_DEBUG_LEVEL >4
    set_debug_state(0xFFF5); // Debug - PRoffset Apply/Remove
#endif

    // PSL horizontal_or_manual_mode
    if ((margin_mode) == 0)
    {
        // PSL banksel
        if (bank == bank_a)
        {
            mem_pl_field_put(ppe_pr_offset_applied_a, lane, applied_tgl);
        }
        else
        {
            mem_pl_field_put(ppe_pr_offset_applied_b, lane, applied_tgl);
        }
    }
    // PSL offset_mask_PN
    else if ((margin_st & offset_mask_PN) != 0)
    {
        // Note: This condition relies on margin_st steering conditionals above to exit the function
        // if no longer in Horizontal mode during the apply phase.
        // It is purposely sparse.
#if IO_DEBUG_LEVEL >4
        set_debug_state(0xFFF6); // Debug - PRoffset Remove - Offset Applied Margin Mode Invert
#endif
        // An offset had been applied and is now completely removed...so if it is margin_mode then invert the offset.
        pr_offset_static_d = 0 - pr_offset_static_d;
        pr_offset_static_e = 0 - pr_offset_static_e;
    }

    int pr_offset_vec_d[2]  = {pr_offset_static_d, pr_offset_static_d};

    // PSL pr_recenter_with_limit_error
    if (pr_recenter_werr(gcr_addr, bank, pr_active, Esave, Dsave, pr_offset_vec_d, pr_offset_static_e, true))
    {
        // If the override request exceeds the available DAC range then set state to sticky offset_st_overfl
        // This halts the margin or toggle sequence for that lane and should leave DAC in original position.
        // The user is responsible for correcting this condition in the DAC values.
        mem_pl_field_put(ppe_margin_offset_overlay, lane, offset_st_overfl);
    }

    return;
}
