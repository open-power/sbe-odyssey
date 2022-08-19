# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/emb/odyssey/io/ioo/odyssey_ioo_common.mk $
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
$(IMAGE)_TARGET=PPE
$(IMAGE)_LINK_SCRIPT=odyssey_ioo_link.cmd

$(IMAGE)_TRACE_HASH_PREFIX := $(shell echo $(IMAGE) | md5sum | cut -c1-4 \
        | xargs -i printf "%d" 0x{})

_PPE_TYPE=std
_PPE_BOLTON_CFG=standard

$(IMAGE)_COMMONFLAGS+= -DIMAGE_NAME=odyssey_ioo
$(IMAGE)_COMMONFLAGS+= -DPK_TIMER_SUPPORT=1
$(IMAGE)_COMMONFLAGS+= -DPK_THREAD_SUPPORT=1
$(IMAGE)_COMMONFLAGS+= -DPK_STACK_CHECK=0
$(IMAGE)_COMMONFLAGS+= -DPK_TRACE_SUPPORT=0
$(IMAGE)_COMMONFLAGS+= -DPK_TRACE_HASH_PREFIX=12324
$(IMAGE)_COMMONFLAGS+= -DUSE_PK_APP_CFG_H=1
$(IMAGE)_COMMONFLAGS+= -DAPPCFG_USE_EXT_TIMEBASE=1
$(IMAGE)_COMMONFLAGS+= -DNO_INIT_DBCR0=1
$(IMAGE)_COMMONFLAGS+= -D__PK__=1

$(IMAGE)_COMMONFLAGS+= -DIOO
$(IMAGE)_COMMONFLAGS+= -DIO_DEBUG_LEVEL=2
$(IMAGE)_COMMONFLAGS+= -DIO_THREADING_METHOD=1

ODYSSEY_IOO_OBJS += eabi.o
ODYSSEY_IOO_OBJS += ppe42_string.o
ODYSSEY_IOO_OBJS += ppe42_gcc.o
ODYSSEY_IOO_OBJS += div32.o

include $(PPE_ROOTPATH)/ppetrace/pktracefiles.mk
ODYSSEY_IOO_OBJS += $(PKTRACE_OBJECTS)
$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(PPE_ROOTPATH)/ppetrace)
PKTRACE_OBJECTS:=

include $(PK_SRCDIR)/kernel/pkkernelfiles.mk
ODYSSEY_IOO_OBJS += $(PK_OBJECTS)
ODYSSEY_IOO_OBJS += $(PK_TIMER_OBJECTS)
ODYSSEY_IOO_OBJS += $(PK_THREAD_OBJECTS)
$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(PK_SRCDIR)/kernel)

include $(PK_SRCDIR)/ppe42/pkppe42files.mk
include $(PPE_ROOTPATH)/baselib/baselibfiles.mk
ODYSSEY_IOO_OBJS += $(PPE42_OBJECTS)
ODYSSEY_IOO_OBJS += $(PPE42_THREAD_OBJECTS)
$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(PK_SRCDIR)/ppe42)
$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(PPE_ROOTPATH)/baselib)

include $(PPE_ROOTPATH)/boltonlib/$(_PPE_BOLTON_CFG)/pk$(_PPE_TYPE)files.mk
ODYSSEY_IOO_OBJS += $(STD_OBJECTS)
$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(PPE_ROOTPATH)/boltonlib/$(_PPE_BOLTON_CFG))

$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(ODYSSEY_IO_COMMON_SRCDIR))
$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(ODYSSEY_IOO_SRCDIR))
include $(ODYSSEY_IOO_SRCDIR)/odyssey_ioo_files.mk
ODYSSEY_IOO_OBJS+=$(ODYSSEY_IOO_OBJECTS)

# add include paths
$(call ADD_PPEIMAGE_INCDIR,$(IMAGE),\
        $(ODYSSEY_IOO_SRCDIR) \
        $(ODYSSEY_IO_COMMON_SRCDIR) \
        $(PK_SRCDIR)/kernel \
        $(PK_SRCDIR)/ppe42 \
        $(PPE_ROOTPATH)/baselib \
        $(PPE_ROOTPATH)/ppetrace \
        $(PPE_ROOTPATH)/boltonlib/$(_PPE_BOLTON_CFG) \
        )

$(IMAGE)_LDFLAGS=-e __system_reset -N -gc-sections -Bstatic --nostdlib --sort-common -EB -Os -nostdlib
