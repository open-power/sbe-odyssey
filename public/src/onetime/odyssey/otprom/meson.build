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

# As this binary is not part of install lets create the dir
odyssey_otprom_install_path_dir = output_dir_path + '/' + odyssey_otprom_install_path
run_command('mkdir','-p', odyssey_otprom_install_path_dir)

#Generate the fuse map binary basis JSON
result_fusemapgen = run_command(fusemapgen, '-i', odyssey_otprom_fuse_map_json[0] ,'bingenerator',
    '-o', odyssey_otprom_install_path_dir + '/' + odyssey_otprom_img_name + '.bin')

if not (result_fusemapgen.returncode() == 0)
    message(result_fusemapgen.stdout())
    error(result_fusemapgen.stderr())
endif
