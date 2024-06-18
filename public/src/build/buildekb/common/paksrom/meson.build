# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/common/paksrom/meson.build $
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

import_dir_path = pub_import_dir_path + '/common/utils/imageProcs/'

infile_c = ['archive.C'
           ]

#Create abs path for input files before creating .o
abs_infiles_c_path = []

foreach infile : infile_c

  abs_infile_c_path = import_dir_path + infile
  abs_infiles_c_path += abs_infile_c_path

endforeach

ekb_cmn_srom_pak_obj_file_cpp = obj_gen_cpp.process(abs_infiles_c_path, extra_args: '-D__SROM_IMAGE__')

lib_file_name = 'ekbcmnsrompak'

infile  = [ekb_cmn_srom_pak_obj_file_cpp]

libekbcmnsrompak = custom_target( lib_file_name,
                                 output : 'lib' + lib_file_name,
                                 input : infile,
                                 command : [ ar,
                                             'crs',
                                             '@OUTPUT@'+'.a',
                                             '@INPUT@'],
                                 build_by_default: true,
                             )

ekb_cmn_srom_pak_libs = '-l' + lib_file_name
ekb_cmn_srom_pak_lib_paths = '-L' + meson.current_build_dir()
ekb_cmn_srom_pak_dep_libs = libekbcmnsrompak