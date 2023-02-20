# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/generic/utils/wrapper/generic_utils_wrap.mk $
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

WRAPPER=poz_ppe_state_wrap
$(call ADD_EXE_INCDIR,$(WRAPPER),$(ROOTPATH)/public/hwp/generic/utils)
$(call ADD_EXE_SRCDIR,$(WRAPPER),$(ROOTPATH)/public/hwp/generic/utils)
$(call ADD_EXE_SHARED_OBJ,$(WRAPPER),poz_ppe_utils.o)
$(call ADD_EXE_OBJ,$(WRAPPER),poz_ppe_state.o poz_ppe_other_regs.o poz_mib_state.o)
$(call BUILD_WRAPPER)
