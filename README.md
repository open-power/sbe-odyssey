Gerrit Project: hw/sbe
--------------------------
This project is SBE repository for P & Z Processor.

To Setup Environment:
./sb workon [<platform>]
./sb workon [<p11 metis p11_dft metis_dft odyssey all>]
Ex: ./sb workon metis

Note: If platform option is not provided, it defaults to "all".
      In this case all images will be built for all supported platforms in the
      repo unless user manually over-rides using mesonwrap option's.

To install required packages to build images:
./sb install

To build a image:
mesonwrap setup
mesonwrap compile
mesonwrap install

For more available build options check:
mesonwrap help
