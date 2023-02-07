# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/common/hwp/meson.build $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2022,2023
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
import_generic_dir_path = pub_import_dir_path + '/hwp/generic/perv/'

generic_infile_c = [
                    'poz_perv_utils.C',
                    'poz_perv_mod_chiplet_clocking.C',
                    'poz_perv_mod_chip_clocking.C',
                    'poz_perv_mod_misc.C',
                    'poz_chiplet_clk_config.C',
                    'poz_chiplet_pll_setup.C',
                    'poz_bist.C',
                    'poz_chiplet_reset.C',
                    'poz_chiplet_repr_initf.C',
                    'poz_perv_mod_bist.C',
                    'poz_chiplet_unused_psave.C',
                    'poz_chiplet_startclocks.C',
                    'poz_chiplet_arrayinit.C',
                    'poz_tp_arrayinit.C',
                    'poz_tp_arrayinit_cleanup.C',
                    'poz_sbe_spi_cmd.C',
                    'poz_tp_stopclocks.C',
                    'poz_common_stopclocks.C',
                    'poz_gettracearray.C',
                    'poz_fastarray.C',
                   ]

#Create abs path for input files before creating .o
ekb_generic_hwp_c_path = []

foreach generic_infile : generic_infile_c

abs_generic_infile_c_path = import_generic_dir_path + generic_infile
ekb_generic_hwp_c_path += abs_generic_infile_c_path

endforeach

import_util_generic_dir_path = pub_import_dir_path + '/common/utils/imageProcs/'

util_generic_infile_c = [
                        'scan_compression.C',
                        ]

foreach util_generic_infile : util_generic_infile_c

abs_util_generic_infile_c_path = import_util_generic_dir_path + util_generic_infile
ekb_generic_hwp_c_path += abs_util_generic_infile_c_path

endforeach
