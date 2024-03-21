/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/ffdc/sbeffdc.C $               */
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
#include "sbeFifoMsgUtils.H"
#include "sbeffdc.H"
#include "heap.H"
#include "plat_hwp_data_stream.H"
#include "sbestates.H"
#include "sbestatesutils.H"
#include "sberegaccess.H"
#include "pk_api.h"
#include "pk_kernel.h"
#include "sbeglobals.H"


extern fapi2::pozFfdcData_t g_FfdcData;

namespace fapi2
{
    extern pozFfdcCtrl_t g_ffdcCtrlSingleton;
}

#define FIFO_BLOB_DATA_UPDATE( length, headerptr) \
    CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(ffdcPakageStream.put(length, headerptr))

/**
 * @brief FFDC plat full size trace size (8k)
 *        which is include trace_header + trace_size(8K)
*/
#define FFDC_TRACE_FULL_SIZE ( sizeof(*G_PK_TRACE_BUF) -       \
                                sizeof(G_PK_TRACE_BUF->cb) +   \
                                G_PK_TRACE_BUF->size )

/**
 * @brief FFDC params utils structure
 *
 */
typedef struct
{
    uint16_t hwpLocalDataLen;
    void *&  hwpLocalDataStartAddr;
    uint16_t hwpRegDataLen;
    void *&  hwpRegDataStartAddr;
}ffdcParams_t;


// Functions Prototype
#if defined( MINIMUM_FFDC_RE )
static void ffdcInitPlatData( const pozPlatFfdcPackageFormat_t * i_platAddr,
                              const uint16_t i_ffdcLen,
                              uint16_t i_slidId,
                              uint16_t i_primRc,
                              uint16_t i_secRc,
                              fapi2::errlSeverity_t i_sev = fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE );
#endif

static void ffdcInitHwpData( const pozHwpFfdcPackageFormat_t * i_hwpAddr,
                             const uint16_t i_ffdcLen,
                             uint32_t i_rc,
                             uint16_t i_slidId,
                             uint16_t i_hwpLocalDataLen,
                             void *&  o_hwpLocalDataStartAddr,
                             uint16_t i_hwpRegDataLen,
                             void *&  o_hwpRegDataStartAddr,
                             fapi2::errlSeverity_t i_sev = fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE);

// TODO: This needs to be updated as part of FFDC story
// PFSBE-407
void captureAsyncFFDC(uint32_t primRc, uint32_t secRc)
{
    SBE_GLOBAL->failedPrimStatus = primRc;
    SBE_GLOBAL->failedSecStatus  = secRc;

    // Transition to dump state
    stateTransition(SBE_EVENT_CMN_DUMP_FAILURE);

    // Set async ffdc bit
    (void)SbeRegAccess::theSbeRegAccess().updateAsyncFFDCBit(true);
}

/********************** FFDC utils functions *************************/
/**
 * @brief FFDC utils function for validate given address is with in scratch
 *        space. In case given address not with in scratch space executing
 *        pk_halt()
 *
 * @param i_addr pointer to the address
 */
static void ffdcUtils_checkScratchPtrCorruptionAndHalt (const void * i_addr)
{
    if ( !Heap::get_instance().is_scratch_pointer( i_addr ) )
    {
        SBE_ERROR ("ffdcUtils_checkScratchPtrCorruptionAndHalt Scratch space corrupted...! addr: [0x%08X]", i_addr);
        pk_halt();
    }
}

/**
 * @brief ffdcUtils_SemaPend: This is a utility function
 *                           Used to call pk_semaphore_pend
 *
 * @return: None (void)
 *
 */
void ffdcUtils_SemaPend()
{
    // Declare all local variable here
    int l_rcPk = PK_OK;

    // Wait on a sbeSemFfdc semaphore
    // It is used to acquire the resources, it means while adding the
    // Node no other thread can delete/modify the addNextNode or
    // While deleting the Node no other thread can add/modify the deleteLastNode .
    l_rcPk = pk_semaphore_pend (
                &SBE_GLOBAL->semphores.sbeSemFfdc, PK_WAIT_FOREVER);

    // PK API failure
    if (l_rcPk != PK_OK)
    {
        SBE_ERROR(" pk_semaphore_pend failed in addNextNode of FFDC infrastructure "
                "l_rcPk=%d, SBE_GLOBAL->semphores.sbeSemFfdc.count=%d ,executing PK_HALT()",
                l_rcPk, SBE_GLOBAL->semphores.sbeSemFfdc.count);

        // If it's a semphore_pend error then halt.
        pk_halt();
    }

}

/**
 * @brief ffdcUtils_SemaPost: This is a utility function
 *                           Used to call pk_semaphore_post
 *
 * @return: None (void)
 *
 */
void ffdcUtils_SemaPost()
{
    // Declare all local variable here
    int l_rcPk = PK_OK;

    // Release sbeSemFfdc semaphore
    // It is used to release resource those acquired by semaphorepend
    // So any other thread can acquire the resouces.
    l_rcPk = pk_semaphore_post(&SBE_GLOBAL->semphores.sbeSemFfdc);
    if (l_rcPk != PK_OK)
    {
        SBE_ERROR(" pk_semaphore_post failed in addNextNode of FFDC infrastructure "
                    "l_rcPk=%d, SBE_GLOBAL->semphores.sbeSemFfdc.count=%d, executing PK_HALT()",
                    l_rcPk, SBE_GLOBAL->semphores.sbeSemFfdc.count);

        // If it's a semphore_post error then halting.
        pk_halt();
    }

}



#if defined(MINIMUM_FFDC_RE)
/**
 * @brief Commit the given error and mask as commit.
 *        For null iv_data validating given RC and create ffdc for genuine RC
 *
 * @param io_rc  FAPI Return Code object
 */
void ffdcUtils_commitError ( fapi2::ReturnCode& io_rc )
{
    if (io_rc.getDataPtr() == 0)
    {
        uint32_t l_fapiRc = io_rc.getRC();
        if ( (l_fapiRc != fapi2::FAPI2_RC_SUCCESS)      &&
            ((l_fapiRc & fapi2::FAPI2_RC_PHAL_MASK)     ||
             (l_fapiRc & fapi2::FAPI2_RC_FAPI2_MASK)    ||
             (l_fapiRc & fapi2::FAPI2_RC_PLAT_MASK))
           )
        {
            void * ptr = NULL;
            io_rc.setDataPtr( ffdcConstructor( io_rc.getRC(), 0, ptr, 0, ptr) );
        }
        else
        {
            SBE_ERROR (SBE_FUNC "Lost the iv_dataPtr for given rc : 0x08X, "
                                "unable to log the error, executing PK_HALT()", l_fapiRc);
            pk_halt();
        }
    }

    do
    {
        // Skip committing the last UE FFDC
        if (fapi2::g_ffdcCtrlSingleton.getLastNode() == fapi2::g_ffdcCtrlSingleton.getLastUeSpace())
        {
            // FFDC's should be linked only once instance, Last UE already linked
            // them should not be linked again, which lead to stream out same
            // data twice
            break;
        }

        pozFfdcNode_t * node = reinterpret_cast<pozFfdcNode_t*>(io_rc.getDataPtr());

        // marks as node is committed
        node->iv_isCommited = true;

        // Set async ffdc bit
        (void)SbeRegAccess::theSbeRegAccess().updateAsyncFFDCBit(true);
    }while (0);
}

#endif


/**
 * @brief Function to create FFDC package
 *
 * @param i_packagePtr package buffer pointer
 * @param i_hwpSize    HWP package size
 * @param i_platSize   plat package size
 * @param i_slid       sbe log ID
 * @param i_sev        severity
 * @param i_rc         FAPI rc
 * @param params       param arg
 */
static void ffdcUtils_createPackage( const void * i_packagePtr,
                                     uint16_t i_hwpSize,
                                     uint16_t i_platSize,
                                     uint16_t i_slid,
                                     fapi2::errlSeverity_t i_sev = fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE,
                                     uint32_t i_rc = fapi2::FAPI2_RC_SUCCESS,
                                     void * params = nullptr
                                    )
{
    const pozHwpFfdcPackageFormat_t * hwpPkg = static_cast<const pozHwpFfdcPackageFormat_t *>(i_packagePtr);

    if (i_hwpSize)
    {
        ffdcParams_t * hwpParam = static_cast<ffdcParams_t *>(params);

        // Calling HWP data initialization function
        ffdcInitHwpData( hwpPkg,
                        i_hwpSize,
                        i_rc,
                        i_slid,
                        hwpParam->hwpLocalDataLen,
                        hwpParam->hwpLocalDataStartAddr,
                        hwpParam->hwpRegDataLen,
                        hwpParam->hwpRegDataStartAddr
                        );
    }

#if defined( MINIMUM_FFDC_RE )

    if (i_platSize)
    {
        // Plat FFDC data pointer
        pozPlatFfdcPackageFormat_t * platPkg = (pozPlatFfdcPackageFormat_t * )
                                                    ( ((uint8_t *)hwpPkg) +
                                                      i_hwpSize
                                                    ) ;

        // Calling PLAT FFDC initialization function
        ffdcInitPlatData( platPkg,
                          i_platSize,
                          i_slid,
                          SBE_PRI_GENERIC_EXECUTION_FAILURE,
                          SBE_SEC_HWP_FAILURE
                        );
    }

#endif
}


#if defined(MINIMUM_FFDC_RE)
/**
 * @brief Function to create scratch space full error rc ffdc package
 *
 * @param i_failedRc      failed RC
 * @param i_requiredSpace scratch space required size
 * @param i_slid          sbe log id
 */
static void ffdcUtils_createScratchFullErrorRcPkg(uint32_t i_failedRc, uint32_t i_requiredSpace, uint16_t i_slid)
{
    pozFfdcNode_t * node  = fapi2::g_ffdcCtrlSingleton.getScratchFullRcSpace();

    uint32_t hwpSize = ffdcUtils_getHwpSize(FFDC_SCRATCH_FULL_RC_LV_SIZE, 0);
    // Scratch FULL Error FFDC doesn't required plat trace
    uint32_t platSize = ffdcUtils_getPlatSize<0>();

    // Updating FFDC node status
    node->set ((uint16_t)(hwpSize + platSize), true, true, hwpSize, platSize);
    node->next = NULL;

    // Assigning dummy data, which will be updated with actual data when an error occurs
    fapi2::sbeFfdc_t * hwpLvPtr = nullptr;
    void * tempPtr = nullptr;
    ffdcParams_t params =
    {
        FFDC_SCRATCH_FULL_RC_LV_SIZE,
        (void *&)hwpLvPtr,
        0,
        tempPtr,
    };
    ffdcUtils_createPackage( (const void *)(((uint8_t *) node) + sizeof(pozFfdcNode_t)) ,
                               hwpSize,
                               platSize,
                               i_slid,
                               fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE,
                               fapi2::RC_POZ_FFDC_SCRATCH_SPACE_FULL_ERROR,
                               &params
                            );
    // FAILED_RC
    hwpLvPtr[0].data = i_failedRc;
    hwpLvPtr[0].size = sizeof(i_failedRc);

    // REQUIRED_SPACE
    hwpLvPtr[1].data = i_requiredSpace;
    hwpLvPtr[1].size = sizeof(i_requiredSpace);

    // AVAILABLE_SPACE
    size_t availSize = Heap::get_instance().getFreeHeapSize();
    hwpLvPtr[2].data = availSize;
    hwpLvPtr[2].size = sizeof(availSize);

    // Commit the scratch space full error
    fapi2::ReturnCode l_rc = fapi2::RC_POZ_FFDC_SCRATCH_SPACE_FULL_ERROR;
    l_rc.setDataPtr(reinterpret_cast<uint32_t>(node));
    ffdcUtils_commitError(l_rc);
}


void pozFfdcCtrl_t::addNextNode(const pozFfdcNode_t * i_newNode)
{
    // Calling function to acquire the resource so no other
    // Thread can modify/delete , while adding the node
    ffdcUtils_SemaPend();

    ffdcUtils_checkScratchPtrCorruptionAndHalt ((const void *) i_newNode);
    if ( !getHead() )
    {
        setHead(i_newNode);
    }
    else
    {
        pozFfdcNode_t * lastNode = getLastNode();
        lastNode->next = const_cast<pozFfdcNode_t *>(i_newNode);
    }

    // Calling function to release the resources so any other
    // Thread can acquire the resources
    ffdcUtils_SemaPost();

}

#endif


/**
 * @brief Function to get last UE space and creates the scratch full rc
 *
 * @return pozFfdcNode_t*
 */
static pozFfdcNode_t * ffdcUtils_getLastUeSpace (uint32_t i_rc = 0, uint16_t i_reqSpace = 0)
{

#if defined( MINIMUM_FFDC_RE )
    // Check scratch full flag
    if (!fapi2::g_ffdcCtrlSingleton.isScratchFull())
    {
        // updating scratch full flag
        fapi2::g_ffdcCtrlSingleton.setScratchFull(true);

        fapi2::g_ffdcCtrlSingleton.incrementSlid();
        // create scratch full error ffdc package
        ffdcUtils_createScratchFullErrorRcPkg( i_rc,
                                               i_reqSpace,
                                               fapi2::g_ffdcCtrlSingleton.iv_localSlid
                                             );
    }
#endif

    return (fapi2::g_ffdcCtrlSingleton.getLastUeSpace());
}


void pozFfdcCtrl_t::deleteLastNode()
{
    // Calling function to acquire the resource so no other
    // Thread can add/modify while deleting the Node
    ffdcUtils_SemaPend();

    pozFfdcNode_t * node = (pozFfdcNode_t *) iv_firstCommitted;
    do
    {
        if(node == nullptr)
        {
            break;
        }

        if(node->next == nullptr)
        {
            // Free up the last node
            Heap::get_instance().scratch_free((const void*)node);
            iv_firstCommitted = nullptr;
            break;
        }

        while(node->next->next != nullptr)
        {
            node = node->next;
        }
        // Free up the last node
        Heap::get_instance().scratch_free((const void*)node->next);
        node->next = nullptr;

    } while(false);

    // Calling function to release the resources so any other
    // Thread can acquire the resources
    ffdcUtils_SemaPost();
}

pozFfdcNode_t * pozFfdcCtrl_t::getLastNode()
{
    pozFfdcNode_t * node = nullptr;
    do
    {
        if (!getHead( ))
        {
            break;
        }

        node = getHead( );
        ffdcUtils_checkScratchPtrCorruptionAndHalt ((const void *) node);

        while (node->next)
        {
            node = node->next;
            ffdcUtils_checkScratchPtrCorruptionAndHalt ((const void *) node);
        }
    }while (0);

    return node;
}


/**
 * @brief Function to send plat ffdc with full trace
 *
 * @param[out] o_byteSent number of byte sent via fifo
 * @param[in] i_slidId slid id (sbe log identifier)
 * @param[in] io_putStream ostream reference
 *
 * @return secondary RC
*/
static uint32_t ffdcPlatCreateAndSendWithFullTrace (
                                    uint32_t &o_byteSent,
                                    uint16_t i_slidId,
                                    fapi2::errlSeverity_t i_sev,
                                    fapi2::sbefifo_hwp_data_ostream& io_putStream )
{
    #define SBE_FUNC "ffdcPlatCreateAndSendWithFullTrace "
    SBE_ENTER(SBE_FUNC);

    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    o_byteSent = 0;

    do
    {
        pozPlatFfdcPackageFormat_t platFfdc;

        /* Calculating the PLAT FFDC len */
        uint32_t ffdcLen = sizeof(pozPlatFfdcPackageFormat_t) +
                           sizeof(packageBlobField_t)         + /* FFDC fields size */
                           FFDC_TRACE_FULL_SIZE;                /* PK FULL trace size */

        // Updating the byte sent
        o_byteSent = ffdcLen;

        platFfdc.header.setLenInWord ( BYTES_TO_WORDS(ffdcLen) );
        platFfdc.header.setCmdInfo   ( 0,
                                       SBE_GLOBAL->sbeFifoCmdHdr.cmdClass,
                                       SBE_GLOBAL->sbeFifoCmdHdr.command );
        platFfdc.header.setSlid      ( i_slidId );
        platFfdc.header.setSeverity  ( i_sev );
        platFfdc.header.setChipId    ( 0 );
        platFfdc.header.setRc        ( fapi2::FAPI2_RC_PLAT_ERR_SEE_DATA );

        platFfdc.platHeader.setRc         ( SBE_GLOBAL->failedPrimStatus, SBE_GLOBAL->failedSecStatus );
        platFfdc.platHeader.setfwCommitId ( SBE_COMMIT_ID );
        platFfdc.platHeader.setDdlevel    ( 0, 0 );
        platFfdc.platHeader.setThreadId   ( pk_current()->priority );

        platFfdc.dumpFields        = SBE_FFDC_TRACE_DATA;

        l_rc = io_putStream.put( BYTES_TO_WORDS(sizeof(pozPlatFfdcPackageFormat_t)),
                                     (uint32_t *)&platFfdc );
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        packageBlobField_t blobField;
        blobField.fieldId = SBE_FFDC_TRACE_DATA;
        blobField.fieldLen = FFDC_TRACE_FULL_SIZE;
        /* Streaming the Blob filed */
        l_rc = io_putStream.put( BYTES_TO_WORDS(sizeof(packageBlobField_t)),
                                        (uint32_t *) &blobField );
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
        /* Streaming the SBE trace */
        l_rc = io_putStream.put( BYTES_TO_WORDS(FFDC_TRACE_FULL_SIZE),
                                        (uint32_t*)(G_PK_TRACE_BUF));
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

    }while(false);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


/**
 * @brief Get the Truncated Trace
 *        Function help to copy truncated trace to given buffer.
 *
 * Assuming TRACE buffer:
 * +----------------------------------------------------+
 * |                 Trace header                       |
 * +----------------------------------------------------+ -\
 * |                                                    |  |
 * |               Trace buffer start                   |   }> 8K trace buffer space
 * |                                                    |  |
 * +----------------------------------------------------+ -/
 *
 * Trace header contain all data about trace, etc, Timestamp, offset,
 * buffer size etc..
 *
 * Truncated trace:
 *   For truncated trace, have to calculate the trace "offset address".
 *   As per trace logic offset is just a increment variable. Once it
 *   reaches the "trace size" it will be reloaded.
 *
 * Note:
 *  - When curTraceOffset >= SBE_FFDC_TRUNCATED_TRACE_LENGTH bytes, then we will
 *    get a continuous truncated buffer directly from trace buffer.
 *  - When curTraceOffset < 512 bytes, we have two sections from trace
 *    buffer going to truncated buffer.
 *      - Section from 0 to curTraceOffset. This will be copied to bottom of
 *        the truncated buffer.
 *      - Section from (8k-(512-curTraceOffset)) to 8k. This will be copied
 *        to top of the truncated buffer.
 *  - Because of this shuffle, we can always set 512 as offset in trace header.
 *  - This has corner case when "(curTraceOffset < 512) and bottom of trace
 *    buffer is empty)", where the starting of truncated buffer will be
 *    empty entires. But since parser will be parsing from offset to
 *    backward, parser still will be able to parse this.
 *
 *
 * @param[in] i_TraceAddr pointer to save truncated trace
 * @return Secondary RC in case any error
 */
void getTruncatedTrace ( const void * i_TraceAddr)
{
    #define SBE_FUNC "getTruncatedTrace "
    SBE_ENTER(SBE_FUNC);



    uint32_t curTraceOffset = 0;
    uint32_t byteRemaining  = 0;
    uint32_t truncatedTraceOffset = 0;
    uint32_t traceStartOffset     = 0;
    PkTraceBuffer * ffdcTraceSpace = const_cast<PkTraceBuffer *>(reinterpret_cast<const PkTraceBuffer*>(i_TraceAddr));

    PkMachineContext    ctx;
    // Enter the critical section
    pk_critical_section_enter(&ctx);

    curTraceOffset = G_PK_TRACE_BUF->state.offset & PK_TRACE_CB_MASK;
    // Copying trace header to given trace space
    memcpy ( (uint8_t *) ffdcTraceSpace, // ffdc Truncated trace buff
                (uint8_t *) G_PK_TRACE_BUF, // Actual PK trace buffer which is 8 KB size
                (sizeof(*G_PK_TRACE_BUF) - sizeof(G_PK_TRACE_BUF->cb)));

    // exit the critical section
    pk_critical_section_exit(&ctx);

    // Number of bytes remains from full trace over truncated trace
    byteRemaining = SBE_FFDC_TRUNCATED_TRACE_LENGTH;
    // To find the roll over in resulting buffer after truncation
    if (curTraceOffset < SBE_FFDC_TRUNCATED_TRACE_LENGTH)
    {
        // remaining byte from start of truncated buffer
        uint32_t rolledOverPortion = SBE_FFDC_TRUNCATED_TRACE_LENGTH - curTraceOffset;
        // truncated buffer start offset
        traceStartOffset = (G_PK_TRACE_BUF->size) - rolledOverPortion;

        // copying the start half of the trace
        memcpy ( (uint8_t *) &ffdcTraceSpace->cb[0],
                (uint8_t *) &G_PK_TRACE_BUF->cb[traceStartOffset],
                (size_t) rolledOverPortion);
        truncatedTraceOffset = rolledOverPortion;
        traceStartOffset     = 0;
        byteRemaining        = curTraceOffset;
    }
    else
    {
        traceStartOffset = curTraceOffset - SBE_FFDC_TRUNCATED_TRACE_LENGTH;
    }
    // copying remaining trace
    memcpy ( (uint8_t *) &ffdcTraceSpace->cb[truncatedTraceOffset],
            (uint8_t *) &G_PK_TRACE_BUF->cb[traceStartOffset],
            (size_t) byteRemaining);
    // Modifying the trace size in truncated ffdc trace (other wise parser logic won't work as expected)
    ffdcTraceSpace->size = SBE_FFDC_TRUNCATED_TRACE_LENGTH;
    ffdcTraceSpace->state.offset = (SBE_FFDC_TRUNCATED_TRACE_LENGTH);

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}


uint32_t sendFFDCOverFIFO( uint32_t &o_wordsSent,
                           sbeFifoType i_type,
                           bool i_forceFullTracePackage )
{
    #define SBE_FUNC "sendFFDCOverFIFO "
    SBE_ENTER(SBE_FUNC);
    uint32_t l_rc = SBE_SEC_OPERATION_SUCCESSFUL;
    uint32_t byteSent = 0;

    do
    {
        // Create the fifo ostream class
        fapi2::sbefifo_hwp_data_ostream ffdcPakageStream(i_type);

        /* Check the first committed FFDC */
        pozFfdcNode_t * node = fapi2::g_ffdcCtrlSingleton.getHead();
        if (node != nullptr)
        {
            pozFfdcNode_t * currentNode = (pozFfdcNode_t *) node;
            pozFfdcNode_t * nextNode = nullptr;

            do
            {
                SBE_DEBUG(SBE_FUNC " currentNode: 0x%08X", currentNode);
                ffdcUtils_checkScratchPtrCorruptionAndHalt((const void *)currentNode);

                if (currentNode->iv_isCommited)
                {
                    uint32_t len = currentNode->iv_ffdcLen;
                    l_rc = ffdcPakageStream.put((uint32_t) BYTES_TO_WORDS( len ),
                                            (uint32_t*) (((uint8_t *)currentNode) +
                                                        sizeof(pozFfdcNode_t)) );
                    CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

                    byteSent += len;

                    /* Check FFDC is fatal, In case of fatal send full trace at last node */
                    if (currentNode->iv_isFatal)
                    {
                        // Stream out full trace at last with different slid id for
                        // any one of the FFDC has FATAL
                        i_forceFullTracePackage = true;
                    }
                }

                /* Logic to get the next FFDC */
                nextNode = currentNode->next;

                // Do not double free the scratch space for persistent space
                if ((fapi2::g_ffdcCtrlSingleton.getLastUeSpace() == currentNode)
#ifdef MINIMUM_FFDC_RE
                   || (fapi2::g_ffdcCtrlSingleton.getScratchFullRcSpace() == currentNode)
#endif
                   )
                {
                    SBE_DEBUG (SBE_FUNC "Scratch persistent addr: 0x%08X, Changing head to 0x%08X", currentNode, nextNode);
                    // clear memory
                    memset ((void *) currentNode, 0x00, (sizeof(pozFfdcNode_t) + currentNode->iv_ffdcLen));
                }
                else
                {
                    SBE_DEBUG (SBE_FUNC "Scratch free addr: 0x%08X, Changing head to 0x%08X", currentNode, nextNode);
                    Heap::get_instance().scratch_free((const void*) currentNode);
                }

                currentNode = nextNode;

            }while (currentNode != nullptr);

            fapi2::g_ffdcCtrlSingleton.setScratchFull(false);
            // Clearing the ASYNC bit
            (void)SbeRegAccess::theSbeRegAccess().updateAsyncFFDCBit(false);
        }
        /* Check RC and Break */
        CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);

        // stream out FULL trace, 1. For any one of FFDC are FATAL,
        //                        2. For get FFDC
        if ( i_forceFullTracePackage )
        {
            uint32_t tempByteSent = 0;
            l_rc = ffdcPlatCreateAndSendWithFullTrace ( tempByteSent,
                                                ++fapi2::g_ffdcCtrlSingleton.iv_localSlid, // applying incremented slid id
                                                fapi2::FAPI2_ERRL_SEV_UNDEFINED,           // severity undefined
                                                ffdcPakageStream );
            CHECK_SBE_RC_AND_BREAK_IF_NOT_SUCCESS(l_rc);
            byteSent += tempByteSent;
        }
        o_wordsSent += BYTES_TO_WORDS(byteSent);

    } while(false);

    /* clearing the first commited FFDC */
    fapi2::g_ffdcCtrlSingleton.setHead((pozFfdcNode_t *) NULL);

    SBE_EXIT(SBE_FUNC);
    return l_rc;
    #undef SBE_FUNC
}


#if defined(MINIMUM_FFDC_RE)
/**
 * @brief Get thread ID for given node
 *
 * @param i_nodeAddr Node address
 * @return pk thread id (PkThreadPriority)
 */
static uint8_t ffdcUtils_getThreadId ( const pozFfdcNode_t * i_nodeAddr )
{
    uint8_t threadId = 0;
    pozFfdcNode_t * node = const_cast<pozFfdcNode_t*>(i_nodeAddr);

    /* Check the node contain Plat data */
    if (node->iv_platSize)
    {
        // Get the plat ffdc package start address
        pozPlatFfdcPackageFormat_t * platAddr = (pozPlatFfdcPackageFormat_t *)
                                                     ( ((uint8_t *)node)      +
                                                       sizeof (pozFfdcNode_t) +
                                                       node->iv_hwpSize
                                                     );
        threadId = platAddr->platHeader.threadId;
    }

    return threadId;
}
#endif



void ffdcFreeUnwantedError(uint8_t i_threadId)
{
    #define SBE_FUNC "ffdcFreeUnwantedError "
    SBE_ENTER(SBE_FUNC);

#if defined( MINIMUM_FFDC_RE )

    // Thread safety
    ffdcUtils_SemaPend();

    // Get the head
    pozFfdcNode_t * node = (pozFfdcNode_t *) fapi2::g_ffdcCtrlSingleton.getHead();
    pozFfdcNode_t * deletingNode = nullptr;
    pozFfdcNode_t * prevNode = node;

    do
    {
        // Checking HEAD if we have data
        if (node == nullptr)
        {
            SBE_INFO (SBE_FUNC "Head is null");
            break;
        }

        // checking uncommitted ffdc and then clear
        while(node != nullptr)
        {
            uint8_t nodeThreadId = ffdcUtils_getThreadId(node);
            SBE_DEBUG(SBE_FUNC "prev: [0x%08X], node: [0x%08X], next: [0x%08X]", prevNode, node, node->next);
            SBE_DEBUG(SBE_FUNC "Is node committed: %d, node: [0x%08X], thread ID: [0x%08X]", node->iv_isCommited, node, nodeThreadId);

            // node is committed
            if ( (!node->iv_isCommited) && (i_threadId == nodeThreadId) )
            {
                // not committed node need to delete and attach node
                deletingNode = node;

                // not committed head need to clear and set with next
                if (node == fapi2::g_ffdcCtrlSingleton.getHead())
                {
                    fapi2::g_ffdcCtrlSingleton.setHead(node->next);
                }

                // linking previous node with next code
                if (node->next != nullptr)
                {
                    prevNode->next = node->next;
                    node = node->next;
                }
                else
                {
                    node = node->next;
                    prevNode->next = node;
                }

                SBE_DEBUG(SBE_FUNC "Deleting node [0x%08X]", deletingNode);
                // Free up the node
                Heap::get_instance().scratch_free((const void*)deletingNode);
            }
            else
            {
                prevNode = node;
                node = node->next;
            }
        }
    }while(0);

    ffdcUtils_SemaPost();
#endif

    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}


/**
 * @brief FFDC HWP data initialization
 *        This function used to update the hwp Respose Header except hwp local
 *        data and hwp reg data. HWP local data will set by set method in
 *        generated RC and HWP reg data updated by auto gen code
 *
 * Below figure is HWP FFDC data frame format and mapped to param of the function
 * +----------+----------+----------+----------+
 * | Byte 0   |  Byte 1  |  Byte 2  |  Bytes 3 |
 * +----------+----------+----------+----------+ --> @ref param i_hwpAddr
 * | Magic Bytes 0xFBAD  |  Len in words       |          start address of the HWP FFDC
 * +----------+----------+----------+----------+
 * |       Seq ID        | Cmd class|    Cmd   |
 * +----------+----------+----------+----------+
 * |        SLID         | Severity |  Chip ID | --> @ref param i_slidId & i_sev
 * +----------+----------+----------+----------+
 * |             FAPI RC ( HWP )               | --> @ref param i_rc
 * +----------+----------+----------+----------+
 * |           HWP FFDC Dump Fields            |
 * +----------+----------+----------+----------+
 * |     Filed ID 0      |   Field ID 0 Length | --> @ref param i_hwpLocalDataLen
 * +----------+----------+----------+----------+ --> @ref param o_hwpLocalDataStartAddr (start address of hwp local data)
 * |       Field Data 0 (size 1, data 2)       | -
 * +----------+----------+----------+----------+  |
 * |                  .......                  |  |--> HWP local data len
 * +----------+----------+----------+----------+  |
 * |         Field Data N (size 1, data 2)     | -
 * +----------+----------+----------+----------+
 * |      Filed ID 1     |  Field ID 1 Length  | --> @ref param i_hwpRegDataLen
 * +----------+----------+----------+----------+ --> @ref param o_hwpRegDataStartAddr (start address of hwp reg data)
 * |       Field Data 0 (size 1, data 2)       | -
 * +----------+----------+----------+----------+  |
 * |                   .......                 |  |-> HWP reg data len
 * +----------+----------+----------+----------+  |
 * |        Field Data N (size 1, data 2)      | -
 * +----------+----------+----------+----------+
 *
 * @param[in] i_hwpAddr HWP local and HWP reg FFDC data start address including
 *                      Respose Header, caller should allocate full frame before
 *                      calling this function and pass the pointer here.
 * @param[in] i_ffdcLen FFDC length in bytes
 * @param[in] i_rc      fapi RC which is assign to the FFDC Respose Header
 * @param[in] i_slidId  slid ID (sbe log identifier)
 * @param[in] i_dumpFields HWP FFDC dump field
 * @param[in] i_hwpLocalDataLen HWP local ffdc data length in bytes, which
 *                      is passed from auto gen code (hwp_ffdc_classes.H). which
 *                      is used to allocate the scratch space.
 * @param[out] o_hwpLocalDataStartAddr HWP local ffdc data start address, were
 *                      using address auto gen code set method will update the
 *                      HWP local data.
 * @param[in] i_hwpRegDataLen HWP reg ffdc data length in bytes, which is passed
 *                      from auto gen code (hwp_ffdc_classes.H). which is used
 *                      to allocate the scratch space.
 * @param[out] o_hwpRegDataStartAddr HWP Ref ffdc data start address, were
 *                      using address auto gen code set method will update the
 *                      HWP local data.
 * @param[in] i_sev     severity of the error,
 *                      default FAPI2_ERRL_SEV_UNRECOVERABLE
 *
*/
static void ffdcInitHwpData( const pozHwpFfdcPackageFormat_t * i_hwpAddr,
                             const uint16_t i_ffdcLen,
                             uint32_t i_rc,
                             uint16_t i_slidId,
                             uint16_t i_hwpLocalDataLen,
                             void *&  o_hwpLocalDataStartAddr,
                             uint16_t i_hwpRegDataLen,
                             void *&  o_hwpRegDataStartAddr,
                             fapi2::errlSeverity_t i_sev)
{
    #define SBE_FUNC "ffdcInitHwpData "
    SBE_ENTER(SBE_FUNC);

    pozHwpFfdcPackageFormat_t * hwpAddr = (pozHwpFfdcPackageFormat_t *) i_hwpAddr;

    if (hwpAddr)
    {
        hwpAddr->header.setMagicbytes(SBE_FFDC_MAGIC_BYTES);
        hwpAddr->header.setLenInWord ( BYTES_TO_WORDS(i_ffdcLen) );
        hwpAddr->header.setCmdInfo   ( 0,
                                       SBE_GLOBAL->sbeFifoCmdHdr.cmdClass,
                                       SBE_GLOBAL->sbeFifoCmdHdr.command );
        hwpAddr->header.setSlid      ( i_slidId );
        hwpAddr->header.setSeverity  ( i_sev );
        hwpAddr->header.setChipId    ( 0 );
        hwpAddr->header.setRc        ( i_rc );

        hwpAddr->dumpFields = 0;

        if (i_hwpLocalDataLen)
        {
            hwpAddr->dumpFields        = SBE_FFDC_HW_DATA;

            // calculating the field data ptr
            packageBlobField_t * packetPtr = (packageBlobField_t *)
                        (((uint8_t *)i_hwpAddr) + sizeof(pozHwpFfdcPackageFormat_t));

            packetPtr->setFields( (uint16_t) SBE_FFDC_HW_DATA,
                                (uint16_t) i_hwpLocalDataLen );
            // calculating the hwp local data start ptr
            o_hwpLocalDataStartAddr = (void *) ( ((uint8_t *)i_hwpAddr)    +
                                            sizeof(pozHwpFfdcPackageFormat_t) +
                                            sizeof(packageBlobField_t) );
        }

        // Checking the HWP reg data dump fields
        if ( i_hwpRegDataLen )
        {
            hwpAddr->dumpFields |= SBE_FFDC_REG_DATA;
            packageBlobField_t * packetPtr = (packageBlobField_t *) ( ((uint8_t *)i_hwpAddr)           +
                                                 sizeof(pozHwpFfdcPackageFormat_t)+
                                                 sizeof(packageBlobField_t)       +
                                                 i_hwpLocalDataLen );
            packetPtr->setFields( (uint16_t) SBE_FFDC_REG_DATA,
                                  (uint16_t) i_hwpRegDataLen );

            // calculating the hwp reg data start ptr
            o_hwpRegDataStartAddr = (void *)( ((uint8_t *)i_hwpAddr)           +
                                              sizeof(pozHwpFfdcPackageFormat_t)+
                                              sizeof(packageBlobField_t)       +
                                              i_hwpLocalDataLen                +
                                              sizeof(packageBlobField_t) );
        }
    }
    SBE_EXIT(SBE_FUNC);
    #undef SBE_FUNC
}

#if defined( MINIMUM_FFDC_RE )
/**
 * @brief FFDC initialization plat ffdc data
 *        This function used to update the plat Response Header including the
 *        truncated trace to given space.
 *
 * Below figure is PLAT FFDC data frame format and mapped to param of the function
 * +----------+----------+----------+----------+
 * | Byte 0   |  Byte 1  |  Byte 2  |  Bytes 3 |
 * +----------+----------+----------+----------+ --> @ref param i_platAddr (start address of the PLAT FFDC)
 * | Magic Bytes 0xFBAD  |  Len in words       |
 * +----------+----------+----------+----------+
 * |       Seq ID        | Cmd class|    Cmd   |
 * +----------+----------+----------+----------+
 * |        SLID         | Severity |  Chip ID | --> @ref param i_slidId & i_sev
 * +----------+----------+----------+----------+
 * |   FAPI RC ( FAPI2_RC_PLAT_ERR_SEE_DATA )  |
 * +----------+----------+----------+----------+
 * |    Primary Status   |  Secondary Status   | --> @ref param i_primRc & i_secRc
 * +----------+----------+----------+----------+
 * |              FW Commit ID                 |
 * +----------+----------+----------+----------+
 * | Reserved | DD Major | DD Minor | Thread ID|
 * +----------+----------+----------+----------+
 * |           HWP FFDC Dump Fields            |
 * +----------+----------+----------+----------+
 * |     Filed ID 0      |   Field ID 0 Length |
 * +----------+----------+----------+----------+
 * |       Field Data 0 (size 1, data 2)       | -
 * +----------+----------+----------+----------+  |
 * |                  .......                  |  |--> PLAT ffdc truncated trace data
 * +----------+----------+----------+----------+  |
 * |         Field Data N (size 1, data 2)     | -
 * +----------+----------+----------+----------+
 * |      Filed ID 1     |  Field ID 1 Length  |
 * +----------+----------+----------+----------+
 * |       Field Data 0 (size 1, data 2)       | -
 * +----------+----------+----------+----------+  |
 * |                   .......                 |  |-> PLAT ffdc attr dump
 * +----------+----------+----------+----------+  |
 * |        Field Data N (size 1, data 2)      | -
 * +----------+----------+----------+----------+
 *
 * @param[in] i_platAddr Start address of the PLAT FFDC, caller should allocate
 *                       full frame before calling this function and pass the
 *                       pointer here.
 * @param[in] i_ffdcLen  FFDC length in bytes
 * @param[in] i_slidId   slid ID (sbe log identifier)
 * @param[in] i_primRc   primary RC
 * @param[in] i_secRc    secondary RC
 * @param[in] i_sev      severity of the error,
 *                       default FAPI2_ERRL_SEV_UNRECOVERABLE
 *
*/
static void ffdcInitPlatData( const pozPlatFfdcPackageFormat_t * i_platAddr,
                              const uint16_t i_ffdcLen,
                              uint16_t i_slidId,
                              uint16_t i_primRc,
                              uint16_t i_secRc,
                              fapi2::errlSeverity_t i_sev )
{
    pozPlatFfdcPackageFormat_t * platAddr = (pozPlatFfdcPackageFormat_t *) i_platAddr;
    if (platAddr)
    {
        // Assigning the PLAT ffdc response header
        platAddr->header.setMagicbytes( SBE_FFDC_MAGIC_BYTES );
        platAddr->header.setLenInWord ( BYTES_TO_WORDS(i_ffdcLen) );
        platAddr->header.setCmdInfo   ( 0,
                                       SBE_GLOBAL->sbeFifoCmdHdr.cmdClass,
                                       SBE_GLOBAL->sbeFifoCmdHdr.command );
        platAddr->header.setSlid      ( i_slidId );
        platAddr->header.setSeverity  ( i_sev );
        platAddr->header.setChipId    ( 0 );
        platAddr->header.setRc        ( fapi2::FAPI2_RC_PLAT_ERR_SEE_DATA );

        platAddr->platHeader.setRc         ( i_primRc, i_secRc );
        platAddr->platHeader.setfwCommitId ( SBE_COMMIT_ID );
        platAddr->platHeader.setDdlevel    ( 0, 0 );
        platAddr->platHeader.setThreadId   ( pk_current()->priority );
        platAddr->dumpFields = 0;

        if ( i_ffdcLen > sizeof(pozPlatFfdcPackageFormat_t) )
        {
            platAddr->dumpFields = SBE_FFDC_TRACE_DATA;

            // Calculating the address to trace blobfield
            packageBlobField_t * blobField = (packageBlobField_t *)
                                            ( ((uint8_t *)i_platAddr)  +
                                                sizeof(pozPlatFfdcPackageFormat_t) );
            blobField->setFields( (uint16_t) SBE_FFDC_TRACE_DATA,
                                    (uint16_t) PLAT_FFDC_TRUNCATED_TRACE_SIZE );

            // Calculating the truncated trace start address
            uint8_t * traceBufferPtr = (uint8_t *) ((uint8_t *)i_platAddr)         +
                                                sizeof(pozPlatFfdcPackageFormat_t) +
                                                sizeof(packageBlobField_t) ;
            getTruncatedTrace ( (void *) traceBufferPtr);
        }
    }
}
#endif


uint32_t ffdcConstructor ( uint32_t i_rc,
                           uint16_t i_hwpLocalDataLen,
                           void *&  o_hwpLocalDataStartAddr,
                           uint16_t i_hwpRegDataLen,
                           void *&  o_hwpRegDataStartAddr,
                           fapi2::errlSeverity_t i_sev
                         )
{
    #define SBE_FUNC "ffdcConstructor "
    SBE_ENTER(SBE_FUNC);
    uint32_t ffdcPlatSize = 0;
    uint32_t ffdcHwpSize  = 0;

    /**
     * Calculating the FFDC size to allocate the heap
     * refer diagram in @file sbeffdctype.H (figure 1 and 2)
    */
    ffdcHwpSize = ffdcUtils_getHwpSize(i_hwpLocalDataLen, i_hwpRegDataLen);
#if defined( MINIMUM_FFDC_RE )
    ffdcPlatSize = ffdcUtils_getPlatSize();
#endif

    uint32_t ffdcLen = ffdcHwpSize           + /* HWP size */
                       ffdcPlatSize;           /* Plat size */

    pozFfdcNode_t * currentNode = nullptr;
    // For RE disable, utilize the last UE space that was created
#if defined( MINIMUM_FFDC_RE )

    // Once the scratch space is full, allocation of space for FFDC is not
    //  allowed until the FFDC data is streamed out.
    if (!fapi2::g_ffdcCtrlSingleton.isScratchFull())
    {
        /* Allocation scratch space */
        currentNode  = (pozFfdcNode_t *) Heap::get_instance().
                            scratch_calloc( ffdcLen + sizeof(pozFfdcNode_t) );
    }

#endif

    if (currentNode == nullptr)
    {
        currentNode = ffdcUtils_getLastUeSpace(i_rc, (ffdcLen + sizeof(pozFfdcNode_t)));
    }

    // Check for Scratch is allocation
    if (currentNode)
    {
        SBE_DEBUG (SBE_FUNC "currentNode: 0x%08X", currentNode);
        // Updating FFDC node status
        currentNode->set ((uint16_t)ffdcLen, false, false, ffdcHwpSize, ffdcPlatSize);
        currentNode->next = NULL;

        // Incrementing SBE log ID, Identical slid ID for particular FFDC
        //  this may roll-over after 2^16, but its ok, since we can safely
        //  assume by that time the first ffdc with slid-id = 0, might have
        //  streamed back
        fapi2::g_ffdcCtrlSingleton.incrementSlid();

#if !defined( MINIMUM_FFDC_RE )
        // Note: For not defined MINIMUM_FFDC_RE, Directly assign allocated scratch
        //        address to iv_firstCommitedFfdc and mark error as commited and
        //        fatal error
        currentNode->iv_isCommited = true;
        currentNode->iv_isFatal    = true;

        /* Add node at last */
        fapi2::g_ffdcCtrlSingleton.setHead( currentNode );
#endif

        // FFDC package data pointer
        pozHwpFfdcPackageFormat_t * ffdcPackagePtr = ( pozHwpFfdcPackageFormat_t * )
                               (((uint8_t *)currentNode) + sizeof(pozFfdcNode_t));

        ffdcParams_t params = {
            i_hwpLocalDataLen,
            o_hwpLocalDataStartAddr,
            i_hwpRegDataLen,
            o_hwpRegDataStartAddr
        };

        ffdcUtils_createPackage( ffdcPackagePtr,
                                 ffdcHwpSize,
                                 ffdcPlatSize,
                                 fapi2::g_ffdcCtrlSingleton.iv_localSlid,
                                 fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE,
                                 i_rc,
                                 &params
                               );

#if defined( MINIMUM_FFDC_RE )
        // Created error will add to list without committing it, LogError will
        // handle the committing error
        fapi2::g_ffdcCtrlSingleton.addNextNode (currentNode);
#endif
    }
    else
    {
        // scratch space not available and lastUe is not initialized
        SBE_ERROR (SBE_FUNC "scratch space not available and lastUe is not initialized, executing pk_halt()");
        pk_halt();
    }

    SBE_EXIT(SBE_FUNC);
    return reinterpret_cast<uint32_t>(currentNode);
    #undef SBE_FUNC
}

#if defined( MINIMUM_FFDC_RE )
void logSbeError( const uint16_t i_primRc,
                  const uint16_t i_secRc,
                  fapi2::errlSeverity_t i_sev,
                  bool i_isFatal
                )
{
    uint32_t ffdcLen = sizeof(pozPlatFfdcPackageFormat_t)+ /* Plat ffdc header + plat header(commit ID + DD level) + Dump field size */
                       sizeof(packageBlobField_t)        + /* FFDC fields size */
                       PLAT_FFDC_TRUNCATED_TRACE_SIZE ;    /* Trace data size  */

    pozFfdcNode_t * currentNode = (pozFfdcNode_t *) Heap::get_instance().
                               scratch_calloc( ffdcLen + sizeof(pozFfdcNode_t) );

    if (currentNode == nullptr)
    {
        currentNode = ffdcUtils_getLastUeSpace(fapi2::FAPI2_RC_PLAT_ERR_SEE_DATA,
                                               ffdcLen + sizeof(pozFfdcNode_t));
    }

    if (currentNode)
    {
        // Updating the FFDC node status, Committing the created node
        currentNode->set ((uint16_t)ffdcLen, true, i_isFatal, 0, ffdcLen);
        currentNode->next = NULL;

        // Incrementing SBE log ID, Identical slid ID for particular FFDC
        fapi2::g_ffdcCtrlSingleton.incrementSlid();

        pozPlatFfdcPackageFormat_t * platFfdcPtr = (pozPlatFfdcPackageFormat_t *)
                                                      ( ((uint8_t *)currentNode) +
                                                        sizeof(pozFfdcNode_t)
                                                      );

        ffdcInitPlatData( platFfdcPtr,
                          ffdcLen,
                          fapi2::g_ffdcCtrlSingleton.iv_localSlid,
                          i_primRc,
                          i_secRc,
                          i_sev
                        );

        /* Add node at last */
        fapi2::g_ffdcCtrlSingleton.addNextNode(currentNode);

        // Set async ffdc bit
        (void)SbeRegAccess::theSbeRegAccess().updateAsyncFFDCBit(true);
    }
    else
    {
        // scratch space not available and lastUe is not initialized
        SBE_ERROR (SBE_FUNC "scratch space not available and lastUe is not initialized, executing pk_halt()");
        pk_halt();
    }
}
#endif


void logFatalError( fapi2::ReturnCode& i_rc )
{
#if defined(MINIMUM_FFDC_RE)

    ffdcUtils_commitError( i_rc );

    pozFfdcNode_t * currentNode = (pozFfdcNode_t *) i_rc.getDataPtr();

    // Mark Error as a FATAL
    currentNode->iv_isFatal    = true;

    // Note: Ideally iv_rc also need to be cleared. but not clearing since,
    //       it will break some of the existing flow where sbe functions
    //       which is calling SBE_EXEC_HWP is returning fapi-rc.
    i_rc.setDataPtr ( NULL );
#endif
}

#if defined(MINIMUM_FFDC_RE)
/**
 * @brief Update FFDC severity to created FFDC node. Will identify the package
 *        (HWP/PLAT) and update severity
 *
 * @param i_nodeAddr Node address
 * @param i_sev      severity of the error, default FAPI2_ERRL_SEV_UNRECOVERABLE
 */
static void ffdcUpdateSeverity ( const pozFfdcNode_t * i_nodeAddr,
                          fapi2::errlSeverity_t i_sev = fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE)
{
    pozFfdcNode_t * node = const_cast<pozFfdcNode_t*>(i_nodeAddr);

    /* Check the node contain HWP local data */
    if (node->iv_hwpSize)
    {
        // Get the HWP ffdc package start address
        pozHwpFfdcPackageFormat_t * hwpAddr = (pozHwpFfdcPackageFormat_t *)
                                ( ((uint8_t *)node) + sizeof (pozFfdcNode_t));
        // Update severity
        hwpAddr->header.setSeverity( i_sev );
    }

    /* Check the node contain Plat data */
    if (node->iv_platSize)
    {
        // Get the plat ffdc package start address
        pozPlatFfdcPackageFormat_t * platAddr = (pozPlatFfdcPackageFormat_t *)
                                                     ( ((uint8_t *)node)      +
                                                       sizeof (pozFfdcNode_t) +
                                                       node->iv_hwpSize
                                                     );
        // Update severity
        platAddr->header.setSeverity( i_sev );
    }
}
#endif

namespace fapi2
{

void logError( fapi2::ReturnCode& io_rc,
               fapi2::errlSeverity_t i_sev,
               bool i_unitTestError )
{

#if defined(MINIMUM_FFDC_RE)

    ffdcUtils_commitError( io_rc );

    pozFfdcNode_t * node = reinterpret_cast<pozFfdcNode_t*>(io_rc.getDataPtr());
    if (node != nullptr)
    {
        if (i_sev != fapi2::FAPI2_ERRL_SEV_UNDEFINED)
        {
            // update severity
            ffdcUpdateSeverity ( node, i_sev );
        }

        // clear rc
        io_rc.setDataPtr ( NULL );
        io_rc.setRC ( FAPI2_RC_SUCCESS );
    }
    else
    {
        SBE_ERROR (SBE_FUNC "Lost the iv_dataPtr unable to log the error, "
                            "executing PK_HALT()");
        pk_halt();
    }
#else
    SBE_ERROR ("logError Logging RC=0x%08X, with severity=%d", io_rc.getRC(), i_sev);
    io_rc.setRC ( FAPI2_RC_SUCCESS );
#endif

}

#ifdef MINIMUM_REG_COLLECTION
// Platform implementation for FAPI API for HW Register Collection
void collectRegisters(
    const void *i_target,
    const uint32_t* i_address_list,
    const uint32_t i_length,
    uint32_t*& io_hwp_reg_ffdc)
{
    plat_target_sbe_handle plat_target;
    auto ffdc_targ_val = reinterpret_cast<const sbe_target_info*>(i_target);

    uint32_t target_search_rc = g_platTarget->getSbePlatTargetHandle(
            ffdc_targ_val->logtargetType,
            ffdc_targ_val->instanceID,
            plat_target);

    // update target position
    *io_hwp_reg_ffdc = ffdc_targ_val->instanceID;
    io_hwp_reg_ffdc++;

    for(uint16_t i = 0; i < i_length; i++)
    {
        uint64_t data;

        if(target_search_rc == SBE_SEC_OPERATION_SUCCESSFUL)
        {
            ReturnCode rc = getscom_abs_wrap(
                &plat_target,
                i_address_list[i],
                &data,
                false,  // i_isIndirectScom
                false); // i_ffdcAllowed
            if(rc)
            {
                // updating data with bad magic word to indicate the failure
                data = (0xBAADBAADllu << 32) | ((uint32_t)rc);
            }
        }
        else
        {
            // updating data with bad magic word to indicate the failure
            data = (0xBAAAAAADllu << 32) | target_search_rc;
        }

        // since hw register data will be stored after lv-ffdc,
        //  and its size is 12 bytes per lv-ffdc, we cannot ensure an
        //  8-byte aligned address here.
        // Hence always using a word by word assignment.
        *io_hwp_reg_ffdc = data >> 32;
        io_hwp_reg_ffdc++;
        *io_hwp_reg_ffdc = data; // copying lower 32-bytes
        io_hwp_reg_ffdc++;
    }
    // each scom will have 2 words, and size of FFDC-ID, Target Pos and size-field
    *io_hwp_reg_ffdc = 3 + i_length * 2;
    io_hwp_reg_ffdc++;
}
#endif // MINIMUM_REG_COLLECTION

} // namespace fapi2

void plat_FfdcInit (void)
{
    constexpr uint32_t ffdcLastUeSize = ( sizeof(pozFfdcNode_t)                   +
                                       ffdcUtils_getHwpSize(MAX_FFDC_LV_SIZE, 0)
#ifdef MINIMUM_FFDC_RE
                                     + ffdcUtils_getPlatSize()
#endif
                                     );

    /* Allocation scratch space for one minimum UE */
    pozFfdcNode_t * node  = (pozFfdcNode_t *) Heap::get_instance().
                                scratch_calloc(ffdcLastUeSize, Heap::AF_PERSIST);
    if (node == nullptr)
    {
        SBE_ERROR ("plat_FfdcInit failed to alloc for min one UE %d", ffdcLastUeSize);
        pk_halt();
    }
    fapi2::g_ffdcCtrlSingleton.setLastUeSpace(node);
    SBE_INFO ("plat_FfdcInit: LAST UE size: %d, addr: [0x%08X]", ffdcLastUeSize, node);

#if defined(MINIMUM_FFDC_RE)
    /* Allocation scratch space for Scratch Full RC Space */
   constexpr  uint32_t ffdcSizeScratchFullRc = sizeof(pozFfdcNode_t) +
                                        ffdcUtils_getHwpSize(FFDC_SCRATCH_FULL_RC_LV_SIZE, 0) +
                                        ffdcUtils_getPlatSize<0>(); // For Scratch FULL RC space ffdc disable the TRACE data

    node  = (pozFfdcNode_t *) Heap::get_instance().
                                    scratch_calloc(ffdcSizeScratchFullRc, Heap::AF_PERSIST);
    if (node == nullptr)
    {
        SBE_ERROR ("plat_FfdcInit failed to alloc for Scratch FULL persistent space %d", ffdcSizeScratchFullRc);
        pk_halt();
    }
    fapi2::g_ffdcCtrlSingleton.setScratchFullRcSpace(node);
    SBE_INFO ("plat_FfdcInit, Scratch full rc size: %d, addr: [0x%08X]", ffdcSizeScratchFullRc, node);
#endif

}