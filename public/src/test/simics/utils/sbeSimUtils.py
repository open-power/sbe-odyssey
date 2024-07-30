#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/test/simics/utils/sbeSimUtils.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2015,2024
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
import time
import conf
from sim_commands import *
import simTargets
import functools
from enum import Enum

# TODO: Need to enable sbeSimUtils for PST
# JIRA: PFSBE-36 - Change sbeSimUtils as common

waitItrCount = 10000000
cyclesPerIter = 20000

SBE_STATE_CMN_AUTO_BOOT_DONE = 0x3
SBE_STATE_CMN_ISTEP = 0x2
SBE_STATE_CMN_RUNTIME = 0x6

ODY_REG_MSG_CFAM     = 0x2824
ODY_REG_SCRATCH11_CFAM = 0x2E08
ODY_REG_SCRATCH12_CFAM = 0x2E10
ODY_REG_SCRATCH16_CFAM = 0x2E1C

#FIFO1 Constants
US_FIFO_ONE_BASE_ADDR = 0x2400
DS_FIFO_ONE_BASE_ADDR = 0x2440

#FIFO2 Constants
US_FIFO_TWO_BASE_ADDR = 0x2480
DS_FIFO_TWO_BASE_ADDR = 0x24C0

US_FIFO_WRITE  = 0x0000
US_FIFO_STATUS = 0x0004
US_FIFO_EOT    = 0x0008
US_FIFO_RESET  = 0x000C

DS_FIFO_READ   = 0x0000
DS_FIFO_STATUS = 0x0004
DS_FIFO_EOT    = 0x0008

class FIFO_TYPES(Enum):
    SBE_FIFO_BASE       = 0
    SBE_HB_FIFO_BASE    = 1
    SBE_PIPE1_BASE      = 2
    SBE_PIPE2_BASE      = 3
    SBE_PIPE3_BASE      = 4
    SBE_PIPE4_BASE      = 5
    SBE_PIPE5_BASE      = 6
    SBE_PIPE6_BASE      = 7
    SBE_PIPE7_BASE      = 8
    SBE_PIPE8_BASE      = 9


def setIarViaRamming(chip):
    #For Pibmem only image we need to set IAR and IVPR by ramming PPE regs
    print("Setting IAR and IVPR")

    chip.pib.iface.memory_space.write(
        None, 0xD00000, (0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06), False)
    chip.pib.iface.memory_space.write(
        None, 0xD00010, (0x10, 0x00, 0x00, 0x00, 0xFF, 0xF8, 0x00, 0x00), False)
    chip.pib.iface.memory_space.write(
        None, 0xD00020, (0x7c, 0xb0, 0x42, 0xA6, 0xff, 0xf8, 0x00, 0x00), False)
    chip.pib.iface.memory_space.write(
        None, 0xD00010, (0x10, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x01, 0x60), False)
    chip.pib.iface.memory_space.write(
        None, 0xD00020, (0x7c, 0xd0, 0x42, 0xA6, 0xc0, 0x00, 0x01, 0x60), False)
    chip.pib.iface.memory_space.write(
        None, 0xD00020, (0x18, 0xa6, 0x00, 0x00, 0xc0, 0x00, 0x01, 0x60), False)
    chip.pib.iface.memory_space.write(
        None, 0xD00050, (0x00, 0x00, 0x00, 0x00, 0xFF, 0xF8, 0x00, 0x40), False)
    print("Setting IAR and IVPR done. Resuming the PPE from new IAR")
    chip.pib.iface.memory_space.write(
        None, 0xD00000, (0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00), False)

def waitAndExecuteRamming(chip):
    while True:
        SIM_run_command( "run-cycles 1000")
        #( rc, out )  =   quiet_run_command( "run-cycles 1000", output_modes.regular )
        fsxcomp_cbs = chip.lbus_map.iface.memory_space.read(None, (0x00002804), 8, 0x0)
        print("CBS_CS:", fsxcomp_cbs)
        # check for CBS_IDLE state (bit 16:31 == 0x0002)
        if((fsxcomp_cbs[2] == 0x00) and (fsxcomp_cbs[3] == 0x02)):
            break
    setIarViaRamming(chip)


def startCbs(procNr=0, nodeNr=0 ):
    if (SIM_simics_is_running()):
        raise CliError("This command supported only when simics is not running")

    chip = simTargets.odysseys[procNr]

    # Set PG-Good
    chip.fsi2host_mbox.port.vdn_pgood.iface.signal.signal_raise()
    SIM_run_command( "run-cycles 1000")
    # command to read 8 bytes from 0x00002804
    fsxcomp_cbs = chip.lbus_map.iface.memory_space.read(None, (0x00002804), 4, 0x0)
    # updating bit 0 to 1 in the read register
    data = list(fsxcomp_cbs)

    # TODO: add support for bldr context also
    if (simenv.sbe_image_type == 'sppe'):
        #TODO: We need to perform SPI init for SPPE pibmem only image for accessing NOR
        # start cbs with prevent_sbe_start
        data[0] = data[0]|0x90
    else:
        data[0] = data[0]|0x80
    chip.lbus_map.iface.memory_space.write(None, (0x00002804), (*data,), 4)

    if (simenv.sbe_image_type == 'sppe'):
        waitAndExecuteRamming(chip)

def getLbus( node, proc=0):
    #Node is 0 by default
    #TODO: Simics does not have object API for different modules.
    #For now i am calling the zero module.
    # if(node == 0):
    # if(node == 1):
    # if(node == 2):
    # if(node == 3):
    lbus=simTargets.odysseys[0].lbus_map
    return lbus

#Default parameters are for single node, node 0
def writeUsFifo( data, i_fifoType=0, node=0, proc=0):
    """Main test Loop"""
    lbus = getLbus(node, proc)
    loopCount = len(data)//4
    address = getUsFifoDataAddrToWrite(i_fifoType)  #Address: 0x2400, 0x2480
    for i in list(range(loopCount)):
        idx = i * 4
        writeEntry(lbus, address, i_fifoType,\
                   (data[idx], data[idx+1], data[idx+2], data[idx+3]), node, proc )

#Default parameters are for single node, node 0
def readDsFifo(data,  i_fifoType=0, node=0, proc=0):
    """Main test Loop"""
    lbus = getLbus(node, proc)
    loopCount = len(data)//4
    read_ds_addr = getDsFifoDataAddrToRead(i_fifoType)  #Address: 0x2440, 0x24C0
    for i in list(range(loopCount)):
        idx = i * 4
        checkEqual(readEntry(lbus, read_ds_addr, 4, i_fifoType, node, proc), (data[idx],\
                                         data[idx+1], data[idx+2], data[idx+3]))

def readDsFifoTillEot(data, i_fifoType=0, node=0, proc=0):
    fifodata = []

    while not checkIfEot(i_fifoType):
        fifoworddata = readDsEntryReturnVal(i_fifoType, node, proc)
        fifodata.extend( list(fifoworddata) )

    checkEqual( fifodata[0:len(data)], data)
    return fifodata

#Default parameters are for single node, node 0
def writeEot(i_fifoType=0, node=0,  proc=0):
    lbus = getLbus(node, proc)
    eot_addr = getUsFiFoEotAddrToWrite(i_fifoType)  #Address: 0x2408, 0x2448
    write(lbus, eot_addr, (0, 0, 0, 1) )

def write(obj, address, value ):
    """ Write to memory space """
    iface = SIM_get_interface(obj, "memory_space")
    iface.write(None, address, value, 0x0)

#Default parameters are for single node, node 0
def readEot(i_fifoType=0, node=0, proc=0):
    """ Read from memory space """

    """ At the end of synchronous command processing, all threads stack usage
        is checked. Because of that, delay is expected. So, if readEot is called
        immediately after the chip-op, it will timeout. To avoid this, runCycles
        has been added.
    """
    runCycles(1000000)

    lbus = getLbus(node, proc)
    eot_addr = getDsFifoStatusAddrToRead(i_fifoType) #Address: 0x2444, 0x2484
    read_addr = getDsFifoDataAddrToRead(i_fifoType)  #Address: 0x2440, 0x2480
    status = read(lbus, eot_addr, 4)
    checkEqual( (status[3] & 0x80), 0x80 )
    read(lbus, read_addr, 4)

#Default parameters are for single node, node 0
def checkIfEot(i_fifoType=0, node=0, proc=0):
    """ Read from memory space """
    lbus = getLbus(node, proc)
    eot_addr = getDsFifoStatusAddrToRead(i_fifoType) #Address: 0x2444, 0x2484
    read_addr = getDsFifoDataAddrToRead(i_fifoType)  #Address: 0x2440, 0x2480
    status = read(lbus, eot_addr, 4)
    if( (status[3] & 0x80 > 0x80) - (status[3] & 0x80 < 0x80) ):
        return False
    read(lbus, read_addr, 4)
    return True

#Default parameters are for single node, node 0
def resetFifo(i_fifoType=0, node=0, proc=0):
    lbus = getLbus(node, proc)
    reset_addr = getResetAddr(i_fifoType) #Address:0x240C, 0x248C
    write(lbus, reset_addr, (0, 0, 0, 1))
    return

#Default parameters are for single node, node 0
def readUsFifoStatus(i_fifoType=0, node=0, proc=0):
    lbus = getLbus(node, proc)
    read_addr = getUsFifoStatusAddrToRead(i_fifoType) #Address:0x2404, 0x2484
    status = read(lbus, read_addr, 4)
    return status

#Default parameters are for single node, node 0
def readDsFifoStatus(i_fifoType=0, node=0, proc=0):
    lbus = getLbus(node, proc)
    read_addr = getDsFifoStatusAddrToRead(i_fifoType) #Address:0x2444, 0x24C4
    status = read(lbus, read_addr, 4)
    return status

def waitTillFifoEmpty(func, i_fifoType=0, node=0, proc=0):
    count = 0
    loop = True
    while(loop is True):
        status = func(i_fifoType, node, proc)
        if(status[1] == 0x10):
            loop = False
            break
        else:
            count = count + 1
            runCycles(200000)
            if(count > 10):
                raise Exception('Timed out waiting for FIFO to get flushed')


def waitTillUsFifoEmpty(i_fifoType=0, node=0, proc=0):
    try:
        waitTillFifoEmpty(readUsFifoStatus, i_fifoType, node, proc)
    except:
        raise Exception('US FIFO did not get empty')


def waitTillDsFifoEmpty(i_fifoType=0, node=0, proc=0):
    try:
        waitTillFifoEmpty(readDsFifoStatus, i_fifoType, node, proc)
    except:
        raise Exception('DS FIFO did not get empty')


# This function will only read the entry but will not compare it
# with anything. This can be used to flush out enteries.
#Default parameters are for single node, node 0
def readDsEntry(entryCount, i_fifoType=0, node=0, proc=0):
    lbus = getLbus(node, proc)
    read_addr = getDsFifoDataAddrToRead(i_fifoType) #Address:0x2440, 0x24C0
    for i in list(range(entryCount)):
        readEntry(lbus, read_addr, 4, i_fifoType, node, proc)

#Default parameters are for single node, node 0
def writeEntry(obj, address, i_fifoType, value, node=0, proc=0):
    lbus = getLbus(node, proc)
    loop = 1
    count = 0
    status_addr = getUsFifoStatusAddrToRead(i_fifoType)
    while( loop ):
        status = read(lbus, status_addr, 4)  # Address 0x2404, 0x2484

        if( status[2] & 0x02):
            count = count + 1
            runCycles(cyclesPerIter)
            # This will cause  test to fail
            if(count > waitItrCount):
                raise Exception('Timeout. FIFO FULL');
        else:
            # write entry
            write(obj, address, value)
            loop = 0

    return value

#Default parameters are for single node, node 0
def readDsEntryReturnVal(i_fifoType, node=0, proc=0):
    lbus = getLbus(node, proc)
    read_addr = getDsFifoDataAddrToRead(i_fifoType) #Address:0x2440, 0x24C0
    runCycles(200000)
    data = readEntry(lbus, read_addr, 4, i_fifoType, node, proc)
    return data

#Default parameters are for single node, node 0
def readEntry(obj, address, size, i_fifoType=0, node=0, proc=0):
    """ Read from memory space """

    """ At the end of synchronous command processing, all threads stack usage
        is checked. Because of that, delay is expected. So, if readEntry is called
        immediately after the chip-op, it will timeout. To avoid this, runCycles
        has been added.
    """
    runCycles(1000000)

    lbus = getLbus(node, proc)
    loop = 1
    count = 0
    value = (0,0,0,0)
    read_addr = getDsFifoStatusAddrToRead(i_fifoType)
    while( loop ):
        status = read(lbus, read_addr, 4)  # Address 0x2444, 0x24C4: Downstream Fifo Status

        if( status[1] & 0x0F):
            # read entry
            value = read(lbus, address, size)
            loop = 0
        else:
            count = count + 1
            runCycles(cyclesPerIter)
            # This will cause  test to fail
            if(count > waitItrCount):
                raise Exception('Timeout. Empty FIFO');

    return value

def extractHWPFFDC(i_fifoType=0, dumpToFile = False, readData = None, node=0, proc=0):
    '''Header extraction'''
    if(readData != None):
        data = readData[:4]
        readData = readData[4:]
    else:
        data = readDsEntryReturnVal(i_fifoType, node, proc)
    magicBytes = ((data[0] << 8) | data[1])
    if (magicBytes == 0xFBAD) :
        print ("\nMagic Bytes Match")
    else :
        raise Exception('data mistmach')
    packLen = ((data[2] << 8) | data[3])
    print ("\nFFDC package length = " + str(packLen))
    # extract Sequence ID, Command class and command
    if(readData != None):
        data = readData[:4]
        readData = readData[4:]
    else:
        data = readDsEntryReturnVal(i_fifoType, node, proc)
    seqId = ((data[0] << 24) | (data[1] << 16))
    cmdClass = data[2]
    cmd = data[3]
    print ("\n SeqId ["+str(seqId)+"] CmdClass ["+str(cmdClass)+"] Cmd ["+str(cmd)+"]")

    if(readData != None):
        data = readData[:4]
        readData = readData[4:]
    else:
        data = readDsEntryReturnVal(i_fifoType, node, proc)
    fapiRc = ((data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3])
    print ("\nFAPI rc = " + str(hex(fapiRc)))

    if(dumpToFile):
        myBin = open('hwp_ffdc.bin', 'wb')
        print ("\nwriting "+'hwp_ffdc.bin')
    for i in list(range(0, packLen-3)):
        if(readData != None):
            data = readData[:4]
            readData = readData[4:]
        else:
            data = readDsEntryReturnVal(i_fifoType, node, proc)
        if(dumpToFile):
            myBin.write(bytearray(data))
    if(dumpToFile):
        print("write to a file Done")
        myBin.close()
    return readData

def read(obj, address, size):
    """ Read from memory space """
    iface = SIM_get_interface(obj, "memory_space")
    value = iface.read(None, address, size, 0x0)
    return value

def runCycles( cycles ):
    if (not SIM_simics_is_running()):
        # throw exception if SBE is already halted.
        # otherwise this funtion will be called again and again untill timeout occured in caller.
        # and this will cause simics to hang for a long time.

        # xsr may indicate it halted if cbs is not done yet.
        # to avoid this situation, confirm IAR != 0.
        reg_values = simTargets.odysseys[0].sppe.cli_cmds.pregs()
        xsr_val = 0
        iar_val = 0
        for reg in reg_values:
            if(reg[0] == 'xsr'):
                xsr_val = reg[1]
            if(reg[0] == 'iar'):
                iar_val = reg[1]
        if((iar_val != 0) and (xsr_val & 0x80000000)):
            raise Exception("SBE HALTED")
        syscmd   =   "run-cycles %d"%(cycles)
        ( rc, out )  =   quiet_run_command( syscmd, output_modes.regular )
        if ( rc ):
            print("simics ERROR running %s: %d "%( syscmd, rc ))

def checkEqual( data, expdata ):
    """ Throw exception if data is not equal """
    if((data > expdata) - (data < expdata)):
        print("Eqality check failed")
        print("Data:", data)
        print("Expected Data", expdata)
        raise Exception('data mistmach')

def collectFFDC():
        simics.SIM_run_command('poz-sbe-regs 0')
        simics.SIM_run_command('poz-sbe-trace 0')
        #simics.SIM_run_command('sbe-stack 0')
        #simics.SIM_run_command('sbe-regffdc 0')
        print(simTargets.odysseys[0].sbe_fifo.upstream_fifo)
        print(simTargets.odysseys[0].sbe_fifo.downstream_fifo)

def getUsFifoDataAddrToWrite(i_fifoType):
    if i_fifoType == 0:
        address = US_FIFO_ONE_BASE_ADDR + US_FIFO_WRITE
    if i_fifoType == 1:
        address = US_FIFO_TWO_BASE_ADDR + US_FIFO_WRITE
    return address

def getDsFifoDataAddrToRead(i_fifoType):
    if i_fifoType == 0:
        address = DS_FIFO_ONE_BASE_ADDR + DS_FIFO_READ
    if i_fifoType == 1:
        address = DS_FIFO_TWO_BASE_ADDR + DS_FIFO_READ
    return address

def getUsFiFoEotAddrToWrite(i_fifoType):
    if i_fifoType == 0:
        address = US_FIFO_ONE_BASE_ADDR + US_FIFO_EOT
    if i_fifoType == 1:
        address = US_FIFO_TWO_BASE_ADDR + US_FIFO_EOT
    return address

def getResetAddr(i_fifoType):
    if i_fifoType == 0:
        address = US_FIFO_ONE_BASE_ADDR + US_FIFO_RESET
    if i_fifoType == 1:
        address = US_FIFO_TWO_BASE_ADDR + US_FIFO_RESET
    return address

def getUsFifoStatusAddrToRead(i_fifoType):
    if i_fifoType == 0:
        address = US_FIFO_ONE_BASE_ADDR + US_FIFO_STATUS
    if i_fifoType == 1:
        address =  US_FIFO_TWO_BASE_ADDR + US_FIFO_STATUS
    return address

def getDsFifoStatusAddrToRead(i_fifoType):
    if i_fifoType == 0:
        address = DS_FIFO_ONE_BASE_ADDR + DS_FIFO_STATUS
    if i_fifoType == 1:
        address = DS_FIFO_TWO_BASE_ADDR + DS_FIFO_STATUS
    return address

def checkIfBootedAndReachedState(lbus, sbestate, cycles=1000000000):
    while True:
        runCycles( cycles )
        msg_reg = read(lbus, ODY_REG_MSG_CFAM, 4)
        print("SBE MSG Value = 0x" + bytes(msg_reg).hex())
        err_reg = read(lbus, ODY_REG_SCRATCH12_CFAM, 4)
        print("Error Reg Value = 0x" + bytes(err_reg).hex())
        curr_sbestate = (msg_reg[1]&0xF0)>>4
        if ((msg_reg[0]&0x80)==0x80) and (curr_sbestate ==sbestate):
            print("SBE is booted at {}, continue 0x{}".format(sbestate,bytes(msg_reg).hex()))
            break
        elif (msg_reg[0]&0x80)!=0x80:
            print("SBE is still not Booted")
        elif curr_sbestate!= sbestate:
            print("SBE is Booted but not at sbestate{} ".format(sbestate))
        else:
            print("Not a valid msg register configuration 0x"+ bytes(msg_reg).hex())
            raise Exception("Invalid Msg register")
        ( regs, out )  =   quiet_run_command( simTargets.odysseys[0].name+".sppe.pregs", output_modes.regular )
        # XSR will be at 36th index in the list
        print("SBE-" + regs[36][0] + " Value = ", hex(regs[36][1]))
        # IAR will be at 22nd index
        print("SBE-" + regs[22][0] + " Value = ", hex(regs[22][1]))
        # fail if "IAR != 0" (which means sbe started), and XSR indicates halt state.
        if((regs[22][1] != 0) and (regs[36][1] & 0x80000000)):
            print("SBE is halted. Booting failed...")
            raise Exception("SBE Halted")
        print("")

def clearMsgReg():
    cmd_write = simTargets.odysseys[0].name+".lbus_map.write address= 0x2824 value =0x00000000 size = 4"
    quiet_run_command( cmd_write, output_modes.regular )

def isAsyncBitSet(lbus):
    msg_reg = read(lbus, ODY_REG_MSG_CFAM, 4)
    # bit 1 in msg register is mapped to async ffdc bit
    print("msg register : ", [hex(i) for i in msg_reg])
    return (msg_reg[0]&0xF0) & 0x40

def clearLfr():
    cmd_write = simTargets.odysseys[0].name+".sppe_mibo.write address = 0xc0002040 value=0x0000000000000000 size =8 -b"
    quiet_run_command(cmd_write, output_modes.regular)

def setMbx11Valid(lbus):
    # set bit 10 (scratch reg 11 to set)
    mbx16 = list(read(lbus, ODY_REG_SCRATCH16_CFAM, 4))
    mbx16[1] = mbx16[1] | 0x20
    mbx16 = tuple(mbx16)
    write(lbus, ODY_REG_SCRATCH16_CFAM, mbx16)

def getBootSide():
    cmd_read = simTargets.odysseys[0].name+".sppe_mibo.read address = 0xc0002040 size = 8"
    ( readValue, out ) = quiet_run_command( cmd_read, output_modes.regular )
    print("\n 0xC0002040 LFR Local Register Value Before HReset: " + str(hex(readValue)))

    ## check for boot selection bits 12 & 13
    value = readValue & 0x000C000000000000
    value = value >> 0x32
    return value

def getSymbolInfo(i_sym_name: str) -> 'tuple[int, int]':
    '''
    Search the provided symbol in odyssey runtime symbol file,
    and return a tuple containing address and size.
    If the symbol is a pointer symbol then size will be 0.
    '''
    filename = 'odyssey_sppe_DD1.syms'
    fullname = SIM_lookup_file(filename)

    with open(fullname, 'r') as sym_file_io:
        for line in sym_file_io:
            words = line.split()
            # it can have 3 or 4 words.
            #   and the last word will be symbol name

            if(words[-1] == i_sym_name):
                address = int(words[0], 16)
                if( len( words ) == 4 ):
                    size = int(words[1], 16)
                else:
                    size = 0

                return (address, size)

    raise Exception(f"Symbol {i_sym_name} not found in {fullname}")
