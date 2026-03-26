// testbench for alu_control
`include "opcode.vh"

module tb_alu_control();

    reg [6:0] opcode;
    reg [2:0] funct3;
    reg [6:0] funct7;
    wire [3:0] alu_op;

    // Instantiate DUT
    alu_control dut (
        .opcode(opcode),
        .funct3(funct3),
        .funct7(funct7),
        .alu_op(alu_op)
    );

    // ALU operation codes (must match the module's definitions)
    localparam ALU_ADD  = 4'b0000;
    localparam ALU_SUB  = 4'b0001;
    localparam ALU_SLT  = 4'b0010;
    localparam ALU_SLTU = 4'b0011;
    localparam ALU_AND  = 4'b0100;
    localparam ALU_OR   = 4'b0101;
    localparam ALU_XOR  = 4'b0110;
    localparam ALU_SLL  = 4'b0111;
    localparam ALU_SRL  = 4'b1000;
    localparam ALU_SRA  = 4'b1001;

    integer errors = 0;

    // Task to apply stimulus and check result
    task check;
        input [6:0] test_opcode;
        input [2:0] test_funct3;
        input [6:0] test_funct7;
        input [3:0] expected;
        begin
            opcode = test_opcode;
            funct3 = test_funct3;
            funct7 = test_funct7;
            #10; // allow propagation
            if (alu_op !== expected) begin
                $display("ERROR: opcode=%b, funct3=%b, funct7=%b: expected %b, got %b",
                         test_opcode, test_funct3, test_funct7, expected, alu_op);
                errors = errors + 1;
            end
        end
    endtask

    initial begin
        $display("Starting ALU control testbench...");

        // ---------- R‑type instructions ----------
        check(`OPC_ARI_RTYPE, `FNC_ADD_SUB, `FNC7_0, ALU_ADD); // add
        check(`OPC_ARI_RTYPE, `FNC_ADD_SUB, `FNC7_1, ALU_SUB); // sub
        check(`OPC_ARI_RTYPE, `FNC_SLL,     7'b0000000, ALU_SLL); // sll
        check(`OPC_ARI_RTYPE, `FNC_SLT,     7'b0000000, ALU_SLT); // slt
        check(`OPC_ARI_RTYPE, `FNC_SLTU,    7'b0000000, ALU_SLTU); // sltu
        check(`OPC_ARI_RTYPE, `FNC_XOR,     7'b0000000, ALU_XOR); // xor
        check(`OPC_ARI_RTYPE, `FNC_SRL_SRA, `FNC7_0, ALU_SRL); // srl
        check(`OPC_ARI_RTYPE, `FNC_SRL_SRA, `FNC7_1, ALU_SRA); // sra
        check(`OPC_ARI_RTYPE, `FNC_OR,      7'b0000000, ALU_OR); // or
        check(`OPC_ARI_RTYPE, `FNC_AND,     7'b0000000, ALU_AND); // and

        // ---------- I‑type arithmetic instructions ----------
        check(`OPC_ARI_ITYPE, `FNC_ADD_SUB, 7'b0000000, ALU_ADD); // addi
        check(`OPC_ARI_ITYPE, `FNC_SLT,     7'b0000000, ALU_SLT); // slti
        check(`OPC_ARI_ITYPE, `FNC_SLTU,    7'b0000000, ALU_SLTU); // sltiu
        check(`OPC_ARI_ITYPE, `FNC_XOR,     7'b0000000, ALU_XOR); // xori
        check(`OPC_ARI_ITYPE, `FNC_OR,      7'b0000000, ALU_OR); // ori
        check(`OPC_ARI_ITYPE, `FNC_AND,     7'b0000000, ALU_AND); // andi
        check(`OPC_ARI_ITYPE, `FNC_SLL,     7'b0000000, ALU_SLL); // slli (funct7 ignored)
        check(`OPC_ARI_ITYPE, `FNC_SRL_SRA, `FNC7_0, ALU_SRL); // srli
        check(`OPC_ARI_ITYPE, `FNC_SRL_SRA, `FNC7_1, ALU_SRA); // srai

        // ---------- Branch instructions ----------
        check(`OPC_BRANCH, `FNC_BEQ, 7'b0000000, ALU_SUB); // beq
        check(`OPC_BRANCH, `FNC_BNE, 7'b0000000, ALU_SUB); // bne
        check(`OPC_BRANCH, `FNC_BLT, 7'b0000000, ALU_SUB); // blt
        check(`OPC_BRANCH, `FNC_BGE, 7'b0000000, ALU_SUB); // bge
        check(`OPC_BRANCH, `FNC_BLTU,7'b0000000, ALU_SUB); // bltu
        check(`OPC_BRANCH, `FNC_BGEU,7'b0000000, ALU_SUB); // bgeu

        // ---------- Load instructions ----------
        check(`OPC_LOAD, `FNC_LB,  7'b0000000, ALU_ADD); // lb
        check(`OPC_LOAD, `FNC_LH,  7'b0000000, ALU_ADD); // lh
        check(`OPC_LOAD, `FNC_LW,  7'b0000000, ALU_ADD); // lw
        check(`OPC_LOAD, `FNC_LBU, 7'b0000000, ALU_ADD); // lbu
        check(`OPC_LOAD, `FNC_LHU, 7'b0000000, ALU_ADD); // lhu

        // ---------- Store instructions ----------
        check(`OPC_STORE, `FNC_SB, 7'b0000000, ALU_ADD); // sb
        check(`OPC_STORE, `FNC_SH, 7'b0000000, ALU_ADD); // sh
        check(`OPC_STORE, `FNC_SW, 7'b0000000, ALU_ADD); // sw

        // ---------- Jump instructions ----------
        check(`OPC_JAL,  3'b000, 7'b0000000, ALU_ADD); // jal (funct3 don't care)
        check(`OPC_JALR, `FNC_ADD_SUB, 7'b0000000, ALU_ADD); // jalr (funct3=0)

        // ---------- Upper‑immediate instructions ----------
        check(`OPC_LUI,   3'b000, 7'b0000000, ALU_ADD); // lui
        check(`OPC_AUIPC, 3'b000, 7'b0000000, ALU_ADD); // auipc

        // ---------- Unsupported opcode (CSR) – default to ADD ----------
        check(`OPC_CSR, 3'b000, 7'b0000000, ALU_ADD);

        // Summary
        if (errors == 0)
            $display("All tests passed!");
        else
            $display("%0d errors occurred.", errors);

        $finish;
    end

endmodule