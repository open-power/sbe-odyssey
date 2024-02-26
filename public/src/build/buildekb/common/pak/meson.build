# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/common/pak/meson.build $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2024
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

tinflate_c = [ 'tinflate.C',
               '../../libs/crypto/keccak.C',
]

#Create abs path for input files before creating .o
abs_pak_c_path = []
abs_tinflate_c_path = []

foreach infile : infile_c

  abs_infile_c_path = import_dir_path + infile
  abs_pak_c_path += abs_infile_c_path

endforeach

foreach infile : tinflate_c
  abs_tinflate_c_path += import_dir_path + infile
endforeach
