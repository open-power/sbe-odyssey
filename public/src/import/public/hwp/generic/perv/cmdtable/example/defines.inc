#define ALL -1

#define REGION_PERV      0xl0800
#define REGION_SBE       0xl0400
#define REGION_PIB       0xl0200
#define REGION_OCC       0xl0100
#define REGION_NET       0xl0080

#define SCANTYPE_REPR    0x0000000000000200

#define SCRATCH_REG_1    0x50038

#define SCAN_REGION_TYPE 0x01030005
#define CLK_REGION       0x01030006
#define CLOCK_STAT_SL    0x01030008
#define CLOCK_STAT_NSL   0x01030009
#define CLOCK_STAT_ARY   0x0103000A

#define START_CLOCKS     0x400000000000E000

#define LONG_ROTATE      0x01039000
#define SCAN64(nbits)    (0x0103E000 | (nbits))

#define SCAN_HEADER      0xAA5555AACAFEBABE

#define CPLT_STAT        0x01000100
#define OPCG_DONE        0xl008

#define ERR_ROTATE_TIMEOUT    0x201
#define ERR_HEADER_CHECK      0x202
#define ERR_CLOCK_STAT        0x203
