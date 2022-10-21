# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/memory/odyssey/common/scominfo/wrapper/odyssey_scom_xlate.mk $
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
EXE=odyssey_scom_xlate
$(call ADD_EXE_INCDIR,$(EXE),$(ROOTPATH)/chips/p10/common/scominfo/wrapper/)
$(call ADD_EXE_SRCDIR,$(EXE),$(ROOTPATH)/chips/ocmb/odyssey/common/scominfo)
OBJS=odyssey_scom_xlate.o odyssey_cu_utils.o odyssey_scominfo.o odyssey_scom_addr.o odyssey_clockcntl.o
# To enable DEBUG_PRINT
# $(EXE)_COMMONFLAGS+= -DDEBUG_PRINT=1
$(call BUILD_EXE)
