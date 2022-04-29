# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/memory/explorer/hwp/00exp_common.mk $
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

-include 00generic.mk

MSS_EXP_INCLUDES := $(GENPATH)
MSS_EXP_INCLUDES += $(MSS_PATH)
#MSS_EXP_INCLUDES += $(MSS_PATH)/public/common/explorer/registers
MSS_EXP_INCLUDES += $(MSS_PATH)/explorer/hwp/lib
MSS_EXP_INCLUDES += $(MSS_PATH)/explorer/hwp
#MSS_EXP_INCLUDES += $(ROOTPATH)/p11/hwp/lib
#MSS_EXP_INCLUDES += $(ROOTPATH)/p11/hwp/memory

# For generic attribute accessors
MSS_EXP_INCLUDES += $(MSS_PATH)/generic/memory/lib
MSS_EXP_INCLUDES += $(MSS_PATH)/generic/memory/lib/utils

#MSS_EXP_LAB_PATH := $(MSS_INTERNAL)/generic/lab

CATCH_UNIT_TESTS_INCLUDES := $(ROOTPATH)/public/common/generic/fapi2/test

export MSS_EXP_INCLUDES := $(MSS_EXP_INCLUDES)

# ADD_EXP_MEMORY_INCDIRS
#     This macro will add additional include paths for all memory modules
ADD_EXP_MEMORY_INCDIRS = $(call __ADD_MODULE_INCDIR,$(1),$(MSS_EXP_INCLUDES))
