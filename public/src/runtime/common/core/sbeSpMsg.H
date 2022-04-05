/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/core/sbeSpMsg.H $                   */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2022                        */
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
 * @file: ppe/sbe/sbefw/sbeSpMsg.H
 *
 * @brief This file contains the message structures for FIFO
 *        communication.
 *
 */

#ifndef __SBEFW_SBESP_MSG_H
#define __SBEFW_SBESP_MSG_H

#include "sbe_sp_intf.H"

//TODO: P11SBE Porting
#if 0
using namespace fapi2;
#endif

static const uint32_t SBE_PUT_RING_RS4_MAX_DOUBLE_WORDS =
                      SBE_PUT_RING_RS4_MAX_PAYLOAD_BYTES/sizeof(uint64_t);
// @NOTE Make sure all FIFO structures are 32 bit alligned ( the largest
// member should be atleast 4 byte). It is required as in sbe fifo
// operation we are casting these structures to uint32_t pointer. It can
// cause alignment issue if largest member of structure is not atleast
// 32 bit. We can use bit fields to optimize  memory requirements.
// These are two coding guidleines we will follow for this file
// 1. All data members less than 32 bits will be bit fields
// 2. All data members more than 32 buts will be divided into small
//    members of 32 bit each. This is required as compiler pads structure
//    to largest data member and we do not want extra padding for data
//    members gretater than 32 bits ( e.g. uint64_t )
/**
  * @brief Command Request Header
  */
typedef struct
{
    uint32_t len;
    uint32_t reserved:12;
    uint32_t clientId:4;
    uint32_t cmdClass:8;
    uint32_t command:8;
}sbeFifoCmdReqBuf_t;

/**
  * @brief structure for generic header for fifo response.
  *
  */
typedef struct
{
    private:
    uint32_t _magicCode:16;
    uint32_t _cmdClass:8;
    uint32_t _command:8;
    uint32_t _primaryStatus:16;
    uint32_t _secondaryStatus:16;

    public:
    const uint8_t  cmdClass() const        { return _cmdClass; }
    const uint8_t  command() const         { return _command; }
    const uint16_t primaryStatus() const   { return _primaryStatus; }
    const uint16_t secondaryStatus() const { return _secondaryStatus; }
    const uint32_t rcStatus() const { return  ((uint32_t)_primaryStatus << 16 ) | _secondaryStatus; }
    /**
      * @brief set the primary and secondary status
      *
      * @param[in] i_prim  Primary status
      * @param[in] i_sec   Secondary status
      *
      * @return
      */
    void setStatus( const uint16_t i_prim, const uint16_t i_sec);

    /**
      * @brief set initial values for response header
      *
      * @note  We did not set this in constructor as based on use case
      *        it is possible that SBE_GLOBAL->sbeFifoCmdHdr does not have proper
      *        values at time of object creation.
      *
      */
    void init();
}sbeRespGenHdr_t;

/**
  * @brief structure for ffdc header for fifo response.
  *
  */
typedef struct sbeResponseFfdc
{
    uint32_t magicBytes:16;
    uint32_t lenInWords:16;  // length in word( 4 byte )
    uint32_t seqId:16;
    uint32_t cmdClass:8;
    uint32_t cmd:8;
    uint32_t fapiRc;

    /**
      * @brief set failed command information
      *
      * @param[in] i_seqId      sequence Id of command
      * @param[in] i_cmdClass   command class
      * @param[in] i_cmd        command
      *
      * @return
      */
    void inline setCmdInfo(const uint16_t i_seqId,
                           const uint8_t i_cmdClass,
                           const uint8_t i_cmd)
    {
        seqId = i_seqId;
        cmdClass = i_cmdClass;
        cmd = i_cmd;
    }

    /**
      * @brief set rc
      *
      * @param[in] i_rc  FAPI RC
      *
      * @return
      */
    void setRc(const uint32_t i_rc)
    {
        fapiRc = i_rc;
    }

    /**
      * @brief return fapiRc
      *
      * @return fapiRc
      */
    uint32_t getRc() const
    {
        return fapiRc;
    }

    /**
      * @brief constructor
      *
      * @param[in] i_rc  FAPI RC
      *
      * @return
      */
    sbeResponseFfdc()
    {
        magicBytes = 0xFFDC;
        seqId = 0;
        cmdClass = SBE_CMD_CLASS_UNKNOWN;
        cmd = SBE_CMD_UNKNOWN;
        lenInWords  = ( sizeof(uint32_t )    // For magicBytes + lenInWords
                        + sizeof(uint32_t)   // For SeqId + CmdClass + Cmd
                        + sizeof(fapiRc))
                           / sizeof(uint32_t);
        fapiRc = 0;
    }
}sbeResponseFfdc_t;

/**
  * @brief Command response structure to hold the primary and secondary
  *        status values. This will be utilized when a command class
  *        validation or state machine check fails.
  *
  */
typedef struct
{
    uint32_t          prim_status:16 ;    // Primary Response Status
    uint32_t          sec_status:16  ;    // Secondary Response Status

    /**
     * @brief initialize the response status
     **/
    void init()
    {
        prim_status   = SBE_PRI_OPERATION_SUCCESSFUL;
        sec_status    = SBE_SEC_OPERATION_SUCCESSFUL;
    }

    /**
     * @brief set the primary and secondary status
     *
     * @param[in] i_prim      Primary status
     * @param[in] i_sec       Secondary status
     *
     **/
    void setStatus(const uint16_t i_prim,
                   const uint16_t i_sec);
} sbeCmdRespHdr_t;

/**
 * @brief Get Ring access message header
 */
typedef struct
{
    uint32_t ringAddr;
    uint32_t ringLenInBits;
    uint32_t reserved:16;
    uint32_t ringMode:16;
}sbeGetRingAccessMsgHdr_t;

typedef struct
{
    uint64_t rs4Payload[SBE_PUT_RING_RS4_MAX_DOUBLE_WORDS];
}sbePutRingMsg_t;

#endif // __SBEFW_SBESP_MSG_H
