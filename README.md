# Gerrit Project: hw/sbe

This project is SBE repository for P, Odyssey & Z projects.

### Before workon:

- Create a file `~/.sbe/customrc` for any personal rc settings. This will be applied during `workon`

- To enable bash complete for sbe script:

```
source public/src/tools/utils/sbe/sbe_complete
```

### To Setup Environment:

```
./sbe workon <project> <imageType>

For example: ./sbe workon odyssey pnor
```

- To list out available `project` and `imageType`

```
./sbe --help
```
Refer at `workon` command usage.

**Note:**

- If `project` option is not provided, it defaults to `all`.
- If `imageType` option is not provided, it defaults to `all`.

In this case all images will be built for all supported platforms in the repo unless user manually over-rides using mesonwrap option's.

### To build SBE vanilla image:

```
./sbe build
```

### To Clean:

```
./sbe clean
```

**Note:**

- After build, all images are available in `images` directory that will be created in sbe root directory during the `build`.
- All .o, .s, .a etc are available in `builddir` that will be created in sbe root directory during the `build`.

### To prepare genesis NOR image:

- Please refer [here](./internal/src/packaging/odyssey/prepareImage.md) to prepare odyssey image.

### To run simics:

All options will take care to prepare the genesis NOR image to use in the simics.

#### Option-1:

- After building the SBE vanilla image run below command.
```
./sbe runsimics
```
- In simics console run this.
```
simics> start-cbs 0
simics> run           # This code will start exeuting code form reset vector.
                      # Put any breakpoint before this if required.
```

#### Option-2:

- After building the SBE vanilla image run below command.
```
./sbe runsimics till_boot     # if 'till_boot' option used, simics will run-till sbe-ready state.
```

#### Option-3:

- A single command to do the build, prepare nor image, setup simics and runsimics.
```
./sbe execute [till_boot]     # if 'till_boot' option used, simics will run-till sbe-ready state.
```

### To get SBE traces:
- Run below command at the simics console.
```
simics> sbe-trace 0
```

### To run test-cases:

- A single command to do the build, prepare nor image, setup simics, runsimics and run test cases.
```
./sbe citest
```
