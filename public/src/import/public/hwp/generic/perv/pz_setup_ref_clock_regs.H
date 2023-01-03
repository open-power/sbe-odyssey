/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/hwp/generic/perv/pz_setup_ref_clock_regs.H $ */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023                             */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
//------------------------------------------------------------------------------
/// @file  pz_setup_ref_clock_regs.H
/// @brief Registers referenced by pz_setup_ref_clock module
//------------------------------------------------------------------------------
// *HWP HW Maintainer   : Sreekanth Reddy (skadapal@in.ibm.com)
// *HWP FW Maintainer   : Raja Das (rajadas2@in.ibm.com)
//------------------------------------------------------------------------------

#pragma once

#include <poz_scom_perv.H>

using namespace scomt::poz;

SCOMT_PERV_USE_FSXCOMP_FSXLOG_GPWRP;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL0;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL0_COPY;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL5;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL5_COPY;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL6;
SCOMT_PERV_USE_FSXCOMP_FSXLOG_ROOT_CTRL6_COPY;

typedef FSXCOMP_FSXLOG_GPWRP_t           GPWRP_t;
typedef FSXCOMP_FSXLOG_ROOT_CTRL0_t      ROOT_CTRL0_t;
typedef FSXCOMP_FSXLOG_ROOT_CTRL0_COPY_t ROOT_CTRL0_COPY_t;
typedef FSXCOMP_FSXLOG_ROOT_CTRL5_t      ROOT_CTRL5_t;
typedef FSXCOMP_FSXLOG_ROOT_CTRL5_COPY_t ROOT_CTRL5_COPY_t;
typedef FSXCOMP_FSXLOG_ROOT_CTRL6_t      ROOT_CTRL6_t;
typedef FSXCOMP_FSXLOG_ROOT_CTRL6_COPY_t ROOT_CTRL6_COPY_t;
