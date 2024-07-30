# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/tools/simics/pozmsgregparser.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2024
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

import simTargets
from tabulate import tabulate
from ctypes import Structure, c_uint32, Union
from sim_commands import *

# ref: public/src/common/core/mbxscratch.H : struct messagingReg_t
class MsgRegisterField(Structure):
    _fields_ = [
        ("iv_progressCode", c_uint32, 6),
        ("iv_reserved2",    c_uint32, 4),
        ("iv_minorStep",    c_uint32, 6),
        ("iv_majorStep",    c_uint32, 4),
        ("iv_currState",    c_uint32, 4),
        ("iv_prevState",    c_uint32, 4),
        ("iv_currImage",    c_uint32, 1),
        ("iv_reserved1",    c_uint32, 1),
        ("iv_asyncFFDC",    c_uint32, 1),
        ("iv_sbeBooted",    c_uint32, 1),
    ]


class MsgRegister(Union):
    _fields_ = [
        ("fields", MsgRegisterField),
        ("value", c_uint32),
    ]

# ref: public/src/runtime/common/core/sbestates.H : enum sbeState
SBE_STATES = {
    0x0 : 'SBE_STATE_CMN_UNKNOWN',
    0x1 : 'SBE_STATE_CMN_IPLING',
    0x2 : 'SBE_STATE_CMN_ISTEP',
    0x3 : 'SBE_STATE_CMN_RUNTIME',
    0x4 : 'SBE_STATE_CMN_DUMP',
    0x5 : 'SBE_STATE_CMN_FAILURE',
    0xA : 'SBE_STATE_CMN_HRESET',
}

def parseSbeState(i_state_val:int)->str:
    return f"{SBE_STATES[i_state_val]} ({hex(i_state_val)})"

def parseMsgRegister ( procNr=0, nodeNr=0, tsbeInstance=0 ):
    msg_reg_val = MsgRegister()
    msg_reg_val.value = simTargets.odysseys[procNr].pib.cli_cmds.read(address=0x500090, size=4)

    msg_reg_table = []
    msg_reg_table.append(["MSG_REG",         hex(msg_reg_val.value   )])

    msg_reg_table.append(["", ""])

    msg_reg_table.append(["iv_sbeBooted",    hex(msg_reg_val.fields.iv_sbeBooted   )])
    msg_reg_table.append(["iv_asyncFFDC",    hex(msg_reg_val.fields.iv_asyncFFDC   )])
    msg_reg_table.append(["iv_reserved1",    hex(msg_reg_val.fields.iv_reserved1   )])
    msg_reg_table.append(["iv_currImage",    hex(msg_reg_val.fields.iv_currImage   )])

    msg_reg_table.append(["iv_prevState",    parseSbeState(msg_reg_val.fields.iv_prevState)])
    msg_reg_table.append(["iv_currState",    parseSbeState(msg_reg_val.fields.iv_currState)])

    msg_reg_table.append(["iv_majorStep",    hex(msg_reg_val.fields.iv_majorStep   )])
    msg_reg_table.append(["iv_minorStep",    hex(msg_reg_val.fields.iv_minorStep   )])
    msg_reg_table.append(["iv_reserved2",    hex(msg_reg_val.fields.iv_reserved2   )])
    msg_reg_table.append(["iv_progressCode", hex(msg_reg_val.fields.iv_progressCode)])

    print(tabulate(msg_reg_table, tablefmt='pretty', ))


if __name__ == "__main__":
    parseMsgRegister(0)