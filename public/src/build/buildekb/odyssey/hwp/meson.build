# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/odyssey/hwp/meson.build $
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
import_dir_path = pub_import_dir_path + '/hwp/odyssey/perv/'

infile_c = [
            'ody_tp_arrayinit.C',
            'ody_tp_arrayinit_cleanup.C',
            'ody_tp_initf.C',
            'ody_tp_startclocks.C',
            'ody_tp_init.C',
            'ody_chiplet_clk_config.C',
            'ody_chiplet_reset.C',
            'ody_chiplet_unused_psave.C',
            'ody_chiplet_pll_setup.C',
            'ody_bist_repr_initf.C',
            'ody_abist.C',
            'ody_lbist.C',
            'ody_chiplet_repr_initf.C',
            'ody_chiplet_arrayinit.C',
            'ody_chiplet_initf.C',
            'ody_chiplet_startclocks.C',
            'ody_chiplet_fir_init.C',
            'ody_chiplet_dts_init.C',
            'ody_nest_enable_io.C',
            'ody_ioppe_load.C',
            ]

#Create abs path for input files before creating .o
foreach infile : infile_c

abs_infile_c_path = import_dir_path + infile
abs_infiles_c_path += abs_infile_c_path

endforeach

import_dir_path = pub_import_dir_path + '/memory/odyssey/hwp/'

infile_c = ['ody_ddrphyinit.C',
            '/lib/phy/ody_ddrphy_phyinit_config.C',
            '/lib/phy/ody_phy_utils.C',
           ]

#Create abs path for input files before creating .o
foreach infile : infile_c

abs_infile_c_path = import_dir_path + infile
abs_infiles_c_path += abs_infile_c_path

endforeach

lib_file_name = 'odysseyhwp'

odyssey_hwp_obj_file_c = obj_gen_cpp_hwp.process(abs_infiles_c_path,
                                        extra_args:[
                                                    odyssey_runtime_abs_inc_paths,
                                                    runtime_cmn_abs_inc_paths,
                                                    odyssey_cmn_abs_inc_paths,
                                                    odyssey_hwp_abs_inc_paths,
                                                    ekb_hwp_abs_inc_paths])
infile  = [odyssey_hwp_obj_file_c]

libodysseyhwp = custom_target( lib_file_name,
                                    output : 'lib' + lib_file_name,
                                    input : infile,
                                    command : [ ar,
                                                'crs',
                                                '@OUTPUT@'+'.a',
                                                '@INPUT@'],
                                    build_by_default: true,
                                )

odyssey_hwp_libs = '-l' + lib_file_name
odyssey_hwp_lib_paths = '-L' + meson.current_build_dir()
odyssey_hwp_dep_libs = libodysseyhwp
