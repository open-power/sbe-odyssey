/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/io/hwp_file_stream.H $   */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
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
/// @file hwp_file_stream.H
/// @brief Common unload functions
///-----------------------------------------------------------------------------
/// *HW HW Maintainer: Josh Chica <Josh.Chica@ibm.com>
/// *HW FW Maintainer:
/// *HW Consumed by  : SBE
///-----------------------------------------------------------------------------
#pragma once

#include <fapi2.H>
#include <hwp_data_stream.H>

///
/// @class hwp_file_stream
///
///     Streams IO Data back to a binary file
///
///
class hwp_file_stream
{

    public:
        /// @brief Checks if the binary file is valid
        /// @return bool True if valid file, false if not
        bool ok()
        {
            return !!iv_file;
        }

        /// @brief hwp_file_stream deconstructor, closes the file
        ~hwp_file_stream()
        {
            if (iv_close && iv_file)
            {
                fclose(iv_file);
            }
        }
    protected:

        /// @brief hwp_file_stream constrcutor, opens the binary file
        hwp_file_stream(const char* i_fname, const char* i_mode, const char* i_modestr, FILE* i_std_file)
        {
            if (strcmp(i_fname, "-"))
            {
                iv_file = fopen(i_fname, i_mode);
                iv_close = true;
            }
            else
            {
                iv_file = i_std_file;
                iv_close = false;
            }

            if (!iv_file)
            {
                printf("Failed to open %s for %s: %s\n", i_fname, i_modestr, strerror(errno));
            }
        }
        FILE* iv_file;
    private:
        bool iv_close;
};


/// @class hwp_file_ostream
///     Streams binary data to file
class hwp_file_ostream : public fapi2::hwp_data_ostream, public hwp_file_stream
{
    public:
        hwp_file_ostream(const char* i_fname) : hwp_file_stream(i_fname, "wb", "writing", stdout) {}

        /// @brief writes the data to the file
        /// @param[in] fapi2::hwp_data_unit     The data to be written to the file
        /// @return  FAPI2_RC_SUCCESS if success, else error code.
        fapi2::ReturnCode put(fapi2::hwp_data_unit i_data)
        {
            fapi2::hwp_data_unit l_data = htobe32(i_data);
            return fwrite(&l_data, sizeof(l_data), 1, iv_file) ?
                   fapi2::FAPI2_RC_SUCCESS : fapi2::FAPI2_RC_PLAT_ERR_SEE_DATA;
        }
};
