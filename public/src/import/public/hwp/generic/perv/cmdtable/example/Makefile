# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/hwp/generic/perv/cmdtable/example/Makefile $
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
CMDTABLE = ../cmdtable.py

all: main.fsm cust.fsm

clean:
	rm *.fsm *.tbl

%.tbl: %.asm defines.inc
	$(CMDTABLE) assemble $< $@

main.fsm: main1.tbl main2.tbl
	$(CMDTABLE) link $^ -o $@

cust.fsm: cust.tbl
	$(CMDTABLE) link $^ -o $@
