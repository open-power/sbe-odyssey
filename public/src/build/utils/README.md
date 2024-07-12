#ecc_ppc64le - NOR ECC Tool v2.0.0 for Linux On Power(LoP) systems

The source code of this tool is available in https://github.com/open-power/ffs (commit 3ec70fb)

**The steps for building this tool on LoP systems:**
**------------------------------------------------**
1. git clone https://github.com/open-power/ffs

2. cd ffs

3. autoreconf -i && ./configure && make

The binary will be available in ecc/ecc. This binary file has been renamed as ecc_ppc64le to
indicate the supported architecture.


