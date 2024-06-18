# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/common/pkkernel/meson.build $
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

import_dir_path = pub_import_dir_path + '/emb/pst/kernels/ppe/pk/kernel/'

infile_c = ['pk_core.c',
            'pk_init.c',
            'pk_stack_init.c',
            'pk_bh_core.c',
            'pk_debug_ptrs.c',
            'pk_timer_core.c',
            'pk_timer_init.c',
            'pk_thread_init.c',
            'pk_thread_core.c',
            'pk_thread_util.c',
            'pk_semaphore_init.c',
            'pk_semaphore_core.c'
            ]

#Create abs path for input files before creating .o
pk_abs_infiles_c_path = []

foreach infile : infile_c

  abs_infile_c_path = import_dir_path + infile
  pk_abs_infiles_c_path += abs_infile_c_path

endforeach