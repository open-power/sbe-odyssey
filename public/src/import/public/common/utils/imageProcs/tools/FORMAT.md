# PAK Format Documentation

## Introduction

### **Goals**
The pak archive format aims to provide a unified solution for storing dissimilar types of data inside a single container, where each piece of data (named "file" from here on) can be referenced by name. There are multiple applications for such a container across the firmware landscape, and the pak format strives to fit the bill for as many of these applications as possible without becoming itself overloaded with complexity.

### **Why PAK?**
Each archive is just a **PA**c**K**age of files.<br>
_(The creators may also have spent their younger years playing too much Quake)_

### **Features**
**Self-describing:** A pak is a container for individual files which are referred to by their names. Given a pak, all information about the files contained within (their names, compressed sizes, uncompressed sizes, checksums…) can be determined purely from the pak itself with no external information required.
<br><br>
**Optional compression:** File contents may be (and most of the time are) stored in a lossless compressed form to save space. When a file is being read, decompression is handled in a way transparent to the caller; the caller will simply receive the original file contents regardless of whether the file was stored in compressed or uncompressed form.
<br><br>
**Concatenateable**: Given two or more paks, they can be easily merged into a single pak comprised of the union of all the files in the original paks.  This is done by simply concatenating the paks, i.e. appending each pak to the end of the previous pak (minus the end marker).
<br><br>
**Embedded Optimized:** The pak entry structure is optimized for 8 byte big-endian reads common in our embedded environments.

## pak layout overview
```
|hdr|payload|hdr|payload|hdr|payload|end|
  |           |           |           |
   \--->>----/ \--->>----/ \--->>----/
```
As shown above, a pak archive is made up from consecutive sets of entries, each entry comprising a variable-length header (denoted `hdr` in the figure).  The header contains the entry type among other fields and is then followed by the entry payload.  Every header contains the necessary info to skip to the next header.
<br><br>
While the header can be variable length, it is guaranteed to be an initial 8 bytes that describes the type of entry to follow.  The three types of headers are:
1. [PAK_FILE](#PAK_FIILE) - a file in the pak
2. [PAK_PAD](#PAK_PAD) - padding in the pak
3. [PAK_END](#PAK_END) - marker for the end of the pak

The first 4 bytes of the 8 byte read tells the type of header, the rest of the format is then determined from the header type.  Descriptions of each of the entry types follow.

## **PAK_FILE**
This is the most common header found in a pak, indicating a file stored in the pak.  The format of an entry in a pak file is comprised of a header, stored in two parts, and the payload.

The next header can by found by fully reading the file header and then advancing to the `psize` location.

### **Header Core**
As described above, the first part of the header is a fixed 8 bytes. After the magic bytes, the size of the header extended to read is given.

| Offset | Size | Name | Description |
| --- | --- | --- | --- |
| 0 | 4 | `magic` | The magic bytes defining the start of a new file.<br> Hex: `x50414B21` aka ASCII `PAK!` |
| 4 | 2 | `version` | pak entry version, currently `x0001` |
| 6 | 2 | `hesize` | The size of the header extended to read next, in bytes |

### **Header Extended**
The second part of the header is of an 8 byte aligned size, defined by `hesize` in the header core
<br><br>
This section of the header contains all the critical self describing data for the file, including the name and size of the payload to read.
| Offset | Size | Name | Description |
| --- | --- | --- | --- |
| 8 | 1 | `flags` | Special indicators [[values]](#flags) |
| 9 | 1 | `method` | How the data is stored in the payload [[values]](#compression-methods) |
| 10 | 2 | `nsize` | The size of the name field at the end of this section |
| 12 | 4 | `crc` | CRC of the uncompressed data |
| 16 | 4 | `csize` | Compressed size of the data after processing by `method` |
| 20 | 4 | `dsize` | Decompressed size of the original data |
| 24 | 4 | `psize` | The total payload size, accounting for alignment or fixed size considerations. |
| 28 | `nsize` | `name` | The name of the pak entry |

### **Payload**
The payload contains the actual contents of the file stored within the pack.  The size is always 8 byte aligned and given by `psize` above.  `psize` will also always be `>= csize`
| Offset | Size | Name | Description |
| --- | --- | --- | --- |
| 8 + `hesize` | `psize` | `payload` | The contents of the file as stored by `method` and accounting for alignment or `flags` |

## **PAK_PAD**
This is a special header useful for padding out space in a pak.  This is used when writing a pak to flash chip that is partition into sections.  The pad helps ensure each part of the pak lands in the appropriate section.

The next header can by found by fully reading this header and then advancing by `padsize` bytes.

| Offset | Size | Name | Description |
| --- | --- | --- | --- |
| 0 | 4 | `magic` | The magic bytes defining the start of a pad.<br> Hex: `x50414B50` aka ASCII `PAKP` |
| 4 | 4 | `padsize` | The size of pad to follow the header, in bytes |

## **PAK_END**
The end marker is used to indicate the end of the pak. Code that traverses the linked list of file headers in search of a specific file should use this marker as an indication that there are no more files to search and it should abort its search, returning a "file not found" error code to the caller.
<br><br>
The end marker also contains a field indicating the total size of the pak in bytes, inclusive of the end marker size. In addition to marking the end of the archive also indicates the full archive size and can thus be used to locate the beginning of the archive given its end.
| Offset | Size | Name | Description |
| --- | --- | --- | --- |
| 0 | 4 | `magic` | The magic bytes defining the end of the pak.<br> Hex: `x2F50414B` aka ASCII `/PAK` |
| 4 | 4 | `size` | The total size of the pak |

### **Concatenation**
Since a pak archive is basically a linked list of files it is easy to concatenate several archives into a single new archive containing all files from the source archives. The only complication is the end marker - when appending one pak to another, the second pak must be written over the beginning of the end marker, and the second paks end marker must be updated to reflect the size of the combined archive.
<br><br>
A sample algorithm in python would be:
```
new_archive = bytearray()
for archive in archives_to_combine:
    new_archive += archive[:-8] # Remove the end marker
# Add new end marker - 8 bytes
new_archive += b"/PAK"
# Add 4 bytes to account for the new 4 byte size field being added
new_archive += (len(new_archive) + 4).to_bytes(4, "big")
```
This is just a sample based on working with the archives in memory, you can adjust for working directly with files, etc..

## Definitions

This sections contains the documentation of the valid values in the pak field header and other supplemental information helpful in understanding the pak format.

### **Compression Methods**
| Value | Name | Definition |
| ---- | --- | --- |
| `x01` | `store` | The data is stored as is |
| `x02` | `zlib` | The data is stored zlib compressed, at max compression (level 9) |
| `x03` | `zlib_fast` | The data is stored zlib compressed, at a lower compression level (level 5) to balance speed and size |
| `x04` | `zlib_ppc` | The data is stored zlib compressed, after being run through a special filter for ppc instructions to increase compression |


### **Flags**
| Value | Name | Definition |
| ---- | --- | --- |
| `x20` | `PAK_FLAG_FIXED_SIZE` | The entry is of a fixed sized<br>The `psize` value may be larger than the data |

## History
All changes to this document will be controlled by git and count on sufficiently descriptive commit messages to properly document updates.
