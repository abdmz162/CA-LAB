module alu (
    input  logic [31:0] op_A,
    input  logic [31:0] op_B,
    input  logic [3:0]  alu_operation,

    output logic [31:0] result,
    output logic        zero
);

    always_comb begin
        unique case (alu_operation)

            4'b0000: result = op_A & op_B;                      // AND
            4'b0001: result = op_A | op_B;                      // OR
            4'b0010: result = op_A + op_B;                      // ADD
            4'b0011: result = op_A ^ op_B;                      // XOR
            4'b0100: result = op_A << op_B[4:0];                // SLL
            4'b0101: result = op_A >> op_B[4:0];                // SRL
            4'b0110: result = op_A - op_B;                      // SUB
            4'b0111: result = $signed(op_A) >>> op_B[4:0];      // SRA
            4'b1000: result = ($signed(op_A) < $signed(op_B)) 
                                ? 32'd1 : 32'd0;                          // SLT
            4'b1001: result = (op_A < op_B) 
                                ? 32'd1 : 32'd0;                          // SLTU

            default: result = 32'd0;

        endcase
    end

    // Zero flag
    assign zero = (result == 32'd0);

endmodule