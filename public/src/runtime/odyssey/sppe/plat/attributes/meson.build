# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/odyssey/sppe/plat/attributes/meson.build $
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

odyssey_abs_attr_c_file_paths = files(
                                ['attribute_override_service.C',
                                'attribute_list_service.C'
           ])

if platform == 'odysseylab'
  subdir_done()
endif

#Keep all genfiles related stuff below as odysseylab will generate its own genfiles
###################### Runtime genfiles ########################################
ODYSSEY_ATTR_DB_FILE_PATH = odyssey_sppe_genfiles_dir_path + '/sbe_attr.db'

run_cmd_result = run_command(attrTool, 'parse', EKB_CMN_ATTR_XMLS, EKB_ODY_ATTR_XMLS,
    '-fx', SBE_CMN_ATTR_FILTER_XML, SBE_ODY_ATTR_FILTER_XML,
    '-db', ODYSSEY_ATTR_DB_FILE_PATH,
    '-tl', ODYSSEY_TARGET_LIST_JSON,
    # we can keep -v options to get the backtrace if it failed.
    #'-v'
    )
if run_cmd_result.returncode() != 0
    message(run_cmd_result.stdout())
    error(run_cmd_result.stderr())
endif

run_cmd_result = run_command(attrTool, 'gensrc', ODYSSEY_ATTR_DB_FILE_PATH, odyssey_sppe_genfiles_dir_path,
    '--vfh', 'odyvirtualattrfunctions.H',
    # we can keep -v options to get the backtrace if it failed.
    #'-v'
    )
if run_cmd_result.returncode() != 0
    message(run_cmd_result.stdout())
    error(run_cmd_result.stderr())
endif

###########################################################################
