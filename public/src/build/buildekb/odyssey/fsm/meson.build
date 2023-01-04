# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/odyssey/fsm/meson.build $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2023
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

#TODO: Leverage make file logic from ekb to auto build main.fsm and cust.fsm

import_dir_path = pub_import_dir_path + '/hwp/odyssey/perv/'

import_dir_path_inc_paths = [
        '-I' + pub_import_dir_path + '/hwp/generic/perv',
    ]

odyssey_fsm_path = meson.current_build_dir()

############################ cust.fsm #######################################

infile_asm = [
    'cust.asm',
    ]

outfile_tbl_list = []

#Create abs path for input files and generate .tbl file for every .asm file
foreach infile : infile_asm

  abs_infile_asm_path = import_dir_path + infile

  outfile_tbl = meson.current_build_dir()+ '/' + infile + '.tbl'
  outfile_tbl_list += outfile_tbl

  run_result = run_command(cmdTableTool,'assemble',import_dir_path_inc_paths,
      abs_infile_asm_path, outfile_tbl)

  if (run_result.returncode() != 0)
    error(run_result.stderr())
    message(run_result.stdout())
  endif

endforeach

#Link all generated tables and generat cust.fsm
run_result = run_command(cmdTableTool, 'link', '-q', '-o', odyssey_fsm_path + '/cust.fsm', outfile_tbl_list)
if (run_result.returncode() != 0)
  error(run_result.stderr())
  message(run_result.stdout())
endif
