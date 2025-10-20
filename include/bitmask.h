// 16-bit bitmasks with respect to the CHIP-8 architecture
#define MASK_N1 0xF000 // First nibble - instruction group
#define MASK_N2 0x0F00 // Second nibble - register lookup
#define MASK_N3 0x00F0 // Third nibble - register lookup
#define MASK_N4 0x000F // Fourth nibble - 4-bit number
#define MASK_B1 0xFF00 // First byte - added for completion sake
#define MASK_B2 0x00FF // Second byte - 8-bit number
#define MASK_MA 0x0FFF // 12-bit memory address

// Utilities for working with the 16-bit bitmasks
#define N1(x) ((x & MASK_N1) >> 12)
#define N2(x) ((x & MASK_N2) >> 8)
#define N3(x) ((x & MASK_N3) >> 4)
#define N4(x) ((x & MASK_N4) >> 0)
#define B1(x) ((x & MASK_B1) >> 8)
#define B2(x) ((x & MASK_B2) >> 0)
#define MA(x) ((x & MASK_MA) >> 0)
