// Author: Le Vu Trung Duong
// Nara Institute of Science and Technology
// Description: Testbench for top_processor with no AXI4 interface
`include "common.vh"

module tb_top_processor ();
    
    reg [`DATA_WIDTH-1:0] data_i;
    reg [`ADDR_WIDTH-1:0] addr_data_i;
    reg                   ena_data_a_i;
    reg                   wea_data_a_i;
    reg                   ena_data_b_i;
    reg                   wea_data_b_i;
    reg                   wea_data_o_i;
    reg                   ena_data_o_i;
    wire [`DATA_WIDTH-1:0]data_o;
    reg [`OP_WIDTH-1:0]   op_i;
    reg [`ADDR_WIDTH-1:0] addr_op_i;
    reg                   ena_op_i;
    reg                   wea_op_i;
    reg                   CLK;
    reg                   RST;
    reg                   start_i;
    wire                  done_o;

    // Instance of top_processor

    top_processor top_processor (
        .data_i             (data_i),
        .addr_data_i        (addr_data_i),
        .ena_data_a_i       (ena_data_a_i),
        .wea_data_a_i       (wea_data_a_i),
        .ena_data_b_i       (ena_data_b_i),
        .wea_data_b_i       (wea_data_b_i),
        .wea_data_o_i       (wea_data_o_i),
        .ena_data_o_i       (ena_data_o_i),
        .data_o             (data_o),
        .op_i               (op_i),
        .addr_op_i          (addr_op_i),
        .ena_op_i           (ena_op_i),
        .wea_op_i           (wea_op_i),
        .CLK                (CLK),
        .RST                (RST),
        .start_i            (start_i),
        .done_o             (done_o)
    );

    // Clock generation

    localparam CLK_PERIOD = 10;

    always begin
        #(CLK_PERIOD/2) CLK = ~CLK;
    end

    // Main testbench

    task write_data (input [`DATA_WIDTH-1:0] data, input [`ADDR_WIDTH-1:0] addr, input [`MASK_ADDR_WIDTH-1:0] mask); 
        begin
            data_i = data;
            addr_data_i = addr;
            if (mask == `SEL_A) begin
                ena_data_a_i = 1;
                wea_data_a_i = 1;
                ena_data_b_i = 0;
                wea_data_b_i = 0;
                ena_data_o_i = 0;
                wea_data_o_i = 0;
            end
            else if (mask == `SEL_B) begin
                ena_data_a_i = 0;
                wea_data_a_i = 0;
                ena_data_b_i = 1;
                wea_data_b_i = 1;
                ena_data_o_i = 0;
                wea_data_o_i = 0;
            end
            else if (mask == `SEL_OUT) begin
                ena_data_a_i = 0;
                wea_data_a_i = 0;
                ena_data_b_i = 0;
                wea_data_b_i = 0;
                ena_data_o_i = 1;
                wea_data_o_i = 1;
            end
            else begin
                ena_data_a_i = 0;
                wea_data_a_i = 0;
                ena_data_b_i = 0;
                wea_data_b_i = 0;
                ena_data_o_i = 0;
                wea_data_o_i = 0;
            end
            ena_data_a_i = 1;
            wea_data_a_i = 1;
            #(CLK_PERIOD)
            ena_data_a_i = 0;
            wea_data_a_i = 0;
        end
    endtask

    task write_op (input [`OP_WIDTH-1:0] op, input [`ADDR_WIDTH-1:0] addr);
        begin
            op_i = op;
            addr_op_i = addr;
            ena_op_i = 1;
            wea_op_i = 1;
            #(CLK_PERIOD)
            ena_op_i = 0;
            wea_op_i = 0;
        end
    endtask

    task read_data (input [`ADDR_WIDTH-1:0] addr);
        begin
            addr_data_i = addr;
            ena_data_o_i = 1;
            wea_data_o_i = 0;
            #(CLK_PERIOD)
            $display("Data at address %d: %d", addr, data_o);
            ena_data_o_i = 0;
            wea_data_o_i = 0;
        end
    endtask

    integer i;
    initial begin
        CLK             = 0;
        RST             = 0;
        start_i         = 0;
        ena_data_a_i    = 0;
        wea_data_a_i    = 0;
        ena_data_b_i    = 0;
        wea_data_b_i    = 0;
        wea_data_o_i    = 0;
        ena_data_o_i    = 0;
        op_i            = 0;
        addr_data_i     = 0;
        addr_op_i       = 0;
        ena_op_i        = 0;
        wea_op_i        = 0;
        data_i          = 0;

        #(CLK_PERIOD*100) RST = 1;

        // Fill data A
        #(CLK_PERIOD*10) 
        for (i = 0; i < 1024; i = i + 1) begin
            write_data(.data(`DATA_WIDTH'd1), .addr(i), .mask(`SEL_A));
        end
        // Fill data B
        #(CLK_PERIOD*10)
        for (i = 0; i < 1024; i = i + 1) begin
            write_data(.data(`DATA_WIDTH'd2), .addr(i), .mask(`SEL_B));
        end
        // Write operations
        #(CLK_PERIOD*10)
        for (i = 0; i < 1024; i = i + 1) begin
            write_op(`OP_ADD, i);
        end
        // Start operation
        #(CLK_PERIOD*10)
        start_i = 1;

        while (~done_o) begin
            #CLK_PERIOD;
        end

        // Read data
        #(CLK_PERIOD*10)
        for (i = 0; i < 1024; i = i + 1) begin
            read_data(i);
        end
        #(CLK_PERIOD*10)
        $finish;

    end


endmodule