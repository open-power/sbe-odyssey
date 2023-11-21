# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/common/attributes/meson.build $
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

#List of all attribute xmls
EKB_CMN_ATTR_XMLS = []

#List of sbe attribute xmls
SBE_CMN_ATTR_FILTER_XML = []

import_generic_dir_path = pub_import_dir_path + 'common/generic/fapi2/xml/attribute_info/'
common_attr_xml = [
        'system_attributes.xml',
        'unit_attributes.xml',
        'chip_attributes.xml',
        'common_attributes.xml',
        ]

foreach infile : common_attr_xml

  abs_infile_xml_path = import_generic_dir_path + infile
  EKB_CMN_ATTR_XMLS += abs_infile_xml_path

endforeach

import_hwp_dir_path = pub_import_dir_path + 'hwp/generic/xml/attribute_info/'
infile_attr_xml = [
            'poz_pervasive_attributes.xml',
            'poz_chiplet_attributes.xml',
            'poz_clock_attributes.xml',
            'poz_chip_attributes.xml',
            'poz_freq_attributes.xml',
            'poz_bist_attributes.xml',
            'poz_chip_ec_attributes.xml',
            'poz_sppefw_attributes.xml',
            ]

foreach infile : infile_attr_xml

  abs_infile_xml_path = import_hwp_dir_path + infile
  EKB_CMN_ATTR_XMLS += abs_infile_xml_path

endforeach

filter_xml_path = import_generic_dir_path + 'sbe_common_attribute.xml'
SBE_CMN_ATTR_FILTER_XML += filter_xml_path

# Additional dependey files for attrtool
attrtool_deps = ['attrdb.py',
                 'attrtank.py',
                 'attrtoolutils.py',
                 'attrmodule.py',
                 '/templates/attribute.C.t',
                 '/templates/attribute.H.t',
                 '/templates/attribute_ids.H.t',
                 '/templates/attribute_macros.H.t',
                 '/templates/attribute_properties.H.t',
                 '/templates/attribute_override.C.t',
                 '/templates/fapi2_chip_ec_feature.H.t',
                 '/templates/attribute_accessors.H.t' ]
attrtool_path = meson.global_source_root() + '/public/src/runtime/tools/attributes/'

foreach depfile : attrtool_deps

  abs_dep_file_path = attrtool_path + depfile
  MESON_SETUP_EXTRA_DEPENDENCIES += abs_dep_file_path

endforeach
