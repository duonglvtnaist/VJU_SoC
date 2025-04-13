// Author: Le Vu Trung Duong
// Description: Testbench for bubble sort unit

`include "common.vh"
`timescale 1ns / 1ps

module tb_bubble_sort_unit (
);
    reg                     CLK;
    reg                     RST;
    reg                     start_i;
    reg     [`BITWIDTH-1:0] in_data0_i;
    reg     [`BITWIDTH-1:0] in_data1_i;
    reg     [`BITWIDTH-1:0] in_data2_i;
    reg     [`BITWIDTH-1:0] in_data3_i;
    reg     [`BITWIDTH-1:0] in_data4_i;
    reg     [`BITWIDTH-1:0] in_data5_i;
    reg     [`BITWIDTH-1:0] in_data6_i;
    reg     [`BITWIDTH-1:0] in_data7_i;
    reg     [`BITWIDTH-1:0] in_data8_i;

    wire                    out_data0_o;
    wire                    out_data1_o;
    wire                    out_data2_o;
    wire                    out_data3_o;
    wire                    out_data4_o;
    wire                    out_data5_o;
    wire                    out_data6_o;
    wire                    out_data7_o;
    wire                    out_data8_o;

    wire                    valid_o;

    bubble_sort_unit inst_bubble_sort_unit (
        .CLK(CLK),
        .RST(RST),
        .start_i(start_i),
        .in_data0_i(in_data0_i),
        .in_data1_i(in_data1_i),
        .in_data2_i(in_data2_i),
        .in_data3_i(in_data3_i),
        .in_data4_i(in_data4_i),
        .in_data5_i(in_data5_i),
        .in_data6_i(in_data6_i),
        .in_data7_i(in_data7_i),
        .in_data8_i(in_data8_i),

        .out_data0_o(out_data0_o),
        .out_data1_o(out_data1_o),
        .out_data2_o(out_data2_o),
        .out_data3_o(out_data3_o),
        .out_data4_o(out_data4_o),
        .out_data5_o(out_data5_o),
        .out_data6_o(out_data6_o),
        .out_data7_o(out_data7_o),
        .out_data8_o(out_data8_o),

        .valid_o(valid_o)
    );

    parameter HALF_PERIOD = 5;

    always #(HALF_PERIOD) CLK = ~CLK;

    initial begin
        CLK = 0;
        RST = 0;
        start_i = 0;
        // Mảng trước khi sắp xếp:
        // 9 3 7 1 4 6 8 2 5
        in_data0_i = 9;
        in_data1_i = 3;
        in_data2_i = 7;
        in_data3_i = 1;
        in_data4_i = 4;
        in_data5_i = 6;
        in_data6_i = 8;
        in_data7_i = 2;
        in_data8_i = 5;
        $display("Unsorted array: %d %d %d %d %d %d %d %d %d", in_data0_i, in_data1_i, in_data2_i, in_data3_i, in_data4_i, in_data5_i, in_data6_i, in_data7_i, in_data8_i);

        #(HALF_PERIOD * 2 * 100) RST = 1; // Clear reset

        #(2*HALF_PERIOD) start_i = 1; // Start sorting
        #(2*HALF_PERIOD) start_i = 0; // Stop sorting



        while(!valid_o) begin
            #(2*HALF_PERIOD);
        end
        // Mảng sau khi sắp xếp:
        // 1 2 3 4 5 6 7 8 9
        $display("Sorted array: %d %d %d %d %d %d %d %d %d", out_data0_o, out_data1_o, out_data2_o, out_data3_o, out_data4_o, out_data5_o, out_data6_o, out_data7_o, out_data8_o);
        $finish;
    end
endmodule