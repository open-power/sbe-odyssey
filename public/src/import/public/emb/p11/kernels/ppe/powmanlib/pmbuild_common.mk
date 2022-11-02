# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/emb/p11/kernels/ppe/powmanlib/pmbuild_common.mk $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022
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

# _WDC_ACTING_OCC build flag:
# - This flag controls the functionality of the Spinal images in that it makes
#   the WOF Data Collector (WDC), which runs on Spinal's GPE0, behave like the 
#   OCC(405) wrt sending DB0 to DCE and receiving VC1/3 from DCE/OCE.
# - The flag ensures that all Spinal images have the same view of the IRQ 
#   routing table in occhw_irq_config.h. This ensures consistent programming of 
#   the OIRR regs a image boot time.
# - To enable the functionality, set the flag :=1.
# - For production code, leave the flag undefined.
_WDC_ACTING_OCC:=1
