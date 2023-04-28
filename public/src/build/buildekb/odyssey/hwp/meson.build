# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/build/buildekb/odyssey/hwp/meson.build $
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

infile_c = [
            '/hwp/odyssey/perv/ody_tp_arrayinit.C',
            '/hwp/odyssey/perv/ody_tp_arrayinit_cleanup.C',
            '/hwp/odyssey/perv/ody_tp_repr_initf.C',
            '/hwp/odyssey/perv/ody_tp_initf.C',
            '/hwp/odyssey/perv/ody_tp_startclocks.C',
            '/hwp/odyssey/perv/ody_tp_init.C',
            '/hwp/odyssey/perv/ody_chiplet_clk_config.C',
            '/hwp/odyssey/perv/ody_chiplet_reset.C',
            '/hwp/odyssey/perv/ody_chiplet_unused_psave.C',
            '/hwp/odyssey/perv/ody_bist_repr_initf.C',
            '/hwp/odyssey/perv/ody_abist.C',
            '/hwp/odyssey/perv/ody_lbist.C',
            '/hwp/odyssey/perv/ody_chiplet_repr_initf.C',
            '/hwp/odyssey/perv/ody_chiplet_arrayinit.C',
            '/hwp/odyssey/perv/ody_chiplet_initf.C',
            '/hwp/odyssey/perv/ody_chiplet_init.C',
            '/hwp/odyssey/perv/ody_chiplet_startclocks.C',
            '/hwp/odyssey/perv/ody_chiplet_fir_init.C',
            '/hwp/odyssey/perv/ody_nest_enable_io.C',
            '/hwp/odyssey/perv/ody_sppe_attr_setup.C',
            '/hwp/odyssey/perv/ody_scratch_regs.C',
            '/hwp/odyssey/perv/ody_scratch_regs_utils.C',
            '/hwp/odyssey/perv/ody_stopclocks.C',
            '/hwp/odyssey/perv/ody_gettracearray.C',
            '/hwp/odyssey/perv/ody_putsram.C',
            '/hwp/odyssey/perv/ody_getsram.C',

            '/memory/odyssey/hwp/ody_sppe_draminit.C',
            '/memory/odyssey/hwp/ody_thermal_init.C',
            '/memory/generic/memory/lib/utils/c_str.C',
            '/memory/odyssey/initfiles/odyssey_scom.C',
            '/memory/odyssey/initfiles/odyssey_mp_scom.C',
            '/memory/odyssey/hwp/lib/power_thermal/ody_throttle.C',
            '/memory/odyssey/hwp/lib/fir/ody_unmask.C',

            # Files needed for IO
            '/hwp/generic/utils/poz_writesram.C',
            '/hwp/generic/utils/poz_readsram.C',
            '/hwp/odyssey/io/ody_omi_unload.C',
            '/hwp/generic/io/io_ppe_cache.C',
            '/hwp/odyssey/io/ody_omi_hss_ppe_load.C',
            '/hwp/odyssey/io/ody_omi_hss_config.C',
            '/hwp/odyssey/io/ody_omi_hss_ppe_start.C',
            '/hwp/odyssey/io/ody_omi_hss_bist_init.C',
            '/hwp/odyssey/io/ody_omi_hss_bist_start.C',
            '/hwp/odyssey/io/ody_omi_hss_bist_poll.C',
            '/hwp/odyssey/io/ody_omi_hss_bist_cleanup.C',
            '/hwp/odyssey/io/ody_omi_hss_init.C',
            '/hwp/odyssey/io/ody_omi_hss_dccal_start.C',
            '/hwp/odyssey/io/ody_omi_hss_dccal_poll.C',
            '/hwp/odyssey/io/ody_omi_hss_tx_zcal.C',
            '/hwp/odyssey/io/ody_omi_pretrain_adv.C',
            '/hwp/odyssey/io/ody_omi_setup.C',
            '/hwp/odyssey/io/ody_omi_train.C',
            '/hwp/odyssey/io/ody_omi_train_check.C',
            '/hwp/odyssey/io/ody_omi_posttrain_adv.C',
            '/memory/odyssey/hwp/ody_scominit.C',

            # Files needed for ody_ddrphyinit
            '/memory/odyssey/hwp/ody_ddrphyinit.C',
            '/memory/odyssey/hwp/lib/phy/ody_draminit_utils.C',
            '/memory/odyssey/hwp/lib/phy/ody_phy_utils.C',
            '/memory/odyssey/hwp/lib/phy/ody_phy_reset.C',
            '/memory/odyssey/hwp/lib/phy/ody_ddrphy_phyinit_config.C',
            '/memory/odyssey/hwp/lib/utils/odyssey_pos.C',
            '/memory/odyssey/hwp/lib/workarounds/ody_dfi_complete_workarounds.C',
            '/memory/odyssey/hwp/lib/workarounds/ody_phy_workarounds.C',

            # Files needed for ody_load_imem and ody_load_dmem
            '/memory/odyssey/hwp/ody_load_imem.C',
            '/memory/odyssey/hwp/ody_load_dmem.C',

            # Files needed for ody_sppe_draminit
            '/memory/odyssey/hwp/lib/phy/ody_draminit_procedure.C',
            '/memory/odyssey/hwp/lib/fir/ody_fir.C',

            # Files needed for ody_load_pie
            '/memory/odyssey/hwp/ody_load_pie.C',
            '/memory/odyssey/hwp/lib/phy/dwc_ddrphy_phyinit_I_loadPIEImage.C',
            '/memory/odyssey/hwp/lib/phy/dwc_ddrphy_phyinit_LoadPieProdCode.C',
            '/memory/odyssey/hwp/lib/phy/dwc_ddrphy_phyinit_LoadPieProdCode_rdimm.C',

            # Files needed for ody_draminit_mc
            '/memory/odyssey/hwp/ody_draminit_mc.C',
            '/memory/odyssey/hwp/lib/mc/ody_port.C',

            # Files needed for ody_enable_ecc
            '/memory/odyssey/hwp/ody_enable_ecc.C',


            # Files needed for ody_thermal_init
            '/memory/odyssey/hwp/lib/power_thermal/ody_thermal_init_utils.C',
            '/hwp/odyssey/perv/ody_dts_read.C',
            # Files needed for Thermal sensor and DQS polling.
            '/memory/odyssey/hwp/ody_thermal_read.C',
            '/memory/odyssey/hwp/ody_dqs_track.C',
            ]

#Create abs path for input files before creating .o
odyssey_hwp_abs_infiles_c_path = []

foreach infile : infile_c

abs_infile_c_path = pub_import_dir_path + infile
odyssey_hwp_abs_infiles_c_path += abs_infile_c_path

endforeach
