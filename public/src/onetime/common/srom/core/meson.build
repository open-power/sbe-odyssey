# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/onetime/common/srom/core/meson.build $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022
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
infile_c = ['shv.C',
            'securebootutil.C'
            ]

lib_file_name = 'cmnsromcore'

cmn_srom_obj_file_cpp = obj_gen_cpp.process(infile_c)

infile_s = ['cmdtable.S',
            ]

fsm_pre_process = custom_target(
                    output : '@BASENAME@.s',
                    input : infile_s,
                    command : [ cc,
                                meson.get_external_property('pre_processing_args'),
                                meson.get_external_property('gcc_defs_cmn'),
                                meson.get_external_property('pre_processing_args'),
                                ekb_cmn_abs_inc_paths, onetime_cmn_abs_inc_paths,
                                '-o','@OUTPUT@','@INPUT@'],
                    build_by_default: true,
                    depfile : '@BASENAME@.d'
                             )

fsm_obj_file_s = obj_gen_s.process(fsm_pre_process)

infile  = [cmn_srom_obj_file_cpp, fsm_obj_file_s]

libcmnsromcore = custom_target( lib_file_name,
                                 output : 'lib' + lib_file_name,
                                 input : infile,
                                 command : [ ar,
                                             'crs',
                                             '@OUTPUT@'+'.a',
                                             '@INPUT@'],
                                 build_by_default: true,
                             )

cmn_srom_core_libs = '-l' + lib_file_name
cmn_srom_core_lib_paths = '-L' + meson.current_build_dir()
cmn_srom_core_dep_libs = libcmnsromcore
