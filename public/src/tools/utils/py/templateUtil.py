#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/tools/utils/py/templateUtil.py $
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

############################################################
# Imports - Imports - Imports - Imports - Imports - Imports
############################################################
#Python Provided
from jinja2 import Template

############################################################
# Function - Functions - Functions - Functions - Functions
############################################################

# Custom Template Wrapper function to make a call to Template
def customTemplate(template : str) -> Template:

    return Template(template, block_start_string = '$$$@@@',
                    block_end_string='@@@$$$',
                    variable_start_string='$@',
                    variable_end_string='@$',
                    line_comment_prefix = '$$$$$')
