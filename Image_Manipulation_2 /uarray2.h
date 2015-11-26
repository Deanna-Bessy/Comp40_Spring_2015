/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  !! This is a hw from the class comp40 at Tufts! If you're in comp40 right now,!!
  !! stop right there. You should not be viewing this code. Course policy and   !! 
  !! academic integrity makes that a big no-no. Bugs have intentionally not been!! 
  !! fixed to discourage plagarism                                              !!
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 


/*
 * Margaret Chapman and Deanna Bessy
 * February 3, 2015
 * uarray2.h
 *
 * The general purpose of uarray2 is to create and manipulate an abstraction for
 * a 2D unboxed array. A UArray2_T is a Uarray_T used to represent the 2D array.
 *
 */
#ifndef UARRAY2_INCLUDED
#define UARRAY2_INDLUDED

#define T UArray2_T
typedef struct T *T; 
/*
 * Creates a new 2D array, dimensions col by row and each element having a size 
 * of size.
 */
extern UArray2_T UArray2_new(int col, int row, int size);

/* Frees memory of 2D array. */
extern void UArray2_free(UArray2_T *uarray2);

/* Returns number of rows in 2D array */ 
extern int UArray2_height(UArray2_T uarray2);

/* Returns number of cols in 2D array */
extern int UArray2_width(UArray2_T uarray2);
 
/* Returns the size of each element in the 2D array */ 
extern int UArray2_size(UArray2_T uarray2);

/*
* Returns a void pointer to the element at the given row and col. If out of
* bounds, gives a checked runtime error.  
*/
void *UArray2_at(UArray2_T uarray2, int col, int row);

/* 
 * Walks through every element of the array, with row indeces changing more 
 * rapidly than col. Calls the apply function on each element. 
*/ 
extern void UArray2_map_col_major(UArray2_T uarray2, void apply( int col, 
                                  int row, UArray2_T uarray2, void *c1, 
                                  void *c2 ), void *c2);
/* 
 * Walks through every element of the array, with col indeces changing more 
 * rapidly than row. Calls the apply function on each element. 
 */ 
extern void UArray2_map_row_major(UArray2_T uarray2, void apply( int col, 
                                  int row, UArray2_T uarray2, 
                                  void *c1, void *c2 ), void *c2);

#undef T
#endif
