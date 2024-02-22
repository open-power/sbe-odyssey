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

import_dir_path = pub_import_dir_path + '/emb/pst/kernels/ppe/pk/ppe42/'

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

#Create abs path for input files before creating .o
ppe42_abs_infiles_c_path = []
ppe42_abs_infiles_s_path = []

foreach infile : infile_c

  abs_infile_c_path = import_dir_path + infile
  ppe42_abs_infiles_c_path += abs_infile_c_path

endforeach

foreach infile : infile_s

  abs_infile_s_path = import_dir_path + infile
  ppe42_abs_infiles_s_path += abs_infile_s_path

endforeach