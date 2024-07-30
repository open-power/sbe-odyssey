# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/test/simics/utils/toolwrappers.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023,2024
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
from sim_commands import *

def run_shell_command(i_cmd, i_isPrintTrace: bool = True):
    if i_isPrintTrace == True:
        print("simics running shell '%s':" %(i_cmd))
    run_command('shell "' + i_cmd + '"')

def parseSbeTrace(i_trace_bin_path:str, i_string_file_path:str, i_out_file_path:str, i_isPrintTrace: bool = True):
    ppe2fsp_path = SIM_lookup_file("ppe2fsp.py")
    if i_isPrintTrace == True:
        print(ppe2fsp_path)
    fsptrace_path = SIM_lookup_file("fsp-trace")
    string_file_path = SIM_lookup_file(i_string_file_path)

    cmd_parse_to_fsp_format = ppe2fsp_path + ' -i ' + i_trace_bin_path + ' -o sbetrace.bin'
    cmd_parse_fsp_trace = fsptrace_path + ' -s ' + string_file_path + ' sbetrace.bin > ' + i_out_file_path
    cmd_print_output = 'cat ' + i_out_file_path

    run_shell_command(cmd_parse_to_fsp_format, i_isPrintTrace)
    run_shell_command(cmd_parse_fsp_trace, i_isPrintTrace)
    if i_isPrintTrace == True:
        run_shell_command(cmd_print_output)

def parseSbeRawDump(i_dump_bin_path:str, i_attr_db_path:str, i_out_file_path:str):
    attrtool_path = SIM_lookup_file("attrtool")
    attr_db_path = SIM_lookup_file(i_attr_db_path)

    #print attr blob using attrtool -> attrtool dumpattr attr.db dump.bin
    cmd_parse_attr_dump = (attrtool_path + " dumpattr " + attr_db_path + " " + i_dump_bin_path +
                " --raw True > " + i_out_file_path)
    cmd_print_output = "cat " + i_out_file_path

    run_shell_command(cmd_parse_attr_dump)
    run_shell_command(cmd_print_output)