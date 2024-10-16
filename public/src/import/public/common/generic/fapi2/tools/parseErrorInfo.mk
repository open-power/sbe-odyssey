# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/common/generic/fapi2/tools/parseErrorInfo.mk $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2021,2022
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

# Makefile to run the parseErrorInfo script.

GENERATED = parseErrorInfo

COMMAND = parseErrorInfo.pl

SOURCES += $(FAPI2_ERROR_XML)
SOURCES += $(GENPATH)/empty_error.xml

TARGETS += hwp_return_codes.H
TARGETS += hwp_error_info.H
TARGETS += hwp_ffdc_classes.H
TARGETS += set_sbe_error.H
TARGETS += set_sbe_error_funcs.H
TARGETS += collect_reg_ffdc_regs.C

CLEAN_TARGETS += $(GENPATH)/hwp_return_codes.H
CLEAN_TARGETS += $(GENPATH)/hwp_error_info.H
CLEAN_TARGETS += $(GENPATH)/hwp_ffdc_classes.H
CLEAN_TARGETS += $(GENPATH)/collect_reg_ffdc_regs.C
CLEAN_TARGETS += $(GENPATH)/set_sbe_error.H

define parseErrorInfo_RUN
		$(C1) $$< --use-variable-buffers \
				  --output-dir=$$($(GENERATED)_PATH) \
				  $$(filter-out $$<,$$^)
endef

$(call BUILD_GENERATED)

# Generate an empty error XML file so that the scripts pass if the
# environment hasn't defined any of its own.
GENERATED = empty_error_xml
TARGETS += empty_error.xml
CLEAN_TARGETS += $(GENPATH)/empty_error.xml

define empty_error_xml_RUN
		$(C1) echo "<hwpErrors/>" > $$($(GENERATED)_PATH)/empty_error.xml
endef

$(call BUILD_GENERATED)
