# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/odyssey/perv/odyssey_perv.mk $
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
# Command table code for istep 1
#
# The order of HWPs matters here since it will
# determine the order in the command table!
#
CMDTABLE_HWPS += ody_tp_chiplet_reset
#CMDTABLE_HWPS += ody_tp_pll_initf
CMDTABLE_HWPS += ody_tp_pll_setup
CMDTABLE_HWPS += ody_pib_repr_initf
CMDTABLE_HWPS += ody_pib_arrayinit
CMDTABLE_HWPS += ody_pib_arrayinit_cleanup
#CMDTABLE_HWPS += ody_pib_initf
CMDTABLE_HWPS += ody_pib_startclocks
$(call BUILD_CMDTABLE,odyssey,10,main,$(CMDTABLE_HWPS))

$(call BUILD_CMDTABLE,odyssey,10,cust,cust)


#
# A macro to contain all our boilerplate
#
define __ODYSSEY_PERV_PROCEDURE
PROCEDURE=$(1)
$$(call ADD_MODULE_INCDIR,$$(PROCEDURE),$(dir $(lastword $(MAKEFILE_LIST)))/../../generic/perv)
$$(call ADD_MODULE_INCDIR,$$(PROCEDURE),$(dir $(lastword $(MAKEFILE_LIST)))/../../../ring/generic)
$$(call ADD_MODULE_SRCDIR,$$(PROCEDURE),$(dir $(lastword $(MAKEFILE_LIST)))/../../generic/perv)
$$(call ADD_MODULE_SHARED_OBJ,$$(PROCEDURE),poz_perv_utils.o)
$$(call ADD_MODULE_SHARED_OBJ,$$(PROCEDURE),poz_perv_mod_misc.o)
$$(call ADD_MODULE_SHARED_OBJ,$$(PROCEDURE),poz_perv_mod_chiplet_clocking.o)
$$(call ADD_MODULE_SHARED_OBJ,$$(PROCEDURE),poz_perv_mod_chip_clocking.o)
$$(call ADD_MODULE_SHARED_OBJ,$$(PROCEDURE),poz_perv_mod_bist.o)
ifneq ($(2),)
$$(call ADD_MODULE_OBJ,$$(PROCEDURE),$(2).o)
endif
$$(call BUILD_PROCEDURE)
endef
ODYSSEY_PERV_PROCEDURE = $(eval $(call __ODYSSEY_PERV_PROCEDURE,$1,$2))

#
# And now the actual HWP definitions
#

# istep 0
$(call ODYSSEY_PERV_PROCEDURE,ody_cbs_start)
$(call ODYSSEY_PERV_PROCEDURE,ody_sppe_config_update)
# istep 1
$(call ODYSSEY_PERV_PROCEDURE,ody_cmdtable_interpreter,poz_cmdtable_interpreter)
$(call ODYSSEY_PERV_PROCEDURE,ody_sppe_boot_check)
$(call ODYSSEY_PERV_PROCEDURE,ody_sppe_attr_setup)
$(call ODYSSEY_PERV_PROCEDURE,ody_tp_repr_initf)
$(call ODYSSEY_PERV_PROCEDURE,ody_tp_arrayinit)
$(call ODYSSEY_PERV_PROCEDURE,ody_tp_arrayinit_cleanup)
$(call ODYSSEY_PERV_PROCEDURE,ody_tp_initf)
$(call ODYSSEY_PERV_PROCEDURE,ody_tp_startclocks)
$(call ODYSSEY_PERV_PROCEDURE,ody_tp_init)
# istep 2

# istep 3
$(call ODYSSEY_PERV_PROCEDURE,ody_chiplet_clk_config,poz_chiplet_clk_config)
$(call ODYSSEY_PERV_PROCEDURE,ody_chiplet_reset,poz_chiplet_reset)
$(call ODYSSEY_PERV_PROCEDURE,ody_chiplet_unused_psave,poz_chiplet_unused_psave)
$(call ODYSSEY_PERV_PROCEDURE,ody_chiplet_pll_setup,poz_chiplet_pll_setup)
$(call ODYSSEY_PERV_PROCEDURE,ody_bist_repr_initf)
$(call ODYSSEY_PERV_PROCEDURE,ody_abist,poz_bist)
$(call ODYSSEY_PERV_PROCEDURE,ody_lbist,poz_bist)
$(call ODYSSEY_PERV_PROCEDURE,ody_chiplet_repr_initf)
$(call ODYSSEY_PERV_PROCEDURE,ody_chiplet_arrayinit,poz_chiplet_arrayinit)
$(call ODYSSEY_PERV_PROCEDURE,ody_chiplet_initf)
$(call ODYSSEY_PERV_PROCEDURE,ody_chiplet_init)
$(call ODYSSEY_PERV_PROCEDURE,ody_chiplet_startclocks,poz_chiplet_startclocks)
$(call ODYSSEY_PERV_PROCEDURE,ody_chiplet_fir_init)
$(call ODYSSEY_PERV_PROCEDURE,ody_nest_enable_io)
