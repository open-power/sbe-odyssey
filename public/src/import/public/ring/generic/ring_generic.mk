# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/ring/generic/ring_generic.mk $
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
PROCEDURE=poz_putRingBackend
$(call ADD_MODULE_INCDIR,$(PROCEDURE),$(ROOTPATH)/public/hwp/generic/perv)
$(call ADD_MODULE_SHARED_OBJ,$(PROCEDURE),poz_perv_mod_misc.o)
$(call ADD_MODULE_SHARED_OBJ,$(PROCEDURE),poz_perv_utils.o)
$(call BUILD_PROCEDURE)

PROCEDURE=poz_putRingUtils
#$(call ADD_MODULE_INCDIR,$(PROCEDURE),$(ROOTPATH)/public/hwp/generic/perv)
#$(call ADD_MODULE_SHARED_OBJ,$(PROCEDURE),poz_perv_mod_misc.o)
#$(call ADD_MODULE_SHARED_OBJ,$(PROCEDURE),poz_perv_utils.o)
$(call BUILD_PROCEDURE)
