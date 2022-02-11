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

echo "### Building test program"
g++ -g -O0 -DARCHIVE_TEST_FIXTURE -I. -I.. -I../../../generic/fapi2/include -I../../../generic/fapi2/include/plat -iquote . -o test archive-test.C ../archive.C ../tinflate.C ../sha3.C

IZTOOL=/afs/bb/u/fenkes/public/imagezip.py

echo "### Creating test zips"
dd if=test of=facontrol.bin bs=12345 count=1             # small file - smaller than dictionary size
cp test pibmem.bin                                       # large file - several times dictionary size
dd if=pibmem.bin of=32ktest.bin bs=1024 count=32         # boundary test - exactly dictionary size; test that stream consumer is called exactly once
dd if=pibmem.bin of=32kplus.bin bs=33026 count=1         # boundary test - exactly first wrap boundary; test that stream consumer is called exactly once
dd if=pibmem.bin of=128ktest.bin bs=1024 count=128       # boundary test - test that stream consumer is called correctly on last chunk
dd if=pibmem.bin of=128kplus.bin bs=131330 count=1       # boundary test - test that stream consumer is called correctly on last chunk
# hashbndy.bin and hashnonbndy.bin are shipped in the repo and happen to compress to exactly a multiple
# and not exactly a multiple of the hash block size respectively, so we can test boundary conditions in the hashing code
$IZTOOL add test.zip facontrol.bin pibmem.bin 32ktest.bin 128ktest.bin 32kplus.bin 128kplus.bin hashbndy.bin hashnonbndy.bin
dd if=pibmem.bin of=stored.bin bs=66001 count=1          # test uncompressed files too, make sure file length is unaligned
$IZTOOL add test.zip stored.bin -m store
./make_nastyzip.py                                       # specially crafted zip with stored block crossing wrap boundary - to test streaming
echo

for flags in 0 1 2 3; do
    echo "### Testing with flags $flags"
    ./test test.zip facontrol.bin   $flags
    ./test test.zip pibmem.bin      $flags
    ./test test.zip 32ktest.bin     $flags
    ./test test.zip 128ktest.bin    $flags
    ./test test.zip 32kplus.bin     $flags
    ./test test.zip 128kplus.bin    $flags
    ./test test.zip hashbndy.bin    $flags
    ./test test.zip hashnonbndy.bin $flags
    ./test test.zip stored.bin      $flags

    ./test nastytest.zip nastytest.bin $flags
    ./test hashblockbndy.zip hashblockbndy.bin $flags
    echo
done
echo '### All tests passed!'
