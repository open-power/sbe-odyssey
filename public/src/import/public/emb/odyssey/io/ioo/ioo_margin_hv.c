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
// *! *** IBM Confidential ***
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
    int apply_val = 1;

    // PSL setup_remove_pr_offset
    if (!apply)
    {
#if IO_DEBUG_LEVEL >4
        set_debug_state(0xFFF1); // Debug - Loffset Remove
#endif
        // Dual Mode behavior:
        // Non Margin mode - Normally both banks must have the offset applied before it can be removed. However, a simulation user may override one or the other to short circuit the removal when only one bank is under test. Use with caution
        // Margin mode - Toggling behavior requires coordination of all banks and lanes to use the same value and inversion. Therefore only invert one all offsets are removed


        bool offset_applied_lane;

        //PSL remove_margin0
        if (margin_mode == 0)
        {
            loff_offset_static_d = 0 - loff_offset_static_d;
            loff_offset_static_e = 0 - loff_offset_static_e;
            apply_val = 0;

            // PSL banksel
            if (bank == bank_a)
            {
                offset_applied_lane = (margin_st & (ppe_loff_offset_applied_a_mask >> ppe_margin_offset_overlay_shift)) != 0;
            }
            else
            {
                offset_applied_lane = (margin_st & (ppe_loff_offset_applied_b_mask >> ppe_margin_offset_overlay_shift)) != 0;
            }
        }
        else
        {
            offset_applied_lane  = (margin_st & (offset_mask_HV | offset_mask_PN)) == offset_st_V_neg;
            offset_applied_lane |= (margin_st & offset_mask_HV) == offset_st_V_off;
            offset_applied_lane &= !((margin_st & offset_mask_Bnk) ^ (bank & offset_mask_Bnk));

            // PSL offset_st_V_end
            if (margin_st == offset_st_V_end)
            {
                margin_st = 0;
                goto LOFF_LAST;
            }
        }

        // PSL force_remove_if_applied
        if (offset_applied_lane)
        {
            loff_offset_pause = false;
        }
        else
        {
            // Override Pause if margin_st hasn't yet applied offsets.
            loff_offset_pause = true;
        }
    } // IF !apply

    // PSL apply_or_remove_loff_offset
    if (!loff_offset_pause && (((margin_mode & 0x2) == 0x2) || (margin_mode == 0)))
    {
        // Veritcal and Horizontal margin modes must be mutually exclusive.
#if IO_DEBUG_LEVEL >4
        set_debug_state(0xFFF2); // Debug - Loffset Apply/Remove
#endif

        // PSL vertical_or_manual_mode
        if ((margin_mode) == 0)
        {
            // PSL banksel
            if (bank == bank_a)
            {
                mem_pl_field_put(ppe_loff_offset_applied_a, lane, apply_val);
            }
            else
            {
                mem_pl_field_put(ppe_loff_offset_applied_b, lane, apply_val);
            }
        }
        else
        {
            // PSL offset_st_V_on
            if ((margin_st & offset_mask_HV) == offset_st_V_on)
            {
                // PSL offset_mask_PN
                if ((margin_st & offset_mask_PN) != 0)
                {
#if IO_DEBUG_LEVEL >4
                    set_debug_state(0xFFF3, 4); // Debug - Loff offset apply - Offset Applied Margin Mode Invert
#endif
                    // An offset had been applied and is now completely removed...so if it is margin_mode then invert the offset.
                    // Veritcal and Horizontal margin modes must be mutually exclusive.
                    // Therefore the conditional 0x2 == 1 is mutually exclusive in this instance.
                    loff_offset_static_d = 0 - loff_offset_static_d;
                    loff_offset_static_e = 0 - loff_offset_static_e;
                }

                // PSL apply_st_advance
                if (apply)
                {
                    //PSL bank_mismatch_on_apply
                    if ((margin_st & offset_mask_Bnk) ^ (bank & offset_mask_Bnk))
                    {
                        return;
                    }

                    margin_st++;
                }
            }
            //PSL remove_st_advance
            else if (((margin_st & offset_mask_HV) == offset_st_V_off) && !apply)
            {
                margin_st++;
            }
            else
            {
                // No longer in Vertical mode
                return;
            }

        LOFF_LAST:
            mem_pl_field_put(ppe_margin_offset_overlay, lane, margin_st);
        }

        apply_rx_dac_offset(gcr_addr, data_only, bank, loff_offset_static_d);
        apply_rx_dac_offset(gcr_addr, edge_only, bank, loff_offset_static_e);

        io_sleep(get_gcr_addr_thread(gcr_addr));
    } //add offset

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
    int apply_val = 1;



    // PSL setup_remove_pr_offset
    if (!apply)
    {
#if IO_DEBUG_LEVEL >4
        set_debug_state(0xFFF4); // Debug - PRoffset Remove
#endif
        // Dual Mode behavior:
        // Non Margin mode - Normally both banks must have the offset applied before it can be removed. However, a simulation user may override one or the other to short circuit the removal when only one bank is under test. Use with caution
        // Margin mode - Toggling behavior requires coordination of all banks and lanes to use the same value and inversion. Therefore only invert one all offsets are removed


        bool offset_applied_lane;

        //PSL remove_margin0
        if (margin_mode == 0)
        {
            pr_offset_static_d = 0 - pr_offset_static_d;
            pr_offset_static_e = 0 - pr_offset_static_e;
            apply_val = 0;

            // PSL banksel
            if (bank == bank_a)
            {
                offset_applied_lane = (margin_st & (ppe_pr_offset_applied_a_mask >> ppe_margin_offset_overlay_shift)) != 0;
            }
            else
            {
                offset_applied_lane = (margin_st & (ppe_pr_offset_applied_b_mask >> ppe_margin_offset_overlay_shift)) != 0;
            }
        }
        else
        {
            offset_applied_lane  = (margin_st & (offset_mask_HV | offset_mask_PN)) == offset_st_H_neg ;
            offset_applied_lane |= (margin_st & offset_mask_HV) == offset_st_H_off;
            offset_applied_lane &= !((margin_st & offset_mask_Bnk) ^ (bank & offset_mask_Bnk));

            // PSL offset_st_H_end
            if (margin_st == offset_st_H_end)
            {
                margin_st = offset_st_V_on;
                goto PR_LAST;
            }
        }

        // PSL force_remove_if_applied
        if (offset_applied_lane)
        {
            pr_offset_pause = false;
        }
        else
        {
            // Override Pause if margin_st hasn't yet applied offsets.
            pr_offset_pause = true;
        }
    } // !apply

    // PSL apply_or_remove_pr_offset
    if (!pr_offset_pause && (((margin_mode & 0x1) == 0x1) || (margin_mode == 0)))
    {
#if IO_DEBUG_LEVEL >4
        set_debug_state(0xFFF5); // Debug - PRoffset Apply/Remove
#endif

        // PSL horizontal_or_manual_mode
        if ((margin_mode) == 0)
        {
            // PSL banksel
            if (bank == bank_a)
            {
                mem_pl_field_put(ppe_pr_offset_applied_a, lane, apply_val);
            }
            else
            {
                mem_pl_field_put(ppe_pr_offset_applied_b, lane, apply_val);
            }
        }
        else
        {
            // PSL offset_H_shortcut_to_V
            if ((margin_mode & 0x1) == 0)
            {
                margin_st = offset_st_V_on;
                return;
            }

            // PSL offset_st_H_on
            if ((margin_st & offset_mask_HV) == offset_st_H_on)
            {
                // PSL offset_mask_PN
                if ((margin_st & offset_mask_PN) != 0)
                {
#if IO_DEBUG_LEVEL >4
                    set_debug_state(0xFFF6); // Debug - PRoffset Remove - Offset Applied Margin Mode Invert
#endif
                    // An offset had been applied and is now completely removed...so if it is margin_mode then invert the offset.
                    // Veritcal and Horizontal margin modes must be mutually exclusive.
                    // Therefore the conditional 0x2 == 1 is mutually exclusive in this instance.
                    pr_offset_static_d = 0 - pr_offset_static_d;
                    pr_offset_static_e = 0 - pr_offset_static_e;
                }

                // PSL apply_st_advance
                if (apply)
                {
                    //PSL bank_mismatch_on_apply
                    if ((margin_st & offset_mask_Bnk) ^ (bank & offset_mask_Bnk))
                    {
                        return;
                    }

                    margin_st++;
                }
            }
            // PSL remove_st_advance
            else if (((margin_st & offset_mask_HV) == offset_st_H_off) && !apply)
            {
                margin_st++;
            }
            else
            {
                // No longer in Horizontal mode
                return;
            }

        PR_LAST:
            mem_pl_field_put(ppe_margin_offset_overlay, lane, margin_st);
        }

        int pr_offset_vec_d[2]  = {pr_offset_static_d, pr_offset_static_d};
        pr_recenter(gcr_addr, bank, pr_active, Esave, Dsave, pr_offset_vec_d, pr_offset_static_e);
    }
}
