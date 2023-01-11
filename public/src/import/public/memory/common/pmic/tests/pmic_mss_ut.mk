# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/memory/common/pmic/tests/pmic_mss_ut.mk $
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

-include 00pmic_common.mk

MSS_PMIC_UT_SOURCE := $(shell find $(ROOTPATH)/chips/ocmb/common/procedures/hwp/pmic/tests -name '*.C' -exec basename {} \;)
MSS_PMIC_UT_SOURCE += $(shell find $(ROOTPATH)/generic/memory/tests -name '*.C' -exec basename {} \;)

WRAPPER=pmic_mss_ut
OBJS += $(patsubst %.C,%.o,$(MSS_PMIC_UT_SOURCE))
$(call ADD_EXE_INCDIR,$(WRAPPER),$(MSS_PMIC_INCLUDES))
$(call ADD_EXE_INCDIR,$(WRAPPER),$(CATCH_UNIT_TESTS_INCLUDES))
$(call ADD_EXE_INCDIR,$(WRAPPER),$(ROOTPATH)/chips/p10/procedures/hwp/memory/lib)
$(call ADD_EXE_INCDIR,$(WRAPPER),$(ROOTPATH)/chips/p10/procedures/hwp/memory)
$(call ADD_EXE_INCDIR,$(WRAPPER),$(ROOTPATH)/chips/ocmb/explorer/procedures/hwp/memory)
$(call ADD_EXE_INCDIR,$(WRAPPER),$(ROOTPATH)/chips/ocmb/explorer/procedures/hwp/memory/lib)
$(call ADD_EXE_INCDIR,$(WRAPPER),$(ROOTPATH)/chips/p10/procedures/hwp/memory/tests)
$(call ADD_EXE_SRCDIR,$(WRAPPER),$(ROOTPATH)/generic/memory/tests)
$(call ADD_EXE_SRCDIR,$(WRAPPER),$(ROOTPATH)/chips/ocmb/common/spd_access)

$(WRAPPER)_DEPLIBS+=mss_generic
$(WRAPPER)_DEPLIBS+=mss_p10
$(WRAPPER)_DEPLIBS+=mss_explorer
$(WRAPPER)_DEPLIBS+=mss_pmic
$(WRAPPER)_DEPLIBS+=pmic_enable
$(WRAPPER)_DEPLIBS+=pmic_n_mode_detect
$(WRAPPER)_DEPLIBS+=p10_mss_freq
$(WRAPPER)_DEPLIBS+=p10_mss_eff_config
$(WRAPPER)_DEPLIBS+=exp_mss_eff_config_thermal
$(WRAPPER)_DEPLIBS+=ody_mss_eff_config_thermal
$(WRAPPER)_DEPLIBS+=p10_mss_freq_system
$(WRAPPER)_DEPLIBS+=p10_mss_volt
$(WRAPPER)_DEPLIBS+=proc_scomt

$(WRAPPER)_COMMONFLAGS+=-fno-var-tracking-assignments

$(WRAPPER)_LDFLAGS+= -Wl,-lrt
$(call BUILD_WRAPPER)
