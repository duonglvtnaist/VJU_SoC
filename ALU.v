// Author: Le Vu Trung Duong
// Nara Institute of Science and Technology

`include "common.vh"

module ALU(
    input  [`DATA_WIDTH-1:0] A_i, B_i,
    input  [`OP_WIDTH-1:0]   Op_i,
    output [`DATA_WIDTH-1:0] Out_o
);
    assign Out_o =    (Op_i == `OP_ADD) ? A_i + B_i : // R-type: add
                      (Op_i == `OP_SUB) ? A_i - B_i : // R-type: sub
                      (Op_i == `OP_AND) ? A_i & B_i : // R-type: and
                      (Op_i == `OP_OR ) ? A_i | B_i : // R-type: or
                      (Op_i == `OP_SLL) ? A_i << B_i[4:0] : // R-type: sll
                      (Op_i == `OP_SRL) ? A_i >> B_i[4:0] : // R-type: srl
                      (Op_i == `OP_SLT) ? (A_i < B_i) ? 1 : 0 : // I-type: slt
                      (Op_i == `OP_XOR) ? A_i ^ B_i : 0; // R-type: xor
    

endmodule