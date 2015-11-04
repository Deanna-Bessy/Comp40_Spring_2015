/* 
 * Graham Goudeau & Deanna Bessy  
 * Homework #6, UM
 * um.h
 * 1/5/15
 */
#ifndef UM_H
#define UM_H

#include "segment_memory.h"

/**************************************************************************
 * Name: run                                                              *
 * Purpose: Runs the UM. Boots the memory, initializes registers, and then*
 *                       executes the instructions of the given file.     *
 * Arguments: const char pointer to the input file                        *
 * Returns: Nothing                                                       *
 **************************************************************************/
void run(const char *fp);

#endif
