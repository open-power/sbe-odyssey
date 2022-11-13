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
pakTool = find_program('import/public/common/utils/imageProcs/tools/paktool')
pakBuildTool = find_program('import/public/common/utils/imageProcs/tools/pakbuild')
flashBuildTool = find_program('import/public/common/utils/imageProcs/tools/flashbuild')
cmdTableTool = find_program('import/public/hwp/generic/perv/cmdtable/cmdtable.py')
signHashList = find_program('build/utils/signHashList')
tracehashwrap = find_program('tools/trace/tracehashwrap.py')
buildInfo = find_program('build/utils/buildInfo.py')
generateImageHash = find_program('build/utils/genImgHash.sh')

# we are using multiple tools for generating source code and using run_command
#   for invoking these tools. So, whenever these tools or files passed to tool is
#   is changed, meson will automatically trigger the setup phase again. But meson
#   is not tracking the files which is invoked by any of these tools. So this
#   variable can be used to collect all such dependency file and we will put a work-around
#   (at the end of this file) to invoke a dummy command, so that any of change in
#   these files will trigger meson setup again.
MESON_SETUP_EXTRA_DEPENDENCIES = []

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

#Output Path for common genfiles
cmn_genfiles_dir_path = genfiles_dir_path + '/common/'
run_command('mkdir', '-p', cmn_genfiles_dir_path)

#Lets define all genfiles paths that can be used elsewhere in the project
onetime_genfiles_dir_path = genfiles_dir_path + '/onetime/'
runtime_genfiles_dir_path = genfiles_dir_path + '/runtime/'

#Common genfiles
result_buildInfo = run_command(buildInfo, cmn_genfiles_dir_path)
if not (result_buildInfo.returncode() == 0)
      message(result_buildInfo.stdout())
      error(result_buildInfo.stderr())
endif

#Update cmn include path with -I and global source dir.
cmn_abs_inc_paths = []
ekb_cmn_abs_inc_paths = []
runtime_cmn_abs_inc_paths = []
onetime_cmn_abs_inc_paths = []
odyssey_cmn_abs_inc_paths = []
odyssey_runtime_abs_inc_paths = []
p11_cmn_abs_inc_paths = []
p11_runtime_abs_inc_paths = []
ekb_hwp_abs_inc_paths  = []
odyssey_hwp_abs_inc_paths = []
p11_sppe_cmn_abs_inc_paths = []
p11_tsbe_cmn_abs_inc_paths = []
p11_hsbe_cmn_abs_inc_paths = []
p11_hwp_abs_inc_paths = []

foreach inc_path : meson.get_external_property('cmn_inc_paths')

  abs_inc_path = '-I' + meson.global_source_root() + inc_path
  cmn_abs_inc_paths += abs_inc_path

endforeach

foreach inc_path : meson.get_external_property('ekb_cmn_inc_path')

  abs_inc_path = '-I' + pub_import_dir_path + inc_path
  ekb_cmn_abs_inc_paths += abs_inc_path

endforeach

foreach inc_path : meson.get_external_property('ekb_hwp_inc_path')

  abs_inc_path = '-I' + pub_import_dir_path + inc_path
  ekb_hwp_abs_inc_paths += abs_inc_path

endforeach

foreach inc_path : meson.get_external_property('runtime_cmn_inc_paths')

  abs_inc_path = '-I' + meson.global_source_root() + inc_path
  runtime_cmn_abs_inc_paths += abs_inc_path

endforeach

foreach inc_path : meson.get_external_property('onetime_cmn_inc_paths')

  abs_inc_path = '-I' + meson.global_source_root() + inc_path
  onetime_cmn_abs_inc_paths += abs_inc_path

endforeach

if platform == 'odyssey' or platform == 'all'
  foreach inc_path : meson.get_external_property('odyssey_cmn_inc_paths')

    abs_inc_path = '-I' + meson.global_source_root() + inc_path
    odyssey_cmn_abs_inc_paths += abs_inc_path

  endforeach

  foreach inc_path : meson.get_external_property('odyssey_runtime_inc_paths')

    abs_inc_path = '-I' + meson.global_source_root() + inc_path
    odyssey_runtime_abs_inc_paths += abs_inc_path

  endforeach

  foreach inc_path : meson.get_external_property('odyssey_hwp_inc_paths')

    abs_inc_path = '-I' + pub_import_dir_path + inc_path
    odyssey_hwp_abs_inc_paths += abs_inc_path

  endforeach

  #Odyssey genfiles dir path
  odyssey_srom_genfiles_dir_path = onetime_genfiles_dir_path + '/odyssey/srom/'
  odyssey_bldr_genfiles_dir_path = onetime_genfiles_dir_path + '/odyssey/bldr/'
  odyssey_cmn_genfiles_dir_path  = onetime_genfiles_dir_path + '/odyssey/common/'
  odyssey_sppe_genfiles_dir_path = runtime_genfiles_dir_path + '/odyssey/sppe/'
  run_command('mkdir','-p', odyssey_srom_genfiles_dir_path, odyssey_bldr_genfiles_dir_path,
      odyssey_sppe_genfiles_dir_path, odyssey_cmn_genfiles_dir_path)
endif

if platform == 'p11' or platform == 'all'
  foreach inc_path : meson.get_external_property('p11_cmn_inc_paths')

    abs_inc_path = '-I' + meson.global_source_root() + inc_path
    p11_cmn_abs_inc_paths += abs_inc_path

  endforeach

  foreach inc_path : meson.get_external_property('p11_cmn_runtime_inc_paths')

    abs_inc_path = '-I' + meson.global_source_root() + inc_path
    p11_runtime_abs_inc_paths += abs_inc_path

  endforeach

  foreach inc_path : meson.get_external_property('p11_cmn_hwp_inc_paths')

    abs_inc_path = '-I' + pub_import_dir_path + inc_path
    p11_hwp_abs_inc_paths += abs_inc_path

  endforeach

  #P11 genfiles dir path
  p11_cmn_genfiles_dir_path  = onetime_genfiles_dir_path + '/p11/common/'
  p11_bldr_genfiles_dir_path = onetime_genfiles_dir_path + '/p11/bldr/'
  p11_srom_genfiles_dir_path  = onetime_genfiles_dir_path + '/p11/srom/'
  p11_sppe_genfiles_dir_path = runtime_genfiles_dir_path + '/p11/sppe/'
  p11_tsbe_genfiles_dir_path = runtime_genfiles_dir_path + '/p11/tsbe/'
  run_command('mkdir','-p', p11_srom_genfiles_dir_path, p11_bldr_genfiles_dir_path,
    p11_sppe_genfiles_dir_path, p11_tsbe_genfiles_dir_path, p11_cmn_genfiles_dir_path)

  foreach inc_path : meson.get_external_property('p11_sppe_cmn_inc_paths')

    abs_inc_path = '-I' + meson.global_source_root() + inc_path
    p11_sppe_cmn_abs_inc_paths += abs_inc_path

  endforeach

  foreach inc_path : meson.get_external_property('p11_tsbe_cmn_inc_paths')

    abs_inc_path = '-I' + meson.global_source_root() + inc_path
    p11_tsbe_cmn_abs_inc_paths += abs_inc_path

  endforeach

  foreach inc_path : meson.get_external_property('p11_hsbe_cmn_inc_paths')

    abs_inc_path = '-I' + meson.global_source_root() + inc_path
    p11_hsbe_cmn_abs_inc_paths += abs_inc_path

  endforeach

endif

#Object file generation generator's (api's)
obj_gen_c = generator(ppetraceapp,
                    output  : '@BASENAME@.o',
                    arguments : [
                                 meson.get_external_property('powerpc_eabi_gcc'),
                                 '@EXTRA_ARGS@',
                                 meson.get_external_property('cc_flags_cmn'),
                                 cmn_abs_inc_paths, ekb_cmn_abs_inc_paths,
                                 meson.get_external_property('gcc_defs_cmn'),
                                 '-o','@OUTPUT@','@INPUT@'],
                    depfile : '@BASENAME@.d'
                    )

obj_gen_cpp = generator(ppetraceapp,
                    output  : '@BASENAME@.o',
                    arguments : [
                                 meson.get_external_property('powerpc_eabi_gcc'),
                                 '@EXTRA_ARGS@',
                                 meson.get_external_property('cc_flags_cmn'),
                                 cmn_abs_inc_paths, ekb_cmn_abs_inc_paths,
                                 meson.get_external_property('obj_args_cpp'),
                                 meson.get_external_property('gcc_defs_cmn'),
                                 meson.get_external_property('gcc_defs_cmn_sbe'),
                                 '-o','@OUTPUT@','@INPUT@'],
                    depfile : '@BASENAME@.d'
                    )

obj_gen_cpp_hwp = generator(ppetraceapp,
                    output  : '@BASENAME@.o',
                    arguments : [
                                 meson.get_external_property('powerpc_eabi_gcc'),
                                 '@EXTRA_ARGS@',
                                 meson.get_external_property('cc_flags_cmn'),
                                 cmn_abs_inc_paths, ekb_cmn_abs_inc_paths,
                                 meson.get_external_property('obj_args_cpp'),
                                 meson.get_external_property('gcc_defs_cmn'),
                                 meson.get_external_property('gcc_defs_cmn_hwp'),
                                 '-o','@OUTPUT@','@INPUT@'],
                    depfile : '@BASENAME@.d'
                    )

obj_gen_s = generator(as,
                    output  : '@BASENAME@.o',
                    arguments : [
                                 '-mppe42x',
                                 '@EXTRA_ARGS@',
                                 cmn_abs_inc_paths, ekb_cmn_abs_inc_paths,
                                 '-o','@OUTPUT@','@INPUT@']
                    )

subdir('tools')
subdir('build')
subdir('common')

if get_option('srom').enabled() or get_option('bldr').enabled() or get_option('otprom').enabled()
  subdir('onetime')
endif

if (get_option('sppe').enabled() or get_option('tsbe').enabled() or get_option('hsbe').enabled()
       or get_option('sbe').enabled() or get_option('pme').enabled())
  subdir('runtime')
endif

#After all images are built lets call packaging
subdir('packaging')

# Dummy call to create dependency on the files invoked by different tools
run_command('ls', MESON_SETUP_EXTRA_DEPENDENCIES)

# If SROM build for ODYSSEY Platform is disabled pick up the frozen DD1 RIT images from repo
if get_option('srom').disabled() and  get_option('platform') == 'odyssey'
    install_subdir(meson.global_source_root() + '/public/src/onetime/odyssey/srom/build/images/',
                      install_dir : 'odyssey/onetime/srom',strip_directory : true)
endif
