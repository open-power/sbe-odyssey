# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: meson.build $
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
project(
        'sbe',
        'c','cpp',
        default_options: [
                            'warning_level=3',
                            'werror=true',
                         ],
        license: 'Apache-2.0',
        version: '0.1',
       )

#Compilers
cc      = find_program('powerpc-eabi-gcc')
objdump = find_program('objdump')
objcopy = find_program('objcopy')
ld      = find_program('ld')
ar      = find_program('ar')
as      = find_program('as')
nm      = find_program('nm')
strip   = find_program('strip')

#Output path for genfiles as well install files.
#Fetch the path from --prefix arg
output_dir_path = get_option('prefix')
run_command('mkdir', output_dir_path)

#Runtime gen-files path
genfiles_dir_path = output_dir_path + '/genfiles/'
run_command('mkdir', genfiles_dir_path)

#subdir('public/')
