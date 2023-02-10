/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/common/cnfg/odyssey/filenames.H $                  */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2022,2023                        */
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
#ifndef _ODYSSEYFILENAMES_H_
#define _ODYSSEYFILENAMES_H_

/*
* @brief This file contains names of all the files that are available in
*        odyssey PNOR pak file
*/

/*
* @brief List of all file names in Boot Loader pak(boot)
*/
static const char hash_list_file_name[]  = "boot/hash.list";
static const char secure_hdr_file_name[] = "boot/secure.hdr";
static const char bldr_file_name[]       = "boot/bldr.bin";

/*
* @brief List of all file names in Runtime Pak(rt)
*/
static const char hash_list_fname[]     = "rt/hash.list";
static const char secure_hdr_fname[]    = "rt/secure.hdr";
static const char sppe_bin_fname[]      = "rt/sppe.bin";
static const char hw_keys_hash_fname[]  = "rt/hwkeyshash.bin";
static const char sppe_ram_pak_fname[]  = "rt/sppe.ram.pak";
static const char cust_sppe_pak_fname[] = "cust/sppe.pak";

// CU: Code Update
namespace CU
{
    /*
     * @brief Enum for image types supported
     */
    enum IMAGES : uint16_t
    {
        IMG_TYPE_INVALID=0,
        BOOTLOADER = 1,    // Updatable(Ody)/ Not-updatable(P11)
        RUNTIME,           // Updatable(both)
        BMC_OVRD,          // Updatable(both)
        HB_OVRD,           // Updatable(both)
        SROM,              // Not-updatable
        CUST,              // Not-updatable
        SPPE_RAM,          // Updatable (both) as part of runtime
        IOPPE_BIN,         // Updatable (both) as part of runtime
        SYNOPSYS_BIN,      // Updatable (both) as part of runtime
        ARC_BIN,           // Updatable (both) as part of runtime
        END_OF_IMG_LIST
    };
} // namespace CU

#endif /* _ODYSSEYFILENAMES_H_ */
