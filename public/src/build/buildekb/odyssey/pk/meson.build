# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/odyssey/pk/meson.build $
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

infiles_list_s = [ppebaselib_abs_infiles_s_path,
                  ppe42_abs_infiles_s_path
                  ]

infile_c = [ppebaselib_abs_infiles_c_path,
            pk_abs_infiles_c_path,
            ppetrace_abs_infiles_c_path,
            ppe42_abs_infiles_c_path,
            boltonlib_abs_infiles_c_path,
            ]

infile_optmize_c = [ppebaselib_abs_strfile_c_path
            ]

lib_file_name = 'odysseyekbcmn'

odyssey_ekb_cmn_pre_process_file_list = []

foreach  infiles_s : infiles_list_s
foreach  S : infiles_s
odyssey_ekb_cmn_pre_process = custom_target(
                                 output : '@BASENAME@.s',
                                 input : S,
                                 command : [ cc,
                                             cmn_abs_inc_paths, ekb_cmn_abs_inc_paths,
                                             odyssey_cmn_abs_inc_paths,
                                             meson.get_external_property('cc_flags_cmn'),
                                             meson.get_external_property('gcc_defs_cmn'),
                                             meson.get_external_property('pre_processing_args'),
                                             '-o','@OUTPUT@','@INPUT@'],
                                 build_by_default: true,
                                 depfile : '@BASENAME@.d'
                             )
odyssey_ekb_cmn_pre_process_file_list = odyssey_ekb_cmn_pre_process_file_list + odyssey_ekb_cmn_pre_process.to_list()
endforeach
endforeach

odyssey_ekb_cmn_obj_file_s = obj_gen_s.process(odyssey_ekb_cmn_pre_process_file_list,
                                               extra_args : [odyssey_cmn_abs_inc_paths])

odyssey_ekb_cmn_obj_file_c = obj_gen_c.process(infile_c,
                                               extra_args : [odyssey_cmn_abs_inc_paths])

odyssey_ekb_cmn_obj_file_optimise = obj_gen_c.process(infile_optmize_c,
                                                 extra_args: ['-O2 -falign-loops=32',
                                                              odyssey_cmn_abs_inc_paths])


infile  = [odyssey_ekb_cmn_obj_file_s,
           odyssey_ekb_cmn_obj_file_c,
           odyssey_ekb_cmn_obj_file_optimise
           ]

odysseyekbcmnlib = custom_target( lib_file_name,
                                 output : 'lib' + lib_file_name,
                                 input : infile,
                                 command : [ ar,
                                             'crs',
                                             '@OUTPUT@'+'.a',
                                             '@INPUT@'],
                                 build_by_default: true,
                             )

odyssey_ekb_cmn_libs = '-l' + lib_file_name
odyssey_ekb_cmn_lib_paths = '-L' + meson.current_build_dir()
odyssey_ekb_cmn_dep_libs = odysseyekbcmnlib
