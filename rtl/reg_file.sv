/*
 * RISC-V Register File
 * Provides 32 general-purpose registers (x0-x31).
 * Register x0 is hardwired to 0.
 * Two asynchronous read ports (rs1, rs2).
 * One synchronous write port (rd) with write enable.
 */
module reg_file
#(
    parameter DATA_WIDTH = 32,
    parameter ADDR_WIDTH = 5
)
(
    input  logic                   clk,
    input  logic                   reg_write, // Write Enable
    input  logic [ADDR_WIDTH-1:0]  r_addr1,        // Read Address 1 (rs1)
    input  logic [ADDR_WIDTH-1:0]  r_addr2,        // Read Address 2 (rs2)
    input  logic [ADDR_WIDTH-1:0]  w_addr,        // Write Address (rd)
    input  logic [DATA_WIDTH-1:0]  w_data,       // Write Data
    output logic [DATA_WIDTH-1:0]  r_data1,       // Read Data 1
    output logic [DATA_WIDTH-1:0]  r_data2        // Read Data 2
);

    // Array of 32 registers, each 32 bits wide
    logic [DATA_WIDTH-1:0] Registers [31:0] = '{default: '0};

    /* * Synchronous Write 
     * Note: Register x0 cannot be overwritten.
     */
    always_ff @(posedge clk) begin
        if (reg_write && (w_addr != 5'b00000)) begin
            Registers[w_addr] <= w_data;
        end
    end

    /* * Asynchronous Read 
     * Register x0 always returns 0.
     */
    assign r_data1 = (r_addr1 != 5'b00000) ? Registers[r_addr1] : {DATA_WIDTH{1'b0}};
    assign r_data2 = (r_addr2 != 5'b00000) ? Registers[r_addr2] : {DATA_WIDTH{1'b0}};

endmodule