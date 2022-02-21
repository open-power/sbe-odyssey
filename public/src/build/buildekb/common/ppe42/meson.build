# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/common/ppe42/meson.build $
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

import_dir_path = pub_import_dir_path + '/emb/p11/kernels/ppe/pk/ppe42/'

infile_s = ['ppe42_boot.S',
            'ppe42_exceptions.S',
            'ppe42_timebase.S',
            'ppe42_thread_init.S'
            ]

infile_c = ['ppe42_core.c',
            'ppe42_init.c',
            'ppe42_irq_core.c',
            'ppe42_irq_init.c'
            ]

lib_file_name = 'pk'

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

ppe42_pre_process_file_list = []

foreach S : abs_infiles_s_path
ppe42_pre_process = custom_target(
                                 output : '@BASENAME@.s',
                                 input : S,
                                 command : [ cc,
                                             meson.get_external_property('cc_flags_cmn'),
                                             cmn_abs_inc_paths, ekb_cmn_abs_inc_paths,
                                             meson.get_external_property('gcc_defs_cmn'),
                                             meson.get_external_property('pre_processing_args'),
                                             '-o','@OUTPUT@','@INPUT@'],
                                 build_by_default: true,
                                 depfile : '@BASENAME@.d'
                             )
ppe42_pre_process_file_list = ppe42_pre_process_file_list + ppe42_pre_process.to_list()
endforeach

pk_ppe42_obj_file_s = obj_gen_s.process(ppe42_pre_process_file_list)

pk_ppe42_obj_file_c = obj_gen_c.process(abs_infiles_c_path)


infile  = [pk_ppe42_obj_file_s,
           pk_ppe42_obj_file_c,
           pk_kernal_obj_file_c,
           boltonlib_std_obj_file_c,
           ppe_ppetrace_obj_file_c]

libpk = custom_target( lib_file_name,
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
ekb_cmn_dep_libs += libpk
