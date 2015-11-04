/*
 * Deanna Bessy and Margaret Chapman
 * February 3, 2015
 * bit2.c 
 *
 * The general purpose of bit2 is to create and manipulate an abstraction for 
 * a 2D bitmap. A Bit2_T is a Bit_T used to represent the 2D bitmap. 
 *
 */

#include <bit2.h>
#include <bit.h>
#include <mem.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct Bit2_T {
        int rows; 
        int cols; 
        Bit_T bit;
};

extern Bit2_T Bit2_new(int col, int row)
{
        Bit2_T newBits = malloc(sizeof(*newBits));  
        assert(newBits != NULL);   
        newBits -> bit = Bit_new(col*row); 

        newBits -> rows = row; 
        newBits -> cols = col;
        return newBits; 
}

extern int Bit2_width(Bit2_T bit2)
{
        return bit2 -> cols;
}

extern int Bit2_height(Bit2_T bit2)
{
        return bit2 -> rows;
}

extern int Bit2_count(Bit2_T bit2)
{
        return Bit_count(bit2 -> bit);
}

extern void Bit2_free(Bit2_T *bit2)
{
        Bit_free( &( (*bit2) -> bit ) ); 
        FREE(*bit2);
}

extern int Bit2_get(Bit2_T bit2, int col, int row)
{
        return Bit_get(bit2 -> bit, (bit2 -> cols) * row + col); 
}

extern int Bit2_put(Bit2_T bit2, int col, int row, int bit)
{
        return Bit_put(bit2 -> bit, (bit2 -> cols) * row + col, bit);
}

extern void Bit2_map_col_major(Bit2_T bit2, void apply(int col, int row,
                               Bit2_T bit2, int bit, void *cl), void *cl)
{
        for(int c = 0; c < bit2 -> cols; c++) {
                for (int r = 0; r < bit2 -> rows; r++) {
                    apply(c, r, bit2, Bit2_get(bit2, c, r), cl);
                }
        }
}

extern void Bit2_map_row_major(Bit2_T bit2, void apply(int col, int row,
                               Bit2_T bit2, int bit, void *cl), void *cl)
{
        for(int r = 0; r < bit2 -> rows; r++) {
                for (int c = 0; c < bit2 -> cols; c++) {
                    apply(c, r, bit2, Bit2_get(bit2, c, r), cl);
                }
        }
}
