/* 
 * Graham Goudeau & Deanna Bessy  
 * Homework #6, UM
 * segment_memory.c
 * 1/4/15
 */

#include <stdlib.h>
#include <table.h>
#include <stack.h>
#include <assert.h>
#include <atom.h>
#include "segment_memory.h"

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
        new_memory->segments = malloc(sizeof(segment) * INIT_SEGMENTS_CAPACITY);
        new_memory->unmapped_ids = malloc(sizeof(segment_id) * INIT_UNMAPPED_CAPACITY);
	new_memory->segments_capacity = INIT_SEGMENTS_CAPACITY;
	new_memory->unmapped_ids_capacity = INIT_UNMAPPED_CAPACITY;
        new_memory->segments_size = 0;
        new_memory->unmapped_ids_size = 0;

        return new_memory;
}

size_t get_operator(size_t program_counter, memory_data memory)
{
        return get_word(0, program_counter, memory);
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

word get_word(segment_id id, size_t offset, memory_data memory)
{
        return memory->segments[id].data[offset];
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

void expand_segments(memory_data memory)
{
	segment *segments = memory->segments;
	segment *new_segments = malloc(sizeof(segment) * memory->segments_capacity * 2);
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
	uint32_t *new_unmapped_ids = malloc(sizeof(uint32_t) * memory->unmapped_ids_capacity * 2);
	memory->unmapped_ids_capacity *= 2;
	size_t i;	
	for (i = 0; i < memory->unmapped_ids_size; i++) {
		new_unmapped_ids[i] = unmapped_ids[i];
	}
	free(memory->unmapped_ids);
	memory->unmapped_ids = new_unmapped_ids;
}

