# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/common/hwp/meson.build $
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
import_generic_dir_path = pub_import_dir_path + '/hwp/generic/perv/'

generic_infile_c = []

#Create abs path for input files before creating .o
abs_generic_infiles_c_path = []
foreach generic_infile : generic_infile_c

abs_generic_infile_c_path = import_generic_dir_path + generic_infile
abs_generic_infiles_c_path += abs_generic_infile_c_path

endforeach

lib_ekb_cmn_file_name = 'ekbcmnhwp'

#TODO: This if condition is temporary to go ahead with the build process
# when no hwp files are mentioned
if ( abs_generic_infiles_c_path.length() != 0 )
    generic_hwp_obj_file_c = obj_gen_cpp.process(abs_generic_infiles_c_path)
    infile  = [generic_hwp_obj_file_c]

    libekbcmnhwp = custom_target( lib_ekb_cmn_file_name,
                                     output : 'lib' + lib_ekb_cmn_file_name,
                                     input : infile,
                                     command : [ ar,
                                                 'crs',
                                                 '@OUTPUT@'+'.a',
                                                 '@INPUT@'],
                                     build_by_default: true,
                                 )

    ekb_cmn_hwp_libs = '-l' + lib_ekb_cmn_file_name
    ekb_cmn_hwp_lib_paths = '-L' + meson.current_build_dir()
    ekb_cmn_hwp_dep_libs = libekbcmnhwp
endif
