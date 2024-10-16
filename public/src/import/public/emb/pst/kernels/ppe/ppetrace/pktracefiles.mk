# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/emb/pst/kernels/ppe/ppetrace/pktracefiles.mk $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2021,2024
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
#  @file pkppe42files.mk
#
#  @brief mk for including ppe42 object files
#
#  @page ChangeLogs Change Logs
#  @section pkppe42files.mk
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
# Include Files
##########################################################################



##########################################################################
# Object Files
##########################################################################
PKTRACE-C-SOURCES = pk_trace_core.c pk_trace_big.c pk_trace_binary.c

PKTRACE-S-SOURCES =

PKTRACE-TIMER-C-SOURCES =
PKTRACE-TIMER-S-SOURCES =

PKTRACE-THREAD-C-SOURCES +=
PKTRACE-THREAD-S-SOURCES +=


PKTRACE_OBJECTS = $(PKTRACE-C-SOURCES:.c=.o) $(PKTRACE-S-SOURCES:.S=.o)
