# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/common/generic/fapi2/test/Makefile $
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
all: fapi2_file_test

-include *.dep

INCLUDEPATH = -I ../include -I ../include/plat -I .

%.o: %.C
	g++ $(INCLUDEPATH) -MM -o $*.dep $<
	g++ $(INCLUDEPATH) -c -o $@ $<

fapi2_file_test: fapi2_file_test.o
	g++ -o $@ $^
