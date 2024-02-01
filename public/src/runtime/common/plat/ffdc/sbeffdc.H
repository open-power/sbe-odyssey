/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/ffdc/sbeffdc.H $               */
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
#ifndef __SBE_FFDC_H
#define __SBE_FFDC_H

#include "pk_trace.h"
#include "sbeffdctype.H"
#include "sbeSpMsg.H"
#include "target.H"
#include "error_info_defs.H"
#include "fapi2.H"
#include "sbe_build_info.H"
#include "sbeglobals.H"

/**
 * @brief FFDC plat truncated trace size
 *        SBE_FFDC_TRUNCATED_TRACE_LENGTH for truncated trace len, and extra logic
 *        includes the trace header size @ref PkTraceBuffer
 *        plat_truncated_trace_size = sizeof (PkTraceBuffer) + truncatedTraceSize
*/
#define PLAT_FFDC_TRUNCATED_TRACE_SIZE  ((sizeof(*G_PK_TRACE_BUF) -        \
                                          sizeof(G_PK_TRACE_BUF->cb)) +    \
                                          SBE_FFDC_TRUNCATED_TRACE_LENGTH)

/**
 * @brief SCRATCH Space required fo scratch full RC ffdc HWP local data size
 *
 * @note This is Required HWP local variable data size fo RC_POZ_FFDC_SCRATCH_SPACE_FULL_ERROR
 *       User modified any of HWP local variable are respective RC which is
 *       directly effect to the below mentioned value
 */
#define FFDC_SCRATCH_FULL_RC_LV_SIZE (3 * sizeof(fapi2::sbeFfdc_t))


/**
 * @brief - Capture Async failure FFDC
 *
 * @param[in] primRc Primary failure rc
 * @param[in] secRc  Secondary failure rc
 *
 */
void captureAsyncFFDC(uint32_t primRc,
                        uint32_t secRc);

//SBE internal FFDC package class
class SbeFFDCPackage
{
private:
    //Disable copy constructor
    SbeFFDCPackage(SbeFFDCPackage const &) = delete;
    //Disable assignment operator
    SbeFFDCPackage& operator=(SbeFFDCPackage const &) = delete;

public:
    //FFDC Package header
    sbeResponseFfdc_t iv_ffdcPackageHeader;

    union
    {
        //( FFDC_HW + FFDC_REG )
        struct
        {
            //FFDC HW data header dump fields
            hwDataHeader_t iv_hwDataHeader;

            // FFDC HW Local, hw Reg Data blob as per FFDC Package type
            ffdcPackageBlob_t iv_hwLocalBlob;
            ffdcPackageBlob_t iv_hwRegBlob;
        };
        //( FFDC_ATTR + FFDC_TRACE )
        struct
        {
            //FFDC SBE data header
            sbeDataHeader_t iv_sbeDataHeader;

            // FFDC Trace and Attr Data blob as per FFDC Package type
            ffdcPackageBlob_t iv_sbeTraceBlob;
            ffdcPackageBlob_t iv_sbeAttrBlob;
        };
    };

    /*
     * @brief updateHWpackageDataHeader - update the HW FFDC data based on
     *                                    the dumpFields_t
     *
     */
    void updateHWpackageDataHeader(void);

    /*
     * @brief updateSBEpackageDataHeader - update the PLAT FFDC data based
     *                                     on the dumpFields_t
     *
     */
    void updateSBEpackageDataHeader(void);

public:
    /**
     * Constructor
     */
    SbeFFDCPackage(const uint32_t i_fieldsConfig)
    {
        // Update FFDC Package Header Stucture
        iv_ffdcPackageHeader.setCmdInfo(
                                SBE_GLOBAL->failedSeqId,
                                SBE_GLOBAL->failedCmdClass,
                                SBE_GLOBAL->failedCmd);
        SBE_INFO(" i_fieldsConfig :0x%08X ", i_fieldsConfig);
        if( i_fieldsConfig & SBE_FFDC_ALL_HW_DATA )
        {
            //length and dumpFields will be filled up depending on the fields
            //to be sent in send APIs
            iv_ffdcPackageHeader.fapiRc = fapi2::current_err;
            iv_hwDataHeader.dumpFields = {0};
            iv_hwDataHeader.dumpFields.set(i_fieldsConfig);
            SBE_INFO(" HW get :0x%08X ", iv_hwDataHeader.dumpFields.get());
        } else if( i_fieldsConfig & SBE_FFDC_ALL_PLAT_DATA )
        {
            iv_ffdcPackageHeader.fapiRc = fapi2::FAPI2_RC_PLAT_ERR_SEE_DATA;
            iv_sbeDataHeader.primaryStatus = SBE_GLOBAL->failedPrimStatus;
            iv_sbeDataHeader.secondaryStatus = SBE_GLOBAL->failedSecStatus;
            iv_sbeDataHeader.fwCommitID = SBE_COMMIT_ID;
            iv_sbeDataHeader.ddLevel = SBE_FFDC_DD1;
            iv_sbeDataHeader.dumpFields = {0};
            iv_sbeDataHeader.dumpFields.set(i_fieldsConfig);
            SBE_INFO(" plat get :0x%08X ", iv_sbeDataHeader.dumpFields.get());
        } else
        {
           SBE_INFO(" Not valid i_fieldsConfig");
        }
    }

    /**
     * @brief createHwPackage  - method to pack and send HW Local and
     *                           Hw Reg FFDC data only if isFifoData is true for
     *                           Unrecoverable Error over FIFO interface and
     *                           Recoverable Error in Heap pointer storage.
     * @param[out] o_wordsSent - number of words to be sent
     *
     * @param[in] isFifoData   - isFifoData FIFO type is set or not
     *                           The default fifo type is FIFO:0
     * @return                 - SBE secondary RC
     *
     */
    uint32_t createHwPackage( uint32_t &o_wordsSent,
                              const bool isFifoData = false, sbeFifoType i_type = SBE_FIFO );

    /**
     * @brief createHwPackage  - method to pack and send PLAT internal FFDC data
     *                           if isFifoData is true for Unrecoverable Error
     *                           over FIFO interface and Recoverable Error
     *                           in Heap pointer.
     * @param[out] o_wordsSent - number of words to be sent
     *
     * @param[in] isFifoData   - isFifoData FIFO type is set or not
     *                           The default fifo type is FIFO:0
     * @return                 - SBE secondary RC
     *
     */
    uint32_t createSbePackage( uint32_t &o_wordsSent, const bool isFifoData = false, sbeFifoType i_type = SBE_FIFO );

};


/**
 * @brief only if isFifoData is true for Recoverable
 *         Unrecoverable Error over FIFO interface
 *
 * @param[out] o_wordsSent       - number of words sent
 *                                 By default it is false.
 * @param[in] i_type             - FIFO type @enum sbeFifoType
 *                                 The default fifo type is FIFO: SBE_FIFO (0)
 * @param[in] i_forceFullTracePackage - Request from get FFDC chipop side or not
 * @return                       - SBE secondary RC
 *
 */
uint32_t sendFFDCOverFIFO( uint32_t &o_wordsSent,
                           sbeFifoType i_type,
                           bool i_forceFullTracePackage );

#if defined( MINIMUM_FFDC_RE )
///
/// @brief Log a sbe internal error. This will create a sbe-FFDC object in heap,
///        which will have sbe-header and only plat package. Newly created
///        object will be marked as committed and added to the list
///        Note: In case of UE fatal is always true
///
/// @param[in] i_primRc Primary RC as @enum sbePrimResponse
/// @param[in] i_secRc SBE Secondary RC as @enum sbeSecondaryResponse
/// @param[in] i_sev Fapi error log severity defaulted to unrecoverable
/// @param[in] i_isFatal true error is fatal error
///
void logSbeError( const uint16_t i_primRc,
                  const uint16_t i_secRc,
                  fapi2::errlSeverity_t i_sev = fapi2::FAPI2_ERRL_SEV_UNRECOVERABLE,
                  bool i_isFatal = true
                );
#endif

///
/// @brief Log a HWP error. but will not be commited by HWP and sbe firmware has
///        to commit it. The given error object will be marked as fatal and
///        committed. If it is marked as fatal, we will be sending full trace
///        package with same slid.
///
/// @param[in] i_rc FAPI Return Code object
///
void logFatalError( fapi2::ReturnCode& i_rc );


/**
 * @brief Function to get HWP FFDC package size
 *
 * @param i_hwpLocalDataLen HWP local variable length
 * @param i_hwpRegDataLen   HWP register data length
 * @return uint16_t size fof HWP FFDC package
 */
constexpr uint16_t ffdcUtils_getHwpSize(uint16_t i_hwpLocalDataLen, uint16_t i_hwpRegDataLen)
{
    return ( sizeof(pozHwpFfdcPackageFormat_t)+ /* hwp ffdc Frame size */
             sizeof(packageBlobField_t)       + /* HWP local data fields size */
             i_hwpLocalDataLen                + /* HWP local data length */
             ((i_hwpRegDataLen)? sizeof(packageBlobField_t) : 0) + /* HWP Reg data field size */
             i_hwpRegDataLen                 ); /* HWP reg data length */
}

/**
 * @brief Function to get PLAT FFDC package size
 *
 * @tparam N truncated trace size default PLAT_FFDC_TRUNCATED_TRACE_SIZE
 *           expected trace size: zero or PLAT_FFDC_TRUNCATED_TRACE_SIZE
 * @return constexpr uint16_t  size of PLAT FFDC package
 */
template <uint16_t N = PLAT_FFDC_TRUNCATED_TRACE_SIZE>
constexpr uint16_t ffdcUtils_getPlatSize()
{
    static_assert(((N == 0) || (N == PLAT_FFDC_TRUNCATED_TRACE_SIZE)), "Unexpected Trace size");

    return ( sizeof(pozPlatFfdcPackageFormat_t) + /* Plat FFDC size (plat data started) */
            ((N)?  sizeof(packageBlobField_t) + N : 0) );
}


/**
 * @brief Function to get FFDC initialization scratch space size
 *
 * @return constexpr uint32_t  FFDC initialization scratch space size
 *
 * @note For any changes in plat_FfdcInit() function directly effect this function,
 *       Have to modify in both place.
 */
constexpr uint32_t plat_ffdcUtilGeFfdcInitSpaceSize(void)
{
    return (// Size of LAST UE FFDC
            sizeof(pozFfdcNode_t) +
            ffdcUtils_getHwpSize(MAX_FFDC_LV_SIZE, 0)
#ifdef MINIMUM_FFDC_RE
        +   ffdcUtils_getPlatSize()

        // Size of scratch space FULL RC ffdc
        +   sizeof(pozFfdcNode_t)
        +   ffdcUtils_getHwpSize(FFDC_SCRATCH_FULL_RC_LV_SIZE, 0)
        +   ffdcUtils_getPlatSize<0>() // For Scratch FULL RC space ffdc disable the TRACE data
#endif // MINIMUM_FFDC_RE
#ifdef MINIMUM_REG_COLLECTION
        +   MAX_REG_FFDC_SIZE
#endif // MINIMUM_REG_COLLECTION
            );
}


/**
 * @brief Delete uncommitted error in FFDC list for given thread ID
 *
 * Function is deleting the uncommitted error in the linked list to avoid
 * memory leakage.
 *
 * @param[in] i_threadId The thread ID for clearing uncommitted FFDC matches
 *                       the thread ID
 *
 */
void ffdcFreeUnwantedError(uint8_t i_threadId);


/**
 * @brief FFDC init to creating persistent space for last Error
 */
void plat_FfdcInit(void);

#endif //__SBE_FFDC_H
