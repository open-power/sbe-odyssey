#!/bin/bash -e
# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: public/src/import/public/common/utils/imageProcs/archive-tests/testsuite.sh $
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

#set -x

echo "### Building test programs"
g++ -g -O0 -DARCHIVE_TEST_FIXTURE -I. -I.. -I../../../generic/fapi2/include -I../../../generic/fapi2/include/plat -iquote . -o test archive-test.C util.C ../archive.C ../tinflate.C ../sha3.C
g++ -g -O0 -DARCHIVE_TEST_FIXTURE -I. -I.. -I../../../generic/fapi2/include -I../../../generic/fapi2/include/plat -iquote . -o append append-test.C util.C ../archive.C ../tinflate.C ../sha3.C

PAKTOOL=../tools/paktool

echo "### Creating test paks"
dd if=test of=facontrol.bin bs=12345 count=1             # small file - smaller than dictionary size
dd if=test of=pibmem.bin bs=264537 count=1               # large file - several times dictionary size
dd if=pibmem.bin of=32ktest.bin bs=1024 count=32         # boundary test - exactly dictionary size; test that stream consumer is called exactly once
dd if=pibmem.bin of=32kplus.bin bs=33026 count=1         # boundary test - exactly first wrap boundary; test that stream consumer is called exactly once
dd if=pibmem.bin of=128ktest.bin bs=1024 count=128       # boundary test - test that stream consumer is called correctly on last chunk
dd if=pibmem.bin of=128kplus.bin bs=131330 count=1       # boundary test - test that stream consumer is called correctly on last chunk
# hashbndy.bin and hashnonbndy.bin are shipped in the repo and happen to compress to exactly a multiple
# and not exactly a multiple of the hash block size respectively, so we can test boundary conditions in the hashing code
$PAKTOOL add test.pak facontrol.bin pibmem.bin 32ktest.bin 128ktest.bin 32kplus.bin 128kplus.bin hashbndy.bin hashnonbndy.bin
dd if=pibmem.bin of=stored.bin bs=66001 count=1          # test uncompressed files too, make sure file length is unaligned
$PAKTOOL add test.pak stored.bin --method store
cp pibmem.bin ppc.bin                                    # large file - test for PPC instruction filter
$PAKTOOL add test.pak ppc.bin --method zlib_ppc
$PAKTOOL add base.pak pibmem.bin 32ktest.bin             # test paks for append operations
$PAKTOOL add update.pak facontrol.bin 32kplus.bin
./make_nastypak.py                                       # specially crafted pak with stored block crossing wrap boundary - to test streaming
echo

for flags in 0 1 2 3; do
    echo "### Testing with flags $flags"
    ./test test.pak facontrol.bin   $flags
    ./test test.pak pibmem.bin      $flags
    ./test test.pak 32ktest.bin     $flags
    ./test test.pak 128ktest.bin    $flags
    ./test test.pak 32kplus.bin     $flags
    ./test test.pak 128kplus.bin    $flags
    ./test test.pak hashbndy.bin    $flags
    ./test test.pak hashnonbndy.bin $flags
    ./test test.pak stored.bin      $flags
    ./test test.pak ppc.bin         $flags

    ./test nastytest.pak nastytest.bin $flags
    ./test hashblockbndy.pak hashblockbndy.bin $flags
    echo
done

echo "### Testing append functions"

./append base.pak update.pak 128ktest.bin pibmem.bin 32kplus.bin 32ktest.bin facontrol.bin 128ktest.bin

echo '### All tests passed!'
