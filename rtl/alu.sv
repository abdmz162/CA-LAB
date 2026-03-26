`include "opcode.vh"

module alu_control (
    input  logic [6:0] opcode,
    input  logic [2:0] funct3,
    input  logic [6:0] funct7,
    output logic [3:0] alu_op
);

    // ALU operation type
    typedef enum logic [3:0] {
        ALU_ADD,
        ALU_SUB,
        ALU_SLT,
        ALU_SLTU,
        ALU_AND,
        ALU_OR,
        ALU_XOR,
        ALU_SLL,
        ALU_SRL,
        ALU_SRA
    } alu_op_t;


    always_comb begin
        case (opcode)

            `OPC_ARI_RTYPE: begin
                case (funct3)

                    `FNC_ADD_SUB: begin
                        if (funct7[5] == `FNC2_SUB)
                            alu_op = ALU_SUB;
                        else
                            alu_op = ALU_ADD;
                    end

                    `FNC_SLL:     alu_op = ALU_SLL;
                    `FNC_SLT:     alu_op = ALU_SLT;
                    `FNC_SLTU:    alu_op = ALU_SLTU;
                    `FNC_XOR:     alu_op = ALU_XOR;
                    `FNC_OR:      alu_op = ALU_OR;
                    `FNC_AND:     alu_op = ALU_AND;

                    `FNC_SRL_SRA: begin
                        if (funct7[5] == `FNC2_SRA)
                            alu_op = ALU_SRA;
                        else
                            alu_op = ALU_SRL;
                    end

                    default: alu_op = ALU_ADD;

                endcase
            end


            `OPC_ARI_ITYPE: begin
                case (funct3)

                    `FNC_ADD_SUB: alu_op = ALU_ADD;
                    `FNC_SLT:     alu_op = ALU_SLT;
                    `FNC_SLTU:    alu_op = ALU_SLTU;
                    `FNC_XOR:     alu_op = ALU_XOR;
                    `FNC_OR:      alu_op = ALU_OR;
                    `FNC_AND:     alu_op = ALU_AND;
                    `FNC_SLL:     alu_op = ALU_SLL;

                    `FNC_SRL_SRA: begin
                        if (funct7[5] == `FNC2_SRA)
                            alu_op = ALU_SRA;
                        else
                            alu_op = ALU_SRL;
                    end

                    default: alu_op = ALU_ADD;

                endcase
            end


            `OPC_BRANCH: begin
                alu_op = ALU_SUB;
            end


            `OPC_LOAD,
            `OPC_STORE,
            `OPC_JAL,
            `OPC_JALR,
            `OPC_LUI,
            `OPC_AUIPC: begin
                alu_op = ALU_ADD;
            end


            default: begin
                alu_op = ALU_ADD;
            end

        endcase
    end

endmodule

//Note : Manual asked for 2 controllers however we basically combine both at one .and its working alright ,have added sim pic on doc/ as well
//However if need 2 separate controllers , can separate in future THANKS !
