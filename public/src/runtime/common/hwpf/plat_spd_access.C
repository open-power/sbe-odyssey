/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/runtime/common/hwpf/plat_spd_access.C $            */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2015,2023                        */
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
///
/// @file plat_attribute_service.C
///
/// @brief Implements the specialized platform functions that access
/// attributes for FAPI2
///

#include <target_types.H>
#include <plat_spd_access.H>
#include <plat_utils.H>
#include <spdenums.H>
#include <i2c_access.H>


namespace fapi2
{

//******************************************************************************
// Function : getSPD()
// Return a blob of SPD data from a DIMM
//******************************************************************************
fapi2::ReturnCode getSPD(
                        const fapi2::Target<fapi2::TARGET_TYPE_DIMM>& i_pTarget,
                        uint8_t *  o_blob,
                        size_t& o_size)
{
    FAPI_DBG(">getSPD");

    fapi2::ReturnCode  l_rc, l_errl;

    do
    {

        // If the caller passed a nullptr for blob then
        // return size of the SPD
        if ( o_blob == nullptr )
        {
            // Get the DRAM generation from SPD
            uint8_t l_memGen = 0x0;
            o_size = 0;
            size_t  l_memSize = sizeof(l_memGen);
            std::vector<uint8_t> l_offset;
            std::vector<uint8_t> l_readData;
            l_offset.push_back(0);l_offset.push_back(2);

            FAPI_TRY(fapi2::getI2c(  i_pTarget,
                                     l_memSize,
                                     l_offset,
                                     l_readData));

            switch(l_readData[0])
            {
                case SPD::DDR4_TYPE:
                {
                    o_size = SPD::DDR4_SPD_SIZE;
                    break;
                }
                case SPD::DDR5_TYPE:
                {
                    o_size = SPD::DDR5_SPD_SIZE;
                    break;
                }// case DDR4 or DDR5

                default:
                {
                    FAPI_ERR("getSPD: Unsupported DIMM DDR Generation");
                    fapi2::current_err = fapi2::FAPI2_RC_FALSE;

                }
                break;

            }// switch

            FAPI_DBG("getSPD: Returning the size of the SPD :%d ", o_size);

        }// endif
        else
        {
            //The SPD data is larger than the max vector support, so we will read 128Bytes chunks
            size_t startOffset=0;
            size_t chunkSize=128;
            size_t remainingBytes=o_size;
            while(remainingBytes!=0){
              size_t bytesToRead=0;
              if(remainingBytes<=chunkSize)
                bytesToRead=remainingBytes;
              else
                bytesToRead=chunkSize;
              std::vector<uint8_t> l_offset;
              std::vector<uint8_t> l_readData;
              l_offset.push_back(static_cast<uint8_t>(startOffset/256));
              l_offset.push_back(static_cast<uint8_t>(startOffset&0xFF));

              FAPI_TRY(fapi2::getI2c(  i_pTarget,
                                       bytesToRead,
                                       l_offset,
                                       l_readData));
              for(size_t i=0;i<bytesToRead;i++)
                  o_blob[startOffset+i]=l_readData[i];

              remainingBytes=remainingBytes-bytesToRead;
              startOffset+=bytesToRead;
            }



        }// end else

        break;

    } while(0);


    FAPI_DBG("getSPD: SPD data Size %d Blob %d",
            o_size,o_blob);

    FAPI_DBG("<getSPD");
    fapi_try_exit:
    return fapi2::current_err;
}

} // End fapi2 namespace
