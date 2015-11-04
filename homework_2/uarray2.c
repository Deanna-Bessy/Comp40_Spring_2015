/*
 * Margaret Chapman and Deanna Bessy
 * February 3, 2015
 * my_uarray2.c
 *
 * The general purpose of uarray2 is to create and manipulate an abstraction for
 * a 2D unboxed array. A Uarray2_T is a Uarray_T used to represent the 2D array.
 *
 */

#include <uarray2.h>
#include <uarray.h>
#include <mem.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct UArray2_T {
        int rows; 
        int cols; 
        UArray_T uarray;
};

extern UArray2_T UArray2_new(int col, int row, int size) 
{     
        UArray2_T newUarray = malloc(sizeof(*newUarray));  
        assert(newUarray != NULL);   
        newUarray -> uarray = UArray_new(col * row, size); 

        newUarray -> rows = row; 
        newUarray -> cols = col;
        return newUarray;  
}

extern void UArray2_free(UArray2_T *uarray2) 
{
        UArray_free( &( (*uarray2) -> uarray ) ); 
        FREE(*uarray2); 
}

extern int UArray2_height(UArray2_T uarray2) 
{
        return uarray2 -> rows; 
}

extern int UArray2_width(UArray2_T uarray2)
{
        return uarray2 -> cols; 
}
 
extern int UArray2_size(UArray2_T uarray2) 
{
        return UArray_size(uarray2 -> uarray); 
}

void *UArray2_at(UArray2_T uarray2, int col, int row)
{

    return UArray_at((uarray2 -> uarray), row * (uarray2 -> cols) + col);
}
 
extern void UArray2_map_col_major(UArray2_T uarray2, void apply(int col, 
                                  int row, UArray2_T uarray2, 
                                  void *member, void *cl ), void *cl)
{
        for(int c = 0; c < uarray2 -> cols; c++) {
                for (int r = 0; r < uarray2 -> rows; r++) {
                    apply(c, r, uarray2, UArray2_at(uarray2, c, r), cl);
                }
        }
}

extern void UArray2_map_row_major(UArray2_T uarray2, void apply( int col, 
                                  int row, UArray2_T uarray2, 
                                  void *member, void *cl), void *cl)
{
        for(int r = 0; r < uarray2 -> rows; r++) {
                for (int c = 0; c < uarray2 -> cols; c++) {
                    apply(c, r, uarray2, UArray2_at(uarray2, c, r), cl);
                }
        }
}


