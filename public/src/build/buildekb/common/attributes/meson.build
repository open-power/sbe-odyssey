# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/common/attributes/meson.build $
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

#List of all attribute xmls
ekb_cmn_attr_xml_paths = []

#List of sbe attribute xmls
ekb_cmn_sbe_attr_filter_xml_paths = []

import_generic_dir_path = pub_import_dir_path + 'common/generic/fapi2/xml/attribute_info/'
common_attr_xml = [
        'system_attributes.xml',
        'unit_attributes.xml',
        'chip_attributes.xml',
        'common_attributes.xml',
        ]

foreach infile : common_attr_xml

  abs_infile_xml_path = import_generic_dir_path + infile
  ekb_cmn_attr_xml_paths += abs_infile_xml_path

endforeach

import_hwp_dir_path = pub_import_dir_path + 'hwp/generic/xml/attribute_info/'
infile_attr_xml = [
            'poz_pervasive_attributes.xml',
            'poz_chiplet_attributes.xml',
            'poz_clock_attributes.xml',
            ]

foreach infile : infile_attr_xml

  abs_infile_xml_path = import_hwp_dir_path + infile
  ekb_cmn_attr_xml_paths += abs_infile_xml_path

endforeach

filter_xml_path = import_generic_dir_path + 'sbe_common_attribute.xml'
ekb_cmn_sbe_attr_filter_xml_paths += filter_xml_path
