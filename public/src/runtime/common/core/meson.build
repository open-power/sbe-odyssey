# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/runtime/common/core/meson.build $
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

COMMON_CHIPOPS_JSON = files('chipop_table.json')[0]
runtime_cmn_core_dir_path = meson.global_source_root() + '/public/src/runtime/common/core/'

infile_c = ['sbeSpMsg.C',
            'irqutils.C',
            'sbeirqhandler.C',
            'sbeFifoMsgUtils.C',
            'sbeglobals.C',
            'chipophandlerutils.C',
            'sberegaccess.C',
            'sbestatesutils.C',
            'sbecmdgeneric.C',
            'sbecmnhwpwrapper.C',
            'sbestreampaktohwp.C',
            'sbecmdgenericffdc.C',
            'sbetimersvc.C',
            'initthreads.C',
            'istep.C',
            'ipl.C',
            'istepIplUtils.C',
            'sbebootutils.C',
            'sbeexceptionhandler.C',
           ]

abs_runtime_cmn_core_c_path = []

foreach infile : infile_c
  abs_runtime_cmn_core_c_path += runtime_cmn_core_dir_path + infile
endforeach

# threads files
abs_runtime_cmn_core_infra_threads_c_files = files('initthreads.C',
                                         )
# irq files
abs_runtime_cmn_core_infra_irq_c_files = files('irqutils.C',
                                  'sbeirqhandler.C',
                                  )
# globals files
abs_runtime_cmn_core_infra_globals_c_files = files('sbeglobals.C',
                                      )
# regaccess files
abs_runtime_cmn_core_infra_regaccess_c_files = files('sberegaccess.C',
                                       )
# stateutils files
abs_runtime_cmn_core_infra_stateutils_c_files = files('sbestatesutils.C',
                                         )
# sbeSpMsg files
abs_runtime_cmn_core_infra_spMsg_c_files = files('sbeSpMsg.C',
                                   )
# chipop_handler files
abs_runtime_cmn_core_infra_chipop_handler_c_files = files('chipophandlerutils.C',
                                            )
# istep files
abs_runtime_cmn_core_infra_istep_c_files = files('istep.C',
                                                  'istepIplUtils.C',
                                                )
# ffdc files
abs_runtime_cmn_core_infra_ffdc_c_files = files('sbecmdgenericffdc.C',
                                  )
# Exception handler files.
exception_handler_cmn_core_infra_c_path = files('sbeexceptionhandler.C')
