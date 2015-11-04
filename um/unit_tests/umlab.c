#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitpack.h"

#include "assert.h"
#include "fmt.h"
#include "seq.h"

/* This is the code from the lab handout */

typedef uint32_t Um_instruction;

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
} Um_opcode;

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction loadval(unsigned ra, unsigned val);

Um_instruction inline three_register(Um_opcode op, int ra, int rb, int rc)
{
	Um_instruction new_instr = 0;
	new_instr = Bitpack_newu(new_instr, 4, 28, op);
	new_instr = Bitpack_newu(new_instr, 3, 6, ra);
	new_instr = Bitpack_newu(new_instr, 3, 3, rb);
	new_instr = Bitpack_newu(new_instr, 3, 0, rc);
	return new_instr;
}

Um_instruction inline loadval(unsigned ra, unsigned val)
{
	Um_instruction new_instr = 0;
	new_instr = Bitpack_newu(new_instr, 4, 28, 13);
	new_instr = Bitpack_newu(new_instr, 3, 25, ra);
	new_instr = Bitpack_newu(new_instr, 25, 0, val);

	return new_instr;
}


static inline Um_instruction halt(void)
{
	return three_register(HALT, 0, 0, 0);
}

enum regs { r0 = 0, r1, r2, r3, r4, r5, r6, r7 };

extern void Um_write_sequence(FILE *output, Seq_T stream);

Um_instruction static inline output(int rc)
{
	return three_register(OUTPUT, 0, 0, rc);
}

Um_instruction static inline loadprogram(int rb, int rc)
{
	return three_register(LOAD_PROG, 0, rb, rc);
}

extern void Um_write_sequence(FILE *output, Seq_T stream)
{
	while (Seq_length(stream) != 0) {
		Um_instruction instr = (Um_instruction)Seq_remlo(stream);
		int i;
		for (i = 0; i < 4; i++) {
			char c = Bitpack_getu(instr, 8, 24 - (8*i));
			fprintf(output, "%c", c);
		}
	}
}


static inline void emit(Seq_T stream, Um_instruction inst)
{
	assert(sizeof(inst) <= sizeof(uintptr_t));
	Seq_addhi(stream, (void *)(uintptr_t) inst);
}

static inline Um_instruction get_inst(Seq_T stream, int i)
{
	assert(sizeof(Um_instruction) <= sizeof(uintptr_t));
	return (Um_instruction) (uintptr_t) (Seq_get(stream, i));
}

static inline void put_inst(Seq_T stream, int i, Um_instruction inst)
{
	assert(sizeof(inst) <= sizeof(uintptr_t));
	Seq_put(stream, i, (void *)(uintptr_t) inst);
}

void emit_halt_test(Seq_T stream)
{
	fprintf(stderr, "emitting halt test\n");
	emit(stream, halt());
	emit(stream, loadval(r1, 'B'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, 'a'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, 'd'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, '!'));
	emit(stream, output(r1));
	emit(stream, loadval(r1, '\n'));
	emit(stream, output(r1));
}

static void add_label(Seq_T stream, int location_to_patch, int label_value)
{
	Um_instruction inst = get_inst(stream, location_to_patch);
	unsigned k = Bitpack_getu(inst, 25, 0);
	inst = Bitpack_newu(inst, 25, 0, label_value + k);
	put_inst(stream, location_to_patch, inst);
}

static void emit_out_string(Seq_T stream, const char *s, int aux_reg);

void emit_out_string(Seq_T stream, const char *s, int aux_reg)
{
	char c = s[0];
	int i = 0;
	while (c != 0) {
		emit(stream, loadval(aux_reg, c));
		emit(stream, output(aux_reg));
		i++;
		c = s[i];
	}
}

void emit_goto_test(Seq_T stream)
{
	int patch_L = Seq_length(stream);
	emit(stream, loadval(r7, 0));	     /* will be patched to 'r7 := L' */
	emit(stream, loadprogram(r0, r7));   /* should goto label L          */
	emit_out_string(stream, "GOTO failed.\n", r1);
	emit(stream, halt());
	/* define 'L' to be here */
	add_label(stream, patch_L, Seq_length(stream));	
	emit_out_string(stream, "GOTO passed.\n", r1);
	emit(stream, halt());
}


void emit_cond_move_zero_test(Seq_T stream)
{
	int patch_L1 = Seq_length(stream);
	emit(stream, loadval(r7, 0));
	int patch_L2 = Seq_length(stream);
	emit(stream, loadval(r6, 0));
	emit(stream, three_register(COND_MOVE, r7, r6, r0));
	emit(stream, loadprogram(r0, r7));
	add_label(stream, patch_L2, Seq_length(stream));
	emit_out_string(stream, "Conditional move on zero register failed\n", r5);
	emit(stream, three_register(HALT, 0, 0, 0));
	add_label(stream, patch_L1, Seq_length(stream));
	emit_out_string(stream, "Conditional move on zero register passed.\n", r5);
	emit(stream, three_register(HALT, 0, 0, 0));
}

void emit_cond_move_nonzero_test(Seq_T stream)
{
	emit(stream, loadval(r7, 100));
	int patch_L1 = Seq_length(stream);
	emit(stream, loadval(r6, 0));
	int patch_L2 = Seq_length(stream);
	emit(stream, loadval(r5, 0));
	emit(stream, three_register(COND_MOVE, r6, r5, r7));
	emit(stream, loadprogram(r0, r6));
	add_label(stream, patch_L2, Seq_length(stream));
	emit_out_string(stream, "Conditional move on nonzero register passed\n", r3);
	emit(stream, three_register(HALT, 0,0,0));
	add_label(stream, patch_L1, Seq_length(stream));
	emit_out_string(stream, "Conditional move on nonzero register failed\n", r3);
	emit(stream, three_register(HALT, 0, 0, 0));
}

void emit_read_put_char_test(Seq_T stream)
{
	emit(stream, three_register(INPUT, 0, 0, r7));
	emit(stream, three_register(OUTPUT, 0, 0, r7));
	emit(stream, three_register(HALT, 0,0,0));
}

void emit_print_digit(Seq_T stream)
{
	emit(stream, loadval(r7, 48));
	emit(stream, loadval(r6, 5));
	emit(stream, three_register(ADD, r7, r6, r7));
	emit(stream, output(r7));
	emit(stream, three_register(HALT, 0,0,0));
}

void emit_nand_print_digit(Seq_T stream)
{
	//uint32_t mask = 33554416;
	uint32_t mask = 15;
	emit(stream, loadval(r7, mask));
	emit(stream, three_register(INPUT, 0,0,r6));
	emit(stream, three_register(NAND, r7, r7, r6));
	emit(stream, three_register(NAND, r7, r7, r7));
	emit(stream, loadval(r6, 48));
	emit(stream, three_register(ADD, r7, r7, r6));
	emit(stream, output(r7));
	emit(stream, three_register(HALT, 0,0,0));
}

void emit_runtime_test(Seq_T stream)
{
        emit(stream, loadval(r7, 0));
        emit(stream, loadval(r6, 1));
        int i;
        for (i = 0; i < 500000; i++)
                emit(stream, three_register(ADD, r7, r7, r6));

        emit(stream, three_register(HALT, 0, 0, 0));
    
}

void emit_map_unmap_test(Seq_T stream)
{
        emit(stream, loadval(r3, 10));
        emit(stream, three_register(MAP_SEGM, 0, r7, r3));
        emit(stream, three_register(UNMAP_SEGM, 0, 0, r7));
        emit_out_string(stream, "Unmapped: ", r5);
        emit(stream, loadval(r6, 48));
        emit(stream, three_register(ADD, r7, r6, r7));
        emit(stream, output(r7));
        emit_out_string(stream, " successfully\n", r5);
        emit(stream, three_register(HALT, 0,0,0));
}

void emit_add_test(Seq_T stream)
{
        emit(stream, loadval(r7, 4));
        emit(stream, loadval(r6, 5));
        emit(stream, three_register(ADD, r7, r6, r7));
        emit(stream, loadval(r5, 48));
        emit(stream, three_register(ADD, r7, r5, r7));
        emit_out_string(stream, "Addition returned ", r4);
        emit(stream, output(r7));
        emit_out_string(stream, " successfully\n", r2);
        emit(stream, three_register(HALT, 0, 0, 0));
}

void emit_mult_test(Seq_T stream)
{
        emit(stream, loadval(r7, 4));
        emit(stream, loadval(r6, 2));
        emit(stream, three_register(MULT, r7, r7, r6));
        emit(stream, loadval(r6, 48));
        emit(stream, three_register(ADD, r7, r6, r7));
        emit_out_string(stream, "Multiplication returned ", r5);
        emit(stream, output(r7));
        emit_out_string(stream, " successfully\n", r5);
        emit(stream, three_register(HALT, 0, 0, 0));
}

void emit_div_test(Seq_T stream)
{
        emit(stream, loadval(r7, 4));
        emit(stream, loadval(r6, 2));
        emit(stream, three_register(DIV, r7, r7, r6));
        emit(stream, loadval(r6, 48));
        emit(stream, three_register(ADD, r7, r6, r7));
        emit_out_string(stream, "Division returned ", r5);
        emit(stream, output(r7));
        emit_out_string(stream, " successfully\n", r5);
        emit(stream, three_register(HALT, 0, 0, 0));

}

void emit_cipher_test(Seq_T stream)
{
        emit(stream, three_register(INPUT, 0, 0, r7));
        emit(stream, three_register(NAND, r6, r7, r7));
        int patch_L1 = Seq_length(stream);
        emit(stream, loadval(r5, 0));
        int patch_L2 = Seq_length(stream);
        emit(stream, loadval(r3, 0));
        emit(stream, three_register(COND_MOVE, r3, r5, r6));
        emit(stream, loadval(r0, 0));
        emit(stream, loadprogram(r0, r3));
        add_label(stream, patch_L1, Seq_length(stream));
        emit(stream, loadval(r1, 1));
        emit(stream, three_register(ADD, r7, r1, r7));
        emit(stream, output(r7));
        emit(stream, three_register(LOAD_PROG, r0, r0, r0));
        add_label(stream, patch_L2, Seq_length(stream));
        emit(stream, three_register(HALT, 0, 0, 0));
        
}
/*



	emit(stream, loadval(r7, 100));
	int patch_L1 = Seq_length(stream);
	emit(stream, loadval(r6, 0));
	int patch_L2 = Seq_length(stream);
	emit(stream, loadval(r5, 0));
	emit(stream, three_register(COND_MOVE, r6, r5, r7));
	emit(stream, loadprogram(r0, r6));
	add_label(stream, patch_L2, Seq_length(stream));
	emit_out_string(stream, "Conditional move on nonzero register passed\n", r3);
	emit(stream, three_register(HALT, 0,0,0));
	add_label(stream, patch_L1, Seq_length(stream));
	emit_out_string(stream, "Conditional move on nonzero register failed\n", r3);
	emit(stream, three_register(HALT, 0, 0, 0));
*/
