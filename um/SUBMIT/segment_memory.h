/* 
 * Graham Goudeau & Deanna Bessy  
 * Homework #6, UM
 * segment_memory.h
 * 1/4/15
 */
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef uint32_t segment_id;
typedef uint32_t word;
typedef struct memory_data *memory_data;

/************************************************************************* 
 * Name: initialize_memory                                               *
 * Purpose: Allocates memory for the segmented memory struct and creates *
 *          the data structures within the struct.                       *
 * Arguments: None                                                       *
 * Returns: new, "blank" memory_data struct                              *
 *************************************************************************/
memory_data initialize_memory(void);

/******************************************************************
 * Name: get_operator                                             *
 * Purpose: Gets a single operator from the program code.         *
 * Arguments: The program counter and the segmented memory struct.*
 * Returns: The correct instruction based on the program counter. *
 ******************************************************************/
size_t get_operator(size_t program_counter, memory_data memory);

/*************************************************************************
 * Name: free_memory                                                     *
 * Purpose: Frees all memory associated with the segmented memory struct.*
 * Arguments: The segmented memory struct.                               *
 * Returns: nothing                                                      *
 *************************************************************************/
void free_memory(memory_data memory);

/*********************************************************************** 
 * Name: map_segment                                                   *
 * Purpose: Given the desired length of a new segment and the segmented*
 *          memory struct, maps a new segment.                         *
 * Arguments: Desired length of new segment and segmented memory struct*
 * Returns: The id of the new segment                                  *
 ***********************************************************************/
segment_id map_segment(size_t length, memory_data memory);

/*************************************************************************
 * Name: unmap_segment                                                   *
 * Purpose: Given a segment id and the segmented memory struct, frees the*
 *          segment with that id.                                        *
 * Arguments: Segment id and segmented memory struct                     *
 * Returns: Nothing                                                      *
 *************************************************************************/
void unmap_segment(segment_id id, memory_data memory);

/**************************************************************************** 
 * Name: get_word                                                           *
 * Purpose: Extracts a single word from the segment with the given id at the*
 *          given offset.                                                   *
 * Arguments: Segment id, offset, segmented memory struct.                  *
 * Returns: The extracted word.                                             *
 ****************************************************************************/
word get_word(segment_id id, size_t offset, memory_data memory);

/***************************************************************************** 
 * Name: write_word                                                          *
 * Purpose: Puts the given word in the segment with the given id at the given* 
 *          offset.                                                          *
 * Arguments: Segment id, offset, word, segmented memory struct.             *
 * Returns: Nothing.                                                         *
 *****************************************************************************/
void write_word(segment_id id, size_t offset, word data, memory_data memory);

/*****************************************************************************
 * Name: load_program                                                        *  
 * Purpose: Duplicates the segment with the given id and overwrites segment 0*
 *          with that segment                                                *
 * Arguments: Segment id, segmented memory struct.                           *
 * Returns: Nothing                                                          *
 *****************************************************************************/
void load_program(segment_id id, memory_data memory);

#endif
