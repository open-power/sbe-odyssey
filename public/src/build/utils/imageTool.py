#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/utils/imageTool.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2023
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

import argparse
import logging
import os
import shutil
import subprocess
import sys

toolDir = os.path.abspath(os.path.dirname(sys.argv[0]))
signHashTool = os.path.join(toolDir, 'signHashList')
measuredHashTool = os.path.join(toolDir, 'genMeasuredHash.sh')

signPakCompId = {
    "boot": "BOOT_LDR",
    "rt":   "RUN_TIME"
}
logLevel = {
    "D" : logging.DEBUG,
    "I" : logging.INFO,
    "W" : logging.WARNING,
    "E" : logging.ERROR,
    "C" : logging.CRITICAL
}

# TODO Find better way to get below hardcoded value
signedCodeContainerMetaData = {
        # Based on the Signed_Code_Container v2_4
        "v_2_4" : {
            "hwSigALen" : 132,
            "hwSigDLen" : 4668,
            # Offset defined from first byte
            "hwSigStartOffset" : 2596,
            "fwSigPLen" : 132,
            "fwSigSLen" : 4668,
            # Offset defined from first byte
            "fwSigStartOffset" : 9992
            }
        }

logger = logging.getLogger('imageTool')

def runCmd(pipeCmd:str, fail:bool=True):

    logger.debug(pipeCmd)
    resp = subprocess.Popen(pipeCmd, shell=True, stdin=subprocess.PIPE,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.STDOUT)
    stdout, stderr = resp.communicate()
    if resp.returncode != 0 and fail:
        err = f"{pipeCmd}\n\t==> ErrorMsg: {stdout}"
        logger.error(err)
        raise Exception(err)

def exists(archive, filename:str):

    if not filename:
        return False

    try:
        result = archive.find(filename)
        for entry in result:
            if entry.name == filename:
                return True
        return False
    except:
        return False

def genHashList(archive, excludeFiles:list, \
                hashListFile:str):

    # Returns all file entries from pak file
    entries = archive.find("")
    if excludeFiles is not None:
        hashEntries = [entry for entry in entries \
                             if entry.name not in excludeFiles]
    else:
        hashEntries = entries

    logger.debug("Creating hashes...")
    hashAlgorithm="sha3_512"
    for entry in hashEntries:
        logger.debug(f"\t{entry.name}")
        entry.hash(hashAlgorithm)

    with open(hashListFile, "wb") as fd:
        logger.debug(f"Writing hashes into {hashListFile}")
        hashes = archive.createHashList(hashAlgorithm)
        fd.write(hashes)

def extractFile(archive, files:list, pathToStore:str):

    entries = archive.find(files)
    logger.debug("Extracting...")
    for entry in entries:
        logger.debug(f"\t{entry.name}")
        ename = os.path.join(pathToStore, entry.name)
        os.makedirs(os.path.dirname(ename), exist_ok=True)
        with open(ename, "wb") as fd:
            fd.write(entry.ddata)

def removeFile(archive, files:list):

    entries = archive.find(files)
    logger.debug("Removing files...")
    for entry in entries:
        logger.debug(f"\t{entry.name}")
        archive.remove(entry)

    archive.save()

def signPak(args:argparse.Namespace):

    for pakName, pakFile in args.pakFiles.items():
        try:
            logger.info(f"Processing \'{pakName}\' pak file")

            signPakWorkDir = os.path.join(os.path.abspath(os.path.dirname( \
                                          pakFile)), "signPak", pakName)
            shutil.rmtree(signPakWorkDir, ignore_errors=True)
            os.makedirs(signPakWorkDir)

            archive = pak.Archive(pakFile)
            archive.load()

            hashList = os.path.join(pakName, "hash.list")
            hashListFile = os.path.join(signPakWorkDir, "hash.list")
            secureHdr = os.path.join(pakName, "secure.hdr")
            secureHdrFile = os.path.join(signPakWorkDir, "secure.hdr")
            hwKeysHash = os.path.join(pakName, "hwkeyshash.bin")
            measuredHash = os.path.join(pakName, "measured.hash")

            if exists(archive, hashList) and not exists(archive, secureHdr):
                extractFile(archive, hashList, os.path.dirname(signPakWorkDir))
            else:
                # Generate hash.list
                if exists(archive, hashList):
                    removeFile(archive, hashList)
                if exists(archive, secureHdr):
                    removeFile(archive, secureHdr)
                if exists(archive, hwKeysHash):
                    removeFile(archive, hwKeysHash)
                if exists(archive, measuredHash):
                    removeFile(archive, measuredHash)

                excludeFiles = None
                if args.excludeFiles is not None and pakName in args.excludeFiles:
                    excludeFiles = args.excludeFiles[pakName]

                genHashList(archive, excludeFiles, hashListFile)

            # Generate secure.hdr by using hash.list
            genSecureHdr = f"{signHashTool} -s {signPakWorkDir}/scratch \
                            -i {hashListFile} -o {signPakWorkDir} \
                            -c {signPakCompId.get(pakName)} \
                            -f {args.signMode} -S {args.secureVersion}"
            runCmd(genSecureHdr)

            # Generate measured.hash file by using generated secure.hdr file
            genMeasuredHash = f"{measuredHashTool} -i {secureHdrFile} \
                               -o {signPakWorkDir} -f measured"
            runCmd(genMeasuredHash)

            # Go to working "pakName" directory to add file into the respective
            # pak file because the pak tool not providing a way to pass dest
            # path to store into the pak file, it creating with the given
            # file absolute path.
            os.chdir(os.path.dirname(signPakWorkDir))

            addReqFiles = f"{pakTool} add {pakFile} {pakName} --method store"
            runCmd(addReqFiles)

            os.chdir(toolDir)
        except Exception as e:
            # Exception will be logged in last exception handler
            raise
        finally:
            shutil.rmtree(os.path.dirname(signPakWorkDir))

def removeSig(archive, secureHdr:str):

    entries = archive.find(secureHdr)
    secureHdrData = bytearray(entries[0].ddata)
    archive.remove(entries[0])

    signCodeContMetaData = signedCodeContainerMetaData.get("v_2_4")

    hwSigTotalLen = signCodeContMetaData.get("hwSigALen") + \
                    signCodeContMetaData.get("hwSigDLen")
    fwSigTotalLen = signCodeContMetaData.get("fwSigPLen") + \
                    signCodeContMetaData.get("fwSigSLen")

    secureHdrData[signCodeContMetaData.get("hwSigStartOffset") : \
                  hwSigTotalLen] = bytearray(hwSigTotalLen)
    secureHdrData[signCodeContMetaData.get("fwSigStartOffset") : \
                  fwSigTotalLen] = bytearray(fwSigTotalLen)

    archive.add(secureHdr, pak.CM.store, secureHdrData)
    archive.save()

def pakHash(args:argparse.Namespace):

    for pakName, pakFile in args.pakFiles.items():
        try:
            logger.info(f"Processing \'{pakName}\' pak file")

            pakHashWorkDir = os.path.join(os.path.abspath(os.path.dirname( \
                                         pakFile)), "pakHash", pakName)
            shutil.rmtree(pakHashWorkDir, ignore_errors=True)
            os.makedirs(pakHashWorkDir)

            archive = pak.Archive(pakFile)
            archive.load()
            bkpArchive = pak.Archive(pakFile)
            bkpArchive.load()

            secureHdr = os.path.join(pakName, "secure.hdr")
            secureHdrFile = os.path.join(pakHashWorkDir, "secure.hdr")
            pakHash = os.path.join(pakName, "image.hash")
            pakHashFile = os.path.join(pakHashWorkDir, "image.hash")

            if exists(archive, secureHdr):
                removeSig(archive, secureHdr)

            if args.excludeFiles is not None and pakName in args.excludeFiles:
                logger.debug("Exclude files for image hash")
                removeFile(archive, args.excludeFiles[pakName])

            logger.debug("Generate image.hash")
            genImgHash = f"{args.openSSLTool} dgst -sha3-512 {pakFile} | \
                           awk \'{{print $2}}\' | \
                           xxd -r -p > {pakHashFile}"
            runCmd(genImgHash)

            # Save backup archive so that we can get original secure.hdr
            # signature and excluded files
            if exists(archive, secureHdr) or \
               args.excludeFiles is not None and pakName in args.excludeFiles:
                bkpArchive.save()

            # Go to working "pakName" directory to add file into the respective
            # pak file because the pak tool not providing a way to pass dest
            # path to store into the pak file, it creating with the given
            # file absolute path.
            os.chdir(os.path.dirname(pakHashWorkDir))

            addImgHash = f"{pakTool} add {pakFile} {pakHash} --method store"
            runCmd(addImgHash)

            os.chdir(toolDir)
        except Exception as e:
            # Exception will be logged in last exception handler
            raise
        finally:
            shutil.rmtree(os.path.dirname(pakHashWorkDir))

class ExcludeFileList(argparse.Action):

    def __call__( self , parser, namespace,
            values, option_string = None):
        setattr(namespace, self.dest, dict())

        for value in values:
            pakName, files = value.split('=')
            fileList = files.split(',')

            getattr(namespace, self.dest)[pakName] = fileList

class SignPakList(argparse.Action):

    def __call__( self , parser, namespace,
            values, option_string = None):
        setattr(namespace, self.dest, dict())

        for value in values:
            pakName, pakPath = value.split('=')

            if pakName not in signPakCompId:
                err = f"The \'{pakName}\' pak name is not found in defined \
                        component id list \'{signPakCompId.keys()}\' for sign"
                raise ValueError(err)

            if not os.path.exists(pakPath):
                raise ValueError(pakPath + " is not exists")

            getattr(namespace, self.dest)[pakName] = pakPath

class PakList(argparse.Action):
    def __call__( self , parser, namespace,
            values, option_string = None):
        setattr(namespace, self.dest, dict())

        pakHashList = ["boot", "rt"]

        for value in values:
            pakName, pakPath = value.split('=')

            if pakName not in pakHashList:
                err = f"The \'{pakName}\' pak name is not found in defined \
                        pak hash list \'{pakHashList}\' \
                        for pak hash generation"
                raise ValueError(err)

            if not os.path.exists(pakPath):
                raise ValueError(pakPath + " is not exists")

            getattr(namespace, self.dest)[pakName] = pakPath

parser = argparse.ArgumentParser(description="Use to add necessary files "
            "required for verification and check for mismatch of code update "
            "pak files")

parser.add_argument("--pakToolDir", required=True, help="Pass PAK tools \
                    directory")
parser.add_argument("-l", "--loglevel", default="E", choices=['D', 'I', \
                    'W', 'E', 'C'], help="Pass log level to get more traces. "
                    "Supported log levels: D-Debug, I-Info, W-Warning, "
                    "E-Error, C-Critical. (default: %(default)s)")

subparsers = parser.add_subparsers()

# Add "signPak" sub command
subCmd = subparsers.add_parser("signPak", description="Sign given pak files",
            formatter_class=argparse.RawDescriptionHelpFormatter)

# Add "signPak" sub command arguments
subCmd.add_argument("--pakFiles", nargs="+", action=SignPakList, required=True,
                                  metavar="pakName=pakFile",
                                  help="List of pak files to be signed")
subCmd.add_argument("--excludeFiles", nargs="*", action=ExcludeFileList,
                                      metavar="pakName=excFile1,a/excFile2",
                                      help="List of files to exclude to " \
                                      "generate hash list")
subCmd.add_argument("--signMode", choices=['Development', 'Production'],
                                  default='Development', help="Signing mode "
                                  "(default: %(default)s)")
subCmd.add_argument("--secureVersion", default='0', help="Security version "
                                       "to sign (default: %(default)s)")
subCmd.set_defaults(func=signPak)

# Add "pakHash" sub command
subCmd = subparsers.add_parser("pakHash", description="Generate hash for given "
            "pak files", formatter_class=argparse.RawDescriptionHelpFormatter)

# Add "pakHash" sub command arguments
subCmd.add_argument("--pakFiles", nargs="+", action=PakList, required=True,
                                  metavar="pakName=pakFile",
                                  help="List of pak files to be hashed")
subCmd.add_argument("--excludeFiles", nargs="*", action=ExcludeFileList,
                                      metavar="pakName=excFile1,a/excFile2",
                                      help="List of files to exclude to " \
                                      "pak hash")
subCmd.add_argument("--openSSLTool", default='/gsa/rchgsa/home/c/e/cengel/' \
                                     'signtool/RHEL7/openssl-1.1.1n/apps/openssl',
                                     help="Pass OpenSSL tool path for pak hash."
                                     " (default: %(default)s)")
subCmd.set_defaults(func=pakHash)

args = parser.parse_args()
if not hasattr(args, "func"):
    parser.print_help()
    sys.exit(1)

#FIXME Not recommanded way, we should import module at begining but, pak tools
# path is part of arguments so importing after parse_args()
sys.path.append(os.path.join(args.pakToolDir, "pymod"))
import pakcore as pak

# Setup Log infra
logging.basicConfig(level=logLevel.get(args.loglevel),
                   format='%(asctime)s | %(name)s | %(levelname)s | \
                           %(message)s', \
                   datefmt='%d-%b-%y %H:%M:%S')
pakTool = os.path.join(args.pakToolDir, 'paktool')

# Call the function defined for each sub command
try:
    args.func(args)
except Exception as e:
    logger.exception("An exception was thrown")
    raise
