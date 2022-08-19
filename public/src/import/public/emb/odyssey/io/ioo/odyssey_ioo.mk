# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/emb/odyssey/io/ioo/odyssey_ioo.mk $
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
ODYSSEY_IOO_TARGET:=odyssey_ioo
$(info(ODYSSEY_IOO_TARGET is $(ODYSSEY_IOO_TARGET))
IMAGE:=$(ODYSSEY_IOO_TARGET)

#Select KERNEL
$(IMAGE)_KERNEL:=__PK__

include $(ODYSSEY_IOO_SRCDIR)/odyssey_ioo_common.mk
OBJS := $(ODYSSEY_IOO_OBJS)

$(info(ODYSSEY_IOO_SRCDIR is $(ODYSSEY_IOO_SRCDIR))
$(info(ODYSSEY_IOO_OBJS is $(ODYSSEY_IOO_OBJS))

$(call BUILD_PPEIMAGE)
