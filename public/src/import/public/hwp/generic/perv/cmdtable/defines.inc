//Functions
#define BIT(pos) (0xl8 << 1 >> (pos + 1))
#define BITS(first, last) ((BIT(first - 1) - 1) & ~(BIT(last) - 1))
#define BITVALUE(first, last, value) ((value << (63 - last)) & BITS(first, last))

//Variables
#define ALL 0xFFFFFFFFFFFFFFFF
