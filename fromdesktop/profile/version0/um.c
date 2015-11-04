/* 
 * Graham Goudeau & Deanna Bessy  
 * Homework #6, UM
 * um.c
 * 1/5/15
 */
#include <stdlib.h>
#include <stdio.h>
#include <bitpack.h>
#include "um.h"
#include "segment_memory.h"
#include "on_disk.h"


#include <stdio.h>
typedef uint32_t instruction;

const unsigned WORD_WIDTH_BITS = sizeof(word) * 8;
const int NUM_REGS = 8;
const unsigned OP_WIDTH = 4;
const unsigned OP_LSB = WORD_WIDTH_BITS - OP_WIDTH;
const unsigned REG_WIDTH = 3;
const unsigned REG_A_LSB = 6;
const unsigned REG_B_LSB = 3;
const unsigned REG_C_LSB = 0;
const unsigned REG_A_OP_13 = 25;
const unsigned VAL_WIDTH = 25;
const unsigned VAL_LSB = 0;

typedef enum {
        COND_MOVE = 0,
        SEGM_LOAD,
        SEGM_STORE,
        ADD,
        MULT,
        DIV,
        NAND,
        HALT,
        MAP_SEGM,
        UNMAP_SEGM,
        OUTPUT,
        INPUT,
        LOAD_PROG,
        LOAD_VAL
} OP_CODES_e;

/*Struct containing info needed to execute an instruction*/
typedef struct {
        OP_CODES_e operation;
        int A;
        int B;
        int C;
        word value;
} operation_info;

/*Initializes all the registers to hold 0*/
void initialize_registers(word registers[])
{
        int i;
        for (i = 0; i < NUM_REGS; i++)
                registers[i] = 0;
}
/***********************************************************************
 * Name: parse_operator                                                *
 * Purpose: Gets the correct operator, registers, and (if needed) value* 
 *          from an extracted 32-bit instruction.                      *
 * Arguments: A 32 bit instruction.                                    *     
 * Returns: Struct containing information parsed from the instruction. *
 ***********************************************************************/
operation_info static inline parse_operator(instruction op_code)
{
        operation_info parsed_info;
        OP_CODES_e operator = Bitpack_getu(op_code, OP_WIDTH, OP_LSB);
        parsed_info.operation = operator;

        if (operator == LOAD_VAL) {
                parsed_info.value = Bitpack_getu(op_code, VAL_WIDTH, VAL_LSB);
                parsed_info.A = Bitpack_getu(op_code, REG_WIDTH, REG_A_OP_13);
        }
        else {
                parsed_info.A = Bitpack_getu(op_code, REG_WIDTH, REG_A_LSB);
                parsed_info.B = Bitpack_getu(op_code, REG_WIDTH, REG_B_LSB);
                parsed_info.C = Bitpack_getu(op_code, REG_WIDTH, REG_C_LSB);
        }

        return parsed_info;
}

/******************************************************************************
 * NOTE: Below are the functions that execute a single instruction given 
         the required info. They work exactly as outlined in UM specs.
******************************************************************************/

void static inline cond_move(operation_info op_code, word registers[])
{
        if (registers[op_code.C] != 0)
                registers[op_code.A] = registers[op_code.B];
}

void static inline segm_load(operation_info op_code, word registers[],
                             memory_data memory)
{
        registers[op_code.A] = get_word(registers[op_code.B], 
                                        registers[op_code.C], memory);
}

void static inline segm_store(operation_info op_code, word registers[],
                              memory_data memory)
{
        write_word(registers[op_code.A], registers[op_code.B], 
                   registers[op_code.C], memory);
}

void static inline add(operation_info op_code, word registers[])
{
        registers[op_code.A] = registers[op_code.B] + registers[op_code.C];
}

void static inline mult(operation_info op_code, word registers[])
{
        registers[op_code.A] = registers[op_code.B] * registers[op_code.C];
}

void static inline divide(operation_info op_code, word registers[])
{
        registers[op_code.A] = registers[op_code.B] / registers[op_code.C];
}

void static inline nand(operation_info op_code, word registers[])
{
        registers[op_code.A] = ~(registers[op_code.B] & registers[op_code.C]);
}

void static inline map_segm(operation_info op_code, word registers[],
                            memory_data memory)
{
        registers[op_code.B] = map_segment(registers[op_code.C], memory);
}

void static inline unmap_segm(operation_info op_code, word registers[],
                              memory_data memory)
{
        unmap_segment(registers[op_code.C], memory);
}

void static inline output(operation_info op_code, word registers[])
{
        fprintf(stdout, "%c", registers[op_code.C]);
}

void static inline input(operation_info op_code, word registers[])
{
        char c;
        fscanf(stdin, "%c", &c);
        if (c == 0) {
                registers[op_code.C] = -1;
        }
        else
                registers[op_code.C] = c;
}

size_t static inline load_prog(operation_info op_code, word registers[],
                             memory_data memory)
{
        size_t new_program_counter = registers[op_code.C];
        if (registers[op_code.B] == 0) return new_program_counter;
        
        load_program(registers[op_code.B], memory);
        return new_program_counter;
}

void static inline load_val(operation_info op_code, word registers[])
{
        registers[op_code.A] = op_code.value;
}

/**********************************************************************
 * Name: execute                                                      *
 * Purpose: Calls the correct function to exacute a given op code     *
 * Arguments: Program counter, op code struct, array of registers, and*
 *            segmented memory struct.                                *
 * Returns: Program counter                                           *
 **********************************************************************/
size_t static inline execute(size_t program_counter, operation_info op_code,
                             word registers[], memory_data memory)
{
        switch (op_code.operation) {
                case (COND_MOVE):
                        cond_move(op_code, registers);
                        break;
                case (SEGM_LOAD):
                        segm_load(op_code, registers, memory);
                        break;
                case (SEGM_STORE):
                        segm_store(op_code, registers, memory);
                        break;
                case (ADD):
                        add(op_code, registers);
                        break;
                case (MULT):
                        mult(op_code, registers);
                        break;
                case (DIV):
                        divide(op_code, registers);
                        break;
                case (NAND):
                        nand(op_code, registers);
                        break;
                case (HALT):
                        break;
                case (MAP_SEGM):
                        map_segm(op_code, registers, memory);
                        break;
                case (UNMAP_SEGM):
                        unmap_segm(op_code, registers, memory);
                        break;
                case (OUTPUT):
                        output(op_code, registers);
                        break;
                case (INPUT):
                        input(op_code, registers);
                        break;
                case (LOAD_PROG):
                        return load_prog(op_code, registers, memory);
                        break;
                case (LOAD_VAL):
                        load_val(op_code, registers);
                        break;
        }

        return program_counter;
}

void run(const char *fp)
{
        memory_data memory = boot_up_memory(fp); 

        word registers[NUM_REGS];
        initialize_registers(registers);
	
	/*Starts program counter at 0, gets and parses the first operator.*/
        size_t program_counter = 0;
        instruction op_code = get_operator(program_counter, memory);
        operation_info parsed_op_code = parse_operator(op_code);
        program_counter++;

	/* Loops through segment 0, getting, parsing, and executing each 
	 * instruction until HALT is reached. 
         */
        while (parsed_op_code.operation != HALT) {
                program_counter = execute(program_counter, parsed_op_code, 
                                          registers, memory);                
                op_code = get_operator(program_counter, memory);
                parsed_op_code = parse_operator(op_code);
                program_counter++;
        }
        free_memory(memory);
}
