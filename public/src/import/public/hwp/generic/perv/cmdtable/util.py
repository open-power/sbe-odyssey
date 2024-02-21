# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/generic/perv/cmdtable/util.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2024
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
import sys, os, argparse, struct, io

def write32(f, v):
    f.write(struct.pack(">I", v))

def write64(f, v):
    f.write(struct.pack(">Q", v))

def read32(f):
    return struct.unpack(">I", f.read(4))[0]

def read64(f):
    return struct.unpack(">Q", f.read(8))[0]

def open_or_stdio(fname, mode="r", **kwargs):
    """
    Like open(), but if fname is "-",
    returns stdin or stdout depending on mode,
    and doesn't close it when the "with" block ends.
    """
    if fname == "-":
        fileno = sys.stdin.fileno() if "r" in mode else sys.stdout.fileno()
        return os.fdopen(fileno, mode, closefd=False, **kwargs)
    else:
        return open(fname, mode, **kwargs)

class EcmdOutputRedirect:
    def __init__(self, pyecmd_module=None):
        self.pyecmd = pyecmd_module or pyecmd

    def __enter__(self, *args):
        self.strio = io.StringIO()
        self._oldstdout = sys.stdout
        sys.stdout = self
        return self

    def __exit__(self, *args):
        self.strio.close()
        sys.stdout = self._oldstdout

    def write(self, *args):
        self.strio.seek(0)
        self.strio.truncate()
        self.strio.write(*args)
        self.pyecmd.output(self.strio.getvalue())

class EcmdArgumentParser(argparse.ArgumentParser):
    """
    An ArgumentParser variant that filters eCmd argument (-n#, -all, -debug etc.) into their own list.
    The resulting namespace will have an extra attribute called ecmd_args.
    Note: Does not depend on ecmd itself
    """
    def __init__(self, *args, **kwargs):
        kwargs["epilog"] = kwargs.pop("epilog", "") + " Cronus arguments like -p#, -a#, -all, -debug are also supported."
        self.ecmd_args_supported = ("-debug",) + kwargs.pop("ecmd_args_supported", ("-a", "-k", "-n", "-s", "-p", "-c", "-t"))
        super().__init__(*args, **kwargs)

    def parse_known_args(self, args=None, namespace=None):
    # argparse can't handle -debug5.1 etc., so we have to filter that out manually,
    # so we can just filter out all ecmd arguments manually
        if args is None:
            args = sys.argv[1:]

        ecmd_args = []
        non_ecmd_args = []
        consume_next = False
        for arg in args:
            if consume_next:
                ecmd_args.append(arg)
                consume_next = False
            else:
                for opt in self.ecmd_args_supported:
                    if arg.startswith(opt):
                        ecmd_args.append(arg)
                        consume_next = arg == opt
                        break
                else:
                    if arg == "-all":
                        ecmd_args.append(arg)
                    else:
                        non_ecmd_args.append(arg)

        namespace, argv = super().parse_known_args(args=non_ecmd_args, namespace=namespace)
        namespace.ecmd_args = ecmd_args
        return namespace, argv

    def add_subparsers(self, **kwargs):
        kwargs.setdefault('parser_class', argparse.ArgumentParser)
        return super().add_subparsers(**kwargs)
