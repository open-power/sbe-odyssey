# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/common/generic/fapi2/test/fapi2_mmio_test.mk $
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
FAPI2_TEST_INCLUDES := $(GENPATH)
FAPI2_TEST_INCLUDES+=$(ROOTPATH)/hwpf/fapi2/include/plat/
WRAPPER=fapi2_mmio_test
$(call ADD_EXE_INCDIR,$(WRAPPER),$(FAPI2_TEST_INCLUDES))
$(call BUILD_WRAPPER)
