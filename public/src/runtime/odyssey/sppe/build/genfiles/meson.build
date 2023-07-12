# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/odyssey/sppe/build/genfiles/meson.build $
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

odyssey_genfiles_c = ['attribute.C',
                      'chipop_table.C',
                      'attribute_override.C',
           ]

odyssey_abs_genfiles_c_file_paths = []
foreach infile : odyssey_genfiles_c

  abs_infile_c_path = odyssey_sppe_genfiles_dir_path + infile
  odyssey_abs_genfiles_c_file_paths += abs_infile_c_path

endforeach

result_tagInfo = run_command(buildInfo, odyssey_sppe_genfiles_dir_path, 'sbe*')
if not (result_tagInfo.returncode() == 0)
      message(result_tagInfo.stdout())
      error(result_tagInfo.stderr())
endif
