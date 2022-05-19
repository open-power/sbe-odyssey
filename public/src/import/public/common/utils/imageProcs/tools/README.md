# paktools
A collection of python tools to build an hardware flash pak and work with existing paks
  
## pakbuild
The official tool for building paks for flash.  It uses an input manifest file that describes all the files to be included in the pak.  It includes verification and validation of the output pak.
  
## paktool
Provided to interogate/manipulate the contents of a pak.  Do not use this tool to generate a pak, use `pakbuild`.  This tool gives you plenty of ammo to shoot yourself in the foot.
  
Provided functions:
- `add` : insert a file
- `hash` : generate hash values for the files in the image
- `extract` : extract files from the archive
- `list` : list the files in the archive
- `remove` : remove a file

## flashbuild
Builds a full flash image from a bunch of pak files and a partition table. Each pak file constitutes an individually updateable partition, and flashbuild will pad those paks as needed to fit the partition sizes. A partition table is required to specify the names and sizes of partitions. Example:

```
[
#    name        size
    ("boot",     0x20000),
    ("runtime",  0xC0000),
    ("bmc",      0x8000),
    ("hostboot", 0x8000),
    ("vpd",      0x10000),
]
```
