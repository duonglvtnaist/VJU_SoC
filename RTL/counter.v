module counter (
    input  wire                      CLK,
    input  wire                      RST,
    input  wire                      increment_i,
    output wire [`COUNTER_WIDTH-1:0] counter_o
);

    reg [`COUNTER_WIDTH-1:0] counter_r;

    always @(posedge CLK or negedge RST)
    begin
        if (~RST)
            counter_r <= 0;
        else begin
            if (increment_i)
                counter_r <= counter_r + `COUNTER_WIDTH'b1;
            else
                counter_r <= counter_o;
        end
    end

    assign counter_o = counter_r;

endmodule