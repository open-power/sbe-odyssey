# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/generic/utils/generic_utils.mk $
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
# A macro to contain all our boilerplate
#
define __GENERIC_UTIL_PROCEDURE
PROCEDURE=$(1)
$$(call BUILD_PROCEDURE)
endef
GENERIC_UTIL_PROCEDURE = $(eval $(call __GENERIC_UTIL_PROCEDURE,$1))

define __GENERIC_PPE_PROCEDURE
PROCEDURE=$(1)
$$(call ADD_MODULE_SHARED_OBJ,$$(PROCEDURE),poz_ppe_utils.o)
$$(call BUILD_PROCEDURE)
endef
GENERIC_PPE_PROCEDURE = $(eval $(call __GENERIC_PPE_PROCEDURE,$1))


#
# And now the actual HWP definitions
#
$(call GENERIC_UTIL_PROCEDURE,poz_writesram)
$(call GENERIC_UTIL_PROCEDURE,poz_readsram)

$(call GENERIC_PPE_PROCEDURE,poz_ppe_state)
$(call GENERIC_PPE_PROCEDURE,poz_ppe_other_regs)
$(call GENERIC_PPE_PROCEDURE,poz_mib_state)
