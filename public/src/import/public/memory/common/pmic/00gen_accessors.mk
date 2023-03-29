# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/memory/common/pmic/00gen_accessors.mk $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023
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

-include $(ROOTPATH)/public/memory/generic/memory/00generic.mk
-include 00pmic_common.mk

GENERATED = gen_pmic_accessors
COMMAND = gen_accessors.pl
$(GENERATED)_COMMAND_PATH = $(MSS_INTERNAL)/generic/memory/tools/

OUTPATH = $(ROOTPATH)/public/memory/common/pmic/lib
FILE_PREFIX = mss_pmic

SOURCES += $(ROOTPATH)/public/memory/common/xml/attribute_info/pmic_eff_attributes.xml
SOURCES += $(ROOTPATH)/public/memory/common/xml/attribute_info/pmic_attributes.xml

TARGETS += mss_pmic_attribute_getters.H
TARGETS += mss_pmic_attribute_setters.H

define gen_ody_accessors_RUN
		$(C1) perl -I$($(GENERATED)_COMMAND_PATH) $$< --system=P10 --output-dir=$(OUTPATH) --output-file-prefix=$(FILE_PREFIX) $$(filter-out $$<,$$^)
		$(C1) cd $(ROOTPATH) && git diff --name-only --diff-filter=AM | grep $(FILE_PREFIX)_attribute_ | xargs internal/common/tools/hooks/tools/pre-commit-actions > /dev/null
endef

$(call BUILD_GENERATED)
