Gerrit Project: hw/sbe
--------------------------
This project is SBE repository for P & Z Processor.

Before workon:
    * Create a file in ~/.sbe/customrc for any personal rc settings. This will be
    applied during workon

    * To enable bash complete for sb script:
        source public/src/tools/utils/sb/sb_complete

To Setup Environment:
./sb workon [<platform>]
./sb workon [<p11 metis p11_dft metis_dft odyssey all>]
Ex: ./sb workon odyssey

Note: If platform option is not provided, it defaults to "all".
      In this case all images will be built for all supported platforms in the
      repo unless user manually over-rides using mesonwrap option's.

To install required packages to build images:
./sb install

To build a image:
mesonwrap setup
mesonwrap build

To Clean:
mesonwrap clean

For more available build options check:
mesonwrap help

Note: After build, all images are available in images dir.
      All .o, .s, .a etc are available in builddir.

