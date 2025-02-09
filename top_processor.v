// Author: Le Vu Trung Duong
// Nara Institute of Science and Technology

`include "common.vh"

module top_processor (
    input   wire [`DATA_WIDTH-1:0]  data_i,
    input   wire [`ADDR_WIDTH-1:0]  addr_data_i,
    input   wire                    ena_data_a_i,
    input   wire                    wea_data_a_i,
    input   wire                    ena_data_b_i,
    input   wire                    wea_data_b_i,
    input   wire                    wea_data_o_i,
    input   wire                    ena_data_o_i,
    output  wire [`DATA_WIDTH-1:0]  data_o,
    input   wire [`OP_WIDTH-1:0]    op_i,
    input   wire [`ADDR_WIDTH-1:0]  addr_op_i,
    input   wire                    ena_op_i,
    input   wire                    wea_op_i,
    input   wire                    CLK,
    input   wire                    RST,
    input   wire                    start_i,
    output  wire                    done_o
);

    // Instance of BRAM

    // BRAM for data A and output

    wire [`DATA_WIDTH-1:0] data_a_w;
    wire [`DATA_WIDTH-1:0] data_b_w;

    wire [`DATA_WIDTH-1:0] data_alu_o_w;


    

    BRAM_1024x32b bram_a (
        .clka(CLK),
        .wea(wea_data_a_i),
        .ena(ena_data_a_i),
        .addra(addr_data_i),
        .dina(data_i),
        .douta(data_a_w)
    );

    // BRAM for data B

    BRAM_1024x32b bram_b (
        .clka(CLK),
        .wea(wea_data_b_i),
        .ena(ena_data_b_i),
        .addra(addr_data_i),
        .dina(data_i),
        .douta(data_b_w)
    );

    // BRAM for output

    wire wea_data_o_w, ena_data_o_w;
    reg  wea_data_o_r, ena_data_o_r;

    assign wea_data_o_w = wea_data_o_r;
    assign ena_data_o_w = ena_data_o_r;

    wire sel_mem_w, wea_data_o_main_w, ena_data_o_main_w;
    reg  sel_mem_r;

    assign sel_mem_w = sel_mem_r;

    assign wea_data_o_main_w = sel_mem_w ? wea_data_o_w : wea_data_o_i;
    assign ena_data_o_main_w = sel_mem_w ? ena_data_o_w : ena_data_o_i;

    wire [`DATA_WIDTH-1:0] data_o_w;

    assign data_o_w = {22'd0, counter_o_w} + data_alu_o_w;

    wire [`ADDR_WIDTH-1:0] addr_data_o_w;

    assign addr_data_o_w = sel_mem_w ? counter_o_w : addr_data_i;

    BRAM_1024x32b bram_o (
        .clka(CLK),
        .wea(wea_data_o_main_w),
        .ena(ena_data_o_main_w),
        .addra(addr_data_o_w),
        .dina(data_o_w),
        .douta(data_o)
    );

    // BRAM for operation

    wire [`OP_WIDTH-1:0]                op_w;
    wire [`MEM_OP_WIDTH-1:0]            op_o_w;

    assign op_w = op_o_w[2:0];     

    wire wea_op_w, ena_op_w, wea_op_sel_w, ena_op_sel_w;
    reg  wea_op_r, ena_op_r;

    assign wea_op_w = wea_op_r;
    assign wea_op_sel_w = sel_mem_w ? wea_op_w : wea_op_i;

    assign ena_op_w = ena_op_r;
    assign ena_op_sel_w = sel_mem_w ? ena_op_w : ena_op_i;

    wire [`MEM_OP_WIDTH-1:0] op_i_w;

    assign op_i_w = {13'b0, op_i};

    BRAM_1024x16b bram_op (
        .clka(CLK),
        .wea(wea_op_i),
        .ena(ena_op_i),
        .addra(addr_op_i),
        .dina(op_i_w),
        .douta(op_o_w)
    );

    // Instance of address counter

    wire                        counter_increment_w;
    reg                         counter_increment_r;
    wire [`COUNTER_WIDTH-1:0]   counter_o_w;

    assign counter_increment_w = counter_increment_r;

    counter counter (
        .CLK(CLK),
        .RST(RST),
        .increment_i(counter_increment_w),
        .counter_o(counter_o_w)
    );

    // Instance of ALU

    ALU alu (
        .A_i(data_a_w),
        .B_i(data_b_w),
        .Op_i(op_w),
        .Out_o(data_alu_o_w)
    );

    // FSM

    wire [1:0] state_w, next_state_w;
    reg  [1:0] state_r, next_state_r;

    assign state_w = state_r;
    assign next_state_w = next_state_r;

    localparam  S0_IDLE = 2'b00,
                S1_EXEC = 2'b01,
                S2_DONE = 2'b10;

    // State transition logic
    always @(posedge CLK or negedge RST) begin
        if (~RST)
            state_r <= 2'b00;
        else
            state_r <= next_state_w;
    end

    // Next state determination logic
    always @(state_w or start_i or counter_o_w) begin
        case (state_w)
            S0_IDLE: begin
                if (start_i)
                    next_state_r = S1_EXEC;
                else
                    next_state_r = S0_IDLE;
            end
            S1_EXEC: begin
                if (counter_o_w == `COUNTER_WIDTH'd1023)
                    next_state_r = S2_DONE;
                else
                    next_state_r = S1_EXEC;
            end
            S2_DONE: begin
                if (start_i == 1'b0)
                    next_state_r = S0_IDLE;
                else
                    next_state_r = S2_DONE;
            end
            default: begin
                next_state_r = S0_IDLE;
            end
        
        endcase
    
    end

    // Control output logic

    reg                 done_r;

    assign done_o = done_r;

    always @(state_w) begin
        case (state_w)
            S0_IDLE: begin
                done_r              = 1'b0;
                counter_increment_r = 1'b0;
                wea_data_o_r        = 1'b0;
                ena_data_o_r        = 1'b0;
                wea_op_r            = 1'b0;
                ena_op_r            = 1'b0;
                sel_mem_r           = 1'b0;
            end
            S1_EXEC: begin
                done_r              = 1'b0;
                counter_increment_r = 1'b1;
                wea_data_o_r        = 1'b1;
                ena_data_o_r        = 1'b1;
                wea_op_r            = 1'b1;
                ena_op_r            = 1'b1;
                sel_mem_r           = 1'b1;
            end
            S2_DONE: begin
                done_r              = 1'b1;
                counter_increment_r = 1'b0;
                wea_data_o_r        = 1'b0;
                ena_data_o_r        = 1'b0;
                wea_op_r            = 1'b0;
                ena_op_r            = 1'b0;
                sel_mem_r           = 1'b0;

            end
            default: begin
                done_r              = 1'b0;
                counter_increment_r = 1'b0;
                wea_data_o_r        = 1'b0;
                ena_data_o_r        = 1'b0;
                wea_op_r            = 1'b0;
                ena_op_r            = 1'b0;
                sel_mem_r           = 1'b0;
            end   
        endcase
    end


endmodule