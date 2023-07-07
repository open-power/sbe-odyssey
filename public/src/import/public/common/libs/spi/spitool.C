/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: public/src/import/public/common/libs/spi/spitool.C $          */
/*                                                                        */
/* OpenPOWER sbe Project                                                  */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2023,2024                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */
#include <fapi2ClientCapi.H>
#include <ecmdClientCapi.H>
#include <ecmdDataBuffer.H>
#include <ecmdUtils.H>
#include <ecmdSharedUtils.H>
#include <fapi2.H>
#include <stdio.h>
#include <stdlib.h>
#include "spi_base.H"
#include "spi_memory.H"
#include "spi_tpm.H"

using namespace fapi2;

/**
 * @brief Calls ecmdOutput() but applies string formatting and adds a newline
 */
void _output(const char* msg, ...)
{
    char str[1024];
    va_list args;
    va_start(args, msg);
    vsnprintf(str, sizeof(str), msg, args);
    va_end(args);
    ecmdOutput(str);
}
#define output(msg, args...) _output(msg "\n", ##args)

/**
 * @brief Similar to FAPI_TRY(), will jump to a label "out" on failure
 *        and expects an "rc" variable in scope.
 */
#define TRY(stuff, msg, args...) {                  \
        rc = stuff;                                 \
        if (rc != ECMD_SUCCESS)                     \
        {                                           \
            output("spitool: " msg, ##args);        \
            goto out;                               \
        }                                           \
    }

#define TRY_NOMSG(stuff) {                          \
        rc = stuff;                                 \
        if (rc != ECMD_SUCCESS)                     \
        {                                           \
            goto out;                               \
        }                                           \
    }

#define CHECK(stuff, msg, args...) TRY((stuff) ? ECMD_SUCCESS : ECMD_INVALID_ARGS, msg, ##args)

void help()
{
    static const auto helptext =
    {
        "Usage: spitool [-h] [-k#] [-n#] [-s#] [-p#] [--reset] chip base resp command arguments",
        "  -h         Display this help message.",
        "  -k#        Specify which cage to act on (default = all).",
        "  -n#        Specify which node to act on (default = all).",
        "  -s#        Specify which slot to act on (default = all).",
        "  -p#        Specify which chip position to act on (default = all).",
        "  --reset    Reset SPI controller before first transaction",
        "  chip       Specify chip to operate on",
        "  base       SCOM base address of SPI controller port",
        "  resp       Responder number (0-3)",
        "  command    Subcommand to run, see below",
        "  arguments  Arguments for subcommand, see below",
        "",
        "Execute a raw SPI transaction (in generic SPI or TPM mode):",
        "          rawspi cmd rsp_len [--input INPUT] [--infile FILENAME] [--outfile FILENAME]",
        "          rawtpm cmd rsp_len [--input INPUT] [--infile FILENAME] [--outfile FILENAME]",
        "  cmd        Command (hex string, max 8 bytes) to send to the device. Use \"\" for empty command",
        "  rsp_len    Response length (bytes) to receive back. Use 0 if no response expected.",
        "  --input    Send INPUT (hex string, arbitrary length) to the device after the command",
        "  --infile   Send contents of the binary file FILENAME to the device after the command",
        "  --outfile  Write response to FILENAME as binary data instead of dumping to the console",
        "",
        "Read from a SEEPROM or Flash device:",
        "          eeread addr length [--raw] [--noecc] [--outfile FILENAME]",
        "          flashread addr length [--raw] [--noecc] [--outfile FILENAME]",
        "  addr       Address (hex) to start reading from. Will be ECC translated unless --raw is used",
        "  length     Length in bytes (hex) to read. Will be ECC translated unless --raw is used",
        "  --raw      Disable ECC checking - read raw memory",
        "  --noecc    Ignore ECC and strip it out from the read data",
        "  --outfile  Write read data to FILENAME as binary data instead of dumping to the console",
        "",
        "Write to a SEEPROM or Flash device:",
        "          eewrite addr [--raw] [--input INPUT] [--infile FILENAME]",
        "          flashwrite addr [--raw] [--input INPUT] [--infile FILENAME]",
        "  addr       Address (hex) to start writing. Will be ECC translated unless --raw is used",
        "  --raw      Disable on-the-fly ECC generation - write raw memory",
        "  --noverify Disable verify after write - faster but unsafe",
        "  --input    Write INPUT (hex string, arbitrary length) to memory",
        "  --infile   Write contents of the binary file FILENAME",
        "",
        "Execute a TPM command (command, TPM_GO, response):",
        "          tpmcmd [--locality LOCALITY] [--input INPUT] [--infile FILENAME]",
        "                 [--maxlen LEN] [--outfile FILENAME] [--timeout TIMEOUT_MS]",
        "  --locality TPM locality to access, defaults to 0",
        "  --input    Send INPUT (hex string, arbitrary length) to the device",
        "  --infile   Send contents of the binary file FILENAME to the device",
        "  --maxlen   Maximum response length (bytes) to expect. Defaults to 10MB",
        "  --outfile  Write response to FILENAME as binary data instead of dumping to the console",
        "  --timeout  Timeout (in ms) for the TPM to respond. Defaults to 10s.",
    };

    for (auto& s : helptext)
    {
        output("%s", s);
    }
}

enum tool_command
{
    CMD_GROUP_MASK = 0xF0,
    CMD_GROUP_BASE = 0x00,
    CMD_GROUP_TPM  = 0x20,

    CMD_RAWSPI = CMD_GROUP_BASE,
    CMD_EEREAD,
    CMD_EEWRITE,
    CMD_FLASHREAD,
    CMD_FLASHWRITE,

    CMD_RAWTPM = CMD_GROUP_TPM,
    CMD_TPMCMD,

    CMD_UNKNOWN = 0xFF
};

static const struct
{
    tool_command cmd;
    const char* name;
    int nargs;
} command_names[] =
{
    { CMD_RAWSPI,     "rawspi",     2 },
    { CMD_EEREAD,     "eeread",     2 },
    { CMD_EEWRITE,    "eewrite",    1 },
    { CMD_FLASHREAD,  "flashread",  2 },
    { CMD_FLASHWRITE, "flashwrite", 1 },
    { CMD_RAWTPM,     "rawtpm",     2 },
    { CMD_TPMCMD,     "tpmcmd",     0 },
};

struct
{
    bool do_reset = false;
    bool verify = true;
    bool secure = false;
    int locality = 0;
    char* chip = NULL;
    uint32_t base_addr = 0;
    int responder = 0;
    tool_command command = CMD_UNKNOWN;
    ecmdDataBuffer input;
    char* outfile = NULL;
    ecmdDataBuffer dev_cmd;
    size_t maxlen = 10 * 1024 * 1024;
    size_t output_len = 0;
    spi::ecc_mode ecc_mode = spi::ECC_ENABLED;
    uint32_t rw_start_addr = 0;
    uint32_t timeout = 10000;
} args;

int parse_args(int& argc, char**& argv)
{
    int rc = ECMD_SUCCESS;
    char* opt;
    char* endptr;

    // Parse out all optional arguments
    TRY(ecmdCommandArgs(&argc, &argv), "Error calling ecmdCommandArgs.");

    args.do_reset = ecmdParseOption(&argc, &argv, "--reset");
    args.verify = !ecmdParseOption(&argc, &argv, "--noverify");

    if (ecmdParseOption(&argc, &argv, "--raw"))
    {
        args.ecc_mode = spi::ECC_DISABLED;
    }

    if ((opt = ecmdParseOptionWithArgs(&argc, &argv, "--maxlen")))
    {
        args.maxlen = strtoul(opt, &endptr, 0);
        CHECK(*endptr == 0, "Invalid maxlen parameter");
    }

    if ((opt = ecmdParseOptionWithArgs(&argc, &argv, "--timeout")))
    {
        args.timeout = strtoul(opt, &endptr, 0);
        CHECK(*endptr == 0, "Invalid timeout parameter");
    }

    if ((opt = ecmdParseOptionWithArgs(&argc, &argv, "--locality")))
    {
        args.locality = strtoul(opt, &endptr, 0);
        CHECK(*endptr == 0, "Invalid locality parameter");
        CHECK(args.locality < 5, "Invalid locality value, must be 0..4");
    }

    if (ecmdParseOption(&argc, &argv, "--noecc"))
    {
        CHECK(args.ecc_mode == spi::ECC_ENABLED, "--raw and --noecc cannot both be specified");
        args.ecc_mode = spi::ECC_DISCARD;
    }

    if ((opt = ecmdParseOptionWithArgs(&argc, &argv, "--input")))
    {
        TRY(ecmdReadDataFormatted(args.input, opt, "x"), "Error parsing --input data");
        CHECK((args.input.getBitLength() & 7) == 0, "--input data must be an even number of hex nibbles");
    }

    if ((opt = ecmdParseOptionWithArgs(&argc, &argv, "--infile")))
    {
        CHECK(args.input.getBitLength() == 0, "--input and --infile cannot both be specified");
        TRY(args.input.readFile(opt, ECMD_SAVE_FORMAT_BINARY_DATA), "Error reading --infile file");
    }

    if ((opt = ecmdParseOptionWithArgs(&argc, &argv, "--outfile")))
    {
        args.outfile = opt;
    }

    // Now that all options have been removed from argv we can extract the positionals
    CHECK(argc >= 5, "Not enough arguments");
    args.chip = argv[1];
    args.base_addr = strtoul(argv[2], &endptr, 16);
    CHECK(*endptr == 0, "Invalid base address");
    args.responder = strtoul(argv[3], &endptr, 0);
    CHECK(*endptr == 0, "Invalid responder number");
    CHECK((args.responder >= 0) && (args.responder <= 3), "Invalid responder number");

    for (auto c : command_names)
    {
        if (!strcmp(argv[4], c.name))
        {
            CHECK(argc == (5 + c.nargs), "Invalid number of arguments for %s command", c.name);
            args.command = c.cmd;
            break;
        }
    }

    CHECK(args.command != CMD_UNKNOWN, "Unknown command: %s", argv[4]);

    switch (args.command)
    {
        case CMD_RAWSPI:
        case CMD_RAWTPM:
            TRY(ecmdReadDataFormatted(args.dev_cmd, argv[5], "x"), "Error parsing device command");
            CHECK(args.dev_cmd.getByteLength() <= 8, "Device command is too long, max 8 bytes - use -input or -infile for more");
            args.output_len = strtoul(argv[6], &endptr, 0);
            CHECK(*endptr == 0, "Invalid response length");
            break;

        case CMD_EEREAD:
        case CMD_FLASHREAD:
            args.rw_start_addr = strtoul(argv[5], &endptr, 16);
            CHECK(*endptr == 0, "Invalid start address");
            args.output_len = strtoul(argv[6], &endptr, 16);
            CHECK(*endptr == 0, "Invalid read length");
            break;

        case CMD_EEWRITE:
        case CMD_FLASHWRITE:
            args.rw_start_addr = strtoul(argv[5], &endptr, 16);
            CHECK(*endptr == 0, "Invalid start address");
            CHECK(args.input.getByteLength() > 0, "One of --input or --infile must be provided");
            break;

        case CMD_TPMCMD:
            CHECK(args.input.getByteLength() > 0, "One of --input or --infile must be provided");
            args.output_len = args.maxlen;
            break;

        default:
            break;
    }

    return ECMD_SUCCESS;

out:
    return ECMD_INVALID_ARGS;
}

void hexdump(const uint8_t* const data, size_t len)
{
    static const int GROUP_SIZE = 4;
    static const int NUM_GROUPS = 4;
    static const int BYTES_PER_LINE = GROUP_SIZE * NUM_GROUPS;
    static const char hextab[] = "0123456789abcdef";

    char hex[GROUP_SIZE * 2 * NUM_GROUPS + NUM_GROUPS];
    char print[GROUP_SIZE * NUM_GROUPS + 1];

    memset(hex, 32, sizeof(hex) - 1);
    memset(print, 32, sizeof(print) - 1);
    hex[sizeof(hex) - 1] = 0;
    print[sizeof(print) - 1] = 0;

    size_t hex_ofs = 0;

    for (size_t i = 0; i < len; i++)
    {
        const size_t line_ofs = i % BYTES_PER_LINE;
        const uint8_t c = data[i];

        hex[hex_ofs++] = hextab[c >> 4];
        hex[hex_ofs++] = hextab[c & 15];
        print[line_ofs] = isprint(c) ? c : '.';

        if ((line_ofs % GROUP_SIZE) == (GROUP_SIZE - 1))
        {
            hex_ofs++;
        }

        if ((line_ofs == (BYTES_PER_LINE - 1)) || (i == len - 1))
        {
            // Flush out one line of dump
            output(" %04x | %s |%s|", i / BYTES_PER_LINE * BYTES_PER_LINE, hex, print);
            hex_ofs = 0;
            memset(hex, 32, sizeof(hex) - 1);
            memset(print, 32, sizeof(print) - 1);
        }
    }
}

int emit_output_data(const uint8_t* const output_data, size_t output_len)
{
    int rc = ECMD_SUCCESS;

    if (output_len)
    {
        if (args.outfile)
        {
            FILE* f = fopen(args.outfile, "w");
            CHECK(f, "Failed to open %s for writing", args.outfile);
            size_t written = fwrite(output_data, output_len, 1, f);
            CHECK(written, "Failed to write output file");
            fclose(f);
        }
        else
        {
            hexdump(output_data, output_len);
        }
    }

out:
    return rc;
}

ReturnCode run_spibase(
    Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const uint8_t* const input_data, const size_t input_len,
    uint8_t* const output_data, const size_t output_len)
{
    spi::SPIPort port(i_target, args.base_addr, args.responder, args.ecc_mode);
    spi::AbstractMemoryDevice* mem = NULL;

    if (args.do_reset)
    {
        FAPI_TRY(port.reset_controller());
    }

    switch (args.command)
    {
        case CMD_EEREAD:
        case CMD_EEWRITE:
            mem = new spi::SEEPROMDevice(port, 0x1000000, args.verify);
            break;

        case CMD_FLASHREAD:
        case CMD_FLASHWRITE:
            {
                spi::FlashDevice::device_type type;
                FAPI_TRY(spi::FlashDevice::detect_device(port, type));
                mem = new spi::FlashDevice(port, type, 0x1000000,
                                           new uint8_t[spi::FlashDevice::ERASE_BUFFER_SIZE], args.verify);
            }

        default:
            break;
    }

    switch (args.command)
    {
        case CMD_RAWSPI:
            {
                int cmd_len = args.dev_cmd.getByteLength();
                args.dev_cmd.shiftRightAndResize(64 - args.dev_cmd.getBitLength());
                uint64_t cmd = args.dev_cmd.getDoubleWord(0);
                FAPI_TRY(port.transaction(cmd, cmd_len, input_data, input_len, output_data, output_len));
                break;
            }

        case CMD_EEREAD:
        case CMD_FLASHREAD:
            FAPI_TRY(mem->read(args.rw_start_addr, output_len, output_data));
            break;

        case CMD_EEWRITE:
        case CMD_FLASHWRITE:
            FAPI_TRY(mem->write_begin(args.rw_start_addr, input_len));
            FAPI_TRY(mem->write_data(input_data, input_len));
            FAPI_TRY(mem->write_end());
            break;

        default:
            break;
    }

fapi_try_exit:
    return current_err;
}

ReturnCode run_tpm(
    Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target,
    const uint8_t* const input_data, const size_t input_len,
    uint8_t* const output_data, size_t& output_len)
{
    spi::SPITPMPort port(i_target, args.base_addr, args.responder);

    if (args.do_reset)
    {
        FAPI_INF("Resetting port");
        FAPI_TRY(port.reset_controller());
    }

    if (args.command == CMD_RAWTPM)
    {
        int cmd_len = args.dev_cmd.getByteLength();
        args.dev_cmd.shiftRightAndResize(64 - args.dev_cmd.getBitLength());
        uint64_t cmd = args.dev_cmd.getDoubleWord(0);
        FAPI_TRY(port.transaction(cmd, cmd_len, input_data, input_len, output_data, output_len));
    }
    else
    {
        uint32_t rsp_len;
        spi::TPM tpm(port);

        tpm.set_locality(args.locality);
        FAPI_TRY(tpm.command(input_data, input_len, output_data, rsp_len, output_len, args.timeout));
        output_len = rsp_len;
    }

fapi_try_exit:
    return current_err;
}

ReturnCode run(Target<TARGET_TYPE_ANY_POZ_CHIP>& i_target)
{
    const size_t input_len = args.input.getByteLength();
    const uint8_t* const input_data = !input_len ? NULL : [input_len]()
    {
        auto tmp = new uint8_t[input_len];
        args.input.memCopyOut(tmp, input_len);
        return tmp;
    }();

    size_t output_len = args.output_len;
    uint8_t* const output_data = !output_len ? NULL : new uint8_t[output_len];

    switch (args.command & CMD_GROUP_MASK)
    {
        case CMD_GROUP_BASE:
            FAPI_TRY(run_spibase(i_target, input_data, input_len, output_data, output_len));
            break;

        case CMD_GROUP_TPM:
            FAPI_TRY(run_tpm(i_target, input_data, input_len, output_data, output_len));
            break;
    }

    FAPI_TRY(emit_output_data(output_data, output_len) ? FAPI2_RC_FALSE : FAPI2_RC_SUCCESS);

fapi_try_exit:
    delete[] input_data;
    delete[] output_data;
    return current_err;
}

int loop_targets()
{
    int rc = ECMD_SUCCESS;
    ecmdChipTarget target;
    ecmdLooperData looper;
    bool target_found = false;

    target.chipType      = args.chip;
    target.chipTypeState = ECMD_TARGET_FIELD_VALID;
    target.cageState     = ECMD_TARGET_FIELD_WILDCARD;
    target.nodeState     = ECMD_TARGET_FIELD_WILDCARD;
    target.slotState     = ECMD_TARGET_FIELD_WILDCARD;
    target.posState      = ECMD_TARGET_FIELD_WILDCARD;
    target.threadState   = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitTypeState  = ECMD_TARGET_FIELD_UNUSED;
    target.chipUnitNumState   = ECMD_TARGET_FIELD_UNUSED;

    TRY(ecmdConfigLooperInit(target, ECMD_SELECTED_TARGETS_LOOP_DEFALL, looper),
        "Error calling ecmdConfigLooperInit.");

    // loop over specified configured positions
    while (ecmdConfigLooperNext(target, looper))
    {
        target_found = true;
        output("%s", ecmdWriteTarget(target).c_str());

        auto fapi_target = Target<TARGET_TYPE_ANY_POZ_CHIP>(&target);
        auto fapi_rc = run(fapi_target);
        CHECK(fapi_rc == FAPI2_RC_SUCCESS, "Run failed");
    }

    CHECK(target_found, "No targets found, procedure did not run!");

out:
    return rc;
}

int main(int argc, char* argv[])
{
    int rc = ECMD_SUCCESS;

    TRY(ecmdLoadDll(""), "Error calling ecmdLoadDll.");
    TRY(fapi2InitExtension(), "Error calling fapi2InitExtension.");

    // Parse out user options (excluding -pX, -cX, -coe, -debug, etc
    if (ecmdParseOption(&argc, &argv, "-h"))
    {
        help();
        rc = ECMD_SUCCESS;
        goto out;
    }

    TRY_NOMSG(parse_args(argc, argv));
    TRY_NOMSG(loop_targets());

out:
    ecmdUnloadDll();
    return rc;
}
