# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/test/simics/utils/chipopexecuter.py $
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


"""! @package A class to execute the chipop and parse the response
"""

from ctypes import BigEndianStructure, c_uint32, sizeof

from ffdcparser import FfdcParser

def displayCtype(obj)->None:
    for field in obj._fields_:
        n = str(field[0]).ljust(20)
        d = getattr(obj, field[0])
        h = hex(d).ljust(16)
        d = str(d)
        #ic(n+h+d)

class ChipopRespHeader_t(BigEndianStructure):
    _pack_ = 1
    _fields_ = [
        ("iv_magic",     c_uint32, 16),
        ("iv_cmd_cls",   c_uint32,  8),
        ("iv_command",   c_uint32,  8),
        ("iv_prim_resp", c_uint32, 16),
        ("iv_sec_resp",  c_uint32, 16),
    ]

class FifoDriver(object):
    def __init__(self) -> None:
        pass

    def doFifoSubmit(self, i_request: bytearray) -> bytearray:
        raise NotImplementedError("Not implemented in Base class")

class ChipopExecuter(object):
    def __init__(
            self,
            i_cmd_class : int,
            i_cmd : int) -> None:

        assert 0 < i_cmd_class <= 0xFF, "i_cmd_class is not 1 byte"
        assert 0 < i_cmd < 0x100, "i_cmd is not 1 byte"

        self.cmd_class = i_cmd_class
        self.command = i_cmd
        self.req_data = bytearray()
        self.raw_replay = bytearray()

    def addRequestData(self, i_req_data:'int|bytes') -> None:
        if(type(i_req_data) == int):
            self.req_data.extend(
                i_req_data.to_bytes(4, 'big'))
        else:
            assert (len(i_req_data) % 4 == 0), "len(i_req_data) is not multiple of 4 bytes"
            self.req_data.extend(i_req_data)

    def execute(
            self,
            i_fifo_driver: FifoDriver) -> None:
        '''
            Send the chipop, collect reply, parse reply and do basic sanity checks.
        '''
        self._constructChipopRequest()

        self.raw_replay = i_fifo_driver.doFifoSubmit(self.full_request)

        self._parseReply()

    def _constructChipopRequest(self):
        self.full_request = bytearray()
        chipop_len_in_words = (
            2 +       # 1 word for lenght + 1 word for command
            len(self.req_data) // 4)

        self.full_request.extend(chipop_len_in_words.to_bytes(4, 'big'))
        self.full_request.extend(bytes([0, 0, self.cmd_class, self.command]))
        self.full_request.extend(self.req_data)

    def _parseReply(self) -> None:
        len_in_word_to_magic = int.from_bytes(self.raw_replay[-4:], 'big')
        self.data_returned = self.raw_replay[:-len_in_word_to_magic * 4]

        raw_response = self.raw_replay[- (len_in_word_to_magic * 4) : -4]
        self.resp_hdr = ChipopRespHeader_t.from_buffer(
            raw_response, 0)

        self._sanityCheck()

        self.ffdc = FfdcParser(
            raw_response[sizeof(ChipopRespHeader_t):])

    def checkResult(
            self,
            i_prim_resp: int,
            i_sec_resp: int) -> None:
        '''
        Check whether recieved primary and secondary response as expected or not.
        If not as expected it will raise an exception
        '''

        if(self.resp_hdr.iv_prim_resp != i_prim_resp):
            raise Exception(
                f"Unexpected primary response 0x{self.resp_hdr.iv_prim_resp:02X} Recieved. "
                f"Expected response is 0x{i_prim_resp:02X}")
        if(self.resp_hdr.iv_sec_resp != i_sec_resp):
            raise Exception(
                f"Unexpected secondary response 0x{self.resp_hdr.iv_sec_resp:02X} Recieved. "
                f"Expected response is 0x{i_sec_resp:02X}")

    def _sanityCheck(self) -> None:
        if(
            (self.resp_hdr.iv_magic != 0xC0DE) or
            (self.resp_hdr.iv_cmd_cls != self.cmd_class) or
            (self.resp_hdr.iv_command != self.command)
        ):
            raise Exception("Invalid Reply recieved")


