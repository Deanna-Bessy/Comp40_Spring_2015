/* 
 * Graham Goudeau & Deanna Bessy  
 * Homework #6, UM
 * main.c
 * 1/5/15
 */
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "um.h"


/* Checks that only one argument was given to the um and then runs the um. */
int main(int argc, char **argv)
{
	if(argc != 2) {
		fprintf(stderr, "Too many arguments given.\n");
		return EXIT_FAILURE; 
	}
        run(argv[1]);
        return EXIT_SUCCESS;
}
