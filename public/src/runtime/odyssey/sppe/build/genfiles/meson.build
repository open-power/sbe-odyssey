# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/odyssey/sppe/build/genfiles/meson.build $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022
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

abs_infile_c_path = odyssey_sppe_genfiles_dir_path + '/attribute.C'
abs_attribute_c_file_paths = []
abs_attribute_c_file_paths += abs_infile_c_path

lib_file_name = 'odysseysppegenfiles'

odyssey_sppe_obj_file_cpp = obj_gen_cpp.process(abs_attribute_c_file_paths,
                                        extra_args : [
                                            meson.get_external_property('gcc_defs_cmn_odyssey'),
                                            meson.get_external_property('gcc_defs_odyssey_sppe'),
                                            odyssey_runtime_abs_inc_paths,
                                            runtime_cmn_abs_inc_paths,
                                            odyssey_cmn_abs_inc_paths])

infile  = [odyssey_sppe_obj_file_cpp]

libodysseyattribute = custom_target( lib_file_name,
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
odyssey_sppe_dep_libs += libodysseyattribute