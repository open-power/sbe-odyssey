# Command table tooling for ROM finite state machine

These are tools to create command tables that drive the minimal state machine
in secure ROM which is used during very early boot.

`cmdtable.py` - the assembler/linker/disassembler/runner tool
`poz_cmdtable_interpreter.[CH]` - FAPI2 code to execute a binary command table
`cmdtable.S` - PPE assembler code to execute a binary command table (in SBE repo)

# Command table instructions

For a given chip, there can only be two command tables - the main table which
is launched from the secure ROM code, and an optional customization table, or
"cust" for short, which may contain chip specific initialization (such as repair
data) and can be called from the main table.

Either command table supports these instructions:

 * `nop` - do nothing
 * `call location` - Only available in the main table. Jump to instruction
   offset `location` in the cust table.
 * `return` - Return to the main table from the cust table, or return to ROM
   code from the main table.
 * `putscom address, mask, data` - Perform a SCOM write operation, with optional
   read-modify-write under mask. Generally, read from `address`, replace every
   bit for which the corresponding bit in `mask` is set with the corresponding
   bit from `data`, then write back. If `mask` is all-1, omit the read operation
   and perform a pure SCOM write.
 * `test address, mask, data, errcode` - Read from `address` and compare under
   mask. Each bit in the read value for which the corresponding `mask` bit is 1
   must match the corresponding bit in `data`. If the value doesn't match, abort
   the command table and return `errcode`.
 * `poll address, mask, data, errcode` - Poll `address` for data under mask.
   Repeatedly read from `address` until the read value matches `data` with the
   same masking as for the `test` instruction. Proceed only when the value
   matches. If the value doesn't match after a hardcoded timeout (about 1s),
   abort the command table and return `errcode`.
   *Special case:* If `errcode` is zero, poll indefinitely, without timeout.
 * `cmpbeq address, mask, data, label` - Compare and Branch if Equal. Read
   `address` and compare to `data` under `mask` as in the `test` instruction.
   If the value matches, jump forward to `label`. If not, continue with the
   next instruction. *Note that jumps must be strictly forward for security
   reasons; it is impossible to jump to the same or previous instructions to
   ensure forward progress and prevent loops.*
 * `cmpbne address, mask, data, label` - Compare and Branch if Not Equal.
   Same as `cmpbeq` except the jump is taken if the data does _not_ match.
 * `b label` - Unconditionally jump to `label`.
 * `fail errcode` - Unconditionally abort and return `errcode`.

# Building a command table

## Assembler

Command tables can be written in a simple assembly language which supports the
instructions described above. In addition, labels are supported just like in
other assembly languages or the C language.

The assembly code is also run through the C preprocessor (`cpp`) before it is
parsed, so you can use things like `#define`, `#ifdef` and `#include` to make
your source more readable and versatile.

*Integer values* can be provided in decimal form (which is the default),
hexadecimal (with a `0x` prefix), octal (with a `0o` prefix) or binary (with
a `0b` prefix). For the `mask` or `data` fields of an instruction, in addition
a left-aligned hex notation is allowed, where you use the `0xl` prefix and the
value will be implicitly expanded to 64 bits by appending zeros, so for example
`0xl0408` will become `0x0408000000000000`.

The `assemble` subcommand of the `cmdtable.py` tool will assemble a single
source file into a single intermediate file:

    ./cmdtable.py assemble test.asm test.tbl

The intermediate format is still a text format but with a very rigid structure
and all arguments expanded to hexadecimal numbers.


## Linker

To build the binary command table, the `link` subcommand must be used. It will
load one or more `.tbl` files and build a single binary command table from them.
The instructions in the `.tbl` files will be concatenated in the order the files
were provided at the command line.

    ./cmdtable.py link step1.tbl step2.tbl -o main.fsm


## Example code

There is a small example in the `example` subdirectory. It's got a `Makefile`
too so you can build it just by typing `make`.


# Running a command table

## Via the `cmdtable.py`

    ./cmdtable.py run [--dry] pu main.fsm [cust.fsm]

This will run a main binary (and optionally a cust binary) on a Cronus target.
If you don't have a Cronus target you can add `--dry` to run on a fake target.
That target will always return zeros on read so your code will probably fail
but it's still helpful for some early evaluation (and for testing the tool
itself :).


## Via the FAPI2 code

I should probably write the FAPI2 code first :)
