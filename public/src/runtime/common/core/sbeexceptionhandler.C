/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbeexceptionhandler.C $        */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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

#include "sbeexceptionhandler.H"

/**
 * @brief This file contains all common SBE handlers and save off functions
 *        If the project has project specific save off, exceptionhandler.C & .H
 *        files need to be implemented in project specific dirs.
 *        __plat_sbe_register_saveoff --> project specific save off API
 *        platregistersave_t --> project specific save off struct
 *
 *       To enable project specific collection, PLAT_REGISTER_SAVEOFF macro to be defined
 *       in project specific cross_file.txt.
 *
 */

registersave_t __g_register_ffdc __attribute__((aligned(8), section (".sbss")));

#define REGISTER_SAVEOFF_FORMAT_VERSION 0x2

/**
 * @brief  Common SBE handler to save off specific registers
 *
 */
extern "C" void __sbe_register_saveoff()
{
    asm(
    "# Save r4, r5, r6 to stack, since it is going to be used\n"
    "# inside here, move the stack by 12\n"
    "stwu %r1, -12(%r1)\n"
    "# Store r4 into stack\n"
    "stw  %r4, 0(%r1)\n"
    "# Store r5 into stack\n"
    "stw  %r5, 4(%r1)\n"
    "# Store r6 into stack\n"
    "stw  %r6, 8(%r1)\n"

    "# Start gathering registers into FFDC\n"
    "# Read SRR0 into r4\n"
    "mfsrr0  %r4\n"
    "stw %r4, __g_register_ffdc+4@sda21(0)\n"

    "# Read SRR1 into r4\n"
    "mfsrr1  %r4\n"
    "stw %r4, __g_register_ffdc+8@sda21(0)\n"

    "# Read ISR into r4\n"
    "mfisr  %r4\n"
    "stw %r4, __g_register_ffdc+12@sda21(0)\n"

    "# Read LR into r4\n"
    "mflr  %r4\n"
    "stw %r4, __g_register_ffdc+16@sda21(0)\n"

    "# Read EIMR into r4\n"
    "lis %r6, 0xc000\n"
    "lvd %d4, 0x0020(%r6)\n"
    "stw %r4, __g_register_ffdc+20@sda21(0)\n"
    "stw %r5, __g_register_ffdc+24@sda21(0)\n"

    "# Read EISR into r4\n"
    "lis %r6, 0xc000\n"
    "lvd %d4, 0x0000(%r6)\n"
    "stw %r4, __g_register_ffdc+28@sda21(0)\n"
    "stw %r5, __g_register_ffdc+32@sda21(0)\n"

    "# Read EISTR into r4\n"
    "lis %r6, 0xc000\n"
    "lvd %d4, 0x0080(%r6)\n"
    "stw %r4, __g_register_ffdc+36@sda21(0)\n"
    "stw %r5, __g_register_ffdc+40@sda21(0)\n"

    "# Call project specific API to collect project regs\n"
    #ifdef PLAT_REGISTER_SAVEOFF
        "bl __plat_sbe_register_saveoff\n"
    #endif

    "# Write versioning into the lower 32bit of d5 i.e. r6\n"
    "#lvd %d5, __g_sbe_register_save_version@sda21(0)\n"
    "# Directly load Versioninfo into upper 16bits in r4 \n"
    "lis %r4, 0x2\n"
    "# Write magic byte and valid byte in the lower 16bits\n"
    "ori %r4, %r4, 0xA501\n"
    "stw %r4, __g_register_ffdc@sda21(0)\n"

    "# Get the address of the register ffdc struct and store it in sprg0\n"
    "lis %r6, __g_register_ffdc@h\n"
    "ori %r6, %r6, __g_register_ffdc@l\n"
    "# Store the content of r6 read above into sprg0\n"
    "mtsprg0 %r6\n"

    "# Get back the original value of the registers stored in stack\n"
    "lwz %r6, 8(%r1)\n"
    "lwz %r5, 4(%r1)\n"
    "lwz %r4, 0(%r1)\n"
    "addi %r1, %r1, 12\n"
    "#NOTE pk_halt would alter r31 content\n"
    "b pk_halt\n"
    );
}

/**
 * @brief Common SBE handler for the PPE machine check interrupt
 *
 */
extern "C" void __sbe_machine_check_handler()
{
    asm(

        "# Save r0, r1, r2, r3,r4, r5, r6, r7, r8, r9, r10, r13, r28, r29, r30, r31\n"
        "# lr to stack, since it is going to be used by\n"
        "# this handler\n"
        "# NOTE:stcxu instruction cannot be used to store the stack content as it will modify EDR.\n"
        "# If EDR is modified the __scom_error check logic will not work\n"
        "stwu %r1, -80(%r1)\n"
        "stw  %r0, 0(%r1)\n"
        "stw  %r2, 4(%r1)\n"
        "stw  %r3, 8(%r1)\n"
        "stw  %r4, 12(%r1)\n"
        "stw  %r5, 16(%r1)\n"
        "stw  %r6, 20(%r1)\n"
        "stw  %r7, 24(%r1)\n"
        "stw  %r8, 28(%r1)\n"
        "stw  %r9, 32(%r1)\n"
        "stw  %r10, 36(%r1)\n"
        "stw  %r13, 40(%r1)\n"
        "stw  %r28, 44(%r1)\n"
        "stw  %r29, 48(%r1)\n"
        "stw  %r30, 52(%r1)\n"
        "stw  %r31, 56(%r1)\n"
        "mflr %r0\n"
        "stw  %r0, 60(%r1)\n"

        "# need to check whether the machine check caused by a scom error or not\n"
        "# if it is scom error, we have to continue the sbe firmware, other wise halt\n"
        "# This is the Data_MC path, EDR contains the Data Addr causing the MC\n"
        "mfedr %r4\n"
        "srawi %r4, %r4, 16\n"
        "# If the address is in the range 0x00000000 - 0x7f000000, we treat it as a\n"
        "# failed scom and jump to __scom_error\n"
        "cmplwi %r4, 0x8000\n"
        "blt __scom_error\n"
        "# Else, save-off and halt the SBE\n"
        "# Save-off Register FFDC and Halt\n"

        "# Save-off Register FFDC and Halt\n"
        "b __sbe_register_saveoff\n"

        "__scom_error:\n"
        "# The srr0 contains the address of the instruction that caused the machine\n"
        "# check (since the the interrupt is raised *before* the instruction\n"
        "# completed execution). Since we want the code to continue with the next\n"
        "# instruction, we increment srr0 by 4, restore r4, and rfi to branch to srr0\n"
        "mfsrr0 %r4\n"
        "addi %r4, %r4, 4\n"
        "mtsrr0 %r4\n"

        "#lwz %r0, 0(%r1)\n"
        "lwz %r2, 4(%r1)\n"
        "lwz %r3, 8(%r1)\n"
        "lwz %r4, 12(%r1)\n"
        "lwz %r5, 16(%r1)\n"
        "lwz %r6, 20(%r1)\n"
        "lwz %r7, 24(%r1)\n"
        "lwz %r8, 28(%r1)\n"
        "lwz %r9, 32(%r1)\n"
        "lwz %r10, 36(%r1)\n"
        "lwz %r13, 40(%r1)\n"
        "lwz %r28, 44(%r1)\n"
        "lwz %r29, 48(%r1)\n"
        "lwz %r30, 52(%r1)\n"
        "lwz %r31, 56(%r1)\n"
        "lwz %r0, 60(%r1)\n"
        "mtlr %r0\n"
        "lwz %r0, 0(%r1)\n"
        "addi %r1, %r1, 80\n"

        "rfi\n"
    );
}