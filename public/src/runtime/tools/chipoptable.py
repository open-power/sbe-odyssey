#!/usr/bin/env python3
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/tools/chipoptable.py $
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
import enum
import json
import os
import argparse
import copy
import sys
from enum import Enum
from jinja2 import Template


#User Provided
tmp_path = os.path.dirname(os.path.realpath(__file__))
tmp_path2 = os.path.expandvars("$SBEROOT")+"/public/src/tools/utils/py/"
sys.path.append(os.path.join( tmp_path, tmp_path2  ))
import templateUtil


#####TEMPLATES#####
chipop_default_cmd_class_init ="""
#ifndef __CMD_DEFAULT_INIT_H__
#define __CMD_DEFAULT_INIT_H__
#include "chipop_handler.H"

#define HASH_KEY(key)\\
        $$$@@@ for entry in break_class -@@@$$$
        $$$@@@ if not loop.last -@@@$$$
        (0x$@ entry.cmd_class @$ == key) ? size_t($@ entry.index @$):\\
        $$$@@@ else @@@$$$ size_t($@ entry.index @$)\\$$$@@@- endif @@@$$$
        $$$@@@- endfor@@@$$$

#define CMD_CLASS_LIST \\
constexpr cmdClass_t cmdClassArr[] = { \\
$$$@@@ for cmd_cls in command_classes @@@$$$CMD_CLASS($@cmd_cls@$), \\
$$$@@@endfor@@@$$$}; \\


#define CMD_CLASS_DEFAULT_INTIALISATION \\
CMD_CLASS_LIST \\
CMD_CLASS_COMMON \\


#endif
"""

chipop_table_template = """
#include "fences.H"
#include "cmd_class_init.H"
#include "chipop_handler.H"
#include <stdint.h>
#include <stddef.h>$$$@@@ for hdrFile in headers @@@$$$
#include "$@ hdrFile @$"$$$@@@endfor@@@$$$

$$$@@@ for cmd_cls, cmd_info in command_arr.items() @@@$$$
CMD_ARR($@cmd_cls@$,
    $$$@@@ for v in cmd_info @@@$$$
    {
    $@v.funcName@$,
    $@v.opcode@$,
    $@v.fences|join("|")@$,
    }$$$@@@ if not loop.last @@@$$$,$$$@@@ endif @@@$$$ $$$@@@endfor@@@$$$
)
$$$@@@endfor@@@$$$
CMD_CLASS_DEFAULT_INTIALISATION

"""

#####CLASSES#####
class FenceData(Enum):
    """
    This class is a blueprint for all the fence data which are enums
    """
    #####CONSTRUCTOR#####
    def __init__(self, fence_name):
        '''
        Constructor for FenceData
        '''
        self.fence_name = fence_name

    #####GETTERS#####
    def getFenceName(self):
        """
        Getter for fence name
        """
        return self.fence_name


class ChipopData():
    """
    This class is a blueprint for chipop objects
    used to generate chipops from a combined chipop data
    """
    def __init__(self, **chipop_data:dict ):
        """
        Constructor for chipop data
        """
        #####CONSTRUCTOR#####
        if (( {"cmdClass", "command"}).issubset(chipop_data.keys()) ):
            self.cmdClass = chipop_data["cmdClass"]
            self.command = chipop_data["command"]
            self.function_name = chipop_data.get('function', '')
            self.headerFile = chipop_data.get('headerFile','')
            self.override = chipop_data.get("override", False)

            if("fences" not in chipop_data.keys()):
                self.fences = list()
                self.fences.append("SBE_NO_FENCE")
            else:
                self.fences = chipop_data["fences"]
        else:
            raise ArgumentError("There is format or argument mismatch in your json file")

    #####GETTERS#####
    def getOverride(self):
        """
        Getter for override attribute (override)
        """
        return self.override

    def getCmdClass(self):
        """
        Getter for command class attribute (cmdClass)
        """
        return self.cmdClass

    def getCommand(self):
        """
        Getter for command attribute (command)
        """
        return self.command

    def getFunctionName(self):
        """
        Getter for function name attribute (function_name)
        """
        return self.function_name

    def getFences(self):
        """
        Getter for fences attribute (fences)
        """
        return self.fences

    def getHeaderFile(self):
        """
        Getter for headerFile (headerFile)
        """
        return self.headerFile

    ######SETTERS#####
    def setOverride(self, override):
        """
        Setter for override attribute (override)
        """
        self.override = override

    def setCmdClass(self, cmdClass):
        """
        Setter for command class attribute (cmdClass)
        """
        self.cmdClass = cmdClass

    def setCommand(self, command):
        """
        Setter for command attribute (command)
        """
        self.command = command

    def setFunctionName(self, function_name):
        """
        Setter for function name  attribute (function_name)
        """
        self.function_name = function_name

    def setFences(self, fences):
        """
        Setter for fences attribute (fences)
        """
        self.fences = fences

    #####FUNCTIONS#####
    def updateChipop(self, new_entry:'ChipopData'):
        """
        this function can be used for
        updating and filtering chipop data
        """
        if new_entry.getOverride() == False:
            raise Exception("Redefining chipop without override flag")
        if new_entry.getFences() != None:
            self.setFences(new_entry.getFences())
        if len(new_entry.getFunctionName())!= 0:
            self.setFunctionName(new_entry.getFunctionName())

class FenceConflictHandler():
    """
    Class for handling conflict in fences
    Such as no two fences can have same data,
    Same fence name can't be used for two different fence value
    There should be a one to one mapping
    from a fence name to its value in combined list of fences
    """
    #####CONSTRUCTOR#####
    def __init__(self, combined_fences):
        """
        constructor for fence data
        """
        self.combined_fences = combined_fences

    #####GETTER#####
    def getCombinedFences(self)->dict:
        """
        Getter for combined fences attribute (combined_fences)
        """
        return self.combined_fences

    #####FUNCTIONS#####
    def _checkIthConflict(self, i:int, fence_values:list)->bool:
        """
        Utility function to check if ith bit is conflicting
        in the list of combined fences
        """
        count_ith_bit = 0
        for fence in fence_values:
            #count number of fences with ithbit set
            ith_bit = int(fence,16)& (1 << (i))
            if(ith_bit!=0):
                count_ith_bit += 1
            if(count_ith_bit>1):
                print("conflict at fence {}", hex(fence))
                return True
        return False

    def checkFenceConflict(self)->bool:
        """
        Function to check if we have conflicting fences or not
        """
        fence_values = self.getCombinedFences().values()
        if(len(set(fence_values)) != len(fence_values)):
            return False
        for i in range(0,16):
            if(self._checkIthConflict(i, fence_values)):
                return True
        return False

class JsonExtractor():
    """
    Extractor class to get data from json files
    into unified data structures
    """
    #####CONSTRUCTOR#####
    def __init__(self, chipop_json_list:list):
        """
        Constructor for Json Extractor class
        every json will have a field of chipoptable and fenceList
        """
        self.chipop_json_list = list(chipop_json_list)
        self.chipoptable = list()
        self.fence_list = dict()
        self.combined_data = dict()

    #####GETTER#####
    def getChipopJsonList(self)->dict:
        """
        Getter for list of chipop json files attribute (chiop_json_list)
        """
        return self.chipop_json_list

    def getChipopTableData(self)->list:
        """
        Getter for chipop data attribute (chipop_data)
        Json field 'chipoptable'
        """
        return self.chipoptable

    def getFenceData(self)->dict:
        """
        Getter for fence data attribute (fence_data)
        Json field 'fenceList'
        """
        return self.fence_list

    #####SETTERS#####
    def setFenceData(self, fenceList):
        """
        Setter for fence list attribute (fence_list)
        Json field fenceList
        """
        if(self.getFenceData() == None):
            self.fence_list = fenceList
        elif(fenceList != None and fenceList.keys() != self.getFenceData().keys()):
            self.fence_list.update(fenceList)
        else:
            raise Exception("something wrong with fence list")

    def getCombinedData(self)->dict:
        """
        Getter for combined data attribute (combined_data)
        """
        return self.combined_data

    #####FUNCTIONS#####
    def validateChipopData(self)->bool:
        """
        Validating chipop functionality to check
        that all required fields are there in json file
        """
        necessary_fields = {"cmdClass", "command"}
        for chipop in self.getChipopTableData():
            if(not necessary_fields.issubset(chipop.keys()) ):
                return False
        return True

    def extractJson(self):
        """
        Function to populate data structures
        from json file having chipop information
        """
        for i_json_file in self.getChipopJsonList():
            try:
                if(os.path.exists(i_json_file)):
                    with open(i_json_file, "r") as file_pointer:
                        try:
                            data = json.load(file_pointer)
                            self.getChipopTableData().extend(data["chipoptable"])
                        except ValueError as err:
                            print(err)
                            print("invalid json data in {}".format(file_pointer.name))
                            sys.exit(1)
            except Exception as e:
                 print("File Path {} doesn't exist".format(i_json_file))
                 sys.exit(1)

    def createChipopCombined(self)->dict:
        """
        Function to create combined chipop data
        """
        # creating temporary data structure to process
        if(not self.validateChipopData()):
            print("Chipop data is not valid {}".format(self.getChipopTableData()))
            sys.exit(1)
        temp_chipop_data = {}
        for chipop in self.getChipopTableData():
            combined_command = chipop["cmdClass"]+"#"+chipop["command"]
            try:
                if(combined_command not in temp_chipop_data.keys()):
                    temp_chipop_data[combined_command] = ChipopData(**chipop)
                else:
                    temp_chipop_data[combined_command].updateChipop(ChipopData(**chipop))
            except ArgumentError as e:
                print("Error in initializing chipop {}".format(chipop) )
                sys.exit(1)
        chipop_data = temp_chipop_data.values()
        try:
            for chipop in chipop_data:
                if(chipop.getFunctionName() is None or len(chipop.getFunctionName())==0):
                    raise ValueError("Chipop doesnot have a function name {} ".format(chipop))
                else:
                    return chipop_data
        except Exception as e:
            print(e)
            sys.exit(1)

class FileGeneration():
    """
    Class responsible for creating the C files
    and reference json file from combined data
    """
    #####CONSTRUCTOR#####
    def __init__(self, dir_path, file_name):
        """
        Constructor for File generation
        """

        if(os.path.exists(dir_path)):
            self.dir_path = dir_path
        else:
            raise Exception("{} does not exist".format(dir_path))
        self.file_name = file_name
        self.file_path = dir_path+"/"+file_name

    #####GETTER#####
    def getDirPath(self)->str:
        """
        Getter for directory path of file to be generated attribute (dir_path)
        """
        return self.dir_path

    def getFileName(self)->str:
        """
        Getter for name of the file being generated attribute (file_name)
        """
        return self.file_name

    def getFilePath(self)->str:
        """
        Getter for path of generated file (file_path)
        """
        return self.file_path

    #####FUNCTIONS#####

    def generateCFile(self, chipops_data:'list[ChipopData]')->dict:
        """
        Function to generate chipop C file
        """
        sorted_cmd_classes = list({chipop.getCmdClass() for chipop in chipops_data})
        sorted_cmd_classes.sort()
        headerFiles = list({chipop.getHeaderFile() for chipop in chipops_data })
        headerFiles.sort()
        my_json_combined = []
        for chipop in chipops_data:
            del chipop.__dict__["override"]
            my_json_combined.append(chipop.__dict__)
        cmd_arr ={}
        for chipop in chipops_data:
            cmd_class = chipop.getCmdClass()
            if cmd_class not in cmd_arr:
                cmd_arr[cmd_class]= list()
            t = {"funcName":chipop.getFunctionName(),
                 "opcode":chipop.getCommand(),
                 "fences":chipop.getFences()}
            cmd_arr[cmd_class].append(t)
        template_c = templateUtil.customTemplate(chipop_table_template)
        items = template_c.render(command_arr = cmd_arr, headers = headerFiles)
        # write to file
        with open(self.getFilePath(), "w") as file_pointer:
            file_pointer.write(items)
        print("{} file generated in {}".format(
                                        self.getFileName(),self.getDirPath()))
        return  my_json_combined, sorted_cmd_classes

    def getHashKey(self, sorted_cmd_class):
        hashKeyIndex = []
        for index, cmd_class in enumerate(sorted_cmd_class):
            hashKeyIndex.append({"cmd_class":cmd_class, "index":index})
        return hashKeyIndex

    def generateCmdClassDefaultInit(self, sorted_cmd_classes:'list[str]')->dict:
        """
        Function to generate chipop C file
        """
        print(sorted_cmd_classes)
        template_h = templateUtil.customTemplate(chipop_default_cmd_class_init)
        hashKeyIndices = self.getHashKey(sorted_cmd_classes)
        items = template_h.render(command_classes = sorted_cmd_classes,
        break_class = hashKeyIndices)
        with open(self.getFilePath(), "w") as file_pointer:
            file_pointer.write(items)
        return  sorted_cmd_classes


    def generateReferenceJson( self, my_json_combined:json):
        """
        Function to generate combined reference json file
        """
        with open(self.getFilePath(),"w") as file_pointer:
            file_pointer.write("""{\"usage\":\"This is a reference file\",
                                \n\"combined_data\":""")
            file_pointer.write(json.dumps(my_json_combined, indent=2))
            file_pointer.write("\n}")
        print("Combined reference {} is generated in {}".format(
            self.getFileName(), self.getDirPath()))



#########MAIN SECTION############

if __name__ == "__main__":
    # get arguments
    parser = argparse.ArgumentParser(
             description = "Generate Chipop table from a list of json files"
             )

    parser.add_argument("-d","--genfile_dir",
                       type=str,
                       help = """path of generated output files,
                       path needs to be absolute path""", required = True)

    parser.add_argument("-c","--chipop_list",
                        nargs = '+',
                        help = """list of chipop json files,
                        list of json should be in hierarchical order, ie,
                        common json first""",
                        required = True)

    args = parser.parse_args()
    platform_genfile_path = args.genfile_dir
    chipop_json_list = args.chipop_list
    # json extraction
    chipop_json_object = JsonExtractor(chipop_json_list)
    chipop_json_object.extractJson()

    # create a filtered chipop data list from multiple jsons
    chipop_data = chipop_json_object.createChipopCombined()

    # generating the files
    # generate chipop_table.C - this class has CMD_ARR's for differenct chipops
    file_gen_object= FileGeneration(platform_genfile_path, '/chipop_table.C')
    my_json_combined,sorted_cmd_class = file_gen_object.generateCFile(chipop_data)

    # generate cmd_class_init.H - this class has default initialization for command class
    cmd_class_default_init = FileGeneration(platform_genfile_path, '/cmd_class_init.H')
    cmd_class_default_init.generateCmdClassDefaultInit(sorted_cmd_class)

    # generate chipop_reference.json - this file is for debugging purpose
    reference_json_file_obj = FileGeneration(platform_genfile_path,
                                             '/chipop_reference.json')
    reference_json_file_obj.generateReferenceJson(my_json_combined)
