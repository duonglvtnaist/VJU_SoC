`define         BIT_WIDTH 8

`define         FULL_BIT_WIDTH 32
`define         ADDR_WIDTH 18     // 18-bit 512x512 max size  -> 2^18 = 262144
`define         MODE_ADDR_WIDTH 2 // 2-bit mode address: 00 = data, 01 = start/valid, 10 = height, 11 = width

`define         AXI_DATA_WIDTH 32