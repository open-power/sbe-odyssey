# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/emb/p11/kernels/ppe/iota/test/iota.mk $
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

IMAGE := iota_test

$(IMAGE)_KERNEL:=__IOTA__

# Select the PPE toolchain
$(IMAGE)_TARGET=PPE

# Override the default linker script
$(IMAGE)_LINK_SCRIPT=link.ld

#  Select the bolton
_PPE_TYPE=gpe

# Customize IOTA
$(IMAGE)_COMMONFLAGS = -DPK_TRACE_LEVEL=1
$(IMAGE)_COMMONFLAGS+= -DSIMICS_TUNING=0
$(IMAGE)_COMMONFLAGS+= -DUSE_SIMICS_IO=0
$(IMAGE)_COMMONFLAGS+= -DPK_TRACE_SUPPORT=1
$(IMAGE)_COMMONFLAGS+= -DUSE_APP_CFG_H=1
$(IMAGE)_COMMONFLAGS+= -DPK_TIMER_SUPPORT=0
$(IMAGE)_COMMONFLAGS+= -D__IOTA__
$(IMAGE)_COMMONFLAGS+= -D__PPE_PLAT
$(IMAGE)_COMMONFLAGS+= -DAPPCFG_OCC_INSTANCE_ID=2
$(IMAGE)_COMMONFLAGS+= -DUNIFIED_IRQ_HANDLER_GPE
$(IMAGE)_COMMONFLAGS+= -DSTATIC_IPC_TABLES
$(IMAGE)_COMMONFLAGS+= -D__PPE_QME
$(IMAGE)_COMMONFLAGS+= -D__OCC_PLAT

OBJS := $(_PPE_TYPE)_init.o

# system objects
OBJS += iota_ppe42.o
OBJS += iota_ppe42_vectors.o
OBJS += iota.o
OBJS += iota_debug_ptrs.o
OBJS += eabi.o
OBJS += ppe42_math.o
OBJS += ppe42_gcc.o
OBJS += ppe42_string.o

# Include IPC support
OBJS += ipc_core.o
OBJS += ipc_init.o

# Include PK trace support
OBJS += pk_trace_core.o
OBJS += pk_trace_big.o
OBJS += pk_trace_binary.o

# archive test
OBJS += archive.o

# component objects
OBJS += iota_main1.o
OBJS += iota_test_uih.o
OBJS += iota_test_irq_priority_table.o


# Add source code directories for the above objects

$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(IOTA_SRCDIR))
#$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(PK_SRCDIR)/ppe42)
$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(BOLTONLIB_SRCDIR)/$(_PPE_TYPE))
$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(PPETRACE_SRCDIR))
$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(OCC_SRCDIR)/occlib)
$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(IOTA_SRCDIR)/test)
$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(BASELIB_SRCDIR))
$(call ADD_PPEIMAGE_SRCDIR,$(IMAGE),$(ROOTPATH)/public/common/utils/imageProcs)

# Include paths
$(call ADD_PPEIMAGE_INCDIR,$(IMAGE), \
	$(IOTA_SRCDIR)/test \
	$(IOTA_SRCDIR) \
	$(PPETRACE_SRCDIR) \
	$(OCC_SRCDIR)/commonlib/include \
	$(OCC_SRCDIR)/commonlib \
	$(POWMANLIB_SRCDIR) \
	$(POWMANLIB_SRCDIR)/occ \
	$(PMLIB_INCDIR)/registers \
	$(ROOTPATH)/public/common/utils/imageProcs \
	$(ROOTPATH)/public/common/generic/fapi2/include \
	$(ROOTPATH)/public/common/generic/fapi2/include/plat \
	)

$(IMAGE)_TRACE_HASH_PREFIX := $(shell echo $(IMAGE) | md5sum | cut -c1-4 \
	| xargs -i printf "%d" 0x{})

#Linker flags
$(IMAGE)_LDFLAGS=-e __system_reset -N -gc-sections -Bstatic

$(call BUILD_PPEIMAGE)
