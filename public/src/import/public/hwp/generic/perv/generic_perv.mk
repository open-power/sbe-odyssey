# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/generic/perv/generic_perv.mk $
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
define __GENERIC_PERV_PROCEDURE
PROCEDURE=$(1)
$$(call ADD_MODULE_INCDIR,$$(PROCEDURE),$$(ROOTPATH)/public/ring/generic)
$$(call ADD_MODULE_SHARED_OBJ,$$(PROCEDURE),poz_perv_mod_misc.o)
$$(call ADD_MODULE_SHARED_OBJ,$$(PROCEDURE),poz_perv_mod_chiplet_clocking.o)
$$(call ADD_MODULE_SHARED_OBJ,$$(PROCEDURE),poz_perv_mod_bist.o)
$$(call ADD_MODULE_SHARED_OBJ,$$(PROCEDURE),poz_perv_utils.o)
$$(call ADD_MODULE_SRCDIR,$$(PROCEDURE),$$(ROOTPATH)/public/common/utils/imageProcs)
ifneq ($(2),)
$(foreach DEP,$(2),$$(call ADD_MODULE_OBJ,$$(PROCEDURE),$(DEP).o))
endif
$$(call BUILD_PROCEDURE)
endef
GENERIC_PERV_PROCEDURE = $(eval $(call __GENERIC_PERV_PROCEDURE,$1,$2))

#
# And now the actual HWP definitions
#
$(call GENERIC_PERV_PROCEDURE,poz_cmdtable_interpreter)
$(call GENERIC_PERV_PROCEDURE,poz_fastarray)
$(call GENERIC_PERV_PROCEDURE,poz_bist,poz_scan_compare poz_fastarray sha3)
$(call GENERIC_PERV_PROCEDURE,poz_sbe_spi_cmd)
