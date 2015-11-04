/*
 * decompressMethods.c
 * Deanna Bessy and Josh Berl 2-26-15
 * HW4 - arith
 * 
 * Header File for decompressMethods
 */

#include "pnm.h"
#include "uarray.h"


extern void read_codewords(UArray_T block_avgs, FILE *input);
extern void splitUpVidPix(int i, int j, A2Methods_UArray2 vid_array, 
                          void *elem, void *four_pixs);
extern void vid_to_float(int i, int j, A2Methods_UArray2 float_array, 
                         void *elem, void *vid_array);
extern void unscale_floats(int i, int j, A2Methods_UArray2 float_array, 
                           void *elem, void *denom);
extern void floats_to_int(int i, int j, A2Methods_UArray2 img, void *elem,
                          void *fimage);
extern void avgvid_to_fvid(UArray_T block_avgs, UArray_T four_pixs);
extern void free_vidpixels(UArray_T vid_pixels);
