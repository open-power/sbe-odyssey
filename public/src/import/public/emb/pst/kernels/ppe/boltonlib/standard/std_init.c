/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/emb/pst/kernels/ppe/boltonlib/standard/std_init.c $ */
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

/// \file std_init.c
/// \brief PK initialization for a standard PPE.
///
/// The entry points in this routine are used during initialization.  This
/// code space can be deallocated and reassigned after application
/// initialization if required.

#if defined(__PK__)
    #include "pk.h"
#elif defined(__IOTA__)
    #include "iota.h"
#endif

/// Standard PPE environment initial setup.
///
/// This is setup common to all standard PPE Macro applications.  This setup takes place
/// during boot, before main() is called.

void
__hwmacro_setup(void)
{
#ifndef __SBE__

    //mask all interrupts to prevent spurious pulse to PPE
    out64(STD_LCL_EIMR, 0xffffffffffffffffull);

    //set up the configured polarity
    out64(STD_LCL_EIPR, g_ext_irqs_polarity);

    //set up the configured type
    out64(STD_LCL_EITR, g_ext_irqs_type);

    //clear the status of all edge interrupts
    out64(STD_LCL_EISR_CLR, g_ext_irqs_type);

    //unmask the interrupts that are to be enabled by default
    out64(STD_LCL_EIMR_CLR, g_ext_irqs_enable);

    //wait for the last operation to complete
    sync();

#else

    // Below piece of code does nothing in case of SBE.
    // This is just added so as to not discard global variables
    asm(
        "blr \n"
        "nop \n"
        "nop \n"
        "nop \n"
        "nop \n"
    );

    g_ext_irqs_enable = 0xdead;
    g_ext_irqs_type = 0xc0de;
    g_ext_irqs_polarity = 0xffdc;

#endif
}
