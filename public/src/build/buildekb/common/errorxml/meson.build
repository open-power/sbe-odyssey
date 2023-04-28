# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/common/errorxml/meson.build $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2023
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
import_generic_xml_dir_base_path = pub_import_dir_path + '/hwp/generic/xml/error_info/'

common_error_xml_files = [
                          'poz_perv_mod_chiplet_clocking_errors.xml',
                          'poz_perv_mod_misc_errors.xml',
                          'poz_bist_errors.xml',
                          'poz_sbe_plat_errors.xml',
                          'poz_sbe_spi_cmd_errors.xml',
                          'poz_putRingUtils_errors.xml',
                          'poz_putRingBackend_errors.xml',
                          'poz_chiplet_reset_errors.xml',
                          'poz_stopclocks_errors.xml',
                          'poz_tracearray_errors.xml',
                          'poz_fastarray_errors.xml',
                          'poz_scan_compare_errors.xml',
                          'poz_scan_via_scom_errors.xml',
                          'poz_sbe_i2c_errors.xml'
                        ]

#Create abs path for input xml files
common_abs_error_xml_files = []
foreach error_xml_file : common_error_xml_files

abs_error_xml_file = import_generic_xml_dir_base_path + error_xml_file
common_abs_error_xml_files += abs_error_xml_file

endforeach
