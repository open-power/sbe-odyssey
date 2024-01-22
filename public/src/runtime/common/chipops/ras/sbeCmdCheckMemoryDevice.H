/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/ras/sbeCmdCheckMemoryDevice.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2024                        */
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

#pragma once

#include "sbe_sp_intf.H"

/*
 * @brief structure for check memory chip-op
 */
struct memCheckCmdMsg_t
{
    uint8_t reserved;
    uint8_t scope;
    uint8_t side;
    uint8_t devId;

    /*
     * @brief Member function to validate input parameters
     *
     * @param[in] scope
     * @param[in] side
     * @param[in] devId
     *
     * @return rc
     */
    uint32_t checkInputParamsValid()
    {
        uint32_t rc = SBE_SEC_OPERATION_SUCCESSFUL;

        do
        {
            // Check for validity of scope
            // 0x01: Image
            // 0x02: Full side
            if ( (scope != SCOPE_IMAGE_ONLY) && (scope != SCOPE_FULL_SIDE) )
            {
                SBE_ERROR("Invalid scope check value:[%d]", scope);
                rc = SBE_SEC_INVALID_PARAMS;
                break;
            }

            // Check for validity of side param
            if ( (side == INVALID_SIDE) || (side > END_OF_SIDE_LIST) )
            {
                SBE_ERROR("Invalid memory side value:[%d]", side);
                rc = SBE_SEC_INVALID_PARAMS;
                break;
            }

            // Check for validity of device Id param
            if ( (devId == INVALID_DEVICE_ID) || (devId > END_OF_DEVICE_LIST) )
            {
                SBE_ERROR("Invalid device Id:[%d]", devId);
                rc = SBE_SEC_INVALID_PARAMS;
                break;
            }
        }while(false);

        return rc;
    }
};

/**
 * @brief Structure for memory device status details
 */
struct memDeviceStatusDetails_t
{
    // Side value
    uint8_t side;

    // Device Id
    uint8_t devId;

    // Scrub status
    uint16_t status;

    // Number of correctible errors
    uint16_t numOfCE;

    // Number of unrecoverable errors
    uint16_t numOfUE;

    /**
     * @brief Explicit constructor to initialize its members.
     */
    explicit memDeviceStatusDetails_t() :
        side(0),
        devId(0),
        status(0),
        numOfCE(0),
        numOfUE(0)
    {}
};

/**
 * @brief Implementation of the memory device check chipop (0xA809)
 *
 * @param[in] i_pArg Buffer to be passed to the function
 *
 * @return the RC from the FIFO access utility
 */
uint32_t sbeCmdCheckMemoryDevice(uint8_t *i_pArg);

