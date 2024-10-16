/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/boltonlib/gpe/gpe_init.c $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2021,2024                        */
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

/// \file gpe_init.c
/// \brief PK initialization for GPE
///
/// The entry points in this routine are used during initialization.  This
/// code space can be deallocated and reassigned after application
/// initialization if required.

#if defined(__PK__)
    #include "pk.h"
    #include "pk_api.h"
#elif defined(__IOTA__)
    #include "iota.h"
    #include "iota_trace.h"
#endif

#include "ocb_register_addresses.h"

/// GPE environment initial setup.
///
/// This is setup common to all GPE HW Macro applications.  This setup takes place
/// during boot, before main() is called.

void
__hwmacro_setup(void)
{
    uint64_t owned_actual = 0;
    uint64_t reverse_polarity = 0;

#ifdef __OCC_PLAT
    uint64_t oirrA = 0;
    uint64_t oirrB = 0;
    uint64_t oirrC = 0;

    //verify that this code is running on the correct GPE instance (one time check)
    if((mfspr(SPRN_PIR) & PIR_PPE_INSTANCE_MASK) != APPCFG_PM_INSTANCE_ID)
    {
        //APPCFG_PM_INSTANCE_ID does not match actual instance ID!
        APPCFG_PANIC(PMHW_INSTANCE_MISMATCH);
    }

    // If two engines are ever started at about the same time there could be
    // a race condition, but that would require an ambitious unit tester,
    // and won't happen in production.  Also,the engines should be writing
    // the same values anyway.
    if(0 == (in32(OCB_OIRR0A) | in32(OCB_OIRR1A) | in32(OCB_OIRR0B) |
             in32(OCB_OIRR1B) | in32(OCB_OIRR0C) | in32(OCB_OIRR1C)))
    {
        //This instance must be the first instance to run within the OCC
        //This will be done while all external interrupts are masked.
        //APPCFG_TRACE("Initializing External Interrupt Routing Registers");
        out32(OCB_OIMR0_OR, 0xffffffff);
        out32(OCB_OIMR1_OR, 0xffffffff);

        out32(OCB_OIRR0A, (uint32_t)(g_ext_irqs_routeA >> 32));
        out32(OCB_OIRR1A, (uint32_t)g_ext_irqs_routeA);
        out32(OCB_OIRR0B, (uint32_t)(g_ext_irqs_routeB >> 32));
        out32(OCB_OIRR1B, (uint32_t)g_ext_irqs_routeB);
        out32(OCB_OIRR0C, (uint32_t)(g_ext_irqs_routeC >> 32));
        out32(OCB_OIRR1C, (uint32_t)g_ext_irqs_routeC);
    }

    //Determine from the routing registers which irqs are owned by this instance
    //NOTE: For info about the routing rules, see description of the OIRRna reg
    //      in the PM HW Spec.

    oirrA = ((uint64_t)in32(OCB_OIRR0A)) << 32;
    oirrA |= in32(OCB_OIRR1A);
    oirrB = ((uint64_t)in32(OCB_OIRR0B)) << 32;
    oirrB |= in32(OCB_OIRR1B);
    oirrC = ((uint64_t)in32(OCB_OIRR0C)) << 32;
    oirrC |= in32(OCB_OIRR1C);

    //All interrupts routed to a GPE will have a bit set in routeA
    owned_actual = oirrA;

    //wittle it down by bits in the routeB register
#if APPCFG_PM_INSTANCE_ID & 0x2
    owned_actual &= oirrB;
#else
    owned_actual &= ~oirrB;
#endif

    //wittle it down further by bits in the routeC register
#if APPCFG_PM_INSTANCE_ID & 0x1
    owned_actual &= oirrC;
#else
    owned_actual &= ~oirrC;
#endif

#endif //End of __OCC_PLAT

#ifdef __TCC_PLAT
    uint64_t oirrB = 0;
    uint64_t oirrC = 0;

    //verify that this code is running on the correct GPE instance (one time check)
    if((mfspr(SPRN_PIR) & PIR_PPE_INSTANCE_MASK) != APPCFG_PM_INSTANCE_ID)
    {
        //APPCFG_PM_INSTANCE_ID does not match actual instance ID!
        APPCFG_PANIC(PMHW_INSTANCE_MISMATCH);
    }

    // If two engines are ever started at about the same time there could be
    // a race condition, but that would require an ambitious unit tester,
    // and won't happen in production.  Also,the engines should be writing
    // the same values anyway.
    if(0 == (in32(OCB_OIRR0B) |
             in32(OCB_OIRR1B) | in32(OCB_OIRR0C) | in32(OCB_OIRR1C)))
    {
        //This instance must be the first instance to run within the OCC
        //This will be done while all external interrupts are masked.
        //APPCFG_TRACE("Initializing External Interrupt Routing Registers");
        out32(OCB_OIMR0_OR, 0xffffffff);
        out32(OCB_OIMR1_OR, 0xffffffff);

        out32(OCB_OIRR0B, (uint32_t)(g_ext_irqs_routeB >> 32));
        out32(OCB_OIRR1B, (uint32_t)g_ext_irqs_routeB);
        out32(OCB_OIRR0C, (uint32_t)(g_ext_irqs_routeC >> 32));
        out32(OCB_OIRR1C, (uint32_t)g_ext_irqs_routeC);
    }

    //Determine from the routing registers which irqs are owned by this instance
    //NOTE: For info about the routing rules, see description of the TIRRna reg
    //      in the PM HW Spec.

    oirrB = ((uint64_t)in32(OCB_OIRR0B)) << 32;
    oirrB |= in32(OCB_OIRR1B);
    oirrC = ((uint64_t)in32(OCB_OIRR0C)) << 32;
    oirrC |= in32(OCB_OIRR1C);

#if   (APPCFG_PM_INSTANCE_ID == 0x0)
    owned_actual = ~oirrB & ~oirrC;
#elif (APPCFG_PM_INSTANCE_ID == 0x1)
    owned_actual = ~oirrB &  oirrC;
#elif (APPCFG_PM_INSTANCE_ID == 0x2)
    owned_actual =  oirrB & ~oirrC;
#elif (APPCFG_PM_INSTANCE_ID == 0x3)
    owned_actual =  oirrB &  oirrC;
#endif

#endif //End of __TCC_PLAT

    //Panic if we don't own the irqs we were expecting
    //NOTE: we don't panic if we are given more IRQ's than expected
    if((owned_actual & g_ext_irqs_owned) != g_ext_irqs_owned)
    {
        //IRQ's were not routed to us correctly.
        PK_TRACE_ERR("ERR: Owned actual IRQs (=0x%08x_%08x) != Owned planned IRQs (=0x%08x_%08x)",
                     (uint32_t)(owned_actual >> 32), (uint32_t)owned_actual,
                     (uint32_t)(g_ext_irqs_owned >> 32), (uint32_t)g_ext_irqs_owned);
        APPCFG_PANIC(PMHW_IRQ_ROUTING_ERROR);
    }

    //Mask all external interrupts owned by this instance
    //(even the ones given to us that we weren't expecting)
    out32(OCB_OIMR0_OR, (uint32_t)(owned_actual >> 32));
    out32(OCB_OIMR1_OR, (uint32_t)owned_actual);

    //Set the interrupt type for all interrupts owned by this instance
    out32(OCB_OITR0_CLR, (uint32_t)(g_ext_irqs_owned >> 32));
    out32(OCB_OITR1_CLR, (uint32_t)g_ext_irqs_owned);
    out32(OCB_OITR0_OR, (uint32_t)(g_ext_irqs_type >> 32));
    out32(OCB_OITR1_OR, (uint32_t)g_ext_irqs_type);

    //Set the interrupt polarity for all interrupts owned by this instance
    out32(OCB_OIEPR0_CLR, (uint32_t)(g_ext_irqs_owned >> 32));
    out32(OCB_OIEPR1_CLR, (uint32_t)g_ext_irqs_owned);
    out32(OCB_OIEPR0_OR, (uint32_t)(g_ext_irqs_polarity >> 32));
    out32(OCB_OIEPR1_OR, (uint32_t)g_ext_irqs_polarity);

    //clear the status of all external interrupts owned by this instance
    out32(OCB_OISR0_CLR, ((uint32_t)(g_ext_irqs_owned >> 32)));
    out32(OCB_OISR1_CLR, ((uint32_t)g_ext_irqs_owned));

    //set the status for interrupts that have reverse polarity
    reverse_polarity = ~g_ext_irqs_polarity & g_ext_irqs_owned;
    out32(OCB_OISR0_OR, ((uint32_t)(reverse_polarity >> 32)));
    out32(OCB_OISR1_OR, ((uint32_t)reverse_polarity));

    //Unmask the interrupts owned by this instance that are to be enabled by default
    out32(OCB_OIMR0_CLR, (uint32_t)(g_ext_irqs_enable >> 32));
    out32(OCB_OIMR1_CLR, (uint32_t)g_ext_irqs_enable);

    //Wait for the last out32 operation to complete
    sync();

}
