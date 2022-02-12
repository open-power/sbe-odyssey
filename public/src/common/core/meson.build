# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/common/core/meson.build $
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

infile_c = ['threadutil.C',
             'cmnglobals.C'
            ]

lib_file_name = 'cmncore'

cmn_core_obj_file_cpp = obj_gen_cpp.process(infile_c)

infile  = [cmn_core_obj_file_cpp]

libcmncore = custom_target( lib_file_name,
                                 output : 'lib' + lib_file_name,
                                 input : infile,
                                 command : [ ar,
                                             'crs',
                                             '@OUTPUT@'+'.a',
                                             '@INPUT@'],
                                 build_by_default: true,
                             )

cmn_core_libs = '-l' + lib_file_name
cmn_core_lib_paths = '-L' + meson.current_build_dir()
cmn_core_dep_libs = libcmncore
