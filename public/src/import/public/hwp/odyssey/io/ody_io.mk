# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/odyssey/io/ody_io.mk $
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
define __ODYSSEY_IO_PROCEDURE
PROCEDURE=$(1)
$$(call ADD_MODULE_INCDIR,$$(PROCEDURE),$$(ROOTPATH)/public/hwp/generic/utils)
$$(call ADD_MODULE_SRCDIR,$$(PROCEDURE),$$(ROOTPATH)/public/hwp/generic/utils)
$$(call ADD_MODULE_INCDIR,$$(PROCEDURE),$$(ROOTPATH)/public/hwp/generic/io)
$$(call ADD_MODULE_SRCDIR,$$(PROCEDURE),$$(ROOTPATH)/public/hwp/generic/io)
$$(call ADD_MODULE_OBJ,$$(PROCEDURE),io_ppe_cache.o)
# Depending objs should be delimited by spaces after the first one.
ifneq (($2),)
$(foreach DEP,$(2),$$(call ADD_MODULE_OBJ,$$(PROCEDURE),$(DEP).o))
endif
$$(call BUILD_PROCEDURE)
endef
ODYSSEY_IO_PROCEDURE = $(eval $(call __ODYSSEY_IO_PROCEDURE,$1,$2))
$(call ODYSSEY_IO_PROCEDURE,ody_putsram,poz_writesram)
$(call ODYSSEY_IO_PROCEDURE,ody_getsram,poz_readsram)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_unload)

$(call ODYSSEY_IO_PROCEDURE,ody_omi_hss_ppe_load,ody_putsram poz_writesram)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_hss_config)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_hss_ppe_start)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_hss_bist_init)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_hss_bist_start)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_hss_bist_poll)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_hss_bist_cleanup)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_hss_init)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_hss_dccal_start)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_hss_dccal_poll)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_hss_tx_zcal)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_pretrain_adv)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_setup)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_train)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_train_check)
$(call ODYSSEY_IO_PROCEDURE,ody_omi_posttrain_adv)
