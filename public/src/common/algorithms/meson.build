# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/common/algorithms/meson.build $
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

infile_c = ['ecdsa.C',
            'sha3.C',
            'dilithium.C'
            ]

lib_file_name = 'cmnalgo'

cmn_algo_obj_file_cpp = obj_gen_cpp.process(infile_c, extra_args : '-G0')

infile  = [cmn_algo_obj_file_cpp]

libcmnalgo = custom_target( lib_file_name,
                                 output : 'lib' + lib_file_name,
                                 input : infile,
                                 command : [ ar,
                                             'crs',
                                             '@OUTPUT@'+'.a',
                                             '@INPUT@'],
                                 build_by_default: true,
                             )

cmn_algo_libs = '-l' + lib_file_name
cmn_algo_lib_paths = '-L' + meson.current_build_dir()
cmn_algo_dep_libs = libcmnalgo
