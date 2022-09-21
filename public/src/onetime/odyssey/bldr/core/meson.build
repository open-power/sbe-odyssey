# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/onetime/odyssey/bldr/core/meson.build $
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
#
infile_c = ['main.C',
            'threadroutine.C',
            'irq.C',
            'functionaddress.C',
            abs_pak_c_path,
            abs_cmn_core_c_path,
            abs_cmn_utils_c_path,
            ]

lib_file_name = 'odysseybldr'

odyssey_bldr_obj_file_cpp_gen = obj_gen_cpp.process(infile_c,
                                        extra_args : [onetime_cmn_abs_inc_paths,
                                        odyssey_bldr_abs_inc_paths,
                                        meson.get_external_property('gcc_defs_cmn_odyssey'),
                                        meson.get_external_property('gcc_defs_odyssey_bldr'),
                                        onetime_cmn_abs_inc_paths,
                                        odyssey_cmn_abs_inc_paths])

# tinflate.C must be compiled with -O2 -falign-loops=32 for significantly improved decompression performance
#   -O2 enables a few additional speed optimizations
#   -falign-loops=32 is _crucial_ to performance since the hottest loop in decompression is
#      tiny enough to fit within the PPE's 32-byte instruction buffer, so if we align the loop the PPE
#      can execute it without fetching instructions from PIBMEM, creating a massive speedup.
#      (we're talking a factor of 1.5 to 2 here!)
odyssey_bldr_obj_file_cpp_opt = obj_gen_cpp.process(abs_tinflate_c_path,
                                                    extra_args: ['-O2 -falign-loops=32',
                                                    odyssey_cmn_abs_inc_paths])

infile  = [odyssey_bldr_obj_file_cpp_gen, odyssey_bldr_obj_file_cpp_opt]

libodysseybldr = custom_target( lib_file_name,
                                 output : 'lib' + lib_file_name,
                                 input : infile,
                                 command : [ ar,
                                             'crs',
                                             '@OUTPUT@'+'.a',
                                             '@INPUT@'],
                                 build_by_default: true,
                             )

odyssey_bldr_libs += '-l' + lib_file_name
odyssey_bldr_lib_paths += '-L' + meson.current_build_dir()
odyssey_bldr_dep_libs += libodysseybldr
