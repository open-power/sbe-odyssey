# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/odyssey/sppe/meson.build $
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

#List containing all Odyssey SPPE lib paths
odyssey_sppe_lib_paths = []

#List containing all lib names for odyssey SPPE image
odyssey_sppe_libs = []

#List of all dependent libs for odyssey image (For custom target depends input)
#Image generation will not start unless all the libs in below list are built.
#This list is basically custom target lib generation output object
odyssey_sppe_dep_libs = []

#Tools
packaging = find_program('build/utils/build.sh')

odyssey_sppe_lib_paths = [ekb_cmn_lib_paths,
                          cmn_core_lib_paths,
                          runtime_cmn_core_lib_paths,
                          cmn_utils_lib_paths,
                         ]

odyssey_sppe_libs = [ekb_cmn_libs,
                     cmn_core_libs,
                     runtime_cmn_core_libs,
                     cmn_utils_libs,
                    ]

odyssey_sppe_dep_libs = [ekb_cmn_dep_libs,
                         cmn_core_dep_libs,
                         runtime_cmn_core_dep_libs,
                         cmn_utils_dep_libs,
                        ]

#Odyssey SPPE install path
odyssey_sppe_install_path = 'odyssey/runtime/sppe'

#Odyssey SPPE Base Image name
odyssey_sppe_img_name = 'odyssey_sppe_' + ddlvl

subdir('hwpf')
subdir('plat')
subdir('core')
subdir('build')

########################################## Build Odyssey SPPE Image ######################################################

infile  = meson.global_source_root() + '/public/src/runtime/odyssey/sppe/build/linker/linker.cmd'
outfile = ['linker']
odyssey_link_file_path = meson.global_source_root() + '/public/src/build/links/'

osppe_link_cmd_processing = custom_target('osppe_link_cmd_processing',
                                      output : outfile,
                                      input : infile,
                                      command : [ cc,
                                      '-I' + odyssey_link_file_path, '-MD',
                                      meson.get_external_property('link_cmd_args'),
                                      meson.get_external_property('gcc_defs_cmn'),
                                      '@INPUT0@',
                                      '-o',
                                      '@OUTPUT@'],
                                      depfile : '@BASENAME@.d',
                                      build_by_default: true
                                    )

infile  = [osppe_link_cmd_processing]
outfile = [odyssey_sppe_img_name + '.map', odyssey_sppe_img_name + '.out']

osppe_out_map_gen = custom_target('osppe_out_map_gen',
                             output : outfile,
                             input : infile,
                             depends : odyssey_sppe_dep_libs,
                             command : [ ld,
                                         '-e','__system_reset','-n',
                                         '-T','@INPUT0@',
                                         '-Map','@OUTPUT0@',
                                         '-Bstatic','-o','@OUTPUT1@',
                                         odyssey_sppe_lib_paths,
                                         '--start-group',odyssey_sppe_libs,'--end-group','--gc-sections',
                                          '--undefined=_ZN5fapi24ATTR18ATTR_IS_SIMULATIONE'],
                            build_by_default: true,
                            build_always_stale : true,
                            install : true,
                            install_dir : odyssey_sppe_install_path
                          )

infile  = osppe_out_map_gen
outfile = odyssey_sppe_img_name + '.syms'

osppe_symbol_file_gen = custom_target('osppe_symbol_file_gen',
                                      output : outfile,
                                      input : infile,
                                      command : [ nm, '-C', '-S','@INPUT1@'],
                                      build_by_default : true,
                                      capture : true,
                                      install : true,
                                      install_dir : odyssey_sppe_install_path
                                      )

infile  = osppe_out_map_gen
outfile = odyssey_sppe_img_name + '.dis'

osppe_dis_file = custom_target( 'osppe_dis_gen',
                               output : outfile,
                               input : infile,
                               command : [ objdump,'-S','@INPUT1@'],
                               build_by_default : true,
                               capture : true,
                               install : true,
                               install_dir : odyssey_sppe_install_path
                            )

# include actual offset of the attribute variable in attr db file
infiles = [odyssey_sbe_attr_db_file_name,
            osppe_symbol_file_gen]
outfile = odyssey_sppe_img_name + '.attr.db'

osppe_attr_tank = custom_target('ospee_attr_compile',
                            output : outfile,
                            input : infiles,
                            command : [attrTool, 'compile',
                                        '@INPUT@', '@OUTPUT@', '-v'],
                            build_by_default : true,
                            install : true,
                            install_dir : odyssey_sppe_install_path
                                )

infile  = osppe_out_map_gen
outfile = odyssey_sppe_img_name + '.bin'

osppe_bin_gen = custom_target('osppe_bin_gen',
                              input : infile,
                              output : outfile,
                              command : [ parsAndCutElf, '@INPUT1@'],
                              build_by_default: true,
                              install : true,
                              install_dir : odyssey_sppe_install_path
                            )

#Generate a rpt file which has elf header & sections info
infile  = osppe_out_map_gen
outfile = odyssey_sppe_img_name + '.rpt'

osppe_elf_rpt_file = custom_target( 'osppe_elf_rpt_gen',
                               output : outfile,
                               input : infile,
                               command : [ 'readelf','-S','-h','@INPUT1@'],
                               build_by_default : true,
                               capture : true,
                               install : true,
                               install_dir : odyssey_sppe_install_path
                            )

#Remove previous string file if present.
osppe_rm_string_file = custom_target( 'osppe_rm_string_file',
                               output : 'osppe_rm_string_file_none',
                               command : [ 'rm','-f',
                                            meson.current_build_dir() +
                                            '/odysseySppeStringFile_' + ddlvl ],
                               build_by_default : true,
                               build_always_stale : true,
                              )

#Generate string file
osppe_string_file_gen = custom_target(output : 'odysseySppeStringFile_' + ddlvl,
                                      depends : [osppe_out_map_gen,osppe_rm_string_file],
                                      command : [ tracehash, '-c', '-FC', '-d',
                                                  meson.global_build_root() + '/public/src/',
                                                  '-s','@OUTPUT@', ],
                                      build_by_default : true,
                                      install : true,
                                      install_dir : odyssey_sppe_install_path
                                )

#Now lets run the post build script to package the image
osppe_package_img = custom_target( 'osppe_package_img',
                               output : 'osppe_package_img_none',
                               depends : [ osppe_string_file_gen, osppe_symbol_file_gen,
                                           osppe_dis_file, osppe_attr_tank ],
                               command : [ packaging,
                                           meson.current_build_dir() + '/' + odyssey_sppe_img_name,
                                           attrTool ],
                               build_by_default : true,
                              )
