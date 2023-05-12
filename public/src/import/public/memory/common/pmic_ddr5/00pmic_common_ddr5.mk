# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/memory/common/pmic_ddr5/00pmic_common_ddr5.mk $
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

MSS_PMIC_INCLUDES_DDR5 := $(GENPATH)
MSS_PMIC_INCLUDES_DDR5 += $(ROOTPATH)
MSS_PMIC_INCLUDES_DDR5 += $(ROOTPATH)/public/memory
MSS_PMIC_INCLUDES_DDR5 += $(ROOTPATH)/public/memory/common/include
MSS_PMIC_INCLUDES_DDR5 += $(ROOTPATH)/public/memory/common/pmic
MSS_PMIC_INCLUDES_DDR5 += $(ROOTPATH)/public/memory/common/pmic/lib
MSS_PMIC_INCLUDES_DDR5 += $(ROOTPATH)/public/memory/common/pmic/lib/i2c
MSS_PMIC_INCLUDES_DDR5 += $(ROOTPATH)/public/memory/common/pmic/lib/utils
MSS_PMIC_INCLUDES_DDR5 += $(ROOTPATH)/public/memory/common/pmic_ddr5
MSS_PMIC_INCLUDES_DDR5 += $(ROOTPATH)/public/memory/common/pmic_ddr5/lib
MSS_PMIC_INCLUDES_DDR5 += $(ROOTPATH)/public/memory/common/pmic_ddr5/lib/utils

# For generic attribute accessors
MSS_PMIC_INCLUDES_DDR5 += $(ROOTPATH)/public/memory/generic/memory/lib
MSS_PMIC_INCLUDES_DDR5 += $(ROOTPATH)/public/memory/generic/memory/lib/utils

MSS_PMIC_LAB_PATH := $(ROOTPATH)/internal/memory/common/pmic/lab
MSS_PMIC_LAB_PATH_DDR5 := $(ROOTPATH)/internal/memory/common/pmic_ddr5/lab

CATCH_UNIT_TESTS_INCLUDES := $(ROOTPATH)/public/common/generic/fapi2/test

export MSS_PMIC_INCLUDES_DDR5 := $(MSS_PMIC_INCLUDES_DDR5)

# ADD_PMIC_MEMORY_INCDIRS_DDR5
#     This macro will add additional include paths for all memory modules
ADD_PMIC_MEMORY_INCDIRS_DDR5 = $(call __ADD_MODULE_INCDIR,$(1),$(MSS_PMIC_INCLUDES_DDR5))

# Include main PMIC Lab makefile
-include $(MSS_PMIC_LAB_PATH_DDR5)/00pmic_lab_ddr5.mk
