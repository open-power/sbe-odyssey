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

infile_c = ['attribute_override_service.C'
           ]

lib_file_name = 'odysseysppeattr'

odyssey_sppe_attr_obj_file_cpp = obj_gen_cpp.process(
                                    infile_c,
                                    extra_args : [
                                                  odyssey_runtime_abs_inc_paths,
                                                  odyssey_cmn_abs_inc_paths,
                                                  runtime_cmn_abs_inc_paths
                                                 ])

infile  = [odyssey_sppe_attr_obj_file_cpp]

libodysseysppeattr  = custom_target( lib_file_name,
                                   output : 'lib' + lib_file_name,
                                   input : infile,
                                   command : [ ar,
                                               'crs',
                                               '@OUTPUT@'+'.a',
                                               '@INPUT@'],
                                   build_by_default: true,
                               )

odyssey_sppe_libs += '-l' + lib_file_name
odyssey_sppe_lib_paths += '-L' + meson.current_build_dir()
odyssey_sppe_dep_libs += libodysseysppeattr
