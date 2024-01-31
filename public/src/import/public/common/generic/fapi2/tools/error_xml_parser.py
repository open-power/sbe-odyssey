# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/common/generic/fapi2/tools/error_xml_parser.py $
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

'''@package error_xml_parser
This file contains logic to parse an error xml with following sample format.

<hwpErrors>
  <hwpError>
    <rc>RC_MSS_CHANNEL_MIRROR_SETTINGS_MISMATCH_RANK0</rc>
    <description>
       Mirroring settings mismatch between channels in DDR5 SPD byte 365
    </description>
    <ffdc>PORT_TARGET</ffdc>
    <ffdc>VALUE_CHA_CS0</ffdc>
    <ffdc>VALUE_CHB_CS0</ffdc>
    <callout>
      <target>PORT_TARGET</target>
      <priority>MEDIUM</priority>
    </callout>
    <deconfigure>
      <target>OCMB_TARGET</target>
    </deconfigure>
    <callout>
      <hw>
        <hwid>VPD_PART</hwid>
        <refTarget>OCMB_TARGET</refTarget>
      </hw>
      <priority>HIGH</priority>
    </callout>
  </hwpError>
</hwpErrors>

Above XML will be parsed to a python object tree like below (only few is shown)
   (HwpErrorDB)
      |- hwp_errors (list of HwpError)
             |- rc = RC_MSS_CHANNEL_MIRROR_SETTINGS_MISMATCH_RANK0
             |- description
             |- ffdc (list of str)
                     |- 'PORT_TARGET'
                     |- 'VALUE_CHA_CS0'
                     |- 'VALUE_CHB_CS0'


Apart from simple parsing , it has some validation and post-processing logic
  also, like
  1. hwpError should have a non-empty RC
  2. It will calculate md5 checksum of each rc, and each rc should have
     unique value
  3. It will check whether any unexpected tag present in the xml.
  4. description will be converted to a single line string.
  etc.

  TODO (JIRA PFSBE-906): validate the tags used inside callout/guard/deconfig is a valid ffdc.


An example of how to use this module can be found in fapiffdctool.cmd_parse()
'''


#standard library imports
from xml.dom.minidom import Element
import xml.etree.ElementTree as etree
import hashlib

# As of now only supported for SBE platform
SUPPORTED_PLATFORMS = ['SBE']

class ParseError(Exception):
    '''
    A new type of Exception that can be thrown for parsing related error.
    '''
    pass

class CollectRegisterFfdc(object):
    '''
    Class to parse 'collectRegisterFfdc' tag in error xml and create an instance
    of this class, which models all properties of xml tag.
    '''

    def __init__(self, i_node:Element):
        '''
        Constructor which will parse the input XML node.
        '''
        id, targetType, target = (
            i_node.find(tag) for tag in ("id", "targetType", "target"))

        self.id = id.text
        self.target_type = targetType.text
        self.target = target.text

        i_node.remove(id)
        i_node.remove(targetType)
        i_node.remove(target)

        if(len(i_node)):
            raise ParseError(
                f"There are unprocessed nodes {list(i_node)} in {self.id}")

    def process(self, i_parent_rc:'HwpError'):
        '''
        Find out the index of LV-FFDC for the target mentioned in 'collectRegisterFfdc'
        '''
        self.target_index = i_parent_rc.ffdc.index(self.target)
        self.id_hash = i_parent_rc.db.register_ffdcs[self.id].hash

    @property
    def id_hash_hex(self) -> str:
        return "0x%08x" % self.id_hash

    def display(self):
        from icecream.icecream import ic

        temp = ic.enabled
        try:
            ic.enable()
            ic('---------------------------------------------')
            ic(self.id)
            ic(self.target_type)
            ic(self.target)
            ic(self.target_index)
        finally:
            ic.enabled = temp

class HwpError(object):
    '''
    Class to parse 'hwpError' tag in error xml and create an instance
    of this class, which models all properties of xml tag.
    '''

    def __init__(self, i_db:'HwpErrorDB', i_node:Element, i_is_poz:bool):
        '''
        Constructor which will parse the input XML node.

        There can be a file level tag 'poz', which need to be applied to all hwpError
        in that file.
        '''
        self.db = i_db
        self.is_poz = i_is_poz
        self.is_sbeError = False
        self.is_platScomFail = False

        rc, description = (
            i_node.find(tag) for tag in ("rc", "description"))

        if rc is None:
            raise ParseError("Invalid error node")
        self.rc = rc.text
        i_node.remove(rc)

        sbeError = i_node.find('sbeError')
        if sbeError is not None:
            self.is_sbeError = True
            i_node.remove(sbeError)

        if description is None:
            self.description = ''
        else:
            self.description = description.text
        i_node.remove(description)

        platScomFail = i_node.find('platScomFail')
        if platScomFail is not None:
            self.is_platScomFail = True
            i_node.remove(platScomFail)

        ffdc = i_node.findall('ffdc')
        # this has to be a list, since we might be using this to parse the ffdc
        self.ffdc : list[str] = []
        for f in ffdc:
            self.ffdc.append(f.text)
            i_node.remove(f)

        collectRegisterFfdc = i_node.findall('collectRegisterFfdc')
        self.collect_reg_ffdc : list[CollectRegisterFfdc] = []
        for crf in collectRegisterFfdc:
            self.collect_reg_ffdc.append(CollectRegisterFfdc(crf))
            i_node.remove(crf)

        buffer = i_node.findall('buffer')
        for b in buffer:
            # TODO: parse buffer
            i_node.remove(b)

        callout = i_node.findall('callout')
        for c in callout:
            # TODO: parse callout info
            i_node.remove(c)

        deconfigure = i_node.findall('deconfigure')
        for d in deconfigure:
            # TODO: parse deconfigure info
            i_node.remove(d)

        # since some xml is using deconfig
        deconfigure = i_node.findall('deconfig')
        for d in deconfigure:
            # TODO: parse deconfigure info
            i_node.remove(d)

        gard = i_node.findall('gard')
        for g in gard:
            # TODO: parse gard info
            i_node.remove(g)

        if(len(i_node)):
            raise ParseError(
                f"There are unprocessed nodes {list(i_node)} in {self.rc}")

    def processDescription(self):
        '''
        convert the 'description' to a c/c++ compatible
        string literal.
         1. make the multi-line string to single line string.
         2. add escape character for double quote
         3. raise an error if '%' character present.
        '''
        str_lst = self.description.split()
        self.description = " ".join(str_lst).strip()

        self.description = self.description.replace('"', '\\"')

        if(self.description.find('%') != -1):
            raise ParseError(
                f"description for {self.rc} has invalid character '%'")

    def process(self):
        '''
        Post-process the XML data.
          1. Calculate 24-bit hash of each rc.
          2. Confirm there is no duplicate hash.
          3. If 'platScomFail' tag present, then add additional lv-ffdcs.
        '''
        hash16bytes = hashlib.md5(self.rc.encode()).digest()
        hash24bits = int.from_bytes(hash16bytes[0:3], "big")
        if hash24bits in self.db.hwp_err_hash_set:
            raise ParseError("Hash for hwp error " + self.rc +
                    " already used")
        else:
            self.hash = hash24bits
            self.db.hwp_err_hash_set.add(hash24bits)

        self.processDescription()

        if(self.is_platScomFail):
            self.ffdc.append('address')
            self.ffdc.append('pcb_pib_rc')

        for crf in self.collect_reg_ffdc:
            crf.process(self)

    @property
    def hash_hex(self) -> str:
        return "0x%06x" % self.hash

    @property
    def ffdc_len(self) -> int:
        return len(self.ffdc)

    @property
    def hasCollectRegister(self) -> bool:
        return (len(self.collect_reg_ffdc) > 0)

    @property
    def reg_ffdc_size_in_bytes(self) -> int:
        ret_size = 0

        for crf in self.collect_reg_ffdc:
            ret_size += 3 * 4 # size for ffdc-id, target-position and size fields

            num_of_scoms = self.db.register_ffdcs[crf.id].scom_list_len

            # Since this tool now only support scom reg ffdc, and each scom data
            #  will be 8-byte length
            ret_size += num_of_scoms * 8

        return ret_size

    def display(self):
        from icecream.icecream import ic

        temp = ic.enabled
        try:
            ic.enable()
            ic('=============================================')
            ic(self.rc)
            ic('---------------------------------------------')
            ic(self.is_poz)
            ic(self.description)
            ic(self.ffdc)

            for crf in self.collect_reg_ffdc:
                crf.display()
        finally:
            ic.enabled = temp

class RegisterFfdc(object):
    '''
    Class to parse 'registerFfdc' tag in error xml and create an instance
    of this class, which models all properties of xml tag.
    '''

    def __init__(self, i_db:'HwpErrorDB', i_node:Element):
        '''
        Constructor which will parse the input XML node.
        '''
        self.db = i_db

        id = i_node.find('id')

        if id is None:
            raise ParseError("Invalid register ffdc node")
        self.id = id.text
        i_node.remove(id)

        cfamregs = i_node.findall('cfamRegister')
        for reg in cfamregs:
            #TODO: parse cfam registers
            i_node.remove(reg)

        self.scom_list = []
        scomregs = i_node.findall('scomRegister')
        for reg in scomregs:
            self.scom_list.append(reg.text)

            i_node.remove(reg)

        if(len(i_node)):
            raise ParseError(
                f"There are unprocessed nodes {list(i_node)} in {self.id}")

    def process(self) -> None:
        '''
        Post-process the XML data.
          1. Calculate 32-bit hash of each reg-ffdc-id.
          2. Confirm there is no duplicate hash.
        '''
        hash16bytes = hashlib.md5(self.id.encode()).digest()
        hash32bits = int.from_bytes(hash16bytes[0:4], "big")
        if hash32bits in self.db.hwp_err_hash_set:
            raise ParseError("Hash for register ffdc " + self.id +
                    " already used")
        else:
            self.hash = hash32bits
            self.db.hwp_err_hash_set.add(hash32bits)

        # confirm there is no duplicate scoms
        if(len(set(self.scom_list)) != self.scom_list_len):
            raise ParseError(f"Duplicate scoms present in {self.id}")

    @property
    def scom_list_len(self):
        return len(self.scom_list)

    def display(self):
        from icecream.icecream import ic

        temp = ic.enabled
        try:
            ic.enable()
            ic('=============================================')
            ic(self.id)
            ic('---------------------------------------------')
            for scom in self.scom_list:
                ic(scom)
        finally:
            ic.enabled = temp

class HwpErrorDB(object):
    '''
    Class to parse multiple error xml files and aggreagte all 'HwpError' objects
    and 'RegisterFfdc' objects corresponding to all "hwpError" tags and "registerFfdc"
    tags
    '''
    def __init__(self):
        '''
        Constructor which will initialize the aggregation.
        Expected to call loadXml() one or more times after the construction.
        '''
        self.hwp_errors : dict[str, HwpError] = dict()
        self.hwp_err_hash_set : set[int] = set()
        self.max_ffdc_len = 0
        self.register_ffdcs : dict[str, RegisterFfdc] = dict()
        self.reg_ffdc_hash_set : set[int] = set()
        self.max_reg_ffdc_size = 0

    def parserHwpError(
            self,
            i_node:Element,
            i_is_poz:bool,
            i_platform: str) -> None:
        '''
        parser all "hwpError" tags and aggregate to 'self.hwp_errors'
        '''
        hwpErrList = i_node.findall("hwpError")
        for elem in hwpErrList:
            hwperr = HwpError(self, elem, i_is_poz)

            i_node.remove(elem)

            if((i_platform == 'SBE') and (not hwperr.is_sbeError)):
                # skip this error
                continue

            if hwperr.rc in self.hwp_errors:
                raise ParseError(
                    f"Duplicate RC {hwperr.rc}")

            self.hwp_errors[hwperr.rc] = hwperr
            if(hwperr.ffdc_len > self.max_ffdc_len):
                self.max_ffdc_len = hwperr.ffdc_len

    def parseRegisterFfdc(self, i_node : Element) -> None:
        '''
        parser all "registerFfdc" tags and aggregate to 'self.register_ffdcs'
        '''
        registerFfdc = i_node.findall('registerFfdc')
        for regffdc_node in registerFfdc:
            regffdc = RegisterFfdc(self, regffdc_node)

            if(regffdc.id in self.register_ffdcs):
                raise ParseError(
                    f"Duplicate register ffdc {regffdc.id}")

            i_node.remove(regffdc_node)

            self.register_ffdcs[regffdc.id] = regffdc

    def loadXml(
            self,
            fname: str,
            i_platform: str):
        '''
        load an XML and aggregate all "hwpError" and "registerFfdc" tags
        '''
        root = etree.parse(fname).getroot()

        format = root.find('format')
        is_poz = False
        if(format != None):
            if(format.text == 'poz'):
                is_poz = True
            root.remove(format)

        try:
            self.parseRegisterFfdc(root)
        except Exception as e:
            raise ParseError(
                f"Loading xml {fname} failed") from e

        try:
            self.parserHwpError(root, is_poz, i_platform)
        except Exception as e:
            raise ParseError(
                f"Loading xml {fname} failed") from e

        if(len(root)):
            raise ParseError(
                f"There are unprocessed nodes {list(root)} in {fname}")

    def process(self) -> None:
        for regffdc in self.register_ffdcs:
            self.register_ffdcs[regffdc].process()

        for rc in self.hwp_errors:
            self.hwp_errors[rc].process()
            if(self.hwp_errors[rc].reg_ffdc_size_in_bytes > self.max_reg_ffdc_size):
                self.max_reg_ffdc_size = self.hwp_errors[rc].reg_ffdc_size_in_bytes

    def display(self):
        for regffdc in self.register_ffdcs:
            self.register_ffdcs[regffdc].display()
        for rc in self.hwp_errors:
            self.hwp_errors[rc].display()
