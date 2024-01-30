/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sberegaccess.H $               */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2024                        */
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
 * @file: ppe/sbe/sbefw/sberegaccess.H
 *
 * @brief This file contains interfaces to get/set FW flags either in the
 * scratch registers and/or the FW attributes.
 */

#pragma once

#include <stdint.h>
#include "mbxscratch.H"
#include "cmnglobals.H"

/**
 * @brief Utility singleton that SBEFW can use to read write various scratch
 * registers/FW attributes
 */
class SbeRegAccess
{
    public:
        // Disable copy construction and assignment operators
        SbeRegAccess(const SbeRegAccess&) = delete;
        SbeRegAccess& operator=(const SbeRegAccess&) = delete;

        /**
         * @brief Returns the instance of this class
         *
         * @return A reference to SbeRegAccess
         *
         */
        static SbeRegAccess& theSbeRegAccess()
        {
            return cv_instance;
        }

        /*! Enum for supported Boot modes
         * [ATTR_OCMB_BOOT_FLAGS]: public/src/import/public/hwp/odyssey/xml/attribute_info/ody_perv_attributes.xml
        */
        enum BootMode
        {
            /** 0- AutoBoot ( b'00 ) */
            AUTOBOOT=0,
            /** 1- Pause and Boot (b'01) */
            PAUSE_AND_BOOT,
            /** 2- Jump to runtime (b'10) */
            JUMP_TO_RUNTIME,
            /** 3- Istep (b'11) */
            ISTEP
        };

        /**
         * @brief Initializes the class for use
         *
         * @param [in] forced Forcefully initialise reg access
         *                    singleton.
         *                    Default argument set to false.
         *
         * @return An RC indicating success/failure
         *
         */
        uint32_t init(bool forced=false);

        /* Platform specific functions */
        /**
         * @brief Initializes the class for use based on attribute boot flags
         * if scratch is invalid, internally called from init function
         */
        void platInitAttrBootFlags();

        /* plat specific function ends here */

        /**
         * @brief Update Mbox 11 from the value passed
         *
         * @param [in] i_mbx11 value of mbox 11 to update
         *
         * @return void
         *
        */
        void updateMbx11(const uint64_t i_mbx11);

        /**
         * @brief Update the SBE states into the SBE messaging register. The
         * function does a read-modify-write, so any bits other than the state
         * bits are preserved. The current state of the register is set to
         * i_state, whereas the old current state is copied to previous state
         *
         * @param [in] i_state The current SBE state
         *
         * @return RC indicating success/failure.
         *
         */
        uint32_t updateSbeState(const uint8_t &i_state);

        /**
         * @brief Update the SBE IPL steps into the SBE messaging register. The
         * function does a read-modify-write, so any bits other than the IPL
         * steps are retianed
         *
         * @param [in] i_major IPL major step number
         * @param [in] i_minor IPL minor step number
         *
         * @return RC indicating success/failure.
         *
         */
        uint32_t updateSbeStep(const uint8_t i_major, const uint8_t i_minor);

        /**
         * @brief Set the SBE ready bit into the SBE messaging register
         * (meaning that SBE control loop is initialized) The function does a
         * read-modify-write, so any bits other than the SBE ready bit remain
         * unchanged.
         *
         * @return RC indicating success/failure.
         *
         */
        uint32_t setSbeReady();

        /**
         * @brief Set the MPIPL mode bit into the mailbox scratch reg. 3
         * The function does a read-modify-write, so any bits other than the
         * SBE ready bit remain unchanged. It also updates the attribute
         * ATTR_MPIPL
         *
         * @param i_set [in] true == set, false == clear
         *
         * @return RC indicating success/failure.
         *
         */
        uint32_t setMpIplMode(const bool i_set);

        /**
         * @brief get Boot Mode
         *
         * @return BootMode based on boot flags value
         * 0 - AUTOBOOT
         * 1 - PAUSE_AND_BOOT
         * 2 - JUMP_TO_RUNTIME
         * 3 - ISTEP
         *
         */
        inline BootMode getBootMode() const
        {
            return static_cast<BootMode>(mbx11.iv_bootFlags);
        }

        /**
         * @brief Check if architected data collection in
         * MPIPL path needs to be skipped.
         * @return true if skip dump collection, false otherwise
         *
         */
        inline bool isSkipMpiplDump() const
        {
            return mbx11.iv_skipMpiplDumpCollection;
        }

        /**
         * @brief Check if SBE should send internal FFDC for any chip op
         * failures as a part of the response
         *
         * @return true if FFDC should be sent, false otherwise
         *
         */
        inline bool isSendInternalFFDC() const
        {
            return !mbx11.iv_disableFFDC;
        }

        inline bool isSystemCheckStopForDMTFail() const
        {
            return !mbx11.iv_dontCheckStopForDMT;
        }

        /**
         * @brief Check if mbox 11 is valid
         *
         * @return true if in mbx11 valid, false otherwise
         *
         */
        inline bool isMbx11Valid() const
        {
            return mbx16.iv_mbx11Valid;
        }

        /**
         * @brief Get the SBE current State
         *
         * @return SBE current State, sbeState enum
         *
         */
        uint64_t getSbeState() const
        {
            return messagingReg.iv_currState;
        }

        /**
         * @brief Get the SBE previous State
         *
         * @return SBE previous State, sbeState enum
         *
         */
        uint64_t getSbePrevState() const
        {
            return messagingReg.iv_prevState;
        }

        /**
         * @brief Get the SBE major istep number
         *
         * @return SBE current major istep number
         *
         */
        uint8_t getSbeMajorIstepNumber() const
        {
            return messagingReg.iv_majorStep;
        }

        /**
         * @brief Get the SBE minor istep number
         *
         * @return SBE current minor istep number
         *
         */
        uint8_t getSbeMinorIstepNumber() const
        {
            return messagingReg.iv_minorStep;
        }

        /**
         * @brief Get the Boot Selection from LFR
         *
         * @return uint8_t boot selection
         */
        uint8_t getBootSelection()
        {
            return lfrReg.boot_selection;
        }

        /**
         * @brief Get the Secure Mode bit from lfr reg(SAB bit)
         *        This bit is set in SROM code by reading SAB bit
         *
         * @return true SAB set
         * @return false SAB not set
         */
        bool getSecureMode()
        {
            return lfrReg.secure_mode;
        }

        /**
         * @brief Get the Is Imprint Mode value from LFR reg
         *        This value is updated in boot loader flow
         *        based on runtime image prefix header flags
         *        in secure header
         *
         * @return true runtime image is imprint/lab signed
         * @return false runtime image is production signed
         */
        bool getIsImprintMode()
        {
            return lfrReg.is_imprint_mode;
        }

        /**
         * @brief Get the Emulate Security Enable bit from scratch11
         *        Emulates SAB bit that based on OTPROM fuse values
         *
         * @return true user want to emulate SAB on
         * @return false user does not want to emulate SAB on
         */
        bool getEmulateSABSet()
        {
            return mbx11.iv_emulateSABSet;
        }

        /**
         * @brief Get the Force Production Mode bit from scratch11
         *
         * @return true User wants to force production level security
         * @return false User wants to proceed with imprint/production
         *               level security based on runtime secure header
         *               prefix flags
         */
        bool getForceProductionMode()
        {
            return mbx11.iv_forceProductionMode;
        }

        /**
         * @brief Update the async bit into the SBE messaging register. The
         * function does a read-modify-write, so any bits other than the async
         * bits are retained.
         *
         * @param [in] i_on True to turn on bit, false to turn off
         *
         * @return RC indicating success/failure.
         *
         */
        uint32_t updateAsyncFFDCBit( bool i_on );

        /**
         * @brief Get Disable invalid scom address bit from LFR
         *
         * @return true  - skip invalid scom address check
         *         false - do invalid scom address check
         */
        bool getDisableInvalidScomAddrBit()
        {
            return lfrReg.disable_invalid_scom_addr_check;
        }

        /*
         * @brief Get disable scom filtering bit from LFR
         *
         * @return scom filtering disable allowed
         */
        bool getDisableScomFilteringBit()
        {
            return lfrReg.disable_scom_filtering;
        }

    private:

        mbx11_t mbx11 = {0};
        mbx16_t mbx16 = {0};
        messagingReg_t messagingReg = {0};
        sbe_local_LFR lfrReg;

        /**
        * @brief Constructor
        */
        SbeRegAccess()
        {}

        static SbeRegAccess cv_instance;
};
