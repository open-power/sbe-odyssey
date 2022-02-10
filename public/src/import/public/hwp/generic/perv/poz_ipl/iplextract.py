#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/generic/perv/poz_ipl/iplextract.py $
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
import re, jinja2, os, sys
import importlib.util
from os import path
from argparse import ArgumentParser

def replace(string, pattern, substitution):
    result = ""
    pos = 0
    for match in pattern.finditer(string):
        result += string[pos:match.start()] + substitution(match)
        pos = match.end()
    return result + string[pos:]

class HWP(object):
    default_params = [
        ("p11t_", "target<TAP_CHIP>"),
        ("p11s_", "target<HUB_CHIP>"),
        ("p11_",  "target<HUB_CHIP>"),
        ("ody_",  "target<OCMB_CHIP>"),
        ("zme_",  "target<PROC_CHIP>"),
        ("poz_",  "target<ANY_POZ_CHIP>"),
    ]

    def __init__(self, istep, name, params):
        self.istep = istep
        self.name = name
        self.params = params
        if not self.params:
            for prefix, params in self.default_params:
                if self.name.startswith(prefix):
                    self.params = params
                    break
        self.body = []
        self.regs = {}

    target_re = re.compile(r"target<(?P<types>.*?)>")
    default_re = re.compile(r"=.*?(,|$)")
    param_name_re = re.compile(r"\s*\w+(,|$)")
    def expand_params(self, default_values, param_names, fapi2):
        expanded = self.params
        expanded = replace(expanded, self.target_re, lambda match: "const fapi2::Target<" + " | ".join("fapi2::TARGET_TYPE_" + type.strip() for type in match.group("types").split("|")) + "> &i_target")

        if not fapi2:
            expanded = expanded.replace("fapi2::", "")

        if not default_values:
            expanded = self.default_re.sub(r"\1", expanded)

        if not param_names:
            expanded = self.param_name_re.sub(r"\1", expanded)

        return expanded

    def param_names(self):
        for param in self.expand_params(False, True, False).split(","):
            param = self.default_re.sub("", param).strip()
            if not param:
                break
            name = param.split()[-1].strip("*&")
            if name.startswith("i_"):
                yield "in", name
            elif name.startswith("o_"):
                yield "out", name

    def targets(self):
        return sorted(set(target for _, _, target in self.regs))

    def used_regs(self):
        return self.regs

    def add_reg(self, regdb, regname):
        if not regdb or regname in self.regs:
            return

        for target, regs in regdb:
            if regname in regs:
                reg = regs[regname]
                result = (reg.regname, target)
                self.regs[regname] = result
                return

        print("Warning: Register '%s' not found in SCOM headers" % regname)
        self.regs[regname] = (None, None)

    regset_re = re.compile(r"(?P<reg>\w+)(\.(?P<field>\w+))?(\[(?P<slice>.*?)\])?\s*=\s*(?P<value>.*?)\s*$")
    call_re = re.compile(r"\w+\(.*\)")
    def expand_body(self, regdb):
        self.expanded_body = []
        nonempty_lines = [i for i, line in enumerate(self.body) if line.strip()]
        if not nonempty_lines:
            self.regs = []
            return

        body = self.body[min(nonempty_lines):max(nonempty_lines) + 1]
        result = []
        self.ring_names = []

        for line in body:
            if line == "pass":
                continue

            # split line into indentation, command and comment
            line, *comment = line.split("#", 1)
            comment = "" if not comment else comment[0].rstrip()
            cmd = line.lstrip()
            indent = line[:len(line) - len(cmd)]
            cmd = cmd.rstrip()

            regset_match = self.regset_re.match(cmd)
            call_match = self.call_re.match(cmd)
            if regset_match:
                reg, field, slice, value = regset_match.group("reg", "field", "slice", "value")
                self.add_reg(regdb, reg)

                if field:
                    cmd = "%s.set_%s(%s);" % (reg, field, value)

            elif cmd.startswith("delay("):
                cmd = "FAPI_TRY(" + cmd + ");"

            elif cmd.startswith("putRing(") or cmd.startswith("compareRing("):
                ring_name, *ring_def = cmd.split("(", 1)[1].rstrip(")").split("=")
                ring_name = ring_name.strip()
                ring_def = ring_def[0].strip() if ring_def else None

                self.ring_names.append((ring_name, ring_def))
                cmd = "FAPI_TRY(putRing(i_target, ring_names::%s%s));" % (ring_name, ", RING_MODE_COMPARE" if cmd.startswith("compareRing(") else "")

            elif call_match:
                cmd = "FAPI_TRY(" + cmd + ");"

            elif cmd:
                pass

            elif comment and comment[0] == "#":
                cmd = 'FAPI_INF("' + comment[1:].strip() + '");'
                comment = ""

            result.append((indent, cmd, comment))

        self.regs = sorted((shortname, fullname, target) for shortname, (fullname, target) in self.regs.items() if target)
        if self.regs:
            result = [("", "%s_t %s;" % (fullname, shortname), "") for shortname, fullname, _ in self.regs] + [("", "", "")] + result

        cmdlen_hist = sorted(len(indent) + len(cmd) for indent, cmd, comment in result if comment)
        if cmdlen_hist:
            cmdlen_90th_percentile = cmdlen_hist[-1-(len(cmdlen_hist) // 10)]
            cmdlen_max = cmdlen_hist[-1]
            comment_indent = cmdlen_max if cmdlen_max <= cmdlen_90th_percentile + 5 else cmdlen_90th_percentile

        for indent, cmd, comment in result:
            line = indent + cmd if cmd else ""
            if cmd and comment:
                line = line.ljust(comment_indent + 1)
            if comment:
                line += "//" + comment
            self.expanded_body.append(("    " + line) if line else "")

class IStep(object):
    def __init__(self, major, minor, name, envs):
        self.major, self.minor, self.name, self.envs = major, minor, name, envs
        self.hwps = []

    def env_string(self):
        return ", ".join(self.envs)

    def targets(self):
        return sorted(set(target for hwp in self.hwps for _, _, target in hwp.regs))

    def used_regs(self):
        return sorted(set(v for hwp in self.hwps for v in hwp.regs))

def parse_script(fname, regdb):
    steps = []
    cur_istep = None
    cur_hwp = None
    with open(fname) as f:
        try:
            for line in f:
                line = line.rstrip()
                if line.startswith("ISTEP("):
                    major, minor, name, envs = [part.strip(" \t\"'") for part in line.split("(")[1].strip(")").split(",", 3)]
                    major = int(major)
                    minor = int(minor)
                    envs = [part.strip() for part in envs.split(",")]
                    cur_istep = IStep(major, minor, name, envs)
                    steps.append(cur_istep)
                elif line.startswith("def ") and cur_istep:
                    name, params = line[4:].split("(")
                    name = name.strip()
                    params = params.strip(" \t):")
                    cur_hwp = HWP(cur_istep, name, params)
                    cur_istep.hwps.append(cur_hwp)
                elif (line.startswith("    ") or not line.strip()) and cur_hwp is not None:
                    cur_hwp.body.append(line[4:])
                else:
                    cur_hwp = None
        except:
            print("Parse error at '" + line + "'")
            raise

    for step in steps:
        for hwp in step.hwps:
            hwp.expand_body(regdb)

    return steps

def print_step_list(steps):
    for step in steps:
        print("%d.%d\t%s  // runs on %s" % (step.major, step.minor, step.name, ", ".join(step.envs)))
        for hwp in step.hwps:
            print("    %s(%s)" % (hwp.name, hwp.expand_params(False, True, True)))

def print_reg_list(steps):
    for step in steps:
        for hwp in step.hwps:
            if not hwp.regs:
                continue
            print("%s:" % hwp.name)
            for myname, regname, target in hwp.used_regs():
                print("  %s: %s.%s" % (myname, target, regname))

def print_step_csv(steps):
    for step in steps:
        names = [step.name] if step.major >= 3 or len(step.hwps) < 2 else [hwp.name for hwp in step.hwps]
        for name in names:
            print("%d, %d, %s" % (step.major, step.minor, name))

def print_ringnames(steps):
    names = []
    for step in steps:
        for hwp in step.hwps:
            hwp.expand_body()
            names.extend(hwp.ring_names)

    for ring_name, ring_def in names:
        line = '    const char %s[] = "%s";' % (ring_name, ring_name)
        if ring_def:
            line += " // " + ring_def
        print(line)

def generate_shells(outdir, steps):
    os.makedirs(outdir, exist_ok=True)

    env = jinja2.Environment(loader=jinja2.FileSystemLoader(path.dirname(sys.argv[0])), trim_blocks=True, lstrip_blocks=True)
    c_template = env.get_template("hwp.C.template")
    h_template = env.get_template("hwp.H.template")

    for step in steps:
        for hwp in step.hwps:
            with open(path.join(outdir, hwp.name + ".C"), "wt") as f:
                f.write(c_template.render(hwp=hwp))
            with open(path.join(outdir, hwp.name + ".H"), "wt") as f:
                f.write(h_template.render(hwp=hwp))

def generate_modules(outdir, steps):
    os.makedirs(outdir, exist_ok=True)

    env = jinja2.Environment(loader=jinja2.FileSystemLoader(path.dirname(sys.argv[0])), trim_blocks=True, lstrip_blocks=True)
    c_template = env.get_template("mod.C.template")
    h_template = env.get_template("mod.H.template")

    for step in steps:
        with open(path.join(outdir, step.name + ".C"), "wt") as f:
            f.write(c_template.render(step=step))
        with open(path.join(outdir, step.name + ".H"), "wt") as f:
            f.write(h_template.render(step=step))

if __name__ == "__main__":
    parser = ArgumentParser(description="Extract information from IPL pseudocode script")
    parser.add_argument("script", help="Script file")
    parser.add_argument("--shells", metavar="outdir", help="Generate HWP shells in outdir")
    parser.add_argument("--modules", metavar="outdir", help="Generate module shells in outdir")
    parser.add_argument("--scom", help="Path to SCOM headers")
    parser.add_argument("--empty", action="store_true", help="Generate HWP/module shells without instructions")
    parser.add_argument("--steplist", action="store_true", help="Print istep list")
    parser.add_argument("--reglist", action="store_true", help="Print register list, won't be fun without --scom")
    parser.add_argument("--csv", action="store_true", help="Print step CSV")
    parser.add_argument("--ringnames", action="store_true", help="Print ring names")
    parser.add_argument("--pattern", help="Limit HWP selection to those matching a regular expression pattern")
    parser.add_argument("--istep", type=int, help="Limit HWP selection to a single istep")
    args = parser.parse_args()

    regdb = []
    if args.scom:
        sys.path.append(args.scom)
        import scomt_regdump
        regdb.append(("perv", scomt_regdump.registers["perv"]))
        for target, regs in scomt_regdump.registers.items():
            if target != "perv":
                regdb.append((target, regs))

        regdb = [(target, {reg.fullname.split(".")[-1]: reg for reg in regs}) for target, regs in regdb]

    steps = parse_script(args.script, regdb)

    if args.istep is not None:
        steps = [step for step in steps if step.major == args.istep]

    if args.pattern:
        for step in steps:
            step.hwps = [hwp for hwp in step.hwps if re.match(args.pattern, hwp.name)]

        steps = [step for step in steps if step.hwps]

    if args.steplist:
        print_step_list(steps)

    if args.reglist:
        print_reg_list(steps)

    if args.csv:
        print_step_csv(steps)

    if args.ringnames:
        print_ringnames(steps)

    if args.empty:
        for step in steps:
            for hwp in step.hwps:
                hwp.body = hwp.expanded_body = []

    if args.shells:
        generate_shells(args.shells, steps)

    if args.modules:
        generate_modules(args.modules, steps)
