# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/memory/odyssey/hwp/00ody_common.mk $
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
-include $(ROOTPATH)/public/memory/generic/memory/00generic.mk

MSS_ODY_INCLUDES := $(GENPATH)
MSS_ODY_INCLUDES += $(ROOTPATH)
MSS_ODY_INCLUDES += $(MSS_PATH)
MSS_ODY_INCLUDES += $(ROOTPATH)/public/common/utils
MSS_ODY_INCLUDES += $(ROOTPATH)/public/common/odyssey/registers
MSS_ODY_INCLUDES += $(MSS_PATH)/odyssey/common/include
MSS_ODY_INCLUDES += $(MSS_PATH)/odyssey/hwp/lib
MSS_ODY_INCLUDES += $(MSS_PATH)/odyssey/hwp
MSS_ODY_INCLUDES += $(MSS_PATH)/odyssey/hwp/wrapper
MSS_ODY_INCLUDES += $(MSS_PATH)/odyssey/initfiles
#MSS_ODY_INCLUDES += $(MSS_PATH)/p11/hwp

# For generic attribute accessors
MSS_ODY_INCLUDES += $(MSS_PATH)/generic/memory/lib
MSS_ODY_INCLUDES += $(MSS_PATH)/generic/memory/lib/utils

MSS_ODY_LAB_PATH := $(MSS_INTERNAL)/memory/generic/lab

CATCH_UNIT_TESTS_INCLUDES := $(ROOTPATH)/public/common/generic/fapi2/test

export MSS_ODY_INCLUDES := $(MSS_ODY_INCLUDES)

# ADD_ODY_MEMORY_INCDIRS
#     This macro will add additional include paths for all memory modules
ADD_ODY_MEMORY_INCDIRS = $(call __ADD_MODULE_INCDIR,$(1),$(MSS_ODY_INCLUDES))
