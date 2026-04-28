/*
 * Data Memory
 * Synchronous write for 'sw' and asynchronous read for 'lw'.
 */
module data_mem
(
    input  logic        clk,
    input  logic        mem_wen,
    input  logic [31:0] addr,
    input  logic [31:0] mem_wdata,
    output logic [31:0] mem_rdata
);

    logic [31:0] memory [1024] = '{default: '0};

    // Synchronous Write
    always_ff @(posedge clk) begin
        if (mem_wen) begin
            memory[addr[11:2]] <= mem_wdata;
        end
    end

    // Asynchronous Read
    assign mem_rdata = memory[addr[11:2]];

    
endmodule