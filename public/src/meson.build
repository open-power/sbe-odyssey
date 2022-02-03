# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/meson.build $
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

#Grab the platform type to build parts of image as per user choise
platform = get_option('platform')

#Grab the DD Level as per user choise
ddlvl = get_option('ddlevel')

#Tools
parsAndCutElf = find_program('build/utils/parsAndCutElf.py')
tracehash = find_program('tools/trace/tracehash.pl')
buildInfo = find_program('build/utils/buildInfo.py')

#ekb-src public mirror path.(import dir path)
pub_import_dir_path = meson.global_source_root() + '/public/src/import/public/'

#Generate ppetrace app
ppetraceapp_dir = 'import/public/emb/p11/kernels/ppe/ppetrace/ppetracepp/'
inc_dir = include_directories(ppetraceapp_dir)
ppetraceapp = executable('ppetraceapp',
              ppetraceapp_dir + 'ppetracepp.C',
              cpp_args : ['-O3','-w','-g'],
              include_directories : inc_dir,
              implicit_include_directories : false,
              native : true)

#Common genfiles
run_command(buildInfo, cmn_genfiles_dir_path)

#Update cmn include path with -I and global source dir.
cmn_abs_inc_paths = []
odyssey_cmn_abs_inc_paths = []

foreach inc_path : meson.get_external_property('cmn_inc_paths')

  abs_inc_path = '-I' + meson.global_source_root() + inc_path
  cmn_abs_inc_paths += abs_inc_path

endforeach

if platform == 'odyssey' or platform == 'all'
  foreach inc_path : meson.get_external_property('odyssey_cmn_inc_paths')

    abs_inc_path = '-I' + meson.global_source_root() + inc_path
    odyssey_cmn_abs_inc_paths += abs_inc_path

  endforeach
endif

#Object file generation generator's (api's)
obj_gen_c = generator(ppetraceapp,
                    output  : '@BASENAME@.o',
                    arguments : [
                                 '/afs/awd/projects/cte/tools/ppetools/prod/bin/powerpc-eabi-gcc',
                                 meson.get_external_property('cc_flags_cmn'),
                                 cmn_abs_inc_paths,
                                 meson.get_external_property('gcc_defs_cmn'),
                                 '@EXTRA_ARGS@','-o','@OUTPUT@','@INPUT@'],
                    depfile : '@BASENAME@.d'
                    )

obj_gen_cpp = generator(ppetraceapp,
                    output  : '@BASENAME@.o',
                    arguments : [
                                 '/afs/awd/projects/cte/tools/ppetools/prod/bin/powerpc-eabi-gcc',
                                 meson.get_external_property('cc_flags_cmn'),
                                 cmn_abs_inc_paths,
                                 meson.get_external_property('obj_args_cpp'),
                                 meson.get_external_property('gcc_defs_cmn'),
                                 '@EXTRA_ARGS@','-o','@OUTPUT@','@INPUT@'],
                    depfile : '@BASENAME@.d'
                    )

obj_gen_s = generator(as,
                    output  : '@BASENAME@.o',
                    arguments : [
                                 '-mppe42x',
                                 cmn_abs_inc_paths,
                                 '@EXTRA_ARGS@','-o','@OUTPUT@','@INPUT@']
                    )

subdir('build/buildekb')
subdir('common')

subdir('onetime')
subdir('runtime')
