# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/odyssey/attributes/meson.build $
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

import_hwp_dir_path = pub_import_dir_path + 'hwp/odyssey/xml/attribute_info/'

infile_attr_xml = [
    'ody_perv_attributes.xml',
    ]

odyssey_attr_xml_paths = []

foreach infile : infile_attr_xml

  abs_infile_xml_path = import_hwp_dir_path + infile
  odyssey_attr_xml_paths += abs_infile_xml_path

endforeach

odyssey_sbe_attr_filter_xml_paths = ''
odyssey_sbe_attr_db_file_name = odyssey_sppe_genfiles_dir_path + '/sbe_attr.db'

run_cmd_result = run_command(attrTool, 'parse', ekb_cmn_attr_xml_paths, odyssey_attr_xml_paths,
    '-fx', ekb_cmn_sbe_attr_filter_xml_paths, odyssey_sbe_attr_filter_xml_paths,
    '-db', odyssey_sbe_attr_db_file_name,
    # we can keep -v options to get the backtrace if it failed.
    '-v')
if run_cmd_result.returncode() != 0
    message(run_cmd_result.stdout())
    error(run_cmd_result.stderr())
endif

run_cmd_result = run_command(attrTool, 'gensrc', odyssey_sbe_attr_db_file_name, odyssey_sppe_genfiles_dir_path,
    # we can keep -v options to get the backtrace if it failed.
    '--vfh', 'odyvirtualattrfunctions.H', '-v')
if run_cmd_result.returncode() != 0
    message(run_cmd_result.stdout())
    error(run_cmd_result.stderr())
endif

# Additional dependey files for attrtool
attrtool_deps = ['attrdb.py',
                 'attrtank.py',
                 'attrtoolutils.py',
                 '/templates/attribute.C.t',
                 '/templates/attribute.H.t',
                 '/templates/attribute_ids.H.t',
                 '/templates/attribute_macros.H.t',
                 '/templates/attribute_properties.H.t',
                 '/templates/fapi2_chip_ec_feature.H.t' ]
attrtool_path = meson.global_source_root() + '/public/src/runtime/tools/attributes/'

foreach depfile : attrtool_deps

  abs_dep_file_path = attrtool_path + depfile
  meson_setup_extra_dependencies += abs_dep_file_path

endforeach
