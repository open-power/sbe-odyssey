# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/odyssey/attributes/meson.build $
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

import_hwp_dir_path = pub_import_dir_path + 'hwp/odyssey/xml/attribute_info/'

infile_attr_xml = [
    'ody_omi_attributes.xml',
    'ody_perv_attributes.xml',
    ]

EKB_ODY_ATTR_XMLS = []

foreach infile : infile_attr_xml

  abs_infile_xml_path = import_hwp_dir_path + infile
  EKB_ODY_ATTR_XMLS += abs_infile_xml_path

endforeach

SBE_ODY_ATTR_FILTER_XML = import_hwp_dir_path + 'sbe_ody_attribute.xml'

import_memory_hwp_dir_path = pub_import_dir_path + 'memory/generic/xml/attribute_info/'

infile_mem_attr_xml = [
    'generic_dq_attributes.xml',
    'generic_memory_attributes.xml',
    'generic_memory_ddr5_mr_attributes.xml',
    'generic_memory_ddr5_eff_attributes.xml',
    'generic_memory_eff_attributes.xml',
    'generic_memory_mrw_attributes.xml',
    'generic_memory_si_attributes.xml',
    'generic_memory_thermal_sensor_attributes.xml',
    'ocmb_power_thermal.xml',
    ]

foreach inmemfile : infile_mem_attr_xml
  abs_in_memfile_xml_path = import_memory_hwp_dir_path + inmemfile
  EKB_ODY_ATTR_XMLS += abs_in_memfile_xml_path
endforeach

import_ody_memory_hwp_dir_path = pub_import_dir_path + 'memory/odyssey/xml/attribute_info/'

infile_ody_mem_attr_xml = [
    'ody_attributes.xml',
    'ody_omi_init.xml',
    'ody_sppefw_attributes.xml',
    ]

foreach inodymemfile : infile_ody_mem_attr_xml
  abs_in_ody_memfile_xml_path = import_ody_memory_hwp_dir_path + inodymemfile
  EKB_ODY_ATTR_XMLS += abs_in_ody_memfile_xml_path
endforeach

