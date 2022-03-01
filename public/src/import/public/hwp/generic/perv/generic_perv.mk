# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/generic/perv/generic_perv.mk $
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
# A macro to contain all our boilerplate
#
define __GENERIC_PERV_PROCEDURE
PROCEDURE=$(1)
#$$(call ADD_MODULE_OBJ,$$(PROCEDURE),poz_perv_mod_misc.o)
#$$(call ADD_MODULE_OBJ,$$(PROCEDURE),poz_perv_mod_chiplet_clocking.o)
$$(call BUILD_PROCEDURE)
endef
GENERIC_PERV_PROCEDURE = $(eval $(call __GENERIC_PERV_PROCEDURE,$1))

#
# And now the actual HWP definitions
#
$(call GENERIC_PERV_PROCEDURE,poz_cmdtable_interpreter)
$(call GENERIC_PERV_PROCEDURE,poz_abist)
$(call GENERIC_PERV_PROCEDURE,poz_bist_repr_initf)
$(call GENERIC_PERV_PROCEDURE,poz_chiplet_arrayinit)
$(call GENERIC_PERV_PROCEDURE,poz_chiplet_clk_config)
$(call GENERIC_PERV_PROCEDURE,poz_chiplet_dts_init)
$(call GENERIC_PERV_PROCEDURE,poz_chiplet_initf)
$(call GENERIC_PERV_PROCEDURE,poz_chiplet_pll_setup)
$(call GENERIC_PERV_PROCEDURE,poz_chiplet_repr_initf)
$(call GENERIC_PERV_PROCEDURE,poz_chiplet_reset)
$(call GENERIC_PERV_PROCEDURE,poz_chiplet_startclocks)
$(call GENERIC_PERV_PROCEDURE,poz_chiplet_unused_psave)
$(call GENERIC_PERV_PROCEDURE,poz_lbist)
$(call GENERIC_PERV_PROCEDURE,poz_nest_enable_io)
$(call GENERIC_PERV_PROCEDURE,poz_stopclocks)
