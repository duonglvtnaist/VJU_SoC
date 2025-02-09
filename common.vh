`define DATA_WIDTH              32
`define OP_WIDTH                3
`define ADDR_WIDTH              10
`define MASK_ADDR_WIDTH         2
`define MEM_OP_WIDTH            16

`define SEL_A                   2'b00
`define SEL_B                   2'b01
`define SEL_OUT                 2'b10

`define OP_ADD                  3'b000
`define OP_SUB                  3'b001
`define OP_AND                  3'b010
`define OP_OR                   3'b011
`define OP_SLL                  3'b100
`define OP_SRL                  3'b101
`define OP_SLT                  3'b110
`define OP_XOR                  3'b111

`define COUNTER_WIDTH           10

// For AXI4

`define AXI_DONE_ADDR           40'h40_0001_0000
`define AXI_START_ADDR          40'h40_0002_0000
`define AXI_DATA_WIDTH          32
`define AXI_TRANSFER_MODE_WIDTH 2

`define AXI_TRANSFER_MASK       4'h4
`define AXI_TRANSFER_A_MASK     2'd0
`define AXI_TRANSFER_B_MASK     2'd1
`define AXI_TRANSFER_O_MASK     2'd2
`define AXI_TRANSFER_OP_MASK    2'd3

