# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/onetime/common/algorithms/ecdsa/meson.build $
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

infile_c = ['ecdsa.C'
            ]

lib_file_name = 'ecdsaalgo'

ecdsa_algo_obj_file_cpp = obj_gen_cpp.process(infile_c)

infile  = [ecdsa_algo_obj_file_cpp]

libecdsaalgo = custom_target( lib_file_name,
                                 output : 'lib' + lib_file_name,
                                 input : infile,
                                 command : [ ar,
                                             'crs',
                                             '@OUTPUT@'+'.a',
                                             '@INPUT@'],
                                 build_by_default: true,
                             )

ecdsa_algo_libs = '-l' + lib_file_name
ecdsa_algo_lib_paths = '-L' + meson.current_build_dir()
ecdsa_algo_dep_libs = libecdsaalgo
