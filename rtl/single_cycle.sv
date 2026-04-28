module single_cycle (
    input logic clk,
    input logic rst
);
    // Internal Wires
    logic [31:0] pc_out, pc_next_addr, pc_plus_4, pc_branch, instruction, imm_extended;
    logic [31:0] r_data1, r_data2, alu_out, operand_b, mem_rdata_out, wb_data;
    logic [3:0]  alu_control_sig;
    logic [1:0]  alu_op, wb_sel;
    logic        reg_we, alu_b_sel, zero_flag;
    logic        mem_we, branch_en, jump_en, pc_select, branch_taken;

    // 1. Program Counter
    pc pc_unit (
        .clk(clk), .rst(rst),
        .pc_next(pc_next_addr), .pc_out(pc_out)
    );

    // 2. PC Logic & Branch Evaluation
    assign pc_plus_4  = pc_out + 32'd4;
    assign pc_branch  = pc_out + imm_extended;

    always_comb begin
        case(instruction[14:12])
            3'b000: branch_taken = zero_flag;   // BEQ
            3'b001: branch_taken = ~zero_flag;  // BNE
            default: branch_taken = 1'b0;
        endcase
    end

    assign pc_select   = jump_en | (branch_en & branch_taken);
    assign pc_next_addr = pc_select ? pc_branch : pc_plus_4;

    // 3. Instruction Memory
    instr_mem i_mem (
        .addr(pc_out), .instr(instruction)
    );

    // 4. Main Controller
    main_controller m_ctrl (
        .opcode(instruction[6:0]),
        .alu_op(alu_op),
        .reg_we(reg_we),
        .alu_sel(alu_b_sel),
        .wb_sel(wb_sel),
        .mem_wen(mem_we),
        .branch(branch_en),
        .jump(jump_en)
    );

    // 5. Immediate Generator
    imm_gen imm_unit (
        .instruction(instruction),
        .immediate(imm_extended)
    );

    // 6. Register File
    reg_file r_file (
        .clk(clk),
        .reg_write(reg_we),
        .r_addr1(instruction[19:15]),
        .r_addr2(instruction[24:20]),
        .w_addr(instruction[11:7]),
        .w_data(wb_data),
        .r_data1(r_data1),
        .r_data2(r_data2)
    );

    // 7. ALU Operand B
    assign operand_b = (alu_b_sel) ? imm_extended : r_data2;

    // 8. ALU Controller
    alu_controller a_ctrl (
        .alu_op(alu_op),
        .func3(instruction[14:12]),
        .func7(instruction[31:25]),
        .alu_operation(alu_control_sig)
    );

    // 9. ALU
    alu alu_unit (
        .op_A(r_data1),
        .op_B(operand_b),
        .alu_operation(alu_control_sig),
        .result(alu_out),
        .zero(zero_flag)
    );

    // 10. Data Memory
    data_mem d_mem (
        .clk(clk),
        .mem_wen(mem_we),
        .addr(alu_out),
        .mem_wdata(r_data2),
        .mem_rdata(mem_rdata_out)
    );

    // 11. Write Back
    always_comb begin
        case(wb_sel)
            2'b00: wb_data = alu_out;
            2'b01: wb_data = mem_rdata_out;
            2'b10: wb_data = pc_plus_4;
            2'b11: wb_data = imm_extended;
            default: wb_data = 32'b0;
        endcase
    end

endmodule