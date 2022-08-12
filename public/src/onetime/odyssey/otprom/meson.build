# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/onetime/odyssey/otprom/meson.build $
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

subdir('core')

#Odyssey OTPROM install path
odyssey_otprom_install_path = 'odyssey/onetime/otprom'

#Odyssey OTPROM Base Image name
odyssey_otprom_img_name = 'odyssey_otprom_' + ddlvl

infile = odyssey_otprom_fuse_map_json[0]
outfile = odyssey_otprom_img_name + '.bin'
depfiles = [pymodtemplateUtil]

#Generate the fuse map binary basis JSON
odyssey_otprom_bin_gen = custom_target( 'odyssey_otprom_bin_gen',
                                 output : outfile,
                                 input : infile,
                                 depend_files : depfiles,
                                 command : [ fusemapgen,
                                              '-i', '@INPUT@','bingenerator','-o','@OUTPUT@'],
                                 build_by_default: true,
                                 install : true,
                                 install_dir : odyssey_otprom_install_path
                             )

##################################################### Genfiles #####################################

#Generate the fuse map header file basis JSON
#Lets keep the output in common as Boot Loader image also uses it
result_fusemapgen = run_command(fusemapgen, '-i', odyssey_otprom_fuse_map_json[0], 'hdrgenerator',
    '-o', odyssey_cmn_genfiles_dir_path + 'otpromfusemap.H')

if not (result_fusemapgen.returncode() == 0)
    message(result_fusemapgen.stdout())
    error(result_fusemapgen.stderr())
endif

