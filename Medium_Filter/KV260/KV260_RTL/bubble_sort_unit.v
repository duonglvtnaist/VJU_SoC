// Author: Le Vu Trung Duong
// Description: Bubble sort unit for sorting 9 8-bit numbers

`include "common.vh"

module bubble_sort_unit(
    input wire       CLK,
    input wire       RST,

    input wire       start_i,

    input wire  [`BITWIDTH-1:0] in_data0_i,
    input wire  [`BITWIDTH-1:0] in_data1_i,
    input wire  [`BITWIDTH-1:0] in_data2_i,
    input wire  [`BITWIDTH-1:0] in_data3_i,
    input wire  [`BITWIDTH-1:0] in_data4_i,
    input wire  [`BITWIDTH-1:0] in_data5_i,
    input wire  [`BITWIDTH-1:0] in_data6_i,
    input wire  [`BITWIDTH-1:0] in_data7_i,
    input wire  [`BITWIDTH-1:0] in_data8_i,

    output wire [`BITWIDTH-1:0] out_data0_o,
    output wire [`BITWIDTH-1:0] out_data1_o,
    output wire [`BITWIDTH-1:0] out_data2_o,
    output wire [`BITWIDTH-1:0] out_data3_o,
    output wire [`BITWIDTH-1:0] out_data4_o,
    output wire [`BITWIDTH-1:0] out_data5_o,
    output wire [`BITWIDTH-1:0] out_data6_o,
    output wire [`BITWIDTH-1:0] out_data7_o,
    output wire [`BITWIDTH-1:0] out_data8_o,

    output reg valid_o
);
    reg     [`BITWIDTH-1:0] data0_r, data1_r, data2_r, data3_r, data4_r, data5_r, data6_r, data7_r, data8_r;
    wire    [`BITWIDTH-1:0] data0_w, data1_w, data2_w, data3_w, data4_w, data5_w, data6_w, data7_w, data8_w;

    assign data0_w = data0_r;
    assign data1_w = data1_r;
    assign data2_w = data2_r;
    assign data3_w = data3_r;
    assign data4_w = data4_r;
    assign data5_w = data5_r;
    assign data6_w = data6_r;
    assign data7_w = data7_r;
    assign data8_w = data8_r;

    // Output data assignment
    assign out_data0_o = data0_r;
    assign out_data1_o = data1_r;
    assign out_data2_o = data2_r;
    assign out_data3_o = data3_r;
    assign out_data4_o = data4_r;
    assign out_data5_o = data5_r;
    assign out_data6_o = data6_r;
    assign out_data7_o = data7_r;
    assign out_data8_o = data8_r;


    // Counter for the number of iterations

    wire [2:0]  count_i_w;
    wire        increment_i_w;
    wire        clear_i_w;

    reg         increment_i_r, clear_i_r;

    assign increment_i_w    = increment_i_r;
    assign clear_i_w        = clear_i_r;

    counter_3_bit counter_i (
        .CLK(CLK),
        .RST(RST),
        .increment_i(increment_i_w),
        .clear_i(clear_i_w),
        .count_o(count_i_w)
    );

    wire [2:0] count_j_w;
    wire increment_j_w;
    wire clear_j_w;

    reg increment_j_r, clear_j_r;
    assign increment_j_w = increment_j_r;
    assign clear_j_w = clear_j_r;

    counter_3_bit counter_j (
        .CLK(CLK),
        .RST(RST),
        .increment_i(increment_j_w),
        .clear_i(clear_j_w),
        .count_o(count_j_w)
    );

    // -----------------------------------------------------

    // State machine

    reg [1:0] state_r, next_state_r;
    wire [1:0] state_w, next_state_w;

    assign state_w = state_r;
    assign next_state_w = next_state_r;

    localparam IDLE = 2'b00;
    localparam SORT = 2'b01;
    localparam DONE = 2'b10;

    // State transition logic

    always @(posedge CLK or negedge RST) begin
        if (!RST) begin
            state_r <= IDLE;
        end else begin
            state_r <= next_state_w;
        end
    end

    // Next state logic

    always @(state_w or start_i or count_i_w or count_j_w) begin
        case (state_w)
            IDLE: begin
                if (start_i) begin
                    next_state_r = SORT;
                end else begin
                    next_state_r = IDLE;
                end
            end
            SORT: begin
                if ((count_i_w==3'd7) && (count_j_w == 3'd0)) begin
                    next_state_r = DONE;
                end else begin
                    next_state_r = SORT;
                end
            end
            DONE: begin
                if (!start_i) begin
                    next_state_r = IDLE;
                end else begin
                    next_state_r = DONE;
                end
            end
            default: begin
                next_state_r = IDLE;
            end
        endcase
    end

    // Output control logic
    wire [3:0] last_j_w;
    assign last_j_w = 3'd7 - count_i_w;

    wire swap_w;
    reg  swap_r;

    assign swap_w = swap_r;

    wire init_input_w;
    reg  init_input_r;
    assign init_input_w = init_input_r;

    always @(state_w or count_j_w or last_j_w) begin
        case (state_w)
            IDLE: begin
                valid_o         = 1'b0;
                increment_i_r   = 1'b0;
                increment_j_r   = 1'b0;
                clear_i_r       = 1'b1;
                clear_j_r       = 1'b1;
                swap_r          = 1'b0;
                init_input_r    = 1'b1;
            end
            SORT: begin
                valid_o         = 1'b0;
                swap_r          = 1'b1;
                init_input_r    = 1'b0;
                if (count_j_w == last_j_w) begin
                    increment_i_r   = 1'b1;
                    increment_j_r   = 1'b0;
                    clear_i_r       = 1'b0;
                    clear_j_r       = 1'b1;
                end
                else begin
                    increment_i_r   = 1'b0;
                    increment_j_r   = 1'b1;
                    clear_i_r       = 1'b0;
                    clear_j_r       = 1'b0;
                end
            end
            DONE: begin
                valid_o         = 1'b1;
                init_input_r    = 1'b0;
                increment_i_r   = 1'b0;
                increment_j_r   = 1'b0;
                clear_i_r       = 1'b1;
                clear_j_r       = 1'b1;
                swap_r          = 1'b0;
            end
            default: begin
                valid_o         = 1'b0;
                init_input_r    = 1'b0;
                increment_i_r   = 1'b0;
                increment_j_r   = 1'b0;
                clear_i_r       = 1'b0;
                clear_j_r       = 1'b0;
                swap_r          = 1'b0;
            end
        endcase
    end

    // Data processing part

    wire [`BITWIDTH-1:0] swap_in_j_w, swap_in_j_incr_w;

    assign swap_in_j_w =       (count_j_w == 3'd0) ? data0_w : 
                               (count_j_w == 3'd1) ? data1_w :
                               (count_j_w == 3'd2) ? data2_w :
                               (count_j_w == 3'd3) ? data3_w :
                               (count_j_w == 3'd4) ? data4_w :
                               (count_j_w == 3'd5) ? data5_w :
                               (count_j_w == 3'd6) ? data6_w : 
                                                     data7_w ;
    assign swap_in_j_incr_w =  (count_j_w == 3'd0) ? data1_w : 
                               (count_j_w == 3'd1) ? data2_w :
                               (count_j_w == 3'd2) ? data3_w :
                               (count_j_w == 3'd3) ? data4_w :
                               (count_j_w == 3'd4) ? data5_w :
                               (count_j_w == 3'd5) ? data6_w :
                               (count_j_w == 3'd6) ? data7_w : 
                                                     data8_w ;
    wire [`BITWIDTH-1:0] max_w, min_w;

    compare_two compare_i (
        .a_i(swap_in_j_w),
        .b_i(swap_in_j_incr_w),
        .max_o(max_w),
        .min_o(min_w)
    );

    always @(posedge CLK or negedge RST) begin
        if(!RST) begin
            data0_r <= 8'b0;
            data1_r <= 8'b0;
            data2_r <= 8'b0;
            data3_r <= 8'b0;
            data4_r <= 8'b0;
            data5_r <= 8'b0;
            data6_r <= 8'b0;
            data7_r <= 8'b0;
            data8_r <= 8'b0;
        end
        else begin
            if (init_input_w) begin
                data0_r <= in_data0_i;
                data1_r <= in_data1_i;
                data2_r <= in_data2_i;
                data3_r <= in_data3_i;
                data4_r <= in_data4_i;
                data5_r <= in_data5_i;
                data6_r <= in_data6_i;
                data7_r <= in_data7_i;
                data8_r <= in_data8_i;
            end
            else begin
                if(swap_w) begin
                    case (count_j_w)
                        3'd0: begin
                            data0_r <= min_w;
                            data1_r <= max_w;
                            data2_r <= data2_w;
                            data3_r <= data3_w;
                            data4_r <= data4_w;
                            data5_r <= data5_w;
                            data6_r <= data6_w;
                            data7_r <= data7_w;
                            data8_r <= data8_w;
                        end
                        3'd1: begin
                            data0_r <= data0_w;
                            data1_r <= min_w;
                            data2_r <= max_w;
                            data3_r <= data3_w;
                            data4_r <= data4_w;
                            data5_r <= data5_w;
                            data6_r <= data6_w;
                            data7_r <= data7_w;
                            data8_r <= data8_w;
                        end
                        3'd2: begin
                            data0_r <= data0_w;
                            data1_r <= data1_w;
                            data2_r <= min_w;
                            data3_r <= max_w;
                            data4_r <= data4_w;
                            data5_r <= data5_w;
                            data6_r <= data6_w;
                            data7_r <= data7_w;
                            data8_r <= data8_w;
                        end
                        3'd3: begin
                            data0_r <= data0_w;
                            data1_r <= data1_w;
                            data2_r <= data2_w;
                            data3_r <= min_w;
                            data4_r <= max_w;
                            data5_r <= data5_w;
                            data6_r <= data6_w;
                            data7_r <= data7_w;
                            data8_r <= data8_w;
                        end
                        3'd4: begin
                            data0_r <= data0_w;
                            data1_r <= data1_w;
                            data2_r <= data2_w;
                            data3_r <= data3_w;
                            data4_r <= min_w;
                            data5_r <= max_w;
                            data6_r <= data6_w;
                            data7_r <= data7_w;
                            data8_r <= data8_w;
                        end
                        3'd5: begin
                            data0_r <= data0_w;
                            data1_r <= data1_w;
                            data2_r <= data2_w;
                            data3_r <= data3_w;
                            data4_r <= data4_w;
                            data5_r <= min_w;
                            data6_r <= max_w;
                            data7_r <= data7_w;
                            data8_r <= data8_w;
                        end
                        3'd6: begin
                            data0_r <= data0_w;
                            data1_r <= data1_w;
                            data2_r <= data2_w;
                            data3_r <= data3_w;
                            data4_r <= data4_w;
                            data5_r <= data5_w;
                            data6_r <= min_w;
                            data7_r <= max_w;
                            data8_r <= data8_w;
                        end
                        3'd7: begin
                            data0_r <= data0_w;
                            data1_r <= data1_w;
                            data2_r <= data2_w;
                            data3_r <= data3_w;
                            data4_r <= data4_w;
                            data5_r <= data5_w;
                            data6_r <= data6_w;
                            data7_r <= min_w;
                            data8_r <= max_w;
                        end
                        default: begin
                            data0_r <= data0_w;
                            data1_r <= data1_w;
                            data2_r <= data2_w;
                            data3_r <= data3_w;
                            data4_r <= data4_w;
                            data5_r <= data5_w;
                            data6_r <= data6_w;
                            data7_r <= data7_w;
                            data8_r <= data8_w;
                        end
                    endcase
                
                end
                else begin
                    data0_r <= data0_w;
                    data1_r <= data1_w;
                    data2_r <= data2_w;
                    data3_r <= data3_w;
                    data4_r <= data4_w;
                    data5_r <= data5_w;
                    data6_r <= data6_w;
                    data7_r <= data7_w;
                    data8_r <= data8_w;
                end
            end
        end
    end

endmodule

module compare_two(
    input wire [`BITWIDTH-1:0] a_i,
    input wire [`BITWIDTH-1:0] b_i,
    output wire [`BITWIDTH-1:0] max_o,
    output wire [`BITWIDTH-1:0] min_o
);
    assign max_o = (a_i > b_i) ? a_i : b_i;
    assign min_o = (a_i < b_i) ? a_i : b_i;
endmodule

module counter_3_bit(
    input wire CLK,
    input wire RST,
    input wire increment_i,
    input wire clear_i,
    output wire [2:0] count_o
);
    reg [2:0] count_r;

    assign count_o = count_r;

    always @(posedge CLK or negedge RST) begin
        if (!RST) begin
            count_r <= 3'b000;
        end else begin
            if (clear_i) begin
                count_r <= 3'b000;
            end else begin
                if (increment_i) begin
                    count_r <= count_o + 3'b001;
                end
                else begin
                    count_r <= count_o;
                end
            end
        end
    end

endmodule