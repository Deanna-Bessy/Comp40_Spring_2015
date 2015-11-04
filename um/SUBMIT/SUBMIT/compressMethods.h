/*
 * compressMethods.h
 * Deanna Bessy and Josh Berl 2-26-15
 * HW4 - arith
 * 
 * Declarations of functions that do parts of image compression
 */

#include "pnm.h"
#include "uarray.h"

extern void ints_to_float(int i, int j, A2Methods_UArray2 img, void *elem,
                          void *fimage); 
extern void scale_floats(int i, int j, A2Methods_UArray2 all_floats, void *elem,
                         void *denom);
extern void float_to_vid(int i, int j, A2Methods_UArray2 vid_array, void *elem,
                         void *float_array);
extern void vid_to_4vid(int i, int j, A2Methods_UArray2 vid_array, void *elem, 
                        void *map_data);
extern void fvid_to_avgvid(UArray_T block_avgs, UArray_T four_pixs); 
extern void codewords(UArray_T block_avgs, int width, int height);
