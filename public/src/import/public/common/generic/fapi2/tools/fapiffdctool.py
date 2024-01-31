#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/common/generic/fapi2/tools/fapiffdctool.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2024
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

'''@package fapiffdctool
Tool to parse error xml and generate source code for handling HWP-FFDCs in different
platforms. (Alternate for 'parseErrorInfo.pl')

usage: fapiffdctool [-h] {parse,display,gensrc} ...

Tools to generate code for error XMLs.

positional arguments:
  {parse,display,gensrc}
    parse               Parse error XML and generate ffdc database
    display             Display ffdc database
    gensrc              Generate ffdc source code

optional arguments:
  -h, --help            show this help message and exit

Expected usage for auto generating source code.
$ fapiffdctool parse <error_xml-1> <error_xml-2> ... -d <path_to_generate_db_file>
$ fapiffdctool gensrc <db_file_path> <dir_to_generate_src>
'''

#standard library imports
from argparse import ArgumentError, ArgumentParser
import pickle
import jinja2
import os
import sys

from error_xml_parser import HwpErrorDB, SUPPORTED_PLATFORMS

def cmd_parse(args):
    '''
    Parse all input error xml and create a HwpErrorDB object.
    Then create a pickle dump for future usage.
    '''
    ffdc_db = HwpErrorDB()

    for xml in args.error_xml:
        ffdc_db.loadXml(xml, args.platform)

    ffdc_db.process()

    pickle.dump(ffdc_db, open(args.dbfile, "wb"))

def cmd_display(args):
    '''
    Just display the HwpErrorDB object parsed from input pickle dump
    '''
    ffdc_db : HwpErrorDB = pickle.load(open(args.dbfile, "rb"))

    ffdc_db.display()

def cmd_gensrc(args):
    '''
    parse input pickle dump and auto-generate source code for ffdc handling
    '''
    ffdc_db = pickle.load(open(args.dbfile, "rb"))

    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader(os.path.join(os.path.dirname(sys.argv[0]), "templates")),
        trim_blocks=True,
        lstrip_blocks=True,
        undefined=jinja2.StrictUndefined
    )
    env.globals = {
        "toolname": "fapiffdctool",
        "hwpErrorDB": ffdc_db
    }

    def generate(fname):
        with open(os.path.join(args.outdir, fname), "w") as f:
            # templates named .t to prevent them from being code-formatted
            f.write(env.get_template(fname + ".t").render())

    generate("hwp_return_codes.H")
    generate("hwp_ffdc_classes.H")
    generate("hwp_error_info.H")
    generate("hwp_reg_ffdc.H")
    generate("hwp_reg_ffdc.C")

if __name__ == '__main__':
    parser = ArgumentParser(description="Tools to generate code for error XMLs.")
    subparsers = parser.add_subparsers()

    sub = subparsers.add_parser("parse", help="Parse error XML and generate ffdc database")
    sub.add_argument("error_xml", nargs="+", help="error XML files")
    sub.add_argument("-d", "--dbfile", help="Name of the database to be written")
    sub.add_argument("-p", "--platform", choices=SUPPORTED_PLATFORMS,
                     help="Platform for which error xml is parsing")
    sub.add_argument("-v", "--verbose", action="store_true", help="Increase verbosity")
    sub.set_defaults(func=cmd_parse)

    sub = subparsers.add_parser("display", help="Display ffdc database")
    sub.add_argument("dbfile", help="Name of the ffdc database")
    sub.add_argument("-v", "--verbose", action="store_true", help="Increase verbosity")
    sub.set_defaults(func=cmd_display)

    sub = subparsers.add_parser("gensrc", help="Generate ffdc source code")
    sub.add_argument("dbfile", help="Name of the ffdc database")
    sub.add_argument("outdir", help="Output directory for generated source")
    sub.add_argument("-v", "--verbose", action="store_true", help="Increase verbosity")
    sub.set_defaults(func=cmd_gensrc)

    args = parser.parse_args()
    if not hasattr(args, "func"):
        parser.print_help()
        exit(1)

    try:
        args.func(args)
    except ArgumentError as e:
        print("Invalid argument: " + str(e))
        exit(1)
