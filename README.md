Gerrit Project: hw/sbe
--------------------------
This project is SBE repository for P & Z Processor.

Before workon:
    * Create a file in ~/.sbe/customrc for any personal rc settings. This will be applied during workon

    * To enable bash complete for sbe script:
        source public/src/tools/utils/sb/sbe_complete

To Setup Environment:
    ./sbe workon [<platform>] [<imageType>]
    ./sbe workon [<p11 metis p11_dft metis_dft odyssey all>][<sppe bldr srom pnor>]
    Ex: ./sbe workon odyssey sppe
        ./sbe workon odyssey pnor

Note: If platform option is not provided, it defaults to "all".
      If image option is not provided, it defaults to "pnor".
      In this case all images will be built for all supported platforms in the
      repo unless user manually over-rides using mesonwrap option's.

To install required packages to build images (this will be done while doing workon):
    ./sbe install

To sync platform env variable with existing meson build dir:
    mesonwrap sync

To re run setup phase:
    mesonwrap reconfigure
(This is required when there is a change in any of the genfile scripts that can't be tracked by meson and that needs to be re run)

To build a image:
    ./sbe build

    OR

    mesonwrap setup
    mesonwrap build

To Clean:
    ./sbe clean

    OR

    mesonwrap clean

For more available build options check:
    mesonwrap help

Note: After build, all images are available in images dir.
      All .o, .s, .a etc are available in builddir.

To Run simics:
    Option-1
    ----------------------------------------------------------------------------
    After building image run following steps.

        ./sbe runsimics

    In simics console run this.
        simics> start-cbs 0
        simics> run     // This code will start exeuting code form reset vector. Put any breakpoint required before this.

    To get the trace run:
        simics> sbe-trace 0

    Option-2
    ----------------------------------------------------------------------------
    After building image run following steps.

        ./sbe runsimics till_boot     // if 'till_boot' option used, simics will run-till sbe-ready state

    To get the trace run:
        simics> sbe-trace 0

    Option-3
    ----------------------------------------------------------------------------
    Build and run using single command
        ./sbe execute [till_boot]     // if 'till_boot' option used, simics will run-till sbe-ready state
