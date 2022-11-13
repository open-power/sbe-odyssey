#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/tools/trace/tracehashwrap.py $
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

############################################################
# Includes
############################################################
import argparse
import textwrap
import os
import shutil
import glob
import subprocess
import tempfile

############################################################
# Variables
############################################################

# Tool Details
toolName = 'tracehash wrapper tool'
toolVersion = 0.1

g_temp_dir_path = '/builddir/'
g_tracehash_file = '/public/src/tools/trace/tracehash.pl'

############################################################
# Functions
############################################################

# Find the .hash file recursive of directories and return the list file
#   in case found the file
def collect_hash_file(path):
    hash_file = []
    for f in glob.glob(path + '/**/*.hash', recursive=True):
        hash_file += [f]
    return hash_file

#Run command function
def run_command(command):
    p = subprocess.Popen( command,
                          stdout=subprocess.PIPE,
                          shell=True)
    stdout = p.communicate()
    print(stdout[0].decode())

# Wrapper function is the main function does the all operations
def wrapper_func(args:argparse.Namespace):
    global g_temp_dir_path, g_tracehash_file
    # List argument logic to parse the list of for collect the .hash
    # Checking tempDir and tracehashTool args. if arg miss then look into SBEROOT env
    if not (args.tempDir and args.tracehashTool):
        # Raise the error in case SBEROOT env not set
        try:
            SBE_ROOT = os.environ['SBEROOT']
            if not args.tempDir:
                g_temp_dir_path = SBE_ROOT + g_temp_dir_path
            if not args.tracehashTool:
                g_tracehash_file = SBE_ROOT + g_tracehash_file
        except KeyError as SBEROOT:
            print("[ERROR] tempDir/tracehashTool arg missing. SBE Workon not set. Run sbe workon and re-run the tool...")
            print('[ERROR] Run "tracehashwrap.py -h" for usage')
            exit()
    else:
        g_temp_dir_path = args.tempDir
        g_tracehash_file = args.tracehashTool

    # Create the Temporary directory
    tempDir = tempfile.mkdtemp(dir = g_temp_dir_path, suffix=".temp")

    # Before pass output file to tracehash tool need to delete if exist
    if os.path.isfile(args.output):
        os.remove(args.output)

    # Looking the list of directories
    for dir in args.listDir:
        # Collect list of path file by finding the .hash file
        hash_file_path = collect_hash_file(dir)
        # Copy the .hash file into tmp folder in case hash file found
        if (len(hash_file_path)):
            for individual_hash_file_path in hash_file_path:
                shutil.copy(individual_hash_file_path, tempDir)

    # Run the trace hash tool
    run_command(g_tracehash_file + ' -c -FC -d ' + tempDir +' -s ' + args.output)

    # Remove the temporary directory before exit
    shutil.rmtree(tempDir, ignore_errors=True)

############################################################
# Main
############################################################
def main():
    print("\r\nRunning tracehashwrap tool...")

    argparser = argparse.ArgumentParser(
        description="Wrapper tool for tracehash tool",
        prog=toolName,
        add_help=False,
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=textwrap.dedent('''
        Version: ''' + str(toolVersion) + '''

        '''))

    # Configure arg functions
    argparser.set_defaults(func=wrapper_func)
    required = argparser.add_argument_group('required arguments')

    # Required argument
    required.add_argument('-l', '--listDir',
                           nargs='+',
                           required=True,
                           help='[IN] List of directories path to collect .hash files')

    required.add_argument('-o', '--output',
                           required=True,
                           help='[OUT] Output file name path of string file')

    # Optional arguments
    argparser.add_argument('-d', '--tempDir',
                        help='[IN] Path of Temporary directory. default: $SBEROOT/builddir/hashtempdir/,\
                            where $SBEROOT is bash environment variable')

    argparser.add_argument('-t', '--tracehashTool',
                        help='[IN] tracehash.pl tool path. default: $SBEROOT/builddir/hashtempdir/,\
                            where $SBEROOT is bash environment variable')

    argparser.add_argument('-v', '--version',
                           action='version',
                           version=textwrap.dedent('''
                           Version: ''' + str(toolVersion) + '''

                           '''))

    argparser.add_argument('-h', '--help',
                           action="help",
                           help="Show this help message and exit")

    args = argparser.parse_args()
    args.func(args)

# PYTHON MAIN
if __name__ == "__main__":
    main()
    exit()