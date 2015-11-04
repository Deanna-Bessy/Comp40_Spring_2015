/* 
 * Chris Gregory & Deanna Bessy  
 * Homework #8, UM Optimization
 * um.c
 * 1/5/15
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>

#define INIT_SEGMENTS_CAPACITY 10000000
#define INIT_UNMAPPED_CAPACITY 10000000

typedef uint32_t instruction;
typedef uint32_t segment_id;
typedef uint32_t word;
typedef struct memory_data *memory_data;
typedef struct segment segment;


const unsigned WORD_WIDTH_BITS = sizeof(word) * 8;
const unsigned OP_LSB = WORD_WIDTH_BITS - 4;
const unsigned REG_WIDTH = 3;
const unsigned REG_A_LSB = 6;
const unsigned REG_B_LSB = 3;
const unsigned REG_C_LSB = 0;
const unsigned REG_A_OP_13 = 25;
const unsigned VAL_WIDTH = 25;
const unsigned VAL_LSB = 0;


/*Struct containing info needed to execute an instruction*/
typedef struct {
        int operation;
        int A;
        int B;
        int C;
        word value;
} operation_info;



/********************************************************************
 * Struct that contains the array of words that represents a segment*
 * and the length of the segment.                                   *
 ********************************************************************/
struct segment {
        size_t length;
        word *data;
};

/*********************************************************************
 * Struct that contains the table that represents segmented memory,  *
 * the stack that stored segment id's that have been freed, and      *
 * the word that keeps track of the next available unused segment id.*
 *********************************************************************/
struct memory_data {
        segment *segments;
	uint32_t segments_capacity;
        segment_id segments_size;
        
        segment_id *unmapped_ids;
	uint32_t unmapped_ids_capacity;
	uint32_t unmapped_ids_size;
};

/*
 * Revert all memory_data references to pointer to memory_data
 * with a typedef memory_data to *memory_data.
 * Keep segments as an array of struct segments
 * Create two expanding functions to expand the unmapped_ids
 * and segments. One called in map_segment and one in unmap_segment
 */

memory_data initialize_memory(void)
{
        memory_data new_memory = malloc(sizeof(struct memory_data));
        new_memory->segments = malloc(sizeof(segment) * 
                                      INIT_SEGMENTS_CAPACITY);
        new_memory->unmapped_ids = malloc(sizeof(segment_id) * 
                                          INIT_UNMAPPED_CAPACITY);
	new_memory->segments_capacity = INIT_SEGMENTS_CAPACITY;
	new_memory->unmapped_ids_capacity = INIT_UNMAPPED_CAPACITY;
        new_memory->segments_size = 0;
        new_memory->unmapped_ids_size = 0;

        return new_memory;
}

word get_word(segment_id id, size_t offset, memory_data memory)
{
        return memory->segments[id].data[offset];
}

void free_memory(memory_data memory)
{
        assert(memory != NULL);
        assert(memory->segments != NULL);
        assert(memory->unmapped_ids != NULL);
        
        segment_id i;
        segment_id j;
	/*
	 * Walks through the array and frees each array of words 
         * and segment struct.
	 */
	
        for (i = 0; i < memory->segments_size; i++) {           
		int found_in_unmapped = 0;		
		for (j = 0; j < memory->unmapped_ids_size; j++) {
                        if (memory->unmapped_ids[j] == i)
                                found_in_unmapped = 1;
                }
                if (found_in_unmapped == 0)
                        free(memory->segments[i].data);
			
	}
        
        free(memory->unmapped_ids);
        free(memory->segments);
        free(memory);
}

void expand_segments(memory_data memory)
{
	segment *segments = memory->segments;
	segment *new_segments = malloc(sizeof(segment) * 
                                       memory->segments_capacity * 2);
	memory->segments_capacity *= 2;
	size_t i;	
	for (i = 0; i < memory->segments_size; i++) {
		new_segments[i].data = segments[i].data;
		new_segments[i].length = segments[i].length; 
	}
	free(memory->segments);
	memory->segments = new_segments;
}

void expand_unmapped_ids(memory_data memory)
{
	uint32_t *unmapped_ids = memory->unmapped_ids;
	uint32_t *new_unmapped_ids = malloc(sizeof(uint32_t) * 
                                            memory->unmapped_ids_capacity * 2);
	memory->unmapped_ids_capacity *= 2;
	size_t i;	
	for (i = 0; i < memory->unmapped_ids_size; i++) {
		new_unmapped_ids[i] = unmapped_ids[i];
	}
	free(memory->unmapped_ids);
	memory->unmapped_ids = new_unmapped_ids;
}

segment_id map_segment(size_t length, memory_data memory)
{      
        segment_id return_id;
	/*
         * If there is a previously unmapped id in the stack, 
         * pop it and use as the id for the segment being mapped.
         * Otherwise, use the max unused id.
         */
        if (memory->unmapped_ids_size != 0) {
                return_id = memory->unmapped_ids[memory->unmapped_ids_size - 1];
		memory->unmapped_ids_size--; 
        }
        else {		
                return_id = memory->segments_size;
		//if (return_id == 0)
		//	assert(Table_length(memory->segment_ids) == 0);
                memory->segments_size++;
		if (memory->segments_size == memory->segments_capacity)
			expand_segments(memory);
        }

        word *new_data = malloc(sizeof(word) * length);
        assert(new_data != NULL);
        memory->segments[return_id].data = new_data;
        memory->segments[return_id].length = length;

        size_t i;
        for (i = 0; i < length; i++)
                memory->segments[return_id].data[i] = 0;

        return return_id;
}

void unmap_segment(segment_id id, memory_data memory)
{
	/* Remove segment from segments array and free associated memory */     
        free(memory->segments[id].data);
        memory->unmapped_ids[memory->unmapped_ids_size] = id; 
	memory->unmapped_ids_size++;
	if (memory->unmapped_ids_size == memory->unmapped_ids_capacity)
		expand_unmapped_ids(memory);
}


void write_word(segment_id id, size_t offset, word data, memory_data memory)
{
        memory->segments[id].data[offset] = data;
}

void load_program(segment_id id, memory_data memory)
{
	/* Change segment 0's length to that of the duplicated segment */
	memory->segments[0].length = memory->segments[id].length;

	/* Create a new array of words */
	word *new_data = malloc(sizeof(word) * memory->segments[0].length);
        assert(new_data != NULL);

	/* Overwrite segment 0's data with the new array of words and fill it 
         * with words from the segment being duplicated. 
         */
        free(memory->segments[0].data);
        memory->segments[0].data = new_data;
        size_t i;
        for (i = 0; i < memory->segments[0].length; i++) {
                memory->segments[0].data[i] = memory->segments[id].data[i];
        }
}

const size_t BYTES_PER_WORD = sizeof(word) / sizeof(char); 

/*************************************************************************
 * Name: fill_segment_zero                                               *
 * Purpose: Fills segment 0 with                                         *
 * Arguments: Segmented memory struct, const  char pointer to input file,* 
 *            length in bytes of input file.                             *
 * Returns: Nothing                                                      *
 *************************************************************************/
static inline int64_t sra(uint64_t word, unsigned bits)
{
        assert(bits <= 64);
        if (bits == 64)
                bits = 63; /* will get all copies of sign bit, 
                            * which is correct for 64
                            */
        return ((int64_t) word) >> bits; 
}

static inline uint64_t shl(uint64_t word, unsigned bits)
{
        assert(bits <= 64);
        if (bits == 64)
                return 0;
        else
                return word << bits;
}

/*
 * shift R logical
 */
static inline uint64_t shr(uint64_t word, unsigned bits)
{
        assert(bits <= 64);
        if (bits == 64)
                return 0;
        else
                return word >> bits;
}

bool Bitpack_fitss( int64_t n, unsigned width)
{
        if (width >= 64)
                return true;
        int64_t narrow = sra(shl(n, 64-width), 64 - width); 
        return narrow == n;
}

bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        if (width >= 64)
                return true;
        /* thanks to Jai Karve and John Bryan */
        return shr(n, width) == 0; // clever shortcut instead of 2 shifts
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        unsigned hi = lsb + width; /* one beyond the most significant bit */
        assert(hi <= 64);
        /* different type of right shift */
        return shr(shl(word, 64 - hi), 64 - width); 
}

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value)
{
        unsigned hi = lsb + width; /* one beyond the most significant bit */
        assert(hi <= 64);
	assert(Bitpack_fitsu(value, width));
        
        return shl(shr(word, hi), hi)                 /* high part */
                | shr(shl(word, 64 - lsb), 64 - lsb)  /* low part  */
                | (value << lsb);                     /* new part  */
}

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                      int64_t value)
{
        return Bitpack_newu(word, width, lsb, Bitpack_getu(value, width, 0));
}

void static inline fill_segment_zero(memory_data memory, const char *fp, 
                                     size_t len) 
{
	FILE *file = fopen(fp, "r"); 
	segment_id id_zero = 0; 

        size_t offset = 0;
	char temp = fgetc(file);
	size_t byte_count = 0;
	word temp_word = 0;
	int byte_location = 3;
	size_t char_size = 8 * sizeof(char);

	/* Writes four bytes from the file into a word, then puts the word 
         * in the next empty spot in segment 0. Continues until the entire file
         * has been read, packed into words, and put in segment 0. 
         */
	while (byte_count < (len * 4)) {
		size_t lsb = char_size * byte_location;
		temp_word = Bitpack_news(temp_word, char_size,
					 lsb, temp);
		if (byte_count % BYTES_PER_WORD == 3) {
			write_word(id_zero, offset, temp_word, memory);
                        offset++;
                }

		byte_count++;
		if (byte_location == 0) byte_location = 3;
		else byte_location--;
		temp = getc(file);
        }
}

memory_data boot_up_memory(const char *fp)
{
        struct stat file_info;
        memory_data memory = initialize_memory();

        if (stat(fp, &file_info) == -1) {
                fprintf(stderr, "Problem with file\n");
                exit(1);
        }

        off_t size = file_info.st_size / sizeof(word);
	/* 
         * This is the very first segment being mapped when the um starts
         * meaning it will always have an id of 0.
         */
        map_segment(size, memory);

        fill_segment_zero(memory, fp, size);
  
        return memory;
}


/*Initializes all the registers to hold 0*/
void initialize_registers(word registers[])
{
        int i;
        for (i = 0; i < 8; i++)
                registers[i] = 0;
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
                case (0):
                        cond_move(op_code, registers);
                        break;
                case (1):
                        segm_load(op_code, registers, memory);
                        break;
                case (2):
                        segm_store(op_code, registers, memory);
                        break;
                case (3):
                        add(op_code, registers);
                        break;
                case (4):
                        mult(op_code, registers);
                        break;
                case (5):
                        divide(op_code, registers);
                        break;
                case (6):
                        nand(op_code, registers);
                        break;
                case (7):
                        break;
                case (8):
                        map_segm(op_code, registers, memory);
                        break;
                case (9):
                        unmap_segm(op_code, registers, memory);
                        break;
                case (10):
                        output(op_code, registers);
                        break;
                case (11):
                        input(op_code, registers);
                        break;
                case (12):
                        return load_prog(op_code, registers, memory);
                        break;
                case (13):
                        load_val(op_code, registers);
                        break;
        }

        return program_counter;
}

void run(const char *fp)
{
        memory_data memory = boot_up_memory(fp); 

        word registers[8];
        initialize_registers(registers);
	
	/*Starts program counter at 0, gets and parses the first operator.*/
        size_t program_counter = 0;
        
	/* Loops through segment 0, getting, parsing, and executing each 
	 * instruction until HALT is reached. 
         */
     int op_code = 0;
     int a = 0;
     int b = 0;
     int c = 0;
     word value = 0;
	do {
		instruction instr = get_word(0, program_counter, memory);

		/*one beyond the most significant bit*/
		unsigned hi = OP_LSB + 4; 
		assert(hi <= 64);
		op_code = shr(shl(instr, 64 - hi), 60);
		// int a;
		// int b;
		// int c;
		if (op_code == 13) {
		        value = shr(shl(instr, 64 - (VAL_LSB + VAL_WIDTH)),
                                    64 - VAL_WIDTH);
		        a = shr(shl(instr, 64 - (REG_A_OP_13 + 3)), 61);
		}
		else {
		        a = shr(shl(instr, 64 - (REG_A_LSB + 3)), 61);
		        b = shr(shl(instr, 64 - (REG_B_LSB + 3)), 61);
		        c = shr(shl(instr, 64 - (REG_C_LSB + 3)), 61);
		}

        operation_info parsed_op_info;
        parsed_op_info.operation = op_code;
        parsed_op_info.A = a;
        parsed_op_info.B = b;
        parsed_op_info.C = c;
        parsed_op_info.value = value;


		program_counter = execute(program_counter, parsed_op_info, 
                                          registers, memory);
		program_counter++;
	} while (op_code != 7);

        free_memory(memory);
}


int main(int argc, char **argv)
{
	if(argc != 2) {
		fprintf(stderr, "Too many arguments given.\n");
		return EXIT_FAILURE; 
	}
        run(argv[1]);
        return EXIT_SUCCESS;
}


