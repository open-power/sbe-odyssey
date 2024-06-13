# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/memory/explorer/hwp/exp_omi_setup.mk $
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
-include 00exp_common.mk

PROCEDURE=exp_omi_setup
$(call ADD_MODULE_INCDIR,$(PROCEDURE),$(MSS_EXP_INCLUDES))
$(call ADD_MODULE_SRCDIR,$(PROCEDURE),$(ROOTPATH)/chips/p10/procedures/hwp/io)
$(call ADD_MODULE_INCDIR,$(PROCEDURE),$(ROOTPATH)/chips/p10/procedures/hwp/io)
lib$(PROCEDURE)_DEPLIBS += p10_io_omi_prbs
lib$(PROCEDURE)_DEPLIBS += exp_collect_explorer_log
$(call BUILD_PROCEDURE)
