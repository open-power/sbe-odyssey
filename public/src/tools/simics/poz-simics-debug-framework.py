# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/tools/simics/poz-simics-debug-framework.py $
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

# Add all sbe specific script paths
for path in simenv.paths_to_add.split(','):
    sys.path.append(path)

print(f"PYTHON path: {sys.path}")

import os
import os.path
import simTargets
from tabulate import tabulate
from struct import Struct
from collections import namedtuple
from sim_commands import *
import toolwrappers
import sbeIstepAuto
import pozmsgregparser

# TODO: Need to enable sbeSimUtils for PST
# Jira item: PFSBE-36 - Simics sbeSimUtils common support for pst and odyssey
if(simenv.sbe_project_type == 'odyssey' or
   simenv.sbe_project_type == 'odysseylab'):
    import sbeSimUtils


def hexFormat(val:int, nibbles:int=8) -> str:
    return "{0:#0{1}X}".format(val, nibbles + 2).replace("X", "x")

def get_dd_level(procNr = 0, nodeNr = 0):
    return "DD1"

def register_sbe_debug_framework_tools():
    # Create command hook.
    new_command("poz-sbe-istep",istep_func,
                  args = [arg(str_t, "Major/start istep"), arg(str_t, "Minor/end istep"), arg(int_t, "procNr", "?", 0), arg(int_t, "nodeNr", "?", 0)],
                  alias = "istep-poz",
                  type = ["sbe-commands"],
                  short = "Runs the debug framework for istep ",
                  doc = "")

    # TODO: we have to add chip type also, when we need to support SPPE, HUB and TAP etc.
    new_command("poz-start-cbs", startCbs,
                args = [arg(int_t, "procNr"), arg(int_t, "arg", "?", 0)],
                alias = "cbs-poz",
                type = ["sbe-commands"],
                short = "Runs the debug framework for staring cbs. Simics need to be in running state ",
                doc = "")

    new_command("poz-sbe-trace", collectTrace,
                args = [arg(int_t, "procNr"), arg(int_t, "arg", "?", 0)],
                alias = "strace-poz",
                type = ["sbe-commands"],
                short = "Runs the debug framework for trace ",
                doc = "")

    new_command("poz-sbe-regs", printRegisters,
                args = [arg(int_t, "procNr"), arg(int_t, "arg", "?", 0)],
                alias = "sregs-poz",
                type = ["sbe-commands"],
                short = "Print the sbe registers ",
                doc = "")

    new_command("poz-msg-reg", pozmsgregparser.parseMsgRegister,
                args = [arg(int_t, "procNr"), arg(int_t, "arg", "?", 0)],
                alias = "msgreg-poz",
                type = ["sbe-commands"],
                short = "Read and parse the sbe message register ",
                doc = "")

    # TODO: Need to enable sbeSimUtils PST
    new_command("poz-sbe-fifo-write", writeToFifo,
                args = [arg(int_t, "procNr"), arg(int_t, "arg", "?", 0), arg(int_t, "word-packets", "+")],
                alias = "fifowrite-poz",
                type = ["sbe-commands"],
                short = "Write to fifo and send EOT at the end",
                doc = "")
    print("SBE Debug Framework: Registered tool:", "poz-sbe-istep")
    print("SBE Debug Framework: Registered tool:", "poz-start-cbs")
    print("SBE Debug Framework: Registered tool:", "poz-sbe-trace")
    print("SBE Debug Framework: Registered tool:", "poz-sbe-regs")
    print("SBE Debug Framework: Registered tool:", "poz-sbe-fifo-write")

def istep_func ( majorIstep, minorIstep, procNr=0, nodeNr=0):
    try:
      sbeIstepAuto.sbe_istep_func(majorIstep, minorIstep, procNr, nodeNr)
    except Exception as e:
      print("istep_func", e)
      raise e

def startSbe(procNr, nodeNr=0 ):
    sbeSimUtils.startCbs(procNr, nodeNr)

def setIarViaRamming(chip):
    #For Pibmem only image we need to set IAR and IVPR by ramming PPE regs
    print("Setting IAR and IVPR")

    if(simenv.sbe_project_type == "pst"):
        if(simenv.sbe_image_type == "hsbe"):
            chip.pib.iface.memory_space.write(
                None, 0xE00000, (0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06), False)
            chip.pib.iface.memory_space.write(
                None, 0xE00010, (0x10, 0x00, 0x00, 0x00, 0xFF, 0xF8, 0x00, 0x00), False)
            chip.pib.iface.memory_space.write(
                None, 0xE00020, (0x7c, 0xb0, 0x42, 0xA6, 0xff, 0xf8, 0x00, 0x00), False)
            chip.pib.iface.memory_space.write(
                None, 0xE00010, (0x10, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x01, 0x60), False)
            chip.pib.iface.memory_space.write(
                None, 0xE00020, (0x7c, 0xd0, 0x42, 0xA6, 0xc0, 0x00, 0x01, 0x60), False)
            chip.pib.iface.memory_space.write(
                None, 0xE00020, (0x18, 0xa6, 0x00, 0x00, 0xc0, 0x00, 0x01, 0x60), False)
            chip.pib.iface.memory_space.write(
                None, 0xE00050, (0x00, 0x00, 0x00, 0x00, 0xFF, 0xF8, 0x00, 0x40), False)
            print("Setting IAR and IVPR done. Resuming the PPE from new IAR")
            chip.pib.iface.memory_space.write(
                None, 0xE00000, (0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00), False)
        elif(simenv.sbe_image_type == "tsbe"):
            chip.pib.iface.memory_space.write(
                None, 0xE00000, (0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06), False)
            chip.pib.iface.memory_space.write(
                None, 0xE00010, (0x10, 0x00, 0x00, 0x00, 0xFF, 0xFA, 0x00, 0x00), False)
            chip.pib.iface.memory_space.write(
                None, 0xE00020, (0x7c, 0xb0, 0x42, 0xA6, 0xff, 0xfa, 0x00, 0x00), False)
            chip.pib.iface.memory_space.write(
                None, 0xE00010, (0x10, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x01, 0x60), False)
            chip.pib.iface.memory_space.write(
                None, 0xE00020, (0x7c, 0xd0, 0x42, 0xA6, 0xc0, 0x00, 0x01, 0x60), False)
            chip.pib.iface.memory_space.write(
                None, 0xE00020, (0x18, 0xa6, 0x00, 0x00, 0xc0, 0x00, 0x01, 0x60), False)
            chip.pib.iface.memory_space.write(
                None, 0xE00050, (0x00, 0x00, 0x00, 0x00, 0xFF, 0xFA, 0x00, 0x40), False)
            print("Setting IAR and IVPR done. Resuming the PPE from new IAR")
            chip.pib.iface.memory_space.write(
                None, 0xE00000, (0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00), False)
        elif(simenv.sbe_image_type == "sppe"):
            chip.pib.iface.memory_space.write(
                None, 0xD00000, (0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06), False)
            chip.pib.iface.memory_space.write(
                None, 0xD00010, (0x10, 0x00, 0x00, 0x00, 0xFF, 0xE8, 0x00, 0x00), False)
            chip.pib.iface.memory_space.write(
                None, 0xD00020, (0x7c, 0xb0, 0x42, 0xA6, 0xff, 0xe8, 0x00, 0x00), False)
            chip.pib.iface.memory_space.write(
                None, 0xD00010, (0x10, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x01, 0x60), False)
            chip.pib.iface.memory_space.write(
                None, 0xD00020, (0x7c, 0xd0, 0x42, 0xA6, 0xc0, 0x00, 0x01, 0x60), False)
            chip.pib.iface.memory_space.write(
                None, 0xD00020, (0x18, 0xa6, 0x00, 0x00, 0xc0, 0x00, 0x01, 0x60), False)
            chip.pib.iface.memory_space.write(
                None, 0xD00050, (0x00, 0x00, 0x00, 0x00, 0xFF, 0xE8, 0x00, 0x40), False)
            print("Setting IAR and IVPR done. Resuming the PPE from new IAR")
            chip.pib.iface.memory_space.write(
                None, 0xD00000, (0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00), False)
        else:
            print("platform type not supported for Simics standalone run. \
                  selected platform: ",simenv.sbe_project_type," Image type: ", simenv.sbe_image_type)
    else:
        print("platform type not supported for Simics standalone run. selected platform: ", simenv.sbe_project_type)

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

def startCbs(procNr, nodeNr=0, tsbeInstance=0 ):
    if (SIM_simics_is_running()):
        raise CliError("This command supported only when simics is not running")

    # Work-around logic to odyssey start-cbs
    if(simenv.sbe_project_type == "odyssey" or
       simenv.sbe_project_type == "odysseylab"):
        sbeSimUtils.startCbs(procNr, nodeNr)

    # TODO: need to moved to common file
    #      Jira item: PFSBE-36
    elif(simenv.sbe_project_type == "pst"):
        chip = None
        if(simenv.sbe_image_type == "tsbe"):
            chip = conf.backplane0.mcm[0].tap[tsbeInstance]
        else:
            chip = conf.backplane0.mcm[0].spinal

        # Set PG-Good
        chip.fsi2host_mbox.port.vdn_pgood.iface.signal.signal_raise()
        ( rc, out )  =   quiet_run_command( "run-cycles 1000", output_modes.regular )
        # command to read 4 bytes from 0x00002804
        fsxcomp_cbs = chip.lbus_map.iface.memory_space.read(None, (0x00002804), 4, 0x0)
        # updating bit 0 to 1 in the read register
        data = list(fsxcomp_cbs)
        data[0] = data[0]|0x80
        chip.lbus_map.iface.memory_space.write(None, (0x00002804), (*data,), 4)

        if (simenv.sbe_image_type == 'sppe' or
            simenv.sbe_image_type == 'tsbe' or
            simenv.sbe_image_type == 'hsbe'):
            waitAndExecuteRamming(chip)

    else:
        print("platform type not supported for Simics standalone run. selected platform: ", simenv.sbe_project_type)

#----------------------------------------------------------------
# memory access helpers
#----------------------------------------------------------------

odysseyMemoryMap = [
    (0x00000000,  0x80000000, None), # TODO how to pib?
    (0xFFF70000,  0xFFF80000, lambda ody: ody.srom_image.iface.image),
    (0xFFF80000, 0x100000000, lambda ody: ody.sram_image.iface.image),
]

pstSpinalMemoryMap = [
    (0x00000000,  0x80000000, None), # TODO how to pib?
    (0xFFE70000,  0xFFE80000, lambda spnl: spnl.srom_image.iface.image),
    (0xFFE80000, 0x100000000, lambda spnl: spnl.sram_image.iface.image),
    (0xFFF80000, 0x100000000, lambda spnl: spnl.pibmem_img.iface.image),
    (0xFFFA0000, 0x100000000, lambda spnl: spnl.pibmem_img.iface.image),
]

pstSpinalPibMemMemoryMap = [
    (0x00000000,  0x80000000, None), # TODO how to pib?
    (0xFFF80000, 0x100000000, lambda spnl: spnl.pibmem_img.iface.image),
]

pstTapPibMemMemoryMap = [
    (0x00000000,  0x80000000, None), # TODO how to pib?
    (0xFFFA0000, 0x100000000, lambda spnl: spnl.pibmem_img.iface.image),
]

def getMemmoryMap():
    if(simenv.sbe_project_type == "odyssey" or
       simenv.sbe_project_type == "odysseylab"):
        memMap = odysseyMemoryMap
    elif(simenv.sbe_project_type == "pst"):
        if(simenv.sbe_image_type == "tsbe"):
            memMap = pstTapPibMemMemoryMap
        elif (simenv.sbe_image_type == "hsbe"):
            memMap = pstSpinalPibMemMemoryMap
        else:
            memMap = pstSpinalMemoryMap
    return memMap

def find_address(mmap, addr):
    for start, end, accessor in mmap:
        if addr >= start and addr < end:
            return (accessor, addr - start)
    raise KeyError(addr)

def read_memory(chip, addr, size):
    memmMap = getMemmoryMap()
    accessor, addr = find_address(memmMap, addr)
    return accessor(chip).get(addr, size)

def write_memory(chip, addr, data):
    memmMap = getMemmoryMap()
    accessor, addr = find_address(memmMap, addr)
    return accessor(chip).set(addr, data)

def read_word(chip, addr):
    return int.from_bytes(read_memory(chip, addr, 4), "big")

def write_word(chip, addr, value):
    write_memory(chip, addr, value.to_bytes(4, "big"))

#----------------------------------------------------------------
# metadata access helpers
#----------------------------------------------------------------

def namedstruct(name, struct, fields):
    t = namedtuple(name, fields)
    s = Struct(struct)
    return lambda v: t(*s.unpack(v))

META_TRA = namedstruct("META_TRA", ">II", "addr size")

def parse_metadata(chip, metadata_base):
    metadata = dict()
    ptr = metadata_base
    while True:
        hdr = read_word(chip, ptr)
        ptr += 4

        if hdr == 0:
            return metadata

        tag = hdr >> 8
        length = (hdr & 0xFF) * 4
        value = read_memory(chip, ptr, length)
        ptr += length

        metadata[tag] = value

def collectTrace ( procNr=0, nodeNr=0, tsbeInstance=0 ):

    print("Platform is %s : Image Type is %s" %(simenv.sbe_project_type,simenv.sbe_image_type))
    if(simenv.sbe_project_type == "odyssey" or
       simenv.sbe_project_type == "odysseylab"):
        cmd = simTargets.odysseys[procNr].name+".pib.read 0x00500090 8"
    elif(simenv.sbe_project_type == "pst"):
        if(simenv.sbe_image_type == "tsbe"):
            cmd = "backplane0.mcm["+str(procNr)+"].tap["+str(nodeNr)+"].pib.read 0x00500090 8"
        else:
            cmd = "backplane0.mcm["+str(procNr)+"].spinal.pib.read 0x00500090 8"
    else:
        print("Invalid/Unsupported platform type")
        return

    ( rValue , out ) = quiet_run_command( cmd, output_modes.regular )
    progressCode=(rValue >> 32) & 0x3f
    imageExecuting=((rValue >> 32) & 0x10000000) >> 28
    print("Message Register value is - %s " % hex(rValue))
    print("Progress code is %s" % hex(progressCode))
    print("Current Executing image is %s" % hex(imageExecuting))

    if(simenv.sbe_project_type == "odyssey" or
       simenv.sbe_project_type == "odysseylab"):
        if ((progressCode == 0x3f and simenv.sbe_image_type == 'sppe') or
            (simenv.sbe_image_type == 'pnor' and progressCode == 0x3f and imageExecuting == 0x01)):
            print("Runtime Image")
            fileName = "odyssey_sppe"+"_" + repr(procNr) + "_tracMERG"
            stringFile = "odysseySppeStringFile_"+get_dd_level(procNr, nodeNr)
        elif imageExecuting == 0x01 and (simenv.sbe_image_type == 'bldr' or simenv.sbe_image_type == 'pnor'):
            print("Bootloader Image")
            fileName = "odyssey_bldr"+"_" + repr(procNr) + "_tracMERG"
            stringFile = "odysseyBldrStringFile_"+get_dd_level(procNr, nodeNr)
        elif imageExecuting == 0x00 and (simenv.sbe_image_type == 'srom' or simenv.sbe_image_type == 'pnor'):
            print("SROM Image")
            fileName = "odyssey_srom"+"_" + repr(procNr) + "_tracMERG"
            stringFile = "odysseySromStringFile_"+get_dd_level(procNr, nodeNr)
        else:
            print("Invalid Messaging register state or mismatch between image type and messaging register value")
            return

    elif(simenv.sbe_project_type == "pst"):
        #TODO: Fix progress code ranges based on pst design
        # Jira item: PFSBE-220
        if (progressCode == 0x3f and simenv.sbe_image_type == 'tsbe'):
            print("TAP Runtime Image")
            fileName = simenv.sbe_project_type+"_"+simenv.sbe_image_type+"_" + repr(procNr) + "_tracMERG"
            stringFile = simenv.sbe_project_type+"TsbeStringFile_"+get_dd_level(procNr, nodeNr)
        elif (progressCode == 0x3f and simenv.sbe_image_type == 'hsbe'):
            print("HUB Runtime Image")
            fileName = simenv.sbe_project_type+"_"+simenv.sbe_image_type+"_"+ repr(procNr) + "_tracMERG"
            stringFile = simenv.sbe_project_type+"HsbeStringFile_"+get_dd_level(procNr, nodeNr)
        elif ((progressCode == 0x3f and simenv.sbe_image_type == 'sppe') or
            (simenv.sbe_image_type == 'seeprom' and progressCode == 0x3f and imageExecuting == 0x01)):
            print("Runtime Image")
            fileName = simenv.sbe_project_type+"_sppe"+"_" + repr(procNr) + "_tracMERG"
            stringFile = simenv.sbe_project_type+"SppeStringFile_"+get_dd_level(procNr, nodeNr)
        elif imageExecuting == 0x01 and (simenv.sbe_image_type == 'bldr' or simenv.sbe_image_type == 'seeprom'):
            print("Bootloader Image")
            fileName = simenv.sbe_project_type+"_bldr"+"_" + repr(procNr) + "_tracMERG"
            stringFile = simenv.sbe_project_type+"BldrStringFile_"+get_dd_level(procNr, nodeNr)
        elif imageExecuting == 0x00 and (simenv.sbe_image_type == 'srom' or simenv.sbe_image_type == 'seeprom'):
            fileName = simenv.sbe_project_type+"_srom"+"_" + repr(procNr) + "_tracMERG"
            stringFile = simenv.sbe_project_type+"SromStringFile_"+get_dd_level(procNr, nodeNr)
        else:
            print("Invalid Messaging register state or mismatch between image type and messaging register value")
            return
    else:
        print("Invalid/Unsupported platform type")
        return

    if(simenv.sbe_project_type == "odyssey" or
       simenv.sbe_project_type == "odysseylab"):
        chip = simTargets.odysseys[procNr]
    elif(simenv.sbe_project_type == "pst"):
        if(simenv.sbe_image_type == "tsbe"):
            chip = conf.backplane0.mcm[0].tap[tsbeInstance]
        else:
            chip = conf.backplane0.mcm[0].spinal

    metadata_base = read_word(chip, 0xFFFFFFFC)
    metadata = parse_metadata(chip, metadata_base)
    trace_offset, trace_size = META_TRA(metadata[0x545241])

    print("Trace Buffer offset : %s, size : %s" % (hex(trace_offset),hex(trace_size)))

    trace_bin = read_memory(chip, trace_offset, trace_size)

    tracedump_path = "ppetrace.bin"
    with open(tracedump_path, "wb") as binary_file:
        # Write bytes to file
        binary_file.write(trace_bin)

    toolwrappers.parseSbeTrace(tracedump_path, stringFile, fileName)

#TODO: Better would be only two parameter chip_type and chip_number
#      Jira item:  PFSBE-162
def printRegisters(procNr=0, nodeNr=0, tsbeInstance=0 ):

    #TODO: Give support for odyssey dimm number "backplane0.dimm[dimmNr].odyssey.sppe.pregs"
    #      Jira item:  PFSBE-162
    if(simenv.sbe_project_type == "odyssey" or
       simenv.sbe_project_type == "odysseylab"):
        print("Print Register value for odyssey")
        cmd_string = simTargets.odysseys[procNr].name+".sppe.pregs"
        print(cmd_string)
        (regs, out) = quiet_run_command(cmd_string, output_modes.regular)

    elif(simenv.sbe_project_type == "pst"):
        if(simenv.sbe_image_type == "tsbe"):
            print("Print Register value for PST TAP image")
            cmd_string = "backplane0.mcm[" + str(procNr) + "].tap[" + str(tsbeInstance) + "].sbe.pregs"
            print(cmd_string)
            (regs, out) = quiet_run_command(cmd_string, output_modes.regular)
        elif(simenv.sbe_image_type == "hsbe"):
            print("Print Register value for PST HUB image")
            cmd_string = "backplane0.mcm[" + str(procNr) + "].spinal.sbe.pregs"
            print(cmd_string)
            (regs, out) = quiet_run_command(cmd_string, output_modes.regular)
        else:
            print("Print Register value for PST SPPE image")
            cmd_string = "backplane0.mcm[" + str(procNr) + "].spinal.sppe.pregs"
            (regs, out) = quiet_run_command(cmd_string, output_modes.regular)
    else:
        print("platform type not supported for Simics standalone run. selected platform: ", simenv.sbe_project_type)

    '''
    00 r0       16 cr            38 d0      |
    01 r1       17 ctr           39 d1      |
    02 r2       18 dacr          40 d2      |
    03 r3       19 dbcr          41 d3      |
    04 r4       20 dec           42 d4      |
    05 r5       21 edr           43 d5      |
    06 r6       22 iar           44 d6       >-- First loop
    07 r7       23 ir            45 d7      |
    08 r8       24 isr           46 d8      |
    09 r9       25 ivpr          47 d9      |
    10 r10      26 lr            48 d28     |
    11 r13      27 msr           49 d29     |
    12 r28      28 pir           50 d30     |
    13 r29      29 pvr           51 d31     |
---------------------------------------------
    14 r30      30 sprg0                     >-- Second loop
    15 r31      31 srr0                     |
---------------------------------------------
                32 srr1                     |
                33 tcr                      |
                34 tsr                       >-- Third loop
                35 xcr                      |
                36 xsr                      |
                37 xer                      |
    '''

    reg_table = []
    for i in range(14):
        reg_table.append([
                        regs[i + 16][0], hexFormat(regs[i + 16][1]), "", # print SPRs before GPRs
                        regs[i][0], hexFormat(regs[i][1]), "",
                        regs[i + 38][0], hexFormat(regs[i + 38][1], 16)])
    for i in range(14,16):
        reg_table.append([
                        regs[i + 16][0], hexFormat(regs[i + 16][1]), "", # print SPRs before GPRs
                        regs[i][0], hexFormat(regs[i][1]), "",
                        "", ""])
    for i in range(16,22):
        reg_table.append([
                        regs[i + 16][0], hexFormat(regs[i + 16][1]), "", # print SPRs before GPRs
                        "", "", "",
                        "", ""])

    print(tabulate(reg_table, tablefmt='pretty', ))

def writeToFifoScriptBranch(packets:list):
    enq_address = sbeSimUtils.getUsFifoDataAddrToWrite(0)
    stat_address = sbeSimUtils.getUsFifoStatusAddrToRead(0)
    eot_address = sbeSimUtils.getUsFiFoEotAddrToWrite(0)
    lbus = sbeSimUtils.getLbus(0)

    ############################################################################
    ###                     WRITE TO UPSTREAM FIFO                           ###
    for packet in packets:
        count = 0
        while True:
            status = sbeSimUtils.read(lbus, stat_address, 4)  # Address 0x2404, 0x2484

            if( status[2] & 0x02):
                count = count + 1
                simTargets.odysseys[0].sppe.cli_cmds.wait_for_cycle(
                            cycle = sbeSimUtils.cyclesPerIter, _relative = True)
                # This will cause  test to fail
                if(count > sbeSimUtils.waitItrCount):
                    print('Error: Timeout. FIFO FULL')
                    return
            else:
                # write entry
                sbeSimUtils.write(lbus, enq_address, tuple(packet.to_bytes(4, 'big')))
                break

    sbeSimUtils.write(lbus, eot_address, tuple((0x01).to_bytes(4, 'big')))

    ############################################################################
    ###                     READ FROM DOWNSTREAM FIFO                        ###
    # TODO: Implement Read DsFIFO

    print("FIFO WRITE COMPLETED")
    return

#TODO need to give support for PST
# Jira Item: PFSBE-158 - FIFO support for PST in simics-debug-framwork.py
def writeToFifo(procNr, nodeNr, packets:list):
    if(simenv.sbe_project_type == "odyssey" or
       simenv.sbe_project_type == "odysseylab"):
        if (not SIM_simics_is_running()):
            raise CliError("This command supported only when simics is running")

        if(packets[0] != len(packets)):
            raise CliError("Invalid first packet. First packet should be number of"
                    + " total packets: ", packet)
        for packet in packets:
            if(packet > 0xFFFFFFFF):
                raise CliError("Invalid packet", packet)

        print("Start writing the fifo. Wait for message 'FIFO WRITE COMPLETED")
        sb_create(lambda: writeToFifoScriptBranch(packets))
    else:
        print("platform type not supported for Simics standalone run. selected platform: ", simenv.sbe_project_type)

# Run the registration automatically whenever this script is loaded.
try:
    register_sbe_debug_framework_tools()
except Exception as e:
    print("register_sbe_debug_framework_tools failed")
    print(e)
    raise
