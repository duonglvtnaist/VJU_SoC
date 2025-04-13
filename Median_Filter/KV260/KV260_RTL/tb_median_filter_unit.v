// Author: Le Vu Trung Duong
// Description: Testbench for median filter unit

`include "common.vh"
`timescale 1ns / 1ps

module tb_median_filter_unit (
);

    reg                                         CLK;
    reg                                         RST;
    reg     [`FULLBITWIDTH-1:0]                 dina_i;
    reg     [`MODE_ADDR_WIDTH+`ADDR_WIDTH-1:0]  addra_i;
    reg                                         wea_i;
    reg                                         ena_i;
    wire    [`FULLBITWIDTH-1:0]                 douta_o;



    median_filter_unit inst_median_filter_unit (
        .CLK(CLK),
        .RST(RST),

        .dina_i(dina_i),
        .addra_i(addra_i),
        .wea_i(wea_i),
        .ena_i(ena_i),
        .douta_o(douta_o)
    );

    always #5 CLK = ~CLK;

    reg [7:0] mem [0:238219];
    
    reg [7:0] mem_out [0:238219];
    

    initial begin
//        $readmemh("G:/My Drive/VJU Giao Trinh/CE433 - Thiet ke he thong SoC/DeAnCuoiKy/RTL/noisyimg.txt", mem);
        $readmemh("/home/duong/Research/2025/DeAnCuoiKy/RTL/noisyimg.txt", mem);
    end

    integer i;
    
    integer f_out;


    initial begin
        CLK = 0;
        RST = 0;
        dina_i = 0;
        addra_i = 0;
        wea_i = 0;
        ena_i = 0;
        #(10*200) RST = 1;
        for (i=0; i<238220; i=i+1) begin
            #10
            dina_i = mem[i];
            addra_i = i;
            wea_i = 1;
            ena_i = 1;
        end
        #10
        dina_i = 430;
        addra_i = 2 << 18;
        wea_i = 1;
        ena_i = 1;
        #10
        dina_i = 554;
        addra_i = 3 << 18;
        wea_i = 1;
        ena_i = 1;
        #10
        dina_i = 1;
        addra_i = 1 << 18;
        wea_i = 1;
        ena_i = 1;
        #10
        while (douta_o != 32'd1) begin
            #10
            dina_i = 0;
            addra_i = 1 << 18;
            wea_i = 0;
            ena_i = 1;
        end
        
        // Write code here for read output

//        #(10*500+480*238210)
        #50
        f_out = $fopen("/home/duong/Research/2025/DeAnCuoiKy/RTL/remove_noisying_v2.txt", "w");
        
        for (i = 0; i < 238220; i = i + 1) begin
            @(posedge CLK);
            dina_i = 0;
            addra_i = i;
            wea_i = 0;
            ena_i = 1;
            #20; // ch? gi� tr? xu?t hi?n
            mem_out[i] = douta_o;
            $fwrite(f_out, "%02x\n", douta_o);
        end

        $fclose(f_out);
        $finish;
    end

endmodule