# paktools
A collection of python tools to build an hardware flash pak and work with existing paks

## pakbuild
The official tool for building paks for flash.  It uses an input manifest file that describes all the files to be included in the pak.  It includes verification and validation of the output pak.

## paktool
Provided to interrogate/manipulate the contents of a pak.  Do not use this tool to generate a pak, use `pakbuild`.  This tool gives you plenty of ammo to shoot yourself in the foot.

Provided functions:
- `add` : insert a file
- `hash` : generate hash values for the files in the image
- `extract` : extract files from the archive
- `list` : list the files in the archive
- `remove` : remove a file

## flashbuild
Builds a full flash image from a bunch of pak files and a partition table. Each pak file constitutes an individually updateable partition, and flashbuild will pad those paks as needed to fit the partition sizes. A partition table is required to specify the names and sizes of partitions. Example:

```
{
    # Version indicator so flashbuild knows how to interpret this file
    "format-version": 2,

    # A partition table may describe multiple memory chips which are numbered starting from 0
    "memories": [
        # SEEPROM 0
        {
            # Space for partitions in bytes; used for overflow checking
            # This is the available size before ECC is added, e.g. a 512K SEEPROM has space for 0x71C70 pre-ECC bytes.
            "size": 0x71C70,
            # Partition sizes are without ECC and each partition follows immediately after the previous one.
            "partitions": [
            #    name    size
                ("boot", 0x8000),
                ("bmc",  0x4000),
                ("host", 0x4000),
                ("rt",   0x61C70),
            ],
        },

        # SEEPROM 1
        {
            # On the second SEEPROM leave 8K of raw space at the end for VPD.
            # That leaves 0x7E000 raw bytes for partitions, or 0x70000 pre-ECC payload bytes
            "size": 0x70000,
            "partitions": [
            #    name    size
                ("rt2",  0x60000),
                ("cust", 0x10000),
            ],
        },
    ],
}

```
