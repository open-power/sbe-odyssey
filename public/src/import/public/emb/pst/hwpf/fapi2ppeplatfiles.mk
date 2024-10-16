# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/emb/pst/hwpf/fapi2ppeplatfiles.mk $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2024
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
#  @file fapi2ppefiles.mk
#
#  @brief mk for including fapi2 object files
#
#  @page ChangeLogs Change Logs
#  @section fapi2ppeplatfiles.mk
#  @verbatim
#
#
# Change Log ******************************************************************
# Flag     Defect/Feature  User        Date         Description
# ------   --------------  ----------  ------------ -----------
#
# @endverbatim
#
##########################################################################
# Object Files
##########################################################################

#enable this if enable ffdc
#FAPI2PLAT-CPP-SOURCES +=  ffdc.C
FAPI2PLAT-CPP-SOURCES +=  pool.C
FAPI2PLAT-CPP-SOURCES +=  target.C
FAPI2PLAT-CPP-SOURCES +=  plat_utils.C

FAPI2PLAT-S-SOURCES =

FAPI2PLATLIB_OBJECTS += $(FAPI2PLAT-CPP-SOURCES:.C=.o)
FAPI2PLATLIB_OBJECTS += $(FAPI2PLAT-S-SOURCES:.S=.o)
