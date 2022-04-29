# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/memory/odyssey/hwp/00gen_accessors.mk $
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

# Makefile to run MSS gen_accessors script.

-include 00ody_common.mk

GENERATED = gen_ody_accessors
COMMAND = gen_accessors.pl
$(GENERATED)_COMMAND_PATH = $(MSS_INTERNAL)/generic/tools/

OUTPATH = $(MSS_PATH)/odyssey/hwp/lib
FILE_PREFIX = mss_odyssey

SOURCES += $(shell find $(MSS_PATH)/odyssey/xml/attribute_info/ -name '*.xml' | sort)

TARGETS += mss_odyssey_attribute_getters.H
TARGETS += mss_odyssey_attribute_setters.H

define gen_ody_accessors_RUN
		$(C1) perl -I$($(GENERATED)_COMMAND_PATH) $$< --system=P10 --output-dir=$(OUTPATH) --output-file-prefix=$(FILE_PREFIX) $$(filter-out $$<,$$^)
		$(C1) cd $(ROOTPATH) && git diff --name-only --diff-filter=AM | grep $(FILE_PREFIX)_attribute_ | xargs internal/common/tools/hooks/tools/pre-commit-actions > /dev/null
endef

$(call BUILD_GENERATED)
