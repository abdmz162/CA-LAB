`include "opcode.vh"

module tb_imm_gen();

    reg  [31:0] instruction;
    wire [31:0] immediate;

    imm_gen dut (
        .instruction(instruction),
        .immediate(immediate)
    );

    integer errors = 0;

    task check;
        input [31:0] test_instruction;
        input [31:0] expected;
        begin
            instruction = test_instruction;
            #10;
            if (immediate !== expected) begin
                $display("ERROR: instr=%h: expected %h, got %h",
                         test_instruction, expected, immediate);
                errors = errors + 1;
            end
        end
    endtask

    initial begin
        $display("Starting Immediate Generator testbench...");

        // ---------- I-type arithmetic ----------

        // addi x1, x0, 100
        check({12'd100, 5'd0, 3'b000, 5'd1, `OPC_ARI_ITYPE},
              32'd100);

        // addi x1, x0, -1
        check({12'hFFF, 5'd0, 3'b000, 5'd1, `OPC_ARI_ITYPE},
              32'hFFFFFFFF);

        // addi x1, x0, 2047
        check({12'd2047, 5'd0, 3'b000, 5'd1, `OPC_ARI_ITYPE},
              32'd2047);

        // addi x1, x0, -2048
        check({12'h800, 5'd0, 3'b000, 5'd1, `OPC_ARI_ITYPE},
              32'hFFFFF800);

        // xori x2, x1, 255
        check({12'd255, 5'd1, 3'b100, 5'd2, `OPC_ARI_ITYPE},
              32'd255);

        // andi x3, x2, 0
        check({12'd0, 5'd2, 3'b111, 5'd3, `OPC_ARI_ITYPE},
              32'd0);

        // slli x1, x2, 5
        check({7'b0000000, 5'd5, 5'd2, 3'b001, 5'd1, `OPC_ARI_ITYPE},
              32'd5);

        // srai x1, x2, 3
        check({7'b0100000, 5'd3, 5'd2, 3'b101, 5'd1, `OPC_ARI_ITYPE},
              32'h00000403);

        // ---------- I-type load ----------

        // lw x1, 4(x2)
        check({12'd4, 5'd2, 3'b010, 5'd1, `OPC_LOAD},
              32'd4);

        // lb x1, -4(x2)
        check({12'hFFC, 5'd2, 3'b000, 5'd1, `OPC_LOAD},
              32'hFFFFFFFC);

        // lh x1, 0(x0)
        check({12'd0, 5'd0, 3'b001, 5'd1, `OPC_LOAD},
              32'd0);

        // lbu x3, 127(x4)
        check({12'd127, 5'd4, 3'b100, 5'd3, `OPC_LOAD},
              32'd127);

        // ---------- I-type JALR ----------

        // jalr x1, x2, 0
        check({12'd0, 5'd2, 3'b000, 5'd1, `OPC_JALR},
              32'd0);

        // jalr x1, x2, 256
        check({12'd256, 5'd2, 3'b000, 5'd1, `OPC_JALR},
              32'd256);

        // jalr x1, x2, -1
        check({12'hFFF, 5'd2, 3'b000, 5'd1, `OPC_JALR},
              32'hFFFFFFFF);

        // ---------- S-type ----------

        // sw x1, 0(x2)
        check({7'b0000000, 5'd1, 5'd2, 3'b010, 5'b00000, `OPC_STORE},
              32'd0);

        // sw x1, 32(x2)
        check({7'b0000001, 5'd1, 5'd2, 3'b010, 5'b00000, `OPC_STORE},
              32'd32);

        // sw x1, -4(x2)
        check({7'b1111111, 5'd1, 5'd2, 3'b010, 5'b11100, `OPC_STORE},
              32'hFFFFFFFC);

        // sb x3, 10(x4)
        check({7'b0000000, 5'd3, 5'd4, 3'b000, 5'b01010, `OPC_STORE},
              32'd10);

        // sh x5, -1(x6)
        check({7'b1111111, 5'd5, 5'd6, 3'b001, 5'b11111, `OPC_STORE},
              32'hFFFFFFFF);

        // ---------- B-type ----------

        // beq x1, x2, +8
        check({1'b0, 6'b000000, 5'd2, 5'd1, 3'b000, 4'b0100, 1'b0, `OPC_BRANCH},
              32'd8);

        // beq x1, x2, -8
        check({1'b1, 6'b111111, 5'd2, 5'd1, 3'b000, 4'b1100, 1'b1, `OPC_BRANCH},
              32'hFFFFFFF8);

        // bne x3, x4, +16
        check({1'b0, 6'b000000, 5'd4, 5'd3, 3'b001, 4'b1000, 1'b0, `OPC_BRANCH},
              32'd16);

        // blt x1, x2, 0
        check({1'b0, 6'b000000, 5'd2, 5'd1, 3'b100, 4'b0000, 1'b0, `OPC_BRANCH},
              32'd0);

        // bge x1, x2, +4094
        check({1'b0, 6'b111111, 5'd2, 5'd1, 3'b101, 4'b1111, 1'b1, `OPC_BRANCH},
              32'd4094);

        // ---------- U-type LUI ----------

        // lui x1, 0x12345
        check({20'h12345, 5'd1, `OPC_LUI},
              32'h12345000);

        // lui x1, 0xFFFFF
        check({20'hFFFFF, 5'd1, `OPC_LUI},
              32'hFFFFF000);

        // lui x1, 0x00000
        check({20'h00000, 5'd1, `OPC_LUI},
              32'h00000000);

        // lui x2, 0x80000
        check({20'h80000, 5'd2, `OPC_LUI},
              32'h80000000);

        // ---------- U-type AUIPC ----------

        // auipc x1, 0xABCDE
        check({20'hABCDE, 5'd1, `OPC_AUIPC},
              32'hABCDE000);

        // auipc x1, 0x00001
        check({20'h00001, 5'd1, `OPC_AUIPC},
              32'h00001000);

        // ---------- J-type JAL ----------

        // jal x1, +100
        check({1'b0, 10'b0000110010, 1'b0, 8'b00000000, 5'd1, `OPC_JAL},
              32'd100);

        // jal x1, -100
        check({1'b1, 10'b1111001110, 1'b1, 8'b11111111, 5'd1, `OPC_JAL},
              32'hFFFFFF9C);

        // jal x1, 0
        check({1'b0, 10'b0000000000, 1'b0, 8'b00000000, 5'd1, `OPC_JAL},
              32'd0);

        // jal x1, +2048
        check({1'b0, 10'b0000000000, 1'b1, 8'b00000000, 5'd1, `OPC_JAL},
              32'd2048);

        // jal x1, -2
        check({1'b1, 10'b1111111111, 1'b1, 8'b11111111, 5'd1, `OPC_JAL},
              32'hFFFFFFFE);

        // ---------- Default ----------

        check(32'h00000000, 32'd0);

        // Summary
        if (errors == 0)
            $display("All tests passed!");
        else
            $display("%0d errors occurred.", errors);

        $finish;
    end

endmodule