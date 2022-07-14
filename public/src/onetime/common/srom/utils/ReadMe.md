Readme, How to use lib layout generator
=========================================================================
How to use public/src/onetime/odyssey/srom/build/utils/gettextsecsize.py?
-------------------------------------------------------------------------

### Please follow below steps to generate liblayout header file:

1. Modify the linker script
   public/src/onetime/odyssey/srom/build/linker/linker.cmd
   that which remove/comment all ".NAME_START_OFFSET;" section (NAME: lib name)
   example: ```". = LIBODYSSEYSROMINIT_START_OFFSET;"```,
            ```". = LIBCMNSROMINIT_START_OFFSET;"```

2. Run the `sbe build`

3. Run the python script with necessary inputs
   example command 1:
   ```gettextsecsize.py -i <inputJsonFile> -s <start offset> hdrgenerator -o <output liblayout file name>```

   example command 2: ```gettextsecsize.py --help```
  ```
    Tool to generate lib layout .H file using lib layout JSON
    usage: Lib Layout Gen -i LIBLAYOUTJSONFILE -s STARTOFFSET [-h] [-m MAPFILE]
                          {hdrgenerator} ...

    Tool to generate Lib layout start offset .H from JSON

    positional arguments:
      {hdrgenerator}
        hdrgenerator        Generate lib layout Header from JSON File

    Required Arguments:
      -i LIBLAYOUTJSONFILE, --libLayoutJsonFile LIBLAYOUTJSONFILE
                            Lib Layout JSON File
      -s STARTOFFSET, --startOffset STARTOFFSET
                            Section Start Offset Address. ex: 0x800

    Optional Arguments:
      -h, --help            Show this help message and exit

    Optional Arguments:
      -m MAPFILE, --mapFile MAPFILE
                            Overide option for .map file from given JSON file
  ```

4. Revert back the linker script, i.e., add revomed lines in linkerscript
   add ".NAME_START_OFFSET;" section

5. Rebuild the project.