/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/plat/ffdc/sbeffdc.H $               */
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
 * @brief sendFFDCOverFIFO       - method to pack and send HW / PLAT internal FFDC
 *                                 only if isFifoData is true for Recoverable
 *                                 Unrecoverable Error over FIFO interface
 * @param[in] i_fieldsConfig     - bitmap indicating the field
 *                                 to be sent in FFDC
 * @param[out] o_wordsSent       - number of words sent
 *                                 By default it is false.
 * @param[in]                    - i_isFifoData FIFO type is set or not
 *                                 The default fifo type is FIFO:0
 * @return                       - SBE secondary RC
 */
uint32_t sendFFDCOverFIFO( const uint32_t i_fieldsConfig, uint32_t &o_wordsSent,
                           const bool isFifoData = false, sbeFifoType i_type = SBE_FIFO );

#endif //__SBE_FFDC_H
