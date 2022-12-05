/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sberegaccess.H $               */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2023                        */
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

#ifndef __SBEFW_SBEREGACCESS_H
#define __SBEFW_SBEREGACCESS_H

#include <stdint.h>
#include "mbxscratch.H"

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
         * @brief Check if we are in ISTEP IPL mode
         *
         * @return true if in istep mode, false otherwise
         *
         */
        inline bool isIstepMode() const
        {
            return mbx11.iv_istepMode;
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
         *
         * @brief Check if SBE should directly go to runtime state
         *
         * @return true if SBE should go directly to runtime state,
         * false otherwise
         *
         */
        inline bool isDestBitRuntime() const
        {
            return mbx11.iv_sbeDestRuntime;
        }

        /**
         * @brief Check if SBE should send internal FFDC for any chip op
         * failures as a part of the response
         *
         * @return true if in istep mode, false otherwise
         *
         */
        inline bool isSendInternalFFDCSet() const
        {
            return mbx11.iv_sendFFDC;
        }

        inline bool isSystemCheckStopForDMTFail() const
        {
            return !mbx11.iv_dontCheckStopForDMT;
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
         * @brief Check if Invalid Scom Addr Check is not required
         *
         * @return true  - skip invalid scom address check
         *         false - do invalid scom address check
         */
        bool isSbeRegressionBit();

        /*
         * @brief Get disable scom filtering bit
         *
         * @return scom filtering disable allowed
         */
        bool disableScomFiltering();

    private:

        mbx11_t mbx11 = {0};
        mbx16_t mbx16 = {0};
        messagingReg_t messagingReg = {0};

        /**
        * @brief Constructor
        */
        SbeRegAccess()
        {}

        static SbeRegAccess cv_instance;
};
#endif //__SBEFW_SBEREGACCESS_H

