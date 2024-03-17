/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/ffdc/sbeffdctype.H $           */
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

#pragma once

#include "sbeffdctypecommon.H"
#include "error_info_defs.H"
#include "sbeSpMsg.H"


enum SBE_FFDC_DD_LEVEL
{
    SBE_FFDC_DD1 = 1,
};

/**
 * @brief Data type for poz SBE ffdc packet
 *
 * FFDC Response packet which is recognize the below structure data
 * +-------------+-------------------+---------------+---------------+
 * | Byte 0      |       Byte 1      |     Byte 2    |    Bytes 3    |
 * +-------------+-------------------+---------------+---------------+
 * |    Magic Bytes 0xFBAD           |         Len in words          | \-
 * +-------------+-------------------+---------------+---------------+   |
 * |           Seq ID                |   Cmd class   |     Cmd       |   |
 * +-----------------------------------------------------------------+   |-> Respose Header @ref pozFfdcRespHeader_t
 * |           SLID                  |   Severity    |   Chip ID     |   |   (header)
 * +-----------------------------------------------------------------+   |
 * |                          FAPI RC ( HWP )                        | \-
 * +-----------------------------------------------------------------+
 * |                        HWP FFDC Dump Fields                     | -->  dumpFields
 * +-----------------------------------------------------------------+
*/
typedef struct __attribute__((aligned(4)))
{
    pozFfdcRespHeader_t    header;     // poz Header FFDC header
    uint32_t               dumpFields; // bitmapped dumpFields [ LOCAL_FFDC/REG_FFDC ]
} pozHwpFfdcPackageFormat_t;

/**
 * @brief Data type for poz plat ffdc packet
 *
 * Below diagram represents the data in structure in FFDC response packet
 * +-------------+-------------------+---------------+---------------+
 * | Byte 0      |       Byte 1      |     Byte 2    |    Bytes 3    |
 * +-------------+-------------------+---------------+---------------+
 * |    Magic Bytes 0xFBAD           |         Len in words          | \-
 * +-------------+-------------------+---------------+---------------+   |
 * |           Seq ID                |   Cmd class   |     Cmd       |   |
 * +-----------------------------------------------------------------+   |-> Respose Header
 * |           SLID                  |   Severity    |   Chip ID     |   |      @ref pozFfdcRespHeader_t
 * +-----------------------------------------------------------------+   |
 * |                          FAPI RC ( HWP )                        | \-
 * +-------------+-------------------+---------------+---------------+
 * |        Primary Status           |        Secondary Status       | \-
 * +-----------------------------------------------------------------+   |
 * |                           FW Commit ID                          |   |-> Plat FFDC header
 * +-------------+-------------------+---------------+---------------+   |      @ref sbePlatHeader_t
 * |   Reserved  |     DD Major      |    DD Minor   |   Thread ID   | \-
 * +-----------------------------------------------------------------+
 * |                 SBE FFDC DUMP FIELDS (Bit Mapped)               |      --> @ref dumpFields_t
 * +-------------+-------------------+---------------+---------------+
*/
typedef struct __attribute__((aligned(4)))
{
    pozFfdcRespHeader_t          header;     // poz Header FFDC header
    sbePlatHeader_t              platHeader; // Plat header (commit ID + DD level)
    uint32_t                     dumpFields; // bitmapped dumpFields [ TRACE / ATTR ]
} pozPlatFfdcPackageFormat_t;


/**************************** Structure for NODE's ****************************/

/**
 * FFDC will create in heap space at the time of RC constructor.
 * Created FFDC will considered as a Node and will linked created node while
 * committing it (by using logError/logFatalError API's). below Diagram
 * shown how node and FFDC structure sit'in heap (scratch)
 *
 * Limitation: In case Created FFDC not committed and clear the current_err (RC),
 * then lost the created the FFDC node pointer (node address). That will stay
 * in heap until clearing all heap space
 *
 * Figure: 2
 * +-----------------------------------------------------------------+
 * |              Node data (@ref pozFfdcNode_t)                     |
 * +-----------------------------------------------------------------+
 * |                  FFDC data (HWP & Plat)                         |
 * +-----------------------------------------------------------------+
 *
 *
 *
 * Node (FFDC) will be created at the time the Failure. And allocate the in heap
 * Node1 and Node 2 are two FFDC are sit's in heap while commited it will
 * linked to *next. First commited will be in @ref pozFfdcCtrl_t.
 * At the time of streaming out isFatal flag is set then last stream
 * out full trace with seperate FFDC same slid ID
 *
 *                                  Create FFDC on Failure            Create FFDC on Failure
 *                       logError           ^              logError             ^
 *                         ^                |                 ^                 |
 *                         |          +--------------+        |         +--------------+
 * iv_firstCommitted <-- Committed --  |    Node 1    | <-- Committed -- |    Node 2    | --> fatal
 *                                    +--------------+                  +--------------+
 *                                    |     FFDC     |                  |     FFDC     |
 *                                    +--------------+                  +--------------+
*/

/**
 * @brief Data type for poz ffdc node for scratch allocation
*/
typedef struct pozFfdcNode
{
    uint32_t                    iv_ffdcLen    :16; // Len in Bytes
    uint32_t                    reserved      :14;
    uint32_t                    iv_isCommited : 1;
    uint32_t                    iv_isFatal    : 1;
    uint32_t                    iv_hwpSize    :16; // size in bytes
    uint32_t                    iv_platSize   :16; // size in bytes
    struct pozFfdcNode         *next;

    /**
     * @brief set method for setting member variables
     *
     * @param[in] i_ffdcLen ffdc length
     * @param[in] i_isCommited is commited flag
     * @param[in] i_isFatal is error is fatal flag
     * @param[in] i_hwpSize HWP ffdc size
     * @param[in] i_platSize plat ffdc size
    */
    void inline set ( const uint16_t i_ffdcLen,
                      const bool i_isCommited,
                      const bool i_isFatal,
                      const uint16_t i_hwpSize,
                      const uint16_t i_platSize )
    {
        iv_ffdcLen    = i_ffdcLen;
        iv_isCommited = i_isCommited;
        iv_isFatal    = i_isFatal;
        iv_hwpSize    = i_hwpSize;
        iv_platSize   = i_platSize;
    }

}__attribute__((packed, aligned(8))) pozFfdcNode_t; // ffdc package

/**
 * @brief poz ffdc control structure used to hold the first commited error
*/
typedef struct pozFfdcCtrl
{
    uint32_t        iv_localSlid         : 16; // Which hold the SBE log ID which is incrementing for every full FFDC
    uint32_t        iv_isScratchFull     :  1; // which hold the scratch full error data
    uint32_t        reserved             : 15;
    pozFfdcNode_t * iv_firstCommitted;         // holds the pointer to the first committed FFDC
    pozFfdcNode_t * iv_lastUEPersistentSpace;  // holds a pointer pointing to the space reserved for the last error
#if defined(MINIMUM_FFDC_RE)
    pozFfdcNode_t * iv_scratchFullRcSpace;     // holds a pointer pointing to the space reserved for scratch full RC error
#endif

    /**
     * @brief Constructor a poz Ffdc control object
     */
    pozFfdcCtrl( )
    {
        iv_localSlid                   = 0;
        iv_isScratchFull               = 0;
        iv_lastUEPersistentSpace       = nullptr;
        iv_firstCommitted              = nullptr;
#if defined(MINIMUM_FFDC_RE)
        iv_scratchFullRcSpace          = nullptr;
#endif
    }

    /**
     * @brief set given node to head (first commited node)
     *
     * @param[in] i_head pointer to node
     */
    void setHead(const pozFfdcNode_t  *  i_head)
    {
        iv_firstCommitted = const_cast<pozFfdcNode_t *>(i_head);
    }

    /**
     * @brief get head (fist commited ptr)
     *
     * @return pozFfdcNode_t * node ptr @ref pozFfdcNode_t
     */
    pozFfdcNode_t * getHead( void )
    {
        return iv_firstCommitted;
    }

    /**
     * @brief set given node reserved for last one minimum UE
     *
     * @param[in] i_LastUE pointer to last UE space
     */
    void setLastUeSpace(const pozFfdcNode_t* i_LastUeSpace)
    {
        iv_lastUEPersistentSpace = const_cast<pozFfdcNode_t *>(i_LastUeSpace);
    }

    /**
     * @brief get reserved for last one minimum UE Node
     *
     * @return pozFfdcNode_t* node ptr @ref pozFfdcNode_t
     */
    pozFfdcNode_t * getLastUeSpace( void )
    {
        return iv_lastUEPersistentSpace;

    }

#if defined(MINIMUM_FFDC_RE)
    /**
     * @brief set given node reserved for scratch full RC space
     *
     * @param[in] i_LastUE pointer to scratch full RC space
     */
    void setScratchFullRcSpace(const pozFfdcNode_t* i_scratchFullRcPtr)
    {
        iv_scratchFullRcSpace = const_cast<pozFfdcNode_t *>(i_scratchFullRcPtr);
    }

    /**
     * @brief get reserved for scratch full RC space
     *
     * @return pozFfdcNode_t* node ptr @ref pozFfdcNode_t
     */
    pozFfdcNode_t * getScratchFullRcSpace( void )
    {
        return iv_scratchFullRcSpace;

    }
#endif
    /**
     * @brief Increment the slid by one count
     *
     */
    void incrementSlid(void)
    {
        ++iv_localSlid;
    }

    void setScratchFull(bool i_isScratchFull)
    {
        iv_isScratchFull = (i_isScratchFull)? 1 : 0;
    }

    bool isScratchFull(void)
    {
        return (iv_isScratchFull);
    }

    /**
     * @brief Find last/next node and extend node for given node
     *
     * @param[in] i_newNode node pointer
     */
    void addNextNode(const pozFfdcNode_t * i_newNode);

    /**
     * @brief Function to get the last node of the list
     *
     * @return pozFfdcNode_t* node pointer
     */
    pozFfdcNode_t * getLastNode();

    /**
     * @brief Delete last node from the linked list.
     *
     */
    void deleteLastNode();
} __attribute__ ((aligned (8)))  pozFfdcCtrl_t;


/**
 * @brief Create the FFDC (HWP + Plat) package RC class constructor, FFDC
 *        package dump field depends on input arg. This will create a FFDC
 *        object in heap, which will have sbe-header, HWP and plat package.
 *
 * @param[in] i_rc Return codes as @enum ReturnCodes
 * @param[in] i_hwpLocalDataLen FFDC HWP local data length in bytes
 * @param[out] o_hwpLocalDataStartAddr start address to store HWP local data
 * @param[in] i_hwpRegDataLen   FFDC HWP register data length in bytes
 * @param[out] o_hwpRegDataStartAddr start address to store HWP Register data
 *
 * @return FFDC object allocation address
*/
uint32_t ffdcConstructor ( uint32_t i_rc,
                           uint16_t i_hwpLocalDataLen,
                           void *&  o_hwpLocalDataStartAddr,
                           uint16_t i_hwpRegDataLen,
                           void *&  o_hwpRegDataStartAddr,
                           fapi2::errlSeverity_t i_sev = fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE
                         );
