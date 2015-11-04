/*
 * Deanna Bessy and Margaret Chapman
 * February 3, 2015
 * bit2.h 
 *
 * The general purpose of bit2 is to create and manipulate an abstraction for 
 * a 2D bitmap. A Bit2_T is a Bit_T used to represent the 2D bitmap. 
 *
 */

#ifndef BIT2_INCLUDED
#define BIT2_INCLUDED

#define T Bit2_T
typedef struct T *T; 

/* 
 * Creates a 2D bitmap and sets all values to 0. Bit2_new can raise Mem_failed.
 * It is a checked runtime error to pass negative values for col or row.
 */
extern Bit2_T Bit2_new(int col, int row);

/* 
 * Returns the number of cols in the bitmap. It is a checked runtime error to 
 * pass a NULL value for bit2.
 */
extern int Bit2_width(Bit2_T bit2);

/* 
 * Returns the number of rows in the bitmap. It is a checked runtime error to 
 * pass a NULL value for bit2.
 */ 
extern int Bit2_height(Bit2_T bit2); 

/* 
 * Returns the number of 1s in the bitmap. It is a checked runtime error to 
 * pass a NULL value for bit2.
 */
extern int Bit2_count(Bit2_T bit2);

/* 
 * Frees memory associated with bitmap. It is a checked runtime error to 
 * pass a NULL value for bit2.
 */
extern void Bit2_free(Bit2_T *bit2);

/* Returns the bit at specified col and row. It is a checked runtime error to 
 * pass a negative value for col or row.
 */
extern int Bit2_get(Bit2_T bit2, int col, int row); 

/* 
 * Sets the bit at specified col and row to specified value (either 0 or 1).  
 * Returns previous value of specified bit.
 */
extern int Bit2_put(Bit2_T bit2, int col, int row, int bit);

/* 
 * Calls apply on each bit in the bitmap with row indeces changing more rapidly
 * than col indeces. 
 */ 
extern void Bit2_map_col_major(Bit2_T bit2, void apply(int col, int row,
                               Bit2_T bit2, int bit, void *cl), void *cl);

/* 
 * Calls apply on each bit in the bitmap with col indeces changing more rapidly
 * than row indeces. 
 */
extern void Bit2_map_row_major(Bit2_T bit2, void apply(int col, int row,
                               Bit2_T bit2, int bit, void *cl), void *cl);

#undef T
#endif