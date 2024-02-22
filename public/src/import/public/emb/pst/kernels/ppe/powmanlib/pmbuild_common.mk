# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/emb/pst/kernels/ppe/powmanlib/pmbuild_common.mk $
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

################################################################################
# Adjustable build flags
# ----------------------
#
# _BOOT_STANDALONE:
# - In the absence of PM HWP codes, eg p11_hcode_image_build, this flag controls
#   the programming of some key fields in the PPE image header as well as 
#   controls the functionality of the PSPI link enable check.
# - Usage:
#   - To enable standalone mode, set the flag :=1
#   - To disable standalone mode, leave the flag undefined.
#   - For production code, leave the flag undefined.
#         ==========                      =========
#
# _WDC_ACTING_OCC:
# - This flag controls the functionality of the Spinal images in that it makes
#   the WOF Data Collector (WDC), which runs on Spinal's GPE0, behave like the 
#   OCC(405) wrt sending DB0 to DCE and receiving data via VC1/3 from DCE/OCE.
# - The flag ensures that all Spinal images have the same view of the IRQ 
#   routing table in occhw_irq_config.h. This ensures consistent programming of 
#   the OIRR regs at image boot time.
# - Usage:
#   - To enable the functionality, set the flag :=1
#   - To disable the functionality, leave the flag undefined.
#   - For production code, leave the flag undefined.
#         ==========                      =========
################################################################################

_BOOT_STANDALONE:=

_WDC_ACTING_OCC:=


################################################################################
#
#             *** BE CAREFUL MODIFYING THE FOLLOWING SECTION ***
#
################################################################################

################################################################################
# Conditioning the build flags
# ----------------------------
#
# The following section ensures that an accidental setting of the above flags 
# to, say, <flag>:=0 or <flag>==" " still results in the flag getting undefined.
################################################################################

ifneq ($(_BOOT_STANDALONE), 1)
undefine _BOOT_STANDALONE
endif

ifneq ($(_WDC_ACTING_OCC), 1)
undefine _WDC_ACTING_OCC
endif
