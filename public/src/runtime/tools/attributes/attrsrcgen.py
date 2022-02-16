# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/tools/attributes/attrsrcgen.py $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022
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
import os
import sys
from io import TextIOWrapper
from attrdb import *
from attrtank import *
from jinja2 import Template

#User Provided
pyUtilsPath = os.path.expandvars("$SBEROOT")+ "/public/src/tools/utils/py/"
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), pyUtilsPath))
import templateUtil

VIRTUAL_FUNCTION = {
    "ATTR_NAME": "fapi2::_getAttrName",
    "ATTR_EC": "fapi2::_getAttrEC",
    "ATTR_CHIP_UNIT_POS": "fapi2::_getAttrChipUnitPos"
}


class FileWrapper(object):
    def __init__(self, fp: TextIOWrapper) -> None:
        self.fp = fp
        self.tab = ""

    def add_tab(self) -> None:
        self.tab += "    "

    def remove_tab(self) -> None:
        self.tab = self.tab[4:]

    def write(self, data: str = "", end: str = "\n") -> None:
        if data == "":
            # Dont put indent if it is empty line
            self.fp.write(end)
        else:
            self.fp.write(self.tab + data + end)

    def close(self) -> None:
        self.fp.close()


class AttrTankFileGen(object):
    """
    Class to generate   attribute.C  attribute.H  attribute_ids.H
                        attribute_macros.H  attribute_properties.H
    """

    def __init__(
            self,
            sbe_attr: AttributeStructure,
            dir_path: str,
            header_file: str) -> None:

        self.attributes = sbe_attr
        self.virtual_header_file = header_file

        self.fp_id = FileWrapper(
            open(
                os.path.join(
                    dir_path,
                    "attribute_ids.H"),
                "w"))
        self.fp_prop = FileWrapper(
            open(
                os.path.join(
                    dir_path,
                    "attribute_properties.H"),
                "w"))
        self.fp_macro = FileWrapper(
            open(
                os.path.join(
                    dir_path,
                    "attribute_macros.H"),
                "w"))
        self.fp_attr_c = FileWrapper(
            open(
                os.path.join(
                    dir_path,
                    "attribute.C"),
                "w"))
        self.fp_attr_h = FileWrapper(
            open(
                os.path.join(
                    dir_path,
                    "attribute.H"),
                "w"))
        self.fp_ec = FileWrapper(
            open(
                os.path.join(
                    dir_path,
                    "ec_features.H"),
                "w"))

        try:
            self.generate_start()
            self.generate_src_content()
            self.generate_end()
        finally:
            self.fp_id.close()
            self.fp_prop.close()
            self.fp_macro.close()
            self.fp_attr_h.close()
            self.fp_attr_c.close()
            self.fp_ec.close()

    def generate_start(self) -> None:
        # TODO: Need to check whether using jinja template will have benifit on
        #           performance and readability
        self.fp_id.write(
            "/* This file is generated from a python script -",
            end="")
        self.fp_id.write(" {} */\n".format("attrtool"))
        self.fp_id.write("")
        self.fp_id.write("#ifndef FAPI2ATTRIBUTEIDS_H_")
        self.fp_id.write("#define FAPI2ATTRIBUTEIDS_H_")
        self.fp_id.write("")
        # TODO: This may be required after target implementation is completed.
        # self.fp_id.write("#include <target.H>")
        self.fp_id.write("#include <target_types.H>")
        self.fp_id.write("")
        self.fp_id.write("namespace fapi2")
        self.fp_id.write("{")
        self.fp_id.write("/**")
        self.fp_id.write("* @brief Enumeration of attribute IDs")
        self.fp_id.write("*/")
        self.fp_id.write("enum AttributeId")
        self.fp_id.write("{")
        self.fp_id.add_tab()

        self.fp_prop.write(
            "/* This file is generated from a python script -", end="")
        self.fp_prop.write(" {} */\n".format("attrtool"))
        self.fp_prop.write("")
        self.fp_prop.write("#ifndef FAPI2ATTRIBUTEPROP_H_")
        self.fp_prop.write("#define FAPI2ATTRIBUTEPROP_H_")
        self.fp_prop.write("")
        self.fp_prop.write("#include <attribute_ids.H>")
        self.fp_prop.write("")
        self.fp_prop.write("namespace fapi2")
        self.fp_prop.write("{")

        self.fp_attr_h.write(
            "/* This file is generated from a python script -", end="")
        self.fp_attr_h.write(" {} */\n".format("attrtool"))
        self.fp_attr_h.write("")
        self.fp_attr_h.write("#ifndef FAPI2ATTRIBUTE_H_")
        self.fp_attr_h.write("#define FAPI2ATTRIBUTE_H_")
        self.fp_attr_h.write("")
        self.fp_attr_h.write("#include <attribute_properties.H>")
        self.fp_attr_h.write("#include <attribute_macros.H>")
        self.fp_attr_h.write("")
        self.fp_attr_h.write("namespace fapi2")
        self.fp_attr_h.write("{")
        self.fp_attr_h.write("namespace ATTR")
        self.fp_attr_h.write("{")

        self.fp_attr_c.write(
            "/* This file is generated from a python script -", end="")
        self.fp_attr_c.write(" {} */\n".format("attrtool"))
        self.fp_attr_c.write("")
        self.fp_attr_c.write("#include <attribute.H>")
        self.fp_attr_c.write("")
        self.fp_attr_c.write("namespace fapi2")
        self.fp_attr_c.write("{")
        self.fp_attr_c.write("")
        self.fp_attr_c.write("")
        self.fp_attr_c.write("namespace ATTR")
        self.fp_attr_c.write("{")

        self.fp_macro.write(
            "/* This file is generated from a python script -", end="")
        self.fp_macro.write(" {} */\n".format("attrtool"))
        self.fp_macro.write("")
        self.fp_macro.write("#include <attribute.H>")
        self.fp_macro.write("#include <ecfeatureutils.H>")
        self.fp_macro.write("#include <ppe42_string.h>")
        if self.virtual_header_file is not None:
            self.fp_macro.write("#include <" + self.virtual_header_file + ">")
        self.fp_macro.write("")
        self.fp_macro.write("#ifndef FAPI2ATTRIBUTEMACRO_H_")
        self.fp_macro.write("#define FAPI2ATTRIBUTEMACRO_H_")
        self.fp_macro.write("")
        self.fp_macro.write("namespace fapi2")
        # when user calling FAPI_ATTR_GET(fapi2::<ATTR_ID>,...)
        # fapi will convert this to fapi2::<ATTR_ID>_GETMACRO
        # if we add a dummy function call before SET/GET call, 'fapi2::' scope operatot will
        #       be applied to dummy function
        self.fp_macro.write("{")
        self.fp_macro.write("__attribute__ ((always_inline)) inline void attr_macro_dummy(){}")
        self.fp_macro.write("} //fapi2")

        self.fp_ec.write(
            "/* This file is generated from a python script -",
            end="")
        self.fp_ec.write(" {} */\n".format("attrtool"))
        self.fp_ec.write("")
        self.fp_ec.write("#ifndef FAPI2ECFEATURES_H_")
        self.fp_ec.write("#define FAPI2ECFEATURES_H_")
        self.fp_ec.write("")
        self.fp_ec.write("#include <ecfeatureutils.H>")
        self.fp_ec.write("")
        self.fp_ec.write("namespace fapi2")
        self.fp_ec.write("{")

    def generate_end(self) -> None:
        self.fp_id.remove_tab()
        self.fp_id.write("};")
        self.fp_id.write("} //fapi2")
        self.fp_id.write("")
        self.fp_id.write("#endif // FAPI2ATTRIBUTEIDS_H_")

        self.fp_prop.write("} //fapi2")
        self.fp_prop.write("")
        self.fp_prop.write("#endif // FAPI2ATTRIBUTEPROP_H_")

        self.fp_attr_h.write("} //ATTR")
        self.fp_attr_h.write("")
        self.fp_attr_h.write("} //fapi2")
        self.fp_attr_h.write("")
        self.fp_attr_h.write("#endif // FAPI2ATTRIBUTE_H_")

        self.fp_attr_c.write("} //ATTR")
        self.fp_attr_c.write("")
        self.fp_attr_c.write("} //fapi2")

        self.fp_macro.write("#endif // FAPI2ATTRIBUTEMACRO_H_")

        self.fp_ec.write("} //fapi2")
        self.fp_ec.write("")
        self.fp_ec.write("#endif // FAPI2ECFEATURES_H_")

    def generate_src_content(self) -> None:
        for field in self.attributes.field_list:
            self.fp_id.write(field.name + " = " + field.hash + ",")

            if isinstance(field, RealAttrFieldInfo):
                self.generate_tank_variable(field)
            self.generate_macros(field)
            self.generate_properties(field)

    def generate_properties(self, field: AttrFieldInfo) -> None:
        self.fp_prop.write("/**")
        self.fp_prop.write("* @brief Attribute Information")
        self.fp_prop.write("*/")
        self.fp_prop.write("/* " + field.name + " */")

        typedf_line = "typedef " + field.value_type + " " + field.name + "_Type"
        if (isinstance(field, RealAttrFieldInfo)) and (
                len(field.array_dims) == 1):
            typedf_line = typedf_line + "[" + str(field.array_dims[0]) + "]"
        typedf_line = typedf_line + ";"

        self.fp_prop.write(typedf_line)

        # TODO: Update real value
        self.fp_prop.write("const bool " + field.name + "_Privilaged = false;")
        self.fp_prop.write("const fapi2::TargetType " + field.name
                           + "_TargetType = " + field.target + ";")
        self.fp_prop.write("const bool " + field.name + "_PlatInit = true;")
        self.fp_prop.write("const bool " + field.name + "_InitToZero = false;")
        self.fp_prop.write("const bool " + field.name + "_Privileged = false;")

        if field.enum_values is not None:
            self.fp_prop.write("enum " + field.name + "_Enum")
            self.fp_prop.write("{")
            self.fp_prop.add_tab()
            for item in field.enum_values:
                self.fp_prop.write("ENUM_" + field.name + "_" + item + " = "
                                   + "0x%X" % field.enum_values[item] + ",")
            self.fp_prop.remove_tab()
            self.fp_prop.write("};")

    # writing tank variable name inside tank structure
    def generate_tank_variable(self, field: AttrFieldInfo) -> None:
        field_def = field.value_type + " " + field.name
        if field.num_targ_inst > 1:
            field_def += "[" + str(field.num_targ_inst) + "]"
        for count in field.array_dims:
            field_def += "[" + str(count) + "]"
        self.fp_attr_h.write("extern " + field_def + ";")

        # define same variable as global in attribute.C file
        self.fp_attr_c.write(field_def +
                             " __attribute__((section(\".attrs\")));")

    def generate_macros(self, field: AttrFieldInfo) -> None:
        self.fp_macro.write()
        self.fp_macro.write("/*-----------------------------------------*/")

        self.generate_macro_helper(field)
        self.fp_macro.write()

        # Writing SET and GET macros
        if isinstance(field, EcAttrFieldInfo):
            self.generate_ec_macros(field)
        elif isinstance(field, VirtualAttrFieldInfo):
            self.generate_virtual_macros(field)
        else:
            var_name = ("fapi2::ATTR::" + field.name)
            if field.num_targ_inst > 1:
                var_name += "[TARGET.get().getTargetInstance()]"
            if len(field.array_dims) > 0:
                self.generate_array_macros(var_name, field)
            else:
                self.generate_simple_macros(var_name, field)

    def generate_macro_helper(self, field: AttrFieldInfo):
        self.fp_macro.write(
            "#define " +
            field.name +
            "_GETMACRO(ID, TARGET, VAL) " +
            field.name +
            "_GETMACRO_HELPER(ID, TARGET, VAL)")
        self.fp_macro.write(
            "#define " + field.name + "_SETMACRO(ID, TARGET, VAL)", end="")
        if(field.writeable):
            self.fp_macro.write(
                " " + field.name + "_SETMACRO_HELPER(ID, TARGET, VAL)")
        else:
            self.fp_macro.write()
        self.fp_macro.write(
            "#define " + field.name + "_PLAT_INIT(ID, TARGET, VAL)", end="")
        if(field.platinit):
            self.fp_macro.write(
                " " + field.name + "_SETMACRO_HELPER(ID, TARGET, VAL)")
        else:
            self.fp_macro.write()

    def generate_simple_macros(
            self, var_name: str, field: AttrFieldInfo):

        self.fp_macro.write(
            "#define " + field.name + "_GETMACRO_HELPER(ID, TARGET, VAL) \\")

        self.fp_macro.write("    attr_macro_dummy(), VAL = " + var_name)
        if(field.writeable or field.platinit):
            self.fp_macro.write("#define " + field.name +
                                "_SETMACRO_HELPER(ID, TARGET, VAL) \\")

            self.fp_macro.write(
                "    attr_macro_dummy(), " +
                var_name +
                " = VAL")

    def generate_ec_macros(self, field: EcAttrFieldInfo):
        self.fp_macro.write(
            "#define " + field.name + "_GETMACRO_HELPER(ID, TARGET, VAL) \\")

        self.fp_macro.write(
            "    attr_macro_dummy(), fapi2::queryChipEcFeature(" +
            "fapi2::int2Type<ID>(), TARGET, VAL)")

        # Generating hasFeature template functions
        jinja_ip_template = '''
inline uint8_t hasFeature(int2Type< $@attr_name@$ >,
                      fapi2::ATTR_NAME_Type i_name,
                      fapi2::ATTR_EC_Type i_ec)
{
   uint8_t hasFeature = 0;

    if(((i_name == $@chip_name@$ ) &&  (i_ec == $@ec_value@$ )))
    {
        hasFeature = 1;
    }
    return hasFeature;
};
'''
        jinja_parsed_template = templateUtil.customTemplate(jinja_ip_template)
        items = jinja_parsed_template.render({
            "attr_name": field.name,
            "chip_name": field.chip_name,
            "ec_value": field.ec_value})

        self.fp_ec.write(items)

    def generate_virtual_macros(self, field: AttrFieldInfo):
        self.fp_macro.write(
            "#define " + field.name + "_GETMACRO_HELPER(ID, TARGET, VAL) \\")

        self.fp_macro.write("    attr_macro_dummy(), " +
                            VIRTUAL_FUNCTION[field.name] + "(TARGET, VAL)")

    def generate_array_macros(
            self, var_name: str, field: AttrFieldInfo):

        self.fp_macro.write(
            "#define " + field.name + "_GETMACRO_HELPER(ID, TARGET, VAL) \\")
        self.fp_macro.write(
            "    attr_macro_dummy(), memcpy(VAL, " + var_name + ", " +
            str(field.tot_size) + ")")

        if(field.writeable or field.platinit):
            self.fp_macro.write("#define " + field.name +
                                "_SETMACRO_HELPER(ID, TARGET, VAL) \\")
            self.fp_macro.write(
                "    attr_macro_dummy(), memcpy(" + var_name + ", VAL, " +
                str(field.tot_size) + ")")


