# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/emb/odyssey/io/ioo/odyssey_ioo_files.mk $
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
#ifdef IMAGE

ODYSSEY_IOO-C-SOURCES = io_lib.c \
                pk_app_irq_table.c \
                io_irq_handlers.c \
                io_logger.c \
                io_main.c \
                ioo_thread.c \
                supervisor_thread.c \
                ioo_axo_dl_ifc.c \
                ioo_pipe_ifc.c \
                io_ext_cmd.c \
                ioo_ext_cmd_handlers.c \
                io_init_and_reset.c \
                ioo_init_and_reset.c \
                txbist_main.c \
                ioo_common.c \
                eo_wrappers.c \
                eo_main.c \
                eo_ddc.c \
                eo_ctle.c \
                eo_lte.c \
                eo_dfe.c \
                eo_vga.c \
                eo_loff.c \
                eo_eoff.c \
                eo_esd.c \
                eo_qpa.c \
                eo_vclq_checks.c \
                tx_zcal_tdr.c \
                tx_ffe.c \
                eo_bist_init_ovride.c \
                eo_rxbist_ber.c \
                eo_llbist.c \
                tx_seg_test.c \
                eo_dac_test.c \
                eo_eoff_1_lat.c \
                eo_vga_pathoffset.c \
                eo_rxbist_epoff_final.c \
                tx_dcc_main.c \
                tx_dcc_main_servo.c \
                rx_sigdetbist_test.c \
                tx_txdetrx_bist.c \
                tx_txidle_bist.c \
                io_lab_code.c

ODYSSEY_IOO_OBJECTS = $(ODYSSEY_IOO-C-SOURCES:.c=.o) $(ODYSSEY_IOO-CPP-SOURCES:.C=.o)
#endif
