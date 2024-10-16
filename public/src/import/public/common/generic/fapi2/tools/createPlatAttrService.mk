# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/common/generic/fapi2/tools/createPlatAttrService.mk $
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

# Makefile to run fapi2CreateAttrGetSetMacros.pl script.

GENERATED = createPlatAttrService
$(GENERATED)_COMMAND_PATH = $(FAPI2_SCRIPT_PATH)/
COMMAND = fapi2CreateAttrGetSetMacros.pl

SOURCES += $(GENPATH)/attribute_ids.H
TARGETS += plat_attribute_service.H

define createPlatAttrService_RUN
		$(C1) cd $$($(GENERATED)_PATH) && $$<
endef

$(call BUILD_GENERATED)
