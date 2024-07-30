# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/test/simics/utils/sbePipeUtils.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2024
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
import sbeSimUtils
from chipopexecuter import FifoDriver
from sim_commands import *

# @TODO via PFSBE-176: Clean up. We need only base address.
#       Remaining can be derived
PIPE_PAIR_4_5 = {
    'enque_base_addr'     : 0xB0510,
    'dequeue_base_addr'   : 0xB0600,
    'enqueue_eot_addr'    : 0xB0512,
    'dequeue_status_addr' : 0xB0601,
    'dequeue_ackeot_addr' : 0xB0605,
    'config_pibctrl'      : 0x2EE20000,
    'config_pibaccess'    : 0xCECE0000,
    'comment'             : 'dequeing from pipe 6 if enqueue in pipe 5'}

def configurePipePair(i_pipePairDict):

    # Clearing controller id and controller access registers before configuring
    address = 0xB0125
    data0 = 0xFFFFFFFFFFFFFFFF
    sbeSimUtils.fsi2pib_write(address, data0)

    address = 0xB0122
    data1 = 0xFFFFFFFFFFFFFFFF
    sbeSimUtils.fsi2pib_write(address, data1)

    address = 0xB0124
    data0 = i_pipePairDict['config_pibctrl']
    sbeSimUtils.fsi2pib_write(address, data0)

    address = 0xB0121
    data1 = i_pipePairDict['config_pibaccess']
    sbeSimUtils.fsi2pib_write(address, data1)


class SimPipePairDriver(FifoDriver):
    def __init__(
            self,
            i_pipePairDict: dict,
            i_cycles_to_run: int = 0,
            i_exit_after_eot: bool = False) -> None:
        '''
            Constructor

            @param i_cycles_to_run  Number simics cycles to run before collecting the reply
            @param i_exit_after_eot Whether need to exit the script immediately after sending
                                    the chipop or not.
                                    It will be required, if we want to debug
                                    a crash in the chipop, since simics will not break at breakpoint
                                    if runCycles() is called.
                                    In this case execute() can be called with this parameter true,
                                    and then use simics CLI command 'run' which will break when a
                                    breakpoint hit.
        '''

        super().__init__()

        self.enqueu_addr = i_pipePairDict['enque_base_addr']
        self.dequeu_addr = i_pipePairDict['dequeue_base_addr']
        self.enqueue_eot_addr = i_pipePairDict['enqueue_eot_addr']
        self.dequeue_stat_addr = i_pipePairDict['dequeue_status_addr']
        self.dequeue_ackeot_addr = i_pipePairDict['dequeue_ackeot_addr']

        self.cycles_to_run_after_request = i_cycles_to_run
        self.exit_after_eot = i_exit_after_eot


    def doFifoSubmit(self, i_request: bytearray) -> bytearray:
        self._sendToUsFifo(i_request)
        self._writeEot(self.enqueue_eot_addr)

        if(self.exit_after_eot):
            raise Exception("User request to stop python script")

        sbeSimUtils.runCycles(self.cycles_to_run_after_request)

        return self._recieveReply()

    def _recieveReply(self) -> bytearray:
        raw_replay = bytearray()

        while True:
            data, eot = self._dequeuWordAndCheckEot()
            if(eot):
                break

            raw_replay.extend(int.to_bytes(data, 4, 'big'))

        self._writeEot(self.dequeue_ackeot_addr)
        return raw_replay

    def _dequeuWordAndCheckEot(self) -> tuple[int,bool]:
        # TODO PFSBE-1006 : wait untill data available pipe-queue
        data = self._readWordReg(self.dequeu_addr)
        return (data, self._checkIfEot())

    def _checkIfEot(self) -> None:
        status = self._readWordReg(self.dequeue_stat_addr)

        if(status & (1 << (31 - 8))):
            # dequeue the eot word
            self._readWordReg(self.dequeu_addr)
            return True
        else:
            return False

    def _writeWordReg(self, addr, data) -> None:
        sbeSimUtils.fsi2pib_write(addr, data << 32)

    def _readWordReg(self, addr) -> int:
        return sbeSimUtils.fsi2pib_read(addr) >> 32

    # write eot or ack-eot
    def _writeEot(self, addr) -> None:
        eot_data = 0x1
        self._writeWordReg(addr, eot_data)

    def _sendToUsFifo(self, i_data: int) -> None:
        if((len(i_data) % 4) != 0):
            raise Exception('Invalid request length')

        word_aligned_offset = 0
        while True:
            # TODO PFSBE-1006 : wait untill pipe-queue is empty

            slice = i_data[word_aligned_offset:(word_aligned_offset + 4)]
            data = int.from_bytes(slice, 'big')

            self._writeWordReg(self.enqueu_addr, data)

            word_aligned_offset += 4
            if(word_aligned_offset >= len(i_data)):
                break