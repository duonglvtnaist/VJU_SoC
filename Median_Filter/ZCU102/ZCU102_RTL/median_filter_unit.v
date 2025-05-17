// Author: Le Vu Trung Duong
// Description: Median filter unit

`include "common.vh"
`timescale 1ns / 1ps

module median_filter_unit(
    input  wire                                             CLK,
    input  wire                                             RST,
    input  wire  [`FULLBITWIDTH-1:0]                        dina_i,
    input  wire  [`MODE_ADDR_WIDTH + `ADDR_WIDTH-1:0]       addra_i,
    input  wire                                             wea_i,
    input  wire                                             ena_i,
    output wire  [`FULLBITWIDTH-1:0]                        douta_o
);
    

    // Flow for controlling the AXI interface

    reg     [9:0]   width_r;
    wire    [9:0]   width_w;
    reg     [9:0]   height_r;
    wire    [9:0]   height_w;

    assign width_w  = width_r;
    assign height_w = height_r; 

    reg             start_r;
    wire            start_w;

    assign start_w = start_r;

    // reg             bram_wea_r, bram_ena_r;
    

    // assign bram_wea_w = bram_wea_r;
    // assign bram_ena_w = bram_ena_r;

    wire [`MODE_ADDR_WIDTH-1:0] addra_mode_w;

    assign addra_mode_w = addra_i[`MODE_ADDR_WIDTH+`ADDR_WIDTH-1:`ADDR_WIDTH];

    // Type: 1. BRAM_2p18x8b 2. start_r 3. width_r 4. height_r
    // Type: 1. BRAM_2p18x8b 2. valid_r
    always @(posedge CLK or negedge RST) begin
        if (!RST) begin
            width_r     <= 10'd0;
            height_r    <= 10'd0;
            start_r     <= 1'b0;
            // bram_wea_r  <= 1'b0;
            // bram_ena_r  <= 1'b0;
        end
        else begin
            if(ena_i) begin
                if(wea_i) begin
                    if(addra_mode_w == 2'd0) begin
                            // bram_ena_r  <= 1'b1;
                            // bram_wea_r  <= 1'b1;
                            start_r     <= start_w;
                            width_r     <= width_w;
                            height_r    <= height_w;
                    end
                    else if(addra_mode_w == 2'd1) begin
                        // bram_ena_r  <= 1'b0;
                        // bram_wea_r  <= 1'b0;
                        start_r     <= dina_i[0:0];
                        width_r     <= width_w;
                        height_r    <= height_w;
                    end
                    else if(addra_mode_w == 2'd2) begin
                            // bram_ena_r  <= 1'b0;
                            // bram_wea_r  <= 1'b0;
                            start_r     <= start_w;
                            width_r     <= dina_i[9:0];
                            height_r    <= height_w;
                        end
                    else if(addra_mode_w == 2'd3) begin
                            // bram_ena_r  <= 1'b0;
                            // bram_wea_r  <= 1'b0;
                            start_r     <= start_w;
                            width_r     <= width_w;
                            height_r    <= dina_i[9:0];
                        end
                    else begin
                            // bram_ena_r  <= 1'b0;
                            // bram_wea_r  <= 1'b0;
                            start_r     <= start_w;
                            width_r     <= width_w;
                            height_r    <= height_w;
                    end
                end
                else if(!wea_i) begin
                    start_r     <= start_w;
                    width_r     <= width_w;
                    height_r    <= height_w;
                    // case(addra_i[`MODE_ADDR_WIDTH+`ADDR_WIDTH-1:`ADDR_WIDTH]) 
                    //     2'd0: begin
                    //         bram_ena_r  <= 1'b1;
                    //         bram_wea_r  <= 1'b0;
                    //     end
                    //     default: begin
                    //         bram_ena_r  <= 1'b0;
                    //         bram_wea_r  <= 1'b0;
                    //     end
                    // endcase
                end
                else begin
                    start_r     <= start_w;
                    width_r     <= width_w;
                    height_r    <= height_w;
                    // bram_ena_r  <= 1'b0;
                    // bram_wea_r  <= 1'b0;
                end
            end
            else begin
                start_r     <= start_w;
                width_r     <= width_w;
                height_r    <= height_w;
                // bram_ena_r  <= 1'b0;
                // bram_wea_r  <= 1'b0;
            end
        end
    end

       // Counter for controlling the median filter unit
    wire [9:0]  counter_i_w;
    reg         counter_i_increment_r;
    wire        counter_i_increment_w;
    reg         counter_i_clear_r;
    wire        counter_i_clear_w;

    assign counter_i_increment_w = counter_i_increment_r;
    assign counter_i_clear_w     = counter_i_clear_r;

    counter_10_bit counter_10_bit_inst (
        .CLK(CLK),
        .RST(RST),
        .increment_i(counter_i_increment_w),
        .clear_i(counter_i_clear_w),
        .count_o(counter_i_w)
    );

    wire [9:0]  counter_j_w;
    reg         counter_j_increment_r;
    wire        counter_j_increment_w;
    reg         counter_j_clear_r;
    wire        counter_j_clear_w;

    assign counter_j_increment_w = counter_j_increment_r;
    assign counter_j_clear_w     = counter_j_clear_r;

    counter_10_bit counter_10_bit_inst_j (
        .CLK(CLK),
        .RST(RST),
        .increment_i(counter_j_increment_w),
        .clear_i(counter_j_clear_w),
        .count_o(counter_j_w)
    );


    // Counter for filling the filter window

    wire [3:0] counter_window_w;

    reg        counter_window_increment_r;
    wire       counter_window_increment_w;
    assign counter_window_increment_w = counter_window_increment_r;

    reg        counter_window_clear_r;
    wire       counter_window_clear_w;
    assign counter_window_clear_w = counter_window_clear_r;

    counter_4_bit counter_4_bit_inst (
        .CLK(CLK),
        .RST(RST),
        .increment_i(counter_window_increment_w),
        .clear_i(counter_window_clear_w),
        .count_o(counter_window_w)
    );

    reg     sel_fsm_r;
    wire    sel_fsm_w;

    assign sel_fsm_w = sel_fsm_r;

    reg     fsm_ena_r, fsm_wea_r;
    wire    fsm_ena_w, fsm_wea_w;

    assign fsm_ena_w = fsm_ena_r;
    assign fsm_wea_w = fsm_wea_r;


    wire [`BITWIDTH-1:0]   fsm_dina_w;
    
    reg [7:0]           window_r [0:8];
    wire [7:0]          window_w [0:8];

    genvar i;
    generate
        for (i = 0; i < 9; i = i + 1) begin: window_gen
            assign window_w[i] = window_r[i];
        end
    endgenerate

    wire [7:0]             window_out_w [0:8];
    assign fsm_dina_w =    window_out_w[4];

 
    wire [`ADDR_WIDTH-1:0]   fsm_addra_w;


    wire  [`BITWIDTH-1:0]     bram_dina_w;
    wire  [`ADDR_WIDTH-1:0]   bram_addra_w;
    wire                      bram_wea_w, bram_ena_w;
    
    wire  [`BITWIDTH-1:0]     bram_douta_w;
    wire  [`BITWIDTH-1:0]     bram_douta_out_w;

    // wire  [`BITWIDTH-1:0]     global_douta_w;

    wire                      global_valid_w;
    reg                       global_valid_r;

    assign global_valid_w = global_valid_r;

    // assign global_douta_w = (addra_i[`MODE_ADDR_WIDTH+`ADDR_WIDTH-1:`ADDR_WIDTH] == 2'd1) ? global_valid_w : bram_douta_out_w;

    assign bram_addra_w = (~sel_fsm_w) ? addra_i[`ADDR_WIDTH-1:0]          : fsm_addra_w;
    assign bram_dina_w  = (~sel_fsm_w) ? dina_i[7:0]                       : fsm_dina_w;
    assign bram_wea_w   = (~sel_fsm_w) ? (wea_i & (addra_mode_w == 2'b0))  : fsm_wea_w;
    assign bram_ena_w   = (~sel_fsm_w) ? (ena_i & (addra_mode_w == 2'b0))  : fsm_ena_w;

    BRAM_2p18x8b bram_2p18x8b_inst (
        .clka(CLK),
        .wea(bram_wea_w & (~sel_fsm_w)), // Disable write if sel_fsm_w is high
        .addra(bram_addra_w),
        .dina(bram_dina_w),
        .ena(bram_ena_w),
        .douta(bram_douta_w)
    );

    BRAM_2p18x8b bram_2p18x8b_out_inst (
        .clka(CLK),
        .wea(bram_wea_w),
        .addra(bram_addra_w),
        .dina(bram_dina_w),
        .ena(bram_ena_w),
        .douta(bram_douta_out_w)
    );

 
    // Control bubble sort unit

    reg         start_bubble_sort_r;
    wire        start_bubble_sort_w;

    assign start_bubble_sort_w = start_bubble_sort_r;

    wire        valid_bubble_sort_w;

    reg         row_based_clear_r;
    wire        row_based_clear_w;

    assign row_based_clear_w = row_based_clear_r;

    reg         row_based_update_r;
    wire        row_based_update_w;

    assign row_based_update_w = row_based_update_r;
   
    wire  update_window_w;
    reg   update_window_r;

    assign update_window_w = update_window_r;

    wire  write_window_w;
    reg   write_window_r;

    assign write_window_w = write_window_r;

    // FSM for controlling the median filter unit
    reg     [1:0]   state_r, next_state_r;
    wire    [1:0]   state_w, next_state_w;

    assign state_w      = state_r;
    assign next_state_w = next_state_r;

    parameter IDLE = 2'b00;
    parameter EXEC = 2'b01;
    parameter DONE = 2'b10;

    // State transition 

    always @(posedge CLK or negedge RST) begin
        if (!RST) begin
            state_r <= IDLE;
        end
        else begin
            state_r <= next_state_r;
        end
    end

    // Next state logic

    always @(state_w or start_w or counter_i_w or counter_j_w or valid_bubble_sort_w or height_w or width_w) begin
        case(state_w)
            IDLE: begin
                if (start_w) begin
                    next_state_r = EXEC;
                end
                else begin
                    next_state_r = IDLE;
                end
            end
            EXEC: begin
                if ((counter_i_w == height_w-10'd1) & (counter_j_w == width_w-10'd1) & valid_bubble_sort_w) begin
                    next_state_r = DONE;
                end
                else begin
                    next_state_r = EXEC;
                end
            end
            DONE: begin
                if(!start_w) begin
                    next_state_r = IDLE;
                end
                else begin
                    next_state_r = DONE;
                end
            end
            default: begin
                next_state_r = IDLE;
            end
        endcase
    end



    always @(state_w or counter_window_w or valid_bubble_sort_w or counter_j_w or width_w) begin
        case(state_w)
            IDLE: begin
                counter_i_clear_r           = 1'b1;
                counter_j_clear_r           = 1'b1;
                counter_window_clear_r      = 1'b1;
                counter_window_increment_r  = 1'b0;
                counter_i_increment_r       = 1'b0;
                counter_j_increment_r       = 1'b0;
                sel_fsm_r                   = 1'b0;
                fsm_ena_r                   = 1'b0;
                fsm_wea_r                   = 1'b0;
                start_bubble_sort_r         = 1'b0;
                global_valid_r              = 1'b0;
                row_based_clear_r           = 1'b1;
                row_based_update_r          = 1'b0;
                update_window_r             = 1'b0;
                write_window_r              = 1'b0;
            end
            EXEC: begin
                sel_fsm_r                           = 1'b1;
                global_valid_r                      = 1'b0;
                row_based_clear_r                   = 1'b0;
                if(counter_window_w == 4'd8) begin
                    start_bubble_sort_r             = 1'b1;
                    if(valid_bubble_sort_w) begin
                        counter_window_increment_r  = 1'b0;
                        counter_window_clear_r      = 1'b1;
                        fsm_ena_r                   = 1'b1;
                        fsm_wea_r                   = 1'b1;
                        update_window_r             = 1'b1;
                        write_window_r              = 1'b0;
                        if(counter_j_w == width_w-10'd1) begin
                            counter_i_increment_r   = 1'b1;
                            counter_i_clear_r       = 1'b0;
                            row_based_update_r      = 1'b1;
                            counter_j_clear_r       = 1'b1;
                            counter_j_increment_r   = 1'b0;
                        end
                        else begin
                            counter_i_increment_r   = 1'b0;
                            counter_i_clear_r       = 1'b0;
                            row_based_update_r      = 1'b0;
                            counter_j_clear_r       = 1'b0;
                            counter_j_increment_r   = 1'b1;
                        end
                    end
                    else begin
                        update_window_r             = 1'b0;
                        counter_window_increment_r  = 1'b0;
                        counter_window_clear_r      = 1'b0;
                        counter_j_increment_r       = 1'b0;
                        counter_j_clear_r           = 1'b0;
                        counter_i_increment_r       = 1'b0;
                        counter_i_clear_r           = 1'b0;
                        fsm_ena_r                   = 1'b1;
                        fsm_wea_r                   = 1'b0;
                        row_based_update_r          = 1'b0;
                        write_window_r              = 1'b1;
                    end
                end
                else begin
                    start_bubble_sort_r             = 1'b0;
                    counter_window_increment_r      = 1'b1;
                    counter_window_clear_r          = 1'b0;
                    counter_j_increment_r           = 1'b0;
                    counter_j_clear_r               = 1'b0;
                    counter_i_increment_r           = 1'b0;
                    counter_i_clear_r               = 1'b0;
                    fsm_ena_r                       = 1'b1;
                    fsm_wea_r                       = 1'b0;
                    row_based_update_r              = 1'b0;
                    update_window_r                 = 1'b0;
                    write_window_r                  = 1'b1;
                end
            end
            DONE: begin
                counter_i_clear_r           = 1'b1;
                counter_j_clear_r           = 1'b1;
                counter_window_clear_r      = 1'b1;
                counter_window_increment_r  = 1'b0;
                counter_i_increment_r       = 1'b0;
                counter_j_increment_r       = 1'b0;
                sel_fsm_r                   = 1'b0;
                fsm_ena_r                   = 1'b0;
                fsm_wea_r                   = 1'b0;
                start_bubble_sort_r         = 1'b0;
                global_valid_r              = 1'b1;
                row_based_clear_r           = 1'b0;
                row_based_update_r          = 1'b0;
                update_window_r             = 1'b0;
                write_window_r              = 1'b0;
            end
            default: begin
                counter_i_clear_r           = 1'b0;
                counter_j_clear_r           = 1'b0;
                counter_window_clear_r      = 1'b0;
                counter_window_increment_r  = 1'b0;
                counter_i_increment_r       = 1'b0;
                counter_j_increment_r       = 1'b0;
                sel_fsm_r                   = 1'b0;
                fsm_ena_r                   = 1'b0;
                fsm_wea_r                   = 1'b0;
                start_bubble_sort_r         = 1'b0;
                global_valid_r              = 1'b0;
                row_based_clear_r           = 1'b0;
                row_based_update_r          = 1'b0;
                update_window_r             = 1'b0;
                write_window_r              = 1'b0;
            end
        endcase
    end



    



    reg  [17:0]             row_based_r;
    wire [17:0]             row_based_w;

    assign row_based_w = row_based_r;

    always @(posedge CLK or negedge RST) begin
        if(!RST) begin
            row_based_r <= 18'd0;
        end
        else begin
            if (row_based_clear_w) begin
                row_based_r <= 18'd0;
            end
            else if (row_based_update_w) begin
                row_based_r <= row_based_w + width_w;
            end
            else begin
                row_based_r <= row_based_w;
            end
        end
    end
    wire [18:0] cal_read_addra_w;      

    // Delay counter_window_w by 1 clock cycle
    reg [3:0]  delayed_counter_window_r;
    wire [3:0] delayed_counter_window_w;
    assign delayed_counter_window_w = delayed_counter_window_r;

    wire       delayed_start_bubble_sort_w;
    reg        delayed_start_bubble_sort_r;

    assign delayed_start_bubble_sort_w = delayed_start_bubble_sort_r;

    wire       delayed_2clk_start_bubble_sort_w;
    reg        delayed_2clk_start_bubble_sort_r;
    assign delayed_2clk_start_bubble_sort_w = delayed_2clk_start_bubble_sort_r;

    

    always @(posedge CLK or negedge RST) begin
        if (!RST) begin
            delayed_counter_window_r    <= 4'd0;
            delayed_start_bubble_sort_r <= 1'b0;
            delayed_2clk_start_bubble_sort_r <= 1'b0;
        end
        else begin
            delayed_counter_window_r    <= counter_window_w;
            delayed_start_bubble_sort_r <= start_bubble_sort_w;
            delayed_2clk_start_bubble_sort_r <= delayed_start_bubble_sort_w;
        end
    end

    // ----------------------------------------------------------------

    bubble_sort_unit bubble_sort_unit_inst (
        .CLK(CLK),
        .RST(RST),
        .start_i(delayed_2clk_start_bubble_sort_w),
        .in_data0_i(window_w[0]),
        .in_data1_i(window_w[1]),
        .in_data2_i(window_w[2]),
        .in_data3_i(window_w[3]),
        .in_data4_i(window_w[4]),
        .in_data5_i(window_w[5]),
        .in_data6_i(window_w[6]),
        .in_data7_i(window_w[7]),
        .in_data8_i(window_w[8]),

        .out_data0_o(window_out_w[0]),
        .out_data1_o(window_out_w[1]),
        .out_data2_o(window_out_w[2]),
        .out_data3_o(window_out_w[3]),
        .out_data4_o(window_out_w[4]),
        .out_data5_o(window_out_w[5]),
        .out_data6_o(window_out_w[6]),
        .out_data7_o(window_out_w[7]),
        .out_data8_o(window_out_w[8]),

        .valid_o(valid_bubble_sort_w)
    );

    

    wire is_zero_col_w;
    wire is_zero_row_w;
    assign is_zero_col_w = ((counter_j_w==10'd0 &          (delayed_counter_window_w==4'd0|delayed_counter_window_w==4'd3|delayed_counter_window_w==4'd6)) | 
                            (counter_j_w==width_w-8'd1 &  (delayed_counter_window_w==4'd2|delayed_counter_window_w==4'd5|delayed_counter_window_w==4'd8))) ? 1'b1 : 1'b0;
    assign is_zero_row_w = ((counter_i_w==10'd0 &          (delayed_counter_window_w==4'd0|delayed_counter_window_w==4'd1|delayed_counter_window_w==4'd2)) | 
                            (counter_i_w==height_w-8'd1 & (delayed_counter_window_w==4'd6|delayed_counter_window_w==4'd7|delayed_counter_window_w==4'd8))) ? 1'b1 : 1'b0;


    // wire for calculate the condition

    assign cal_read_addra_w = (counter_window_w == 4'd0 ) ? {1'b0,row_based_w} - {11'b0, width_w} - 19'd1 + {1'b0, counter_j_w}:
                              (counter_window_w == 4'd1 ) ? {1'b0,row_based_w} - {11'b0, width_w}         + {1'b0, counter_j_w}:
                              (counter_window_w == 4'd2 ) ? {1'b0,row_based_w} - {11'b0, width_w} + 19'd1 + {1'b0, counter_j_w}:
                              (counter_window_w == 4'd3 ) ? {1'b0,row_based_w}                    - 19'd1 + {1'b0, counter_j_w}: 
                              (counter_window_w == 4'd4 ) ? {1'b0,row_based_w}                            + {1'b0, counter_j_w}: 
                              (counter_window_w == 4'd5 ) ? {1'b0,row_based_w}                    + 19'd1 + {1'b0, counter_j_w}: 
                              (counter_window_w == 4'd6 ) ? {1'b0,row_based_w} + {11'b0, width_w} - 19'd1 + {1'b0, counter_j_w}:
                              (counter_window_w == 4'd7 ) ? {1'b0,row_based_w} + {11'b0, width_w}         + {1'b0, counter_j_w}:
                                                            {1'b0,row_based_w} + {11'b0, width_w} + 19'd1 + {1'b0, counter_j_w};

    

    assign fsm_addra_w = (fsm_wea_w) ? row_based_w + counter_j_w : cal_read_addra_w;
    assign fsm_dina_w = window_out_w[4];


    wire [7:0] cal_bram_douta_w;

    wire is_zero_w;
    assign is_zero_w = (is_zero_col_w | is_zero_row_w) ? 1'b1 : 1'b0;

    assign cal_bram_douta_w = (is_zero_w == 1'b0) ? bram_douta_w : 8'd0;

    always @(posedge CLK or negedge RST) begin
        if(!RST) begin
            window_r[0] <= 8'd0;
            window_r[1] <= 8'd0;
            window_r[2] <= 8'd0;
            window_r[3] <= 8'd0;
            window_r[4] <= 8'd0;
            window_r[5] <= 8'd0;
            window_r[6] <= 8'd0;
            window_r[7] <= 8'd0;
            window_r[8] <= 8'd0;
        end
        else begin
            if(write_window_w) begin
                case(delayed_counter_window_w)
                    4'd0: begin
                        window_r[0] <= cal_bram_douta_w;
                        window_r[1] <= window_w[1];
                        window_r[2] <= window_w[2];
                        window_r[3] <= window_w[3];
                        window_r[4] <= window_w[4];
                        window_r[5] <= window_w[5];
                        window_r[6] <= window_w[6];
                        window_r[7] <= window_w[7];
                        window_r[8] <= window_w[8];
                    end
                    4'd1: begin
                        window_r[0] <= window_w[0];
                        window_r[1] <= cal_bram_douta_w;
                        window_r[2] <= window_w[2];
                        window_r[3] <= window_w[3];
                        window_r[4] <= window_w[4];
                        window_r[5] <= window_w[5];
                        window_r[6] <= window_w[6];
                        window_r[7] <= window_w[7];
                        window_r[8] <= window_w[8];
                    end
                    4'd2: begin
                        window_r[0] <= window_w[0];
                        window_r[1] <= window_w[1];
                        window_r[2] <= cal_bram_douta_w;
                        window_r[3] <= window_w[3];
                        window_r[4] <= window_w[4];
                        window_r[5] <= window_w[5];
                        window_r[6] <= window_w[6];
                        window_r[7] <= window_w[7];
                        window_r[8] <= window_w[8];
                    end
                    4'd3: begin
                        window_r[0] <= window_w[0];
                        window_r[1] <= window_w[1];
                        window_r[2] <= window_w[2];
                        window_r[3] <= cal_bram_douta_w; // If in the same line -> keep the previous window[4] to window[3]
                        window_r[4] <= window_w[4];
                        window_r[5] <= window_w[5];
                        window_r[6] <= window_w[6];
                        window_r[7] <= window_w[7];
                        window_r[8] <= window_w[8];
                    end
                    4'd4: begin
                        window_r[0] <= window_w[0];
                        window_r[1] <= window_w[1];
                        window_r[2] <= window_w[2];
                        window_r[3] <= window_w[3];
                        window_r[4] <= cal_bram_douta_w;
                        window_r[5] <= window_w[5];
                        window_r[6] <= window_w[6];
                        window_r[7] <= window_w[7];
                        window_r[8] <= window_w[8];
                    end
                    4'd5: begin
                        window_r[0] <= window_w[0];
                        window_r[1] <= window_w[1];
                        window_r[2] <= window_w[2];
                        window_r[3] <= window_w[3];
                        window_r[4] <= window_w[4];
                        window_r[5] <= cal_bram_douta_w;
                        window_r[6] <= window_w[6];
                        window_r[7] <= window_w[7];
                        window_r[8] <= window_w[8];
                    end
                    4'd6: begin
                        window_r[0] <= window_w[0];
                        window_r[1] <= window_w[1];
                        window_r[2] <= window_w[2];
                        window_r[3] <= window_w[3];
                        window_r[4] <= window_w[4];
                        window_r[5] <= window_w[5];
                        window_r[6] <= cal_bram_douta_w;
                        window_r[7] <= window_w[7];
                        window_r[8] <= window_w[8];
                    end
                    4'd7: begin
                        window_r[0] <= window_w[0];
                        window_r[1] <= window_w[1];
                        window_r[2] <= window_w[2];
                        window_r[3] <= window_w[3];
                        window_r[4] <= window_w[4];
                        window_r[5] <= window_w[5];
                        window_r[6] <= window_w[6];
                        window_r[7] <= cal_bram_douta_w;
                        window_r[8] <= window_w[8];
                    end
                    4'd8: begin
                        window_r[0] <= window_w[0];
                        window_r[1] <= window_w[1];
                        window_r[2] <= window_w[2];
                        window_r[3] <= window_w[3];
                        window_r[4] <= window_w[4];
                        window_r[5] <= window_w[5];
                        window_r[6] <= window_w[6];
                        window_r[7] <= window_w[7];
                        window_r[8] <= cal_bram_douta_w;
                    end
                    default: begin
                        window_r[0] <= window_w[0];
                        window_r[1] <= window_w[1];
                        window_r[2] <= window_w[2];
                        window_r[3] <= window_w[3];
                        window_r[4] <= window_w[4];
                        window_r[5] <= window_w[5];
                        window_r[6] <= window_w[6];
                        window_r[7] <= window_w[7];
                        window_r[8] <= window_w[8];
                    end
                endcase
            end
            else if(update_window_w) begin
                window_r[0] <= window_out_w[0];
                window_r[1] <= window_out_w[1];
                window_r[2] <= window_out_w[2];
                window_r[3] <= window_out_w[3];
                window_r[4] <= window_out_w[4];
                window_r[5] <= window_out_w[5];
                window_r[6] <= window_out_w[6];
                window_r[7] <= window_out_w[7];
                window_r[8] <= window_out_w[8];
            end
            else begin
                window_r[0] <= window_w[0];
                window_r[1] <= window_w[1];
                window_r[2] <= window_w[2];
                window_r[3] <= window_w[3];
                window_r[4] <= window_w[4];
                window_r[5] <= window_w[5];
                window_r[6] <= window_w[6];
                window_r[7] <= window_w[7];
                window_r[8] <= window_w[8];
            end
        end
    end

    wire [`FULLBITWIDTH-1:0] cal_douta_w, cal_douta_valid_w;

    assign cal_douta_w = (global_valid_w) ? {24'd0, bram_douta_out_w} : 32'd0;

    assign cal_douta_valid_w = {31'd0, global_valid_w};

    assign douta_o = (addra_mode_w == 2'd1) ? cal_douta_valid_w : cal_douta_w;
    
     ila_median_filter ila_median_filter_inst (
         .clk(CLK),

         .probe0(counter_i_w),                           // 10-bit
         .probe1(counter_j_w),                           // 10-bit
         .probe2(fsm_addra_w),                           // 18-bit
         .probe3(fsm_dina_w),                            // 8-bit
         .probe4(fsm_wea_w),                             // 1-bit
         .probe5(fsm_ena_w),                             // 1-bit
         .probe6(delayed_2clk_start_bubble_sort_r),      // 1-bit
         .probe7(valid_bubble_sort_w),                   // 1-bit
         .probe8(window_w[0]),                           // 8-bit
         .probe9(window_w[1]),                           // 8-bit
         .probe10(window_w[2]),                          // 8-bit
         .probe11(window_w[3]),                          // 8-bit
         .probe12(window_w[4]),                          // 8-bit
         .probe13(window_w[5]),                          // 8-bit
         .probe14(window_w[6]),                          // 8-bit
         .probe15(window_w[7]),                          // 8-bit
         .probe16(window_w[8]),                          // 8-bit
         .probe17(addra_mode_w),                         // 2-bit
         .probe18(addra_i),                              // 20-bit
         .probe19(dina_i),                               // 32-bit
         .probe20(ena_i),                                // 1-bit
         .probe21(wea_i),                                // 1-bit
         .probe22(width_w),                              // 10-bit
         .probe23(height_w),                             // 10-bit
         .probe24(douta_o),                              // 32-bit
         .probe25(global_valid_w),                       // 1-bit
         .probe26(row_based_w),                          // 18-bit
         .probe27(cal_read_addra_w),                     // 19-bit
         .probe28(cal_bram_douta_w)                      // 8-bit
     );

endmodule

module counter_10_bit (
    input wire CLK,
    input wire RST,
    input wire increment_i,
    input wire clear_i,
    output wire [9:0] count_o
);
    reg [9:0] count_r;

    assign count_o = count_r;

    always@(posedge CLK or negedge RST) begin
        if (!RST) begin
            count_r <= 10'd0;
        end
        else begin
            if (clear_i) begin
                count_r <= 10'd0;
            end
            else if (increment_i) begin
                count_r <= count_r + 10'b1;
            end
            else begin
                count_r <= count_o;
            end
        end
    end
endmodule

module counter_4_bit(
    input wire CLK,
    input wire RST,
    input wire increment_i,
    input wire clear_i,
    output wire [3:0] count_o
);
    reg [3:0] count_r;

    assign count_o = count_r;

    always@(posedge CLK or negedge RST) begin
        if (!RST) begin
            count_r <= 4'd0;
        end
        else begin
            if (clear_i) begin
                count_r <= 4'd0;
            end
            else if (increment_i) begin
                count_r <= count_r + 4'b1;
            end
            else begin
                count_r <= count_o;
            end
        end
    end
endmodule