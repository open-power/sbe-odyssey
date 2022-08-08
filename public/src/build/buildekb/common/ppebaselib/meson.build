# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/common/ppebaselib/meson.build $
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

import_dir_path = pub_import_dir_path + '/emb/p11/kernels/ppe/baselib/'

infile_s = ['div64.S',
            'div32.S'
            ]

infile_c = ['ppe42_gcc.c',
            'ppe42_scom.c',
            'eabi.c',
            'ppe42_math.c'
            ]

strfile_c = ['ppe42_string.c'
            ]

lib_file_name = 'pkbase'

#Create abs path for input files before creating .o
abs_infiles_c_path = []
abs_infiles_s_path = []

foreach infile : infile_c

  abs_infile_c_path = import_dir_path + infile
  abs_infiles_c_path += abs_infile_c_path

endforeach

foreach infile : infile_s

  abs_infile_s_path = import_dir_path + infile
  abs_infiles_s_path += abs_infile_s_path

endforeach

abs_strfile_c_path = []
foreach infile : strfile_c

  abs_strfile_c_path += import_dir_path + infile

endforeach

# ppe42_string.c must be compiled with -falign-loops=32 for significantly improved
# peformance. -O2 enables few speed optimizations
ppe_baselib_obj_file_str = obj_gen_c.process(abs_strfile_c_path, extra_args: '-O2 -falign-loops=32')

ppe_baselib_pre_process_file_list = []

foreach S : abs_infiles_s_path
ppe_baselib_pre_process = custom_target(
                                 output : '@BASENAME@.s',
                                 input : S,
                                 command : [ cc,
                                             meson.get_external_property('cc_flags_cmn'),
                                             meson.get_external_property('gcc_defs_cmn'),
                                             meson.get_external_property('pre_processing_args'),
                                             '-o','@OUTPUT@','@INPUT@'],
                                 build_by_default: true,
                                 depfile : '@BASENAME@.d'
                             )
ppe_baselib_pre_process_file_list = ppe_baselib_pre_process_file_list + ppe_baselib_pre_process.to_list()
endforeach

ppe_baselib_obj_file_s = obj_gen_s.process(ppe_baselib_pre_process_file_list)

ppe_baselib_obj_file_c = obj_gen_c.process(abs_infiles_c_path)


infile  = [ppe_baselib_obj_file_s,
           ppe_baselib_obj_file_c,
           ppe_baselib_obj_file_str
           ]

pkbaselib = custom_target( lib_file_name,
                                 output : 'lib' + lib_file_name,
                                 input : infile,
                                 command : [ ar,
                                             'crs',
                                             '@OUTPUT@'+'.a',
                                             '@INPUT@'],
                                 build_by_default: true,
                             )

ekb_cmn_libs += '-l' + lib_file_name
ekb_cmn_lib_paths += '-L' + meson.current_build_dir()
ekb_cmn_dep_libs += pkbaselib
