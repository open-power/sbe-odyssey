# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/memory/odyssey/hwp/ody_omi_init.mk $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023
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
# EKB-Mirror-To: hostboot

-include 00ody_common.mk

PROCEDURE=ody_omi_init
$(call ADD_MODULE_INCDIR,$(PROCEDURE),$(MSS_ODY_INCLUDES))
$(call ADD_MODULE_INCDIR,$(PROCEDURE),$(ROOTPATH)/public/memory/odyssey/hwp/lib/inband)
$(call BUILD_PROCEDURE)
