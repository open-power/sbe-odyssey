# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/memory/generic/memory/lib/00gen_system_accessors.mk $
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

# Makefile to run MSS gen_accessors script.

-include $(ROOTPATH)/public/memory/generic/memory/00generic.mk

GENERATED = generic_system_accessors
COMMAND = gen_accessors.pl
$(GENERATED)_COMMAND_PATH = $(MSS_INTERNAL)/generic/memory/tools/

OUTPATH = $(MSS_PATH)/generic/memory/lib
FILE_PREFIX = mss_generic_system

SOURCES += $(ROOTPATH)/public/common/generic/fapi2/xml/attribute_info/system_attributes.xml
SOURCES += $(MSS_PATH)/generic/xml/attribute_info/generic_memory_mrw_attributes.xml

TARGETS += mss_generic_system_attribute_getters.H
TARGETS += mss_generic_system_attribute_setters.H

define generic_system_accessors_RUN
		$(C1) perl -I$($(GENERATED)_COMMAND_PATH) $$< --system=P10 --output-dir=$(OUTPATH) --output-file-prefix=$(FILE_PREFIX) $$(filter-out $$<,$$^)
		$(C1) cd $(ROOTPATH) && git diff --name-only --diff-filter=AM | grep $(FILE_PREFIX)_attribute_ | xargs internal/common/tools/hooks/tools/pre-commit-actions > /dev/null
endef

$(call BUILD_GENERATED)
