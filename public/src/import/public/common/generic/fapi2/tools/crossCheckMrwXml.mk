# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/common/generic/fapi2/tools/crossCheckMrwXml.mk $
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
# Makefile to run crossCheckMrwXml.pl
GENERATED = crossCheckMrwXml
COMMAND = crossCheckMrwXml.pl

# attribute_ids.H is added to SOURCES to be a prereq:
# This file must be generated before this script is run.
ATTRIBUTE_IDS_H_FILE=$(GENPATH)/attribute_ids.H
SOURCES += $(ATTRIBUTE_IDS_H_FILE)
SOURCES += $(FAPI2_MRW_XML)

TARGETS += mrw_attr_crosscheck_errors.txt
TARGETS += plat_init_attributes.txt
TARGETS += mrw_attributes.txt

CLEAN_TARGETS += $(GENPATH)/mrw_attr_crosscheck_errors.txt
CLEAN_TARGETS += $(GENPATH)/plat_init_attributes.txt
CLEAN_TARGETS += $(GENPATH)/mrw_attributes.txt

# Need to also filter-out attribute_ids.H file from SOURCES input to the script.
define crossCheckMrwXml_RUN
$(C1) $$< --attribute-ids-dir=$$($(GENERATED)_PATH) --output-dir=$$($(GENERATED)_PATH) $$(filter-out %/attribute_ids.H,$$(filter-out $$<,$$^))
endef

$(call BUILD_GENERATED)
