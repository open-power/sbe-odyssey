# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/memory/generic/memory/lib/mss_generic.mk $
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

#
# Makefile to build the MSS libraries.
#

# Add common and generated parts to object list.
-include $(ROOTPATH)/public/memory/generic/memory/00generic.mk

MSS_GENERIC_SOURCE := $(shell find $(MSS_GENERIC_PATH) -name '*.C' -exec basename {} \;)
MSS_GENERIC_MODULE_OBJS += $(patsubst %.C,%.o,$(MSS_GENERIC_SOURCE))
MSS_GENERIC_SOURCE_DIRS := $(shell find $(MSS_GENERIC_PATH) -type d)

# Define common source and include paths.
define MSS_GENERIC_MODULE_INCLUDES
$(foreach dir, $(MSS_GENERIC_SOURCE_DIRS), $(call ADD_MODULE_SRCDIR,$(1),$(dir)))
$(call ADD_MODULE_INCDIR,$(1),$(MSS_PATH))
$(call ADD_MODULE_INCDIR,$(1),$(FAPI2_PATH)/include)
$(call ADD_MODULE_INCDIR,$(1),$(GENPATH))
$(call ADD_MODULE_INCDIR,$(1),$(FAPI2_PLAT_INCLUDE))
endef
MODULE = mss_generic
OBJS += $(MSS_GENERIC_MODULE_OBJS)
$(eval $(call MSS_GENERIC_MODULE_INCLUDES,$(MODULE)))
$(call BUILD_MODULE)
