/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/chipops/collectdump/sbecollectdump.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
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
#ifndef __SBE_SBECOLLECTDUMP_H
#define __SBE_SBECOLLECTDUMP_H

#include "sbefifo.H"
#include "sbedumpconstants.H"
#include "plat_hwp_data_stream.H"
#include "file_access.H"
#include "target_types.H"
#include "filenames.H"
#include <fapi2_attribute_service.H>

#define DUMP_DATA_LAYOUT_VERSION 0x3

// Dump chip-op Footer - DONE
static const uint32_t DUMP_CHIP_OP_FOOTER = 0x444F4E45;
static const uint32_t FIFO_DOUBLEWORD_LEN = 2;

//Dump Pre-req bits to updates for command status
enum preReqCommandStatus : uint8_t
{
    PRE_REQ_PASSED          = 0x80, // Good Status
    PRE_REQ_NON_FUNCTIONAL  = 0x40, // Non Functional
    PRE_REQ_CLOCK_ON_STATE  = 0x20, // Clock On in ChipOp
    PRE_REQ_CORE_NOT_SCOM_SCAN_STATE = 0x10, // Target in Stop State
};

extern size_t genericHdctRowSize_table[NO_OF_CMD_TYPES];


/**
 * @brief enums for Trace Array Access Messages
 *
*/
enum sbeArrayAccessCommands
{
    SBE_CMD_CONTROL_FAST_ARRAY    = 0x01, /**< Control Fast Array */
    SBE_CMD_CONTROL_TRACE_ARRAY   = 0x02, /**< Control Trace Array */
};

/**
 * @brief enum for sbe MPIPL command class
 *
*/
enum sbeMpIplCommands
{
    SBE_CMD_MPIPL_STOPCLOCKS = 0x03, /* Stop Clocks */
};

//Struct containing details of HDCT PAK section
// Change this to pak instead of HDCT PAK Section
typedef struct hdctPakSecDetails
{
    uint32_t startAddr;
    uint32_t size;
    uint32_t endAddr;
    uint32_t currAddr;

    /**
     * @brief Get the HDCT Pak offset(Start location) from SBE Image.
     *
     * @return  HDCT Pak offset(Start location)
     */
    hdctPakSecDetails()
    {
        void *image = NULL;
        size_t image_size;
        fapi2::ReturnCode rc = fapi2::plat_loadEmbeddedFile(hdct_binary_fname,
                                                            image, image_size);
        if(rc == fapi2::FAPI2_RC_SUCCESS)
        {
        // get the start address using loadEmbeddedFile api
        startAddr = reinterpret_cast<uint32_t>(image);
        endAddr = startAddr + image_size;
        //Set currAddr to start of HDCT row
        currAddr = startAddr + sizeof(uint64_t);
        //Dump start after EKB CommitID
        SBE_INFO("Start Offset: [0x%08X] Size: [0x%08X] \
                    End offset: [0x%08X] ", startAddr,size,endAddr );
        }
        fapi_try_exit:
            fapi2::freeEmbeddedFile(image);
     }
}hdctPakSecDetails_t;

//Struct for getDump chip-op header
typedef struct __attribute__((packed)) dumpHeader
{
    uint32_t dumpDataLayoutVersion:8;
    uint32_t chipTypeToEncode:8; // Chip Type
    uint32_t ecMajor:4;          // EC major level
    uint32_t ecMinor:4;          // EC minor level
    uint32_t reserved:8;
    //EKB Commit ID
    uint64_t hdctVer;

    /**
     * @brief Constructor
     */
    dumpHeader()
    {
        dumpDataLayoutVersion = DUMP_DATA_LAYOUT_VERSION;
        chipTypeToEncode  = 0;
        fapi2::ATTR_EC_Type chip_ec = 0;
        FAPI_ATTR_GET_PRIVILEGED(fapi2::ATTR_EC,
                                g_platTarget->plat_getChipTarget(), chip_ec);
        SBE_INFO("DUMP Chip ATTR_EC =[ %02X] ", chip_ec);
        uint8_t ecMajorMinor = (uint8_t)chip_ec;
        ecMajor = (ecMajorMinor & 0xF0)>>4;
        ecMinor = (ecMajorMinor & 0x0F);
        reserved          = 0x00;
    }

}dumpHeader_t;

//Generic struct for each HDCT row TOC.
//(Data to be streamed out of FIFO)
typedef struct
{
    struct __attribute__((packed))
    {
        uint32_t preReq:8;
        uint32_t cmdType:8;
        uint32_t chipUnitType:8;
        uint32_t chipUnitNum:8;
        uint64_t address;
        uint32_t dataLength;
    }hdctHeader;

    uint32_t ffdcLen;
    uint64_t cpuCycles;
    fapi2::plat_target_handle_t tgtHndl;

    void hdctHeaderInit(genericHdctRow_t* genericHdctRow)
    {
        hdctHeader.preReq = PRE_REQ_PASSED;
        hdctHeader.cmdType = (uint8_t) genericHdctRow->genericHdr.command;
        hdctHeader.chipUnitType = (uint8_t) genericHdctRow->genericHdr.chipUnitType;
    }
}genericRowTOC_t;


class sbeCollectDump
{
    private:
        fapi2::sbefifo_hwp_data_ostream iv_oStream; // ostream to push FIFO data
        uint8_t iv_dumpType;
        uint8_t iv_clockState;    //Dump collection clock state(ON or OFF)
        sbeFifoType iv_fifoType;  //Type of fifo to stream the data.
        uint16_t iv_hdctDumpTypeMap; //Equivalent HDCT dump type wrt requested dump type.
        //Unset the below flag once we find the 1st clock HDCT entry for Clock OFF state. Flag needs to be unset if clock ON state(Done in constructor).
        bool iv_clockOffEntryFlag = true;
        uint8_t iv_collectFastArray = false;
        uint32_t iv_OCMBInstance; // OCMB instance number

        genericHdctRow_t *iv_hdctRow;
        genericRowTOC_t iv_tocRow;
        dumpHeader_t iv_dumpHeader;
        hdctPakSecDetails_t iv_hdctPakSecDetails;
        sbeResponseFfdc_t iv_chipOpffdc;
        uint8_t iv_EQScanState[8];

    public:
        /**
         * @brief Constructor for the class sbeCollectDump
         *
         * @param [in] dumpID      The id of the dumpType.
         * @param [in] clockState  Dump collection clock state.(ON or OFF)
         * @param [in] type        sbe fifo type.
         */
        sbeCollectDump( uint8_t i_dumpID, uint8_t i_clockState, uint8_t i_collectFastArray,
                        sbeFifoType i_type, uint64_t i_hbMemAddr = 0x00 ):
                        iv_oStream(i_type, false, i_hbMemAddr),
                        iv_clockState(i_clockState), iv_fifoType(i_type), iv_collectFastArray(i_collectFastArray)
        {
            iv_clockState = i_clockState;
            iv_dumpType = i_dumpID;
            if (iv_clockState == SBE_DUMP_CLOCK_ON)
                iv_clockOffEntryFlag = false;
            //Get the equivalent HDCT dump type based on the requested Dump type
            iv_hdctDumpTypeMap = getEquivDumpType(i_dumpID);

            SBE_INFO("Dump Type Map is %x ", iv_hdctDumpTypeMap);
            SBE_INFO("Fast array collection is %x ", iv_collectFastArray);

            //Populate dump header struct with EKB Commit ID
            fapi2::seeprom_hwp_data_istream stream((uint32_t*)iv_hdctPakSecDetails.startAddr,
                                            sizeof(uint64_t));
            uint32_t msbEkbIdValue, lsbEkbIdValue;
            stream.get(msbEkbIdValue), stream.get(lsbEkbIdValue);
            iv_dumpHeader.chipTypeToEncode = getDumpHdrChipTypeToEncode();
            iv_dumpHeader.hdctVer = (((uint64_t)msbEkbIdValue << 32 ) |
                                    ((uint64_t)lsbEkbIdValue));
            SBE_INFO("EKB Commit ID: 0x%.8x%.8x ",
                    SBE::higher32BWord(iv_dumpHeader.hdctVer),
                    SBE::lower32BWord(iv_dumpHeader.hdctVer));
        }

        ~sbeCollectDump()
        {
            SBE_DEBUG("sbeCollectDump() Destructor...");
        }

        /**
         * @brief Collect the data for all the entries present in HDCT.
         *        It will be called from sbeGetDump() func.
         *
         * @return SUCCESS, Incase of no error.
         *         error,   Incase of failure.
         */
        uint32_t collectAllHDCTEntries();

        /**
         * @brief Collect the length of data for MpIpl the entries in HDCT.
         *        It will be called from MpIpl func.
         *
         * @return SUCCESS, Incase of no error.
         *         error,   Incase of failure.
         */
        uint32_t collectLenInBytesOfWriteData()
        {
            return iv_oStream.words_written() * 4;
        }

    private:
        /*
         * @brief Check if requested dump type is present in the current HDCT row generic header.
         *        eg Checkstop, HB, MPIPL.
         * @return bool true  if the dumpType is present in the current HDCT row generic header.
         *              false if the dumpType is not present in the current HDCT row generic header.
         */
        inline bool isDumpTypeMapped();

        /*
         * @brief Check scom and scan state for the core target.
         *
         * @return bool true if the core is scommable or scannable.
         */
        bool checkScomAndScanStateForCore();

        /**
         * @brief Parse the single HDCT entry and populate the genericHDCTRow_t object.
         *
         * @return SUCCESS,  if HDCT row is parsed and structure is populated and the requested dump
         *                   type present in the current HDCT generic header.
         *
         *         ERROR,    When there are no more HDCT entries/rows to be
         *                   parsed.
         */
        uint32_t parserSingleHDCTEntry();

        /**
         * @brief Parse the single HDCT entry and populate
         * the genericHDCTRow_t object.
         *
         * @return SUCCESS,  if HDCT row is parsed and structure is populated.
         *         ERROR,    Incase of failure.
         */
        uint32_t populateHDCTRowTOC();

       /** @brief Update DD Level anf PHP functional states
        *
        * @return SUCCESS,  If util is successful.
        *          ERROR,    Incase of failure.
        */
        void preUpdatePreReqHeaderData();

       /** @brief Write getFastArray data to the FIFO.
        *
        * @return SUCCESS,  If chip-op is successful.
        *          ERROR,    Incase of failure.
        */
        uint32_t writeGetFastArrayPacketToFifo();

       /** @brief stop clocks off as per HDCT row data.
        *
        * @return SUCCESS,  If chip-op is successful.
        *          ERROR,    Incase of failure.
        */
        uint32_t stopClocksOff();

       /**
        * @brief As per p10_query_host_meminfo HWP to determine address and size
        *        of HB memory and then write data to the FIFO
        *
        * @return SUCCESS,  If chip-op is successful.
        *         ERROR,    Incase of failure.
        */
        uint32_t writeGetMemPBAPacketToFifo();

       /**
        * @brief Write tracearry data to FIFO.
        *
        * @return SUCCESS,  If chip-op is successful.
        *         ERROR,    Incase of failure.
        */
        uint32_t writeGetTracearrayPacketToFifo();

       /** @brief Write getRing data to the FIFO.
        *
        * @return SUCCESS,  If chip-op is successful.
        *          ERROR,    Incase of failure.
        */
        uint32_t writeGetRingPacketToFifo();

       /**
        * @brief Write get sram data to the FIFO
        *
        * @return SUCCESS,  If chip-op is successful.
        *         ERROR,    Incase of failure.
        */
        uint32_t writeGetSramPacketToFifo();

       /**
        * @brief Write put scom updated data to the FIFO
        *
        * @return SUCCESS,  If chip-op is successful.
        *         ERROR,    Incase of failure.
        */
        uint32_t writePutScomPacketToFifo();

       /**
        * @brief Write get scom data to the FIFO.
        *
        * @return SUCCESS,  If chip-op is successful.
        *         ERROR,    Incase of failure.
        */
        uint32_t writeGetScomPacketToFifo();

       /**
        * @brief Write DUMP data to the FIFO
        *
        * @return SUCCESS,  If chip-op is successful.
        *         ERROR,    Incase of failure.
        */
        uint32_t writeDumpPacketRowToFifo();

       /**************** Plat Functions Start **********************/

       /**
        * @brief Get target list as per DUMP target id's.
        *
        * @param [in] o_targetList list of plat target handles
        *
        * @return SUCCESS,  If chip-op is successful.
        *         ERROR,    Incase of failure.
        */
        void getTargetList(std::vector<fapi2::plat_target_handle_t>
                            &o_targetList);

       /** @brief Get Dump Header chipTypeToEncode.
        * @return chiptypetoEncode
        */
        uint8_t getDumpHdrChipTypeToEncode();
 
        /***************** Plat Functions Ends ********************/

};

#endif /* __SBEFW_SBECOLLECTDUMP_H */
