/* 
 * Graham Goudeau & Deanna Bessy  
 * Homework #6, UM
 * on_disk.h
 * 1/4/15
 */

#ifndef ON_DISK_H
#define ON_DISK_H

#include "segment_memory.h"

/***********************************************************************
 * Name: boot_up_memory                                                *
 * Purpose: Initializes memory and fills segment zero with the program.*
 * Arguments: const char pointer to the file with program binary       *
 * Returns: Segmented memory struct                                    *
 ***********************************************************************/
memory_data boot_up_memory(const char *fp);

#endif
