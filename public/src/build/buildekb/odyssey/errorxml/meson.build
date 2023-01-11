# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/odyssey/errorxml/meson.build $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2023
# [+] International Business Machines Corp.
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG
############## Build Required Runtime Genfiles ###############################
import_xml_dir_base_path = pub_import_dir_path + '/hwp/odyssey/xml/error_info/'
odyssey_error_xml_files = [
    'ody_scratch_regs_errors.xml',
    'ody_dts_read_errors.xml',
    ]

#Create abs path for input xml files
odyssey_abs_error_xml_files = []
foreach error_xml_file : odyssey_error_xml_files

abs_error_xml_file = import_xml_dir_base_path + error_xml_file
odyssey_abs_error_xml_files += abs_error_xml_file

endforeach

#Create abs path for generic XML files
mss_odyssey_abs_error_xml_files = []
import_xml_dir_base_path = pub_import_dir_path + '/memory/odyssey/xml/error_info/'
mss_odyssey_error_xml_files = [
    'ody_draminit_errors.xml',
    'ody_phyinit_errors.xml',
    ]

foreach error_xml_file : mss_odyssey_error_xml_files

abs_error_xml_file = import_xml_dir_base_path + error_xml_file
mss_odyssey_abs_error_xml_files += abs_error_xml_file

endforeach

#Create abs path for generic XML files
generic_abs_error_xml_files = []
import_xml_dir_base_path = pub_import_dir_path + '/memory/generic/xml/error_info/'
generic_error_xml_files = [
    'generic_ddr5_mr_error.xml',
    'generic_error.xml',
    ]

foreach error_xml_file : generic_error_xml_files

abs_error_xml_file = import_xml_dir_base_path + error_xml_file
generic_abs_error_xml_files += abs_error_xml_file

endforeach

#TODO: This if condition is temporary to go ahead with the build process
# when no error xml files are mentioned
if ( (odyssey_abs_error_xml_files.length() != 0) or
                                  (mss_odyssey_abs_error_xml_files.length() != 0) or
                                  (common_abs_error_xml_files.length() != 0) or
                                  (generic_abs_error_xml_files.length() != 0) )
    run_result = run_command(parseErrorInfo,'--local-ffdc','--output-dir='
            +odyssey_sppe_genfiles_dir_path,odyssey_abs_error_xml_files,generic_abs_error_xml_files,
                                            mss_odyssey_abs_error_xml_files,
                                            common_abs_error_xml_files)
    if (run_result.returncode() != 0)
        error(run_result.stderr())
        message(run_result.stdout())
    endif
endif
