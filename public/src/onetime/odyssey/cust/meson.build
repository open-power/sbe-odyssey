# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/onetime/odyssey/cust/meson.build $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022
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
#Tools
packaging = find_program('build/utils/build.sh')

odyssey_cust_install_path = 'odyssey/onetime/cust'

#Odyssey Boot Loader Base Image name
odyssey_cust_img_name = 'odyssey_cust_' + ddlvl

#Now lets run the post build script to package the image
outfile = odyssey_cust_img_name + '.pak'

ocust_package_img = custom_target( 'ocust_package_img',
                               output : outfile,
                               command : [ packaging, meson.current_build_dir(),
                                           odyssey_cust_img_name, pakTool,
                                           pakBuildTool],
                               build_by_default : true,
                              )
