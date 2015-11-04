/*
 * compressMethods.c
 * Deanna Bessy and Josh Berl 2-26-15
 * HW4 - arith
 * 
 * Implementation of functions defined in compressMethods.h
 */

#include "a2blocked.h"
#include "uarray2b.h"
#include "compressMethods.h"
#include "pnm.h"
#include "a2plain.h"
#include <stdio.h>
#include "mem.h"
#include <stdlib.h>
#include "arith40.h"
#include "bitpack.h"
#include "assert.h"
#include "compressiontypes.h"
#include "calcfunctions.h"


const int PIXINBLOCK = 4;
const int NUMBITSBYTE = 8;

/* ints_to_floats
 *      parameters: i and j are the indeces in img
 *          elem (a single pixel in img)
 *          void *fimage (empty blocked array to be filled with rgb float
                          value pixels)
 *      returns: Nothing
 *      effects: Fills float_pixels with rgb pixels in which the color values 
                 are floats.
 *
 *      invariants: Number of pixels stays the same.
 */
extern void ints_to_float(int i, int j, A2Methods_UArray2 img, void *elem,
                          void *fimage)
{
        A2Methods_T methods = uarray2_methods_blocked; 
        struct float_pixel *new_pix = (struct float_pixel *)(elem);
        A2Methods_UArray2 sourceimage = *(A2Methods_UArray2 *)fimage;
        struct Pnm_rgb *source = 
                (struct Pnm_rgb *)(methods->at(sourceimage, i, j));
        new_pix->red = (float)(source->red);
        new_pix->green = (float)(source->green);
        new_pix->blue = (float)(source->blue);
        (void) img;
}

/* scale_floats
 *      parameters: i and j are the indeces in all_floats
 *                  elem (a single pixel in all_floats, which is empty)
 *                  void *denom (a void pointer to the int denominator )
 *      returns: Nothing
 *      effects: Scales the float red, green, and blue values by the 
 *               denominator.
 *
 *      invariants: Number of pixels stays the same
 */
extern void scale_floats(int i, int j, A2Methods_UArray2 all_floats, 
                         void *elem, void *denom)
{
        struct float_pixel *new_pix = (struct float_pixel *)(elem);
        int denominator = *(int *)(denom);
        new_pix->red = new_pix->red / (float)denominator;
        if (new_pix->red > 1) {
                new_pix->red = 1;
        }
        if (new_pix->red < 0) {
                new_pix->red = 0;
        }
        new_pix->green = new_pix->green / (float)denominator;
        if (new_pix->green > 1) {
                new_pix->green = 1;
        }
        if (new_pix->green < 0) {
                new_pix->green = 0;
        }
        new_pix->blue = new_pix->blue / (float)denominator;
        if (new_pix->blue > 1) {
                new_pix->blue = 1;
        }
        if (new_pix->blue < 0) {
                new_pix->blue = 0;
        }
        (void) i;
        (void) j;
        (void) all_floats;
}


/* float_to_vid
 *      parameters: i and j are the indeces in the vid_array
 *                  A2Methods_UArray2 vid_array (empty blocked array 
 *                                               to be filled with
 *                                               component video formatted 
 *                                               pixels).
 *                  elem (a single pixel in vid_array, which is being filled)
 *                  float_array (a void pointer to the blocked array contianing 
 *                        rgb float value pixels)
 *      returns: Nothing
 *      effects: Fills vid_array with video component pixels calculated from 
 *               rgb float pixels. This is an apply function used with 
 *               block major mapping. 
 *
 *      invariants: Number of pixels stays the same
 */
extern void float_to_vid(int i, int j, A2Methods_UArray2 vid_array, 
                         void *elem, void *float_array)
{
        A2Methods_T methods = uarray2_methods_blocked; 
        struct vid_pixel *new_pix = (struct vid_pixel *)(elem);
        A2Methods_UArray2 float_pixels = *(A2Methods_UArray2 *)float_array;
        struct float_pixel *fpix =
                (struct float_pixel *)(methods->at(float_pixels, i, j));
        new_pix->Y = 0.299 * fpix->red + 0.587 * fpix->green 
                     + 0.114 * fpix->blue;
        new_pix->Pb = -0.168736 * fpix->red - 0.331264 * fpix->green 
                      + 0.5 * fpix->blue;
        new_pix->Pr = 0.5 * fpix->red - 0.418688 * fpix->green 
                      - 0.081312 * fpix->blue;
        (void) vid_array;
}    


/* vid_to_4vid
 *      parameters: int i & j (index of the current vid_array element)
 *                  A2Methods_Uarray2 vid_array (empty 2D array to be filled 
                                                 with vid component pixs)
 *                  void *elem (a void pointer to an element of the vid array)
 *                  void *map_data (a void pointer to a struct containing a 
 *                                   counter and a blocked array of individual
 *                                   video component pixels). *                  
 *      returns: Nothing
 *      effects: Puts 4 seprate pixels together into a struct containing the four.
                 This is an apply function used with blocked mapping. 
 *
 *      invariants: Pixels are the same, but are now grouped together in a struct.
 */
extern void vid_to_4vid(int i, int j, A2Methods_UArray2 vid_array, void *elem, 
                        void *map_data)
{
        block_data arraycounter = (struct block_data *)(map_data);
        struct vpixels *pix_block = 
        (struct vpixels *)(UArray_at(arraycounter->avg_pixs, 
                                     arraycounter->counter / 4));
        if (arraycounter->counter % PIXINBLOCK == 0) {
                pix_block->v1 = (struct vid_pixel *)(elem);
        } else if (arraycounter->counter % PIXINBLOCK == 1) {
                pix_block->v2 = (struct vid_pixel *)(elem);
        } else if (arraycounter->counter % PIXINBLOCK == 2) {
                pix_block->v3 = (struct vid_pixel *)(elem);
        } else if (arraycounter->counter % PIXINBLOCK == 3) {
                pix_block->v4 = (struct vid_pixel *)(elem);
        }
        arraycounter->counter++;
        (void) i;
        (void) j;
        (void) vid_array;
}

/* fvid_to_avgvid
 *      parameters: UArray_T block_avgs (average of four blocks)
 *                  UArray_T four_pixs (A 1d array of structs that
                                        contain 4 video component 
                                        pixels each)
 *      returns: Nothing
 *      effects: Averages four_pixs' Pr, Pb,and Y values into average 
                 Pb and Pr values and calculates a, b, c, and d. 
 *
 *      invariants: fblock_avgs populated when function ends
 */
extern void fvid_to_avgvid(UArray_T block_avgs, UArray_T four_pixs)
{
        for (int i = 0; i < UArray_length(four_pixs); i++) { 
                struct compblock *avg = 
                        (struct compblock *)(UArray_at(block_avgs, i));
                struct vpixels *pix_block = 
                        (struct vpixels *)(UArray_at(four_pixs, i));

                float pb_float = 
                        (((pix_block->v1->Pb) + (pix_block->v2->Pb) +
                        (pix_block->v3->Pb) + (pix_block->v4->Pb)) 
                         / PIXINBLOCK); 
                avg->pbavg = Arith40_index_of_chroma(pb_float);

                float pr_float = 
                        (((pix_block->v1->Pr) + (pix_block->v2->Pr) +
                        (pix_block->v3->Pr) + (pix_block->v4->Pr)) 
                         / PIXINBLOCK);
                avg->pravg = Arith40_index_of_chroma(pr_float);
                 
                avg->a = round_float(511 * (((pix_block->v1->Y) + 
                                             (pix_block->v2->Y) +
                                             (pix_block->v3->Y) + 
                                             (pix_block->v4->Y)) / PIXINBLOCK));
                avg->b =
                        bcd_calc(((pix_block->v4->Y) + (pix_block->v3->Y) -
                                 (pix_block->v2->Y) - (pix_block->v1->Y)));     
                avg->c =
                        bcd_calc(((pix_block->v4->Y) - (pix_block->v3->Y) +
                                 (pix_block->v2->Y) - (pix_block->v1->Y)));
                avg->d =
                        bcd_calc(((pix_block->v4->Y) - (pix_block->v3->Y) -
                                 (pix_block->v2->Y) + (pix_block->v1->Y)));
        } 
}


/* codewords
 *      parameters: UArray_T block_avgs (average of four blocks)
 *                  int width (width of the image)
                    int height (height of the image)
 *      returns: Nothing
 *      effects: Gets values from block_avgs and puts them into 
                 codewords.

 *      invariants: block_avgs isn't empty
 */
extern void codewords(UArray_T block_avgs, int width, int height)
{
        printf("COMP40 Compressed image format 2\n%u %u\n", width, height);
        uint64_t codeword = 0;
        for (int i = 0; i < UArray_length(block_avgs); i++) {
                struct compblock *block = 
                        (struct compblock *)(UArray_at(block_avgs, i));
                codeword = Bitpack_newu(codeword, 4, 0,
                                        (uint64_t) block->pravg);
                codeword = Bitpack_newu(codeword, 4, 4,
                                        (uint64_t) block->pbavg);
                codeword = Bitpack_news(codeword, 5, 8,
                                        (int64_t) block->d);
                codeword = Bitpack_news(codeword, 5, 13,
                                        (int64_t) block->c);
                codeword = Bitpack_news(codeword, 5, 18,
                                        (int64_t) block->b);
                codeword = Bitpack_newu(codeword, 9, 23,
                                        (uint64_t) block->a);
                for (int i = PIXINBLOCK - 1; i >= 0; i--) {
                        putchar(Bitpack_getu(codeword, NUMBITSBYTE, 
                                             i*NUMBITSBYTE));
                }
        }
}



