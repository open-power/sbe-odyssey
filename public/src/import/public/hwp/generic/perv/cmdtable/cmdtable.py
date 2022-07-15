#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/generic/perv/cmdtable/cmdtable.py $
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
import re
import sys, os, subprocess
from util import *
from collections import namedtuple
from enum import IntEnum

FORMAT_MAGIC = 0x434d5461    # 'CMTa'
DUMMY_REG    = 0x50009

class ParseError(Exception):
    pass

class Opcode(IntEnum):
    NOP     = 0b000
    CALL    = 0b001
    RETURN  = 0b010
    PUTSCOM = 0b011
    TEST    = 0b100
    POLL    = 0b101
    CMPBEQ  = 0b110
    CMPBNE  = 0b111

class Command(object):
    __slots__ = ["op", "param", "address", "mask", "data"]

    def __init__(self, op, param, address, mask, data):
        self.op = Opcode(op)
        self.param   = param
        self.address = address & 0xFFFFFFFF
        self.mask    = mask    & 0xFFFFFFFFFFFFFFFF
        self.data    = data    & 0xFFFFFFFFFFFFFFFF

    def __str__(self):
        return "|%-7s|%07X|%08X|%016X|%016X|" % (self.op.name, self.param & 0x0FFFFFFF, self.address, self.mask, self.data)

    LINE_PAT = re.compile(r'\|([A-Za-z0-9\_\s]+)\|([A-Fa-f0-9\-]+)\|([A-Fa-f0-9\-]+)\|([A-Fa-f0-9\-]+)\|([A-Fa-f0-9\-]+)\|$')

    @staticmethod
    def fromText(line):
        entry = Command.LINE_PAT.search(line)
        if not entry:
            raise ParseError("Cannot parse " + line)

        for opcode in Opcode:
            if entry.group(1).startswith(opcode.name):
                op = opcode
                break
        else:
            raise ValueError("Command %s is not recognized" % entry.group(1))

        param   = 0 if "--" in entry.group(2) else int("0x" + entry.group(2), 16)
        address = int("0x" + entry.group(3), 16)
        mask    = int("0x" + entry.group(4), 16)
        data    = int("0x" + entry.group(5), 16)

        return Command(op, param, address, mask, data)

def no_args(op, args):
    return Command(op, 0, 0, 0, 0)

def p_args(op, args):
    param = args[0]
    return Command(op, param, 0, 0, 0)

def add_args(op, args):
    addr, mask, data = args
    return Command(op, 0, addr, mask, data)

def addx_args(op, args):
    addr, mask, data, x = args
    return Command(op, x, addr, mask, data)

def always_fail_alias(op, args):
    x = args[0]
    return Command(op, x, DUMMY_REG, 0, 0xFFFFFFFFFFFFFFFF)

ScriptCommand = namedtuple("ScriptCommand", "op argtypes arg_handler")

SCRIPT_COMMANDS = {
    # Basic instructions
    "NOP":      ScriptCommand(Opcode.NOP,     "",     no_args),
    "RETURN":   ScriptCommand(Opcode.RETURN,  "",     no_args),
    "PUTSCOM":  ScriptCommand(Opcode.PUTSCOM, "add",  add_args),
    "TEST":     ScriptCommand(Opcode.TEST,    "addc", addx_args),
    "POLL":     ScriptCommand(Opcode.POLL,    "addc", addx_args),
    "CMPBEQ":   ScriptCommand(Opcode.CMPBEQ,  "addl", addx_args),
    "CMPBNE":   ScriptCommand(Opcode.CMPBNE,  "addl", addx_args),
    "CALL":     ScriptCommand(Opcode.CALL,    "a",    p_args),
    # Alias instructions that map to basic instructions
    "B":        ScriptCommand(Opcode.CMPBNE,  "l",    always_fail_alias),
    "FAIL":     ScriptCommand(Opcode.TEST,    "c",    always_fail_alias),
}

COMMAND_ARGTYPES = {
    "a": "address",
    "c": "error code",
    "d": "data",
    "l": "label",
}

def has_whitespace(text):
    return " " in text or "\t" in text

HEX_LEFT_REX = re.compile(r"0xl([0-9a-fA-F]+)")

def expand_hex_left(expr):
    result = ""
    prev_end = 0

    for match in HEX_LEFT_REX.finditer(expr):
        value = match.group(1)
        result += expr[prev_end:match.start()] + "0x" + value + "0" * (16 - len(value))
        prev_end = match.end()

    return result + expr[prev_end:]

def parse_command_args(cmd, argtypes, args):
    if len(argtypes) != len(args):
        raise ParseError("command %s requires %d arguments, but %d were given" % (cmd, len(argtypes), len(args)))

    for i, (argtype, arg) in enumerate(zip(argtypes, args)):
        try:
            if argtype in "adc":
                if argtype == "d":
                    arg = expand_hex_left(arg)
                try:
                    # Try to evaluate the parameter as a Python expression with no access to any global or local variables
                    arg = eval(arg, {__builtins__: {}})
                except Exception as e:
                    raise ParseError("invalid %s (%s): %s" % (COMMAND_ARGTYPES[argtype], e.args[0], arg))
                if argtype == "c" and arg & 0xFFFFFF00 != 0x200:
                    raise ParseError("TEST/POLL error code must be 0x2xx")
            elif argtype in "sl":
                if has_whitespace(arg):
                    raise ParseError("%s must not contain whitespace: %s" % (COMMAND_ARGTYPES[argtype], arg))
            else:
                raise ValueError("BUG: Unknown argument type '%s'" % argtype)

        except ParseError as e:
            raise ParseError("%s argument %d: %s" % (cmd, i, e.args[0]))

        args[i] = arg

    return args

def parse_script(script):
    commands = []
    labels = {}
    error = False
    fname = "<unknown>"
    lineno = 0
    tmpline = None

    for line in script:
        lineno += 1

        # if this is a continuation of a previous line, prepend that
        if tmpline:
            line = tmpline + line
            tmpline = None

        # if the line ends in a dangling comma, append another one
        if line.rstrip().endswith(","):
            tmpline = line.rstrip()
            continue

        try:
            # parse cpp line information
            if line and line[0] == "#":
                lineno, fname, *drop = line[1:].split()
                lineno = int(lineno) - 1
                fname = fname.strip('"')
                continue

            # extract label
            if ":" in line:
                label, command = (line + " ").split(":", 1)
                label = label.strip()
                line = command.strip()

                if has_whitespace(label):
                    raise ParseError("label must not contain whitespace: %s" % label)

                labels[label] = len(commands)

            # split line into (potentially multiple) commands
            cmds = line.split(";")
            for cmd in cmds:
                cmd = cmd.strip()

                # skip empty commands
                if not cmd:
                    continue

                cmdname, *args = cmd.split(maxsplit=1)
                cmdname = cmdname.upper()
                if cmdname not in SCRIPT_COMMANDS:
                    raise ParseError("unknown command: %s" % cmdname)

                cmd = SCRIPT_COMMANDS[cmdname]
                args = [part.strip() for part in args[0].split(",")] if args else []
                args = parse_command_args(cmdname, cmd.argtypes, args)
                commands.append((fname, lineno, cmd.arg_handler(cmd.op, args)))

        except ParseError as e:
            print("%s:%d: error: %s" % (fname, lineno, e.args[0]), file=sys.stderr)
            error = True

    for i, (fname, lineno, cmd) in enumerate(commands):
        if cmd.op in (Opcode.CMPBEQ, Opcode.CMPBNE):
            try:
                offset = labels[cmd.param] - i - 1
                if offset < 0:
                    print("%s:%d: error: branch to label '%s' would be backwards" % (fname, lineno, cmd.param), file=sys.stderr)
                    error = True
                cmd.param = offset
            except KeyError:
                print("%s:%d: error: label '%s' not found" % (fname, lineno, cmd.param), file=sys.stderr)
                error = True

    return None if error else [c[2] for c in commands]

def cmd_assemble(args):
    args.includepaths.extend([os.path.dirname(args.script), os.path.dirname(sys.argv[0])])
    cppcmd = [ "/usr/bin/cpp", "-undef", "-nostdinc" ]
    cppcmd.extend("-I" + path for path in args.includepaths if path)
    cppcmd.append(args.script)
    cppout = subprocess.run(cppcmd, stdout=subprocess.PIPE)
    if cppout.returncode:
        exit(1)

    commands = parse_script(cppout.stdout.decode().splitlines())
    if not commands:
        exit(1)

    with open_or_stdio(args.outfile, "wt") as outfile:
        for cmd in commands:
            print(cmd, file=outfile)

def check(condition, message):
    if not condition:
        print(message)
        exit(1)

def cmd_link(args):
    commands = []
    for infile in args.table:
        with open_or_stdio(infile) as f:
            for line in f:
                commands.append(Command.fromText(line))

    if commands[-1].op != Opcode.RETURN:
        if not args.quiet:
            print("WARNING: Commands do not end in RETURN, adding one")
        commands.append(Command(Opcode.RETURN, 0, 0, 0, 0))

    masks = set(cmd.mask for cmd in commands)
    data = set(cmd.data for cmd in commands)
    params = set(cmd.param for cmd in commands)
    addresses = set(cmd.address for cmd in commands)

    # Sort values such that params and masks come first so they have small indices,
    # but still ensure we have no duplicates
    small_data = sorted(params) + sorted(addresses - params)
    big_data = sorted(masks) + sorted(data - masks)

    if not args.quiet:
        print("%d commands" % len(commands))
        print("  %d params + %d addresses -> %d small values" % (len(params), len(addresses), len(small_data)))
        print("  %d masks + %d data -> %d big values" % (len(masks), len(data), len(big_data)))

    if (len(commands) + len(small_data)) & 1:
        # If header + commands + small data would leave big data unaligned, add a dummy value
        small_data.append(0)

    check(len(params) < 2**6, "Too many distinct param values")
    check(len(small_data) < 2**7, "Too many distinct addresses")
    check(len(masks) < 2**6, "Too many distinct mask values")
    check(len(big_data) < 2**9, "Too many distinct data values")
    check(len(commands) < 2**16, "Too many commands")

    with open(args.outfile, "wb") as f:
        write32(f, FORMAT_MAGIC)
        write32(f, ((len(commands) - 1) & 0xFFFF) << 16 | ((len(small_data) - 1) & 0x7F) << 9 | (len(big_data) - 1) & 0x1FF)
        for cmd in commands:
            # cmd 4 + param 6 + addr 7 + mask 6 + data 9 = 32
            write32(f, cmd.op << 28 | small_data.index(cmd.param) << 22 | small_data.index(cmd.address) << 15 | big_data.index(cmd.mask) << 9 | big_data.index(cmd.data))
        for value in small_data:
            write32(f, value)
        assert(f.tell() & 7 == 0)
        for value in big_data:
            write64(f, value)

        if not args.quiet:
            print("Total FSM blob length: %d bytes" % f.tell())

def load_commands(fname):
    commands = []
    with open(fname, "rb") as f:
        magic = read32(f)
        if magic != FORMAT_MAGIC:
            f.close()
            with open(fname) as f:
                for line in f:
                    commands.append(Command.fromText(line))
            return commands

        header = read32(f)
        ncmd   = 1 + (header >> 16)
        nsmall = 1 + ((header >> 9) & 0x7F)
        nbig   = 1 + (header & 0x1FF)

        shrunk_commands = [read32(f) for _ in range(ncmd)]
        small_data      = [read32(f) for _ in range(nsmall)]
        big_data        = [read64(f) for _ in range(nbig)]

    for cmd in shrunk_commands:
        op = cmd >> 28
        param = small_data[(cmd >> 22) & 0x3F]
        address = small_data[(cmd >> 15) & 0x7F]
        mask = big_data[(cmd >> 9) & 0x3F]
        data = big_data[cmd & 0x1FF]
        commands.append(Command(op, param, address, mask, data))

    return commands

def run_commands(main, cust, target):
    ips = [0, 0]
    tables = [main, cust]
    level = 0

    while True:
        if ips[level] >= len(tables[level]):
            print("%s table overrun" % ("main", "cust")[level])
            return -1

        cmd = tables[level][ips[level]]
        ips[level] += 1
        print("%s:%05d%s" % (("main", "cust")[level], ips[level], cmd))

        if cmd.op == Opcode.CALL:
            if level > 0:
                print("CALL inside custom table")
                return -1

            ips[1] = cmd.param
            level = 1

        elif cmd.op == Opcode.RETURN:
            if level == 0:
                return 0
            level -= 1

        elif cmd.op == Opcode.PUTSCOM:
            if cmd.mask < 0xFFFFFFFFFFFFFFFF:
                value = target.getScom(cmd.address).uint
                new_value = (value & ~cmd.mask) | (cmd.data & cmd.mask)
                print("  %016X -> %016X" % (value, new_value))
                target.putScom(cmd.address, new_value)
            else:
                target.putScom(cmd.address, cmd.data)

        elif cmd.op in (Opcode.TEST, Opcode.POLL, Opcode.CMPBEQ, Opcode.CMPBNE):
            for _ in range(1000):
                value = target.getScom(cmd.address).uint
                match = value & cmd.mask == cmd.data
                print("  %016X -> %016X %s %016X" % (value, value & cmd.mask, "==" if match else "!=", cmd.data))
                if cmd.op == Opcode.POLL:
                    if match:
                        break
                else:
                    if cmd.op == Opcode.TEST and not match:
                        print("Test failed, error code 0x%X" % cmd.param)
                        return cmd.param
                    elif cmd.op == Opcode.CMPBEQ and match or cmd.op == Opcode.CMPBNE and not match:
                        ips[level] += cmd.param

                    break
            else:
                print("Polling timed out without match, error code 0x%X" % cmd.param)
                return cmd.param

class FakeTarget(object):
    uint = 0
    def getScom(self, address):
        return self
    def putScom(self, address, data):
        pass

def cmd_run(args):
    main = load_commands(args.main)
    cust = load_commands(args.cust) if args.cust else []

    if args.dry:
        run_commands(main, cust, FakeTarget())
    else:
        import pyecmd
        with pyecmd.Ecmd(args=args.ecmd_args):
            for target in pyecmd.loopTargets(args.chip, pyecmd.ECMD_SELECTED_TARGETS_LOOP_DEFALL):
                print(target)
                run_commands(main, cust, target)

def cmd_reverse(args):
    commands = load_commands(args.blob)
    for cmd in commands:
        print(cmd)

if __name__ == "__main__":
    parser = EcmdArgumentParser(description="Functions around ROM command tables")
    subparsers = parser.add_subparsers(title="Available commands")

    sub = subparsers.add_parser("assemble", help="Compile FSM assembler into raw FSM command table")
    sub.add_argument("script", help="Source script file, use '-' to read from stdin")
    sub.add_argument("outfile", help="Output file, use '-' to write to stdout")
    sub.add_argument("-I", dest="includepaths", metavar="includepath", action="append", default=[],
                     help="Include directory to add to search path; defaults are the locations of the source file and the cmdtable tool")
    sub.set_defaults(func=cmd_assemble)

    sub = subparsers.add_parser("link", help="Combine command tables into blob for SBE")
    sub.add_argument("table", nargs="+", help="Input table(s), use '-' to read from stdin")
    sub.add_argument("-o", "--outfile", default="fsm_table.bin", help="Output file name, defaults to fsm_table.bin")
    sub.add_argument("-q", "--quiet", action="store_true", help="Don't print statistics or warn on missing RETURN")
    sub.set_defaults(func=cmd_link)

    sub = subparsers.add_parser("disassemble", help="Reverse an SBE blob back into a command table")
    sub.add_argument("blob", help="Blob to disassemble")
    sub.set_defaults(func=cmd_reverse)

    sub = subparsers.add_parser("run", help="Run a command table or SBE blob")
    sub.add_argument("chip", help="Chip to operate on")
    sub.add_argument("main", help="Main command file; type will be autodetected")
    sub.add_argument("cust", nargs="?", default=None, help="Optional custom table file for CALL operations")
    sub.add_argument("--dry", action="store_true", help="Dry run - use fake target and don't use ecmd")
    sub.set_defaults(func=cmd_run)

    args = parser.parse_args()
    if not hasattr(args, "func"):
        parser.print_help()
        exit(1)

    args.func(args)
