# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/onetime/odyssey/bldr/meson.build $
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

#List containing all Odyssey Boot Loader lib paths
odyssey_bldr_lib_paths = []

#List containing all lib names for odyssey Boot Loader image
odyssey_bldr_libs = []

#List of all dependent libs for odyssey image (For custom target depends input)
#Image generation will not start unless all the libs in below list are built.
#This list is basically custom target lib generation output object
odyssey_bldr_dep_libs = []

#Update odyssey bldr include path with -I and global source dir.
odyssey_bldr_abs_inc_paths = []
foreach inc_path : meson.get_external_property('odyssey_bldr_inc_paths')

  abs_inc_path = '-I' + meson.global_source_root() + inc_path
  odyssey_bldr_abs_inc_paths += abs_inc_path

endforeach

#Tools
packaging = find_program('build/utils/build.sh')

#Add other required libs to build odyssey SROM image
odyssey_bldr_libs += [odyssey_ekb_cmn_libs,
                      cmn_pak_wrapper_libs
                     ]

odyssey_bldr_lib_paths += [odyssey_ekb_cmn_lib_paths,
                           cmn_pak_wrapper_lib_paths
                          ]

odyssey_bldr_dep_libs += [odyssey_ekb_cmn_dep_libs,
                          cmn_pak_wrapper_dep_libs
                         ]

#Odyssey Boot Loader install path
odyssey_bldr_install_path = 'odyssey/onetime/bldr'

#Odyssey Boot Loader Base Image name
odyssey_bldr_img_name = 'odyssey_bldr_' + ddlvl

subdir('core')

########################################## Build Odyssey Boot Loader Image ######################################################

infile  = meson.global_source_root() + '/public/src/onetime/odyssey/bldr/build/linker/linker.cmd'
outfile = ['linker']
odyssey_link_file_path = meson.global_source_root() + '/public/src/build/links/'

obldr_link_cmd_processing = custom_target('obldr_link_cmd_processing',
                                      output : outfile,
                                      input : infile,
                                      command : [ cc,
                                      '-I' + odyssey_link_file_path, '-MD',
                                      meson.get_external_property('link_cmd_args'),
                                      '@INPUT0@',
                                      '-o',
                                      '@OUTPUT@'],
                                      depfile : '@BASENAME@.d',
                                      build_by_default: true
                                    )

infile  = [obldr_link_cmd_processing]
outfile = [odyssey_bldr_img_name + '.map', odyssey_bldr_img_name + '.out']

obldr_out_map_gen = custom_target('obldr_out_map_gen',
                             output : outfile,
                             input : infile,
                             depends : odyssey_bldr_dep_libs,
                             command : [ ld,
                                         '-e','__system_reset','-n',
                                         '-T','@INPUT0@',
                                         '-Map','@OUTPUT0@',
                                         '-Bstatic','-o','@OUTPUT1@',
                                         odyssey_bldr_lib_paths,
                                         '--start-group',odyssey_bldr_libs,'--end-group','--gc-sections'],
                            build_by_default: true,
                            build_always_stale : true,
                            install : true,
                            install_dir : odyssey_bldr_install_path
                          )

infile  = obldr_out_map_gen
outfile = odyssey_bldr_img_name + '.bin'

obldr_bin_gen = custom_target('obldr_bin_gen',
                              input : infile,
                              output : outfile,
                              command : [ parsAndCutElf, '@INPUT1@'],
                              build_by_default: true,
                              install : true,
                              install_dir : odyssey_bldr_install_path
                            )

infile  = obldr_out_map_gen
outfile = odyssey_bldr_img_name + '.syms'

obldr_symbol_file_gen = custom_target('obldr_symbol_file_gen',
                                      output : outfile,
                                      input : infile,
                                      command : [ nm,'-S','@INPUT1@'],
                                      build_by_default : true,
                                      capture : true,
                                      install : true,
                                      install_dir : odyssey_bldr_install_path
                                      )

infile  = obldr_out_map_gen
outfile = odyssey_bldr_img_name + '.dis'

obldr_dis_file = custom_target( 'obldr_dis_gen',
                               output : outfile,
                               input : infile,
                               command : [ objdump,'-S','@INPUT1@'],
                               build_by_default : true,
                               capture : true,
                               install : true,
                               install_dir : odyssey_bldr_install_path
                            )

#Generate a rpt file which has elf header & sections info
infile  = obldr_out_map_gen
outfile = odyssey_bldr_img_name + '.rpt'

obldr_elf_rpt_file = custom_target( 'obldr_elf_rpt_gen',
                               output : outfile,
                               input : infile,
                               command : [ 'readelf','-S','-h','@INPUT1@'],
                               build_by_default : true,
                               capture : true,
                               install : true,
                               install_dir : odyssey_bldr_install_path
                            )

#Remove previous string file if present.
obldr_rm_string_file = custom_target( 'obldr_rm_string_file',
                               output : 'obldr_rm_string_file_none',
                               command : [ 'rm','-f',
                                            meson.current_build_dir() +
                                            '/odysseyBldrStringFile_' + ddlvl ],
                               build_by_default : true,
                               build_always_stale : true,
                              )

#Generate string file
#TODO need to add algorith path
#Removing -L in lib path by convert list to str and Remove '-L' then str to list
traceshash_lib_path= ' '.join(odyssey_bldr_lib_paths).replace('-L', '').split()
obldr_string_file_gen = custom_target(output : 'odysseyBldrStringFile_' + ddlvl,
                                      depends : [obldr_out_map_gen,obldr_rm_string_file],
                                      command : [ tracehashwrap, '-l',
                                                  traceshash_lib_path ,
                                                  '-o','@OUTPUT@', ],
                                      build_by_default : true,
                                      install : true,
                                      install_dir : odyssey_bldr_install_path
                                )

obldr_part_tbl_gen = custom_target( 'obldr_part_tbl_gen',
                                    input: 'build/utils/partitions',
                                    output: 'part.tbl',
                                    command: [ flashBuildTool, 'compile-ptable', '@INPUT@', '@OUTPUT@' ]
                                  )

#Now lets run the post build script to package the image
outfile = odyssey_bldr_img_name + '.pak'

obldr_package_img = custom_target( 'obldr_package_img',
                               output : outfile,
                               depends : [ obldr_string_file_gen, obldr_symbol_file_gen,
                                           obldr_dis_file, obldr_part_tbl_gen ],
                               command : [ packaging, meson.current_build_dir(),
                                           odyssey_bldr_img_name, pakTool,
                                           pakBuildTool, signHashList],
                               build_by_default : true,
                              )

# Run script to generate hash file of image
# This is required for verification of hash written into measurements
outfile = odyssey_bldr_img_name + '.hash'

obldr_gen_img_hash = custom_target( 'obldr_gen_img_hash',
                               output : outfile,
                               depends : [ obldr_package_img ],
                               command : [ generateImageHash,
                                          '-i', meson.current_build_dir() + '/boot/secure.hdr',
                                          '-o', meson.current_build_dir(),
                                          '-f', odyssey_bldr_img_name
                                          ],
                               build_by_default : true,
                               install : true,
                               install_dir : odyssey_bldr_install_path
                              )
