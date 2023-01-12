# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/common/rings/meson.build $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2023
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
import_generic_dir_path = pub_import_dir_path + '/ring/generic/'

generic_infile_c = [
                    'poz_putRingBackend.C',
                    'poz_putRingUtils.C'
                    ]

#Create abs path for input files before creating .o
ekb_generic_ring_c_path = []

foreach generic_infile : generic_infile_c

abs_generic_infile_c_path = import_generic_dir_path + generic_infile
ekb_generic_ring_c_path += abs_generic_infile_c_path

endforeach
