#!/bin/bash
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/generic/perv/poz_ipl/generate_scratch_regs.sh $
#
# OpenPOWER sbe Project
#
# Contributors Listed Below - COPYRIGHT 2023,2024
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

if ! /usr/bin/env python3 -c "import jinja2, openpyxl" 2> /dev/null ; then
    echo -e "Please install required packages using\n  pip3 install --user jinja2 openpyxl"
    exit 1
fi

cd $(dirname $0)
OUTDIR=scratch_regs_out
rm -rf $OUTDIR
mkdir $OUTDIR
./scratch_regs.py poz_scratch_regs.xlsx PST scratch_regs.md.template > $OUTDIR/pst_scratch_regs.md
./scratch_regs.py poz_scratch_regs.xlsx Odyssey scratch_regs.md.template > $OUTDIR/ody_scratch_regs.md
./scratch_regs.py poz_scratch_regs.xlsx zMetis scratch_regs.md.template > $OUTDIR/zme_scratch_regs.md
