# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/memory/odyssey/hwp/lib/mss_odyssey.mk $
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

#
# Makefile to build the MSS libraries.
#

# Add common and generated parts to object list.

-include $(ROOTPATH)/public/memory/generic/memory/00generic.mk

MSS_ODY_PATH := $(MSS_PATH)/odyssey/hwp/lib

MSS_ODY_SOURCE := $(shell find $(MSS_ODY_PATH) -name '*.C' -exec basename {} \;)

MSS_ODY_MODULE_OBJS += $(patsubst %.C,%.o,$(MSS_ODY_SOURCE))

MSS_ODY_SOURCE_DIRS := $(shell find $(MSS_ODY_PATH) -type d)

# Define common source and include paths.
define MSS_ODY_MODULE_INCLUDES
$(foreach dir, $(MSS_ODY_SOURCE_DIRS), $(call ADD_MODULE_SRCDIR,$(1),$(dir)))
$(call ADD_MODULE_INCDIR,$(1),$(ROOTPATH)/public/hwp/odyssey/perv)
$(call ADD_MODULE_INCDIR,$(1),$(MSS_PATH)/odyssey/hwp)
$(call ADD_MODULE_INCDIR,$(1),$(MSS_INTERNAL)/odyssey/hwp/wrapper)
$(call ADD_MODULE_INCDIR,$(1),$(MSS_PATH)/odyssey/hwp/lib)
$(call ADD_MODULE_INCDIR,$(1),$(ROOTPATH)/public/common/generic/scomt)
$(call ADD_MODULE_INCDIR,$(1),$(ROOTPATH)/public/common/odyssey/registers)
$(call ADD_MODULE_INCDIR,$(1),$(MSS_PATH)/generic/memory/lib)
$(call ADD_MODULE_INCDIR,$(1),$(FAPI2_PATH)/include)
$(call ADD_MODULE_INCDIR,$(1),$(GENPATH))
$(call ADD_MODULE_INCDIR,$(1),$(FAPI2_PLAT_INCLUDE))
$(call ADD_MODULE_INCDIR,$(1),$(ROOTPATH))
$(call ADD_MODULE_INCDIR,$(1),$(MSS_PATH))
endef
MODULE = mss_odyssey
OBJS += $(MSS_ODY_MODULE_OBJS)

$(eval $(call MSS_ODY_MODULE_INCLUDES,$(MODULE)))
$(call BUILD_MODULE)
