# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/odyssey/sppe/plat/errorxml/meson.build $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023
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
# The below var can be used by the odyssey lab as well
odyssey_abs_error_xml_files_all = [
                                    odyssey_abs_error_xml_files,
                                    generic_abs_error_xml_files,
                                    generic_abs_io_error_xml_files,
                                    mss_odyssey_abs_error_xml_files,
                                    common_abs_error_xml_files
]

run_result = run_command(parseErrorInfo,'--local-ffdc','--output-dir='
        +odyssey_sppe_genfiles_dir_path, odyssey_abs_error_xml_files_all)

if (run_result.returncode() != 0)
    error(run_result.stderr())
    message(run_result.stdout())
endif
