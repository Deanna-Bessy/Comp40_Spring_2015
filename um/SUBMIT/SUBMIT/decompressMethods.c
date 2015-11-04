/*
 * decompressMethods.c
 * Deanna Bessy and Josh Berl 2-26-15
 * HW4 - arith
 * 
 * Implementation of functions defined in decompressionMethods.h
 */

#include "a2blocked.h"
#include "mem.h"
#include <stdlib.h>
#include "arith40.h"
#include "bitpack.h"
#include "decompressMethods.h"
#include "compressiontypes.h"
#include "calcfunctions.h"

const int NUMBLOCKPIX = 4; 
const int BITSINBYTE = 8;
const int ASCALER = 511;
const unsigned DENOM = 255;

/* read_codewords
 *      parameters: UArray_T block_avgs (average of four blocks)
 *                  FILE input (A file that contains 32 bit codewords
                                producedby compression.)
 *      returns: Nothing
 *      effects: Gets values from codewords and puts them into 
                 block_avgs.

 *      invariants: input is not NULL and is correctly formatted
 */
extern void read_codewords(UArray_T block_avgs,  FILE *input)
{
        for (int i = 0; i < UArray_length(block_avgs); i++) {
                uint64_t codeword = 0;
                for (int j = NUMBLOCKPIX - 1; j >= 0; j--) {
                        codeword = (Bitpack_newu(codeword, BITSINBYTE, 
                                                 j*BITSINBYTE, getc(input)));
                }
                struct compblock *block = 
                (struct compblock *)(UArray_at(block_avgs, i));
                block->pravg = Bitpack_getu(codeword, 4, 0);
                block->pbavg = Bitpack_getu(codeword, 4, 4);
                block->a = Bitpack_getu(codeword, 9, 23);
                block->b = Bitpack_gets(codeword, 5, 18);
                block->c = Bitpack_gets(codeword, 5, 13);
                block->d = Bitpack_gets(codeword, 5, 8);
        }
}

/* avgvid_to_fvid
 *      parameters: UArray_T block_avgs (average of four blocks)
 *                  UArray_T four_pixs (A 1d array of structs that
 *                                       contain 4 video component 
 *                                       pixels each)
 *      returns: Nothing
 *      effects: Fills four_pixs' structs with Pr, Pb,and Y values 
 *               calculated from the average values.  
 *
 *      invariants: four_pixs populated when function ends
 */
extern void avgvid_to_fvid(UArray_T block_avgs, UArray_T four_pixs) 
{
        for (int i = 0; i < UArray_length(block_avgs); i++) {
                struct compblock *avg = 
                (struct compblock *)(UArray_at(block_avgs, i));
                struct vpixels *pix_block = 
                (struct vpixels *)(UArray_at(four_pixs, i));

                pix_block->v1 = malloc(sizeof(struct vid_pixel)); 
                pix_block->v2 = malloc(sizeof(struct vid_pixel)); 
                pix_block->v3 = malloc(sizeof(struct vid_pixel)); 
                pix_block->v4 = malloc(sizeof(struct vid_pixel)); 

                float pb = Arith40_chroma_of_index(avg->pbavg);
                float pr = Arith40_chroma_of_index(avg->pravg);
                float a = (((float) avg->a) / ASCALER);
                float b = bcd_scale(avg->b);
                float c = bcd_scale(avg->c);
                float d = bcd_scale(avg->d);

                pix_block->v1->Pb = pb;
                pix_block->v1->Pr = pr;
                pix_block->v1->Y = a - b - c + d;

                pix_block->v2->Pb = pb;
                pix_block->v2->Pr = pr;
                pix_block->v2->Y = a - b + c -d;

                pix_block->v3->Pb = pb;
                pix_block->v3->Pr = pr;
                pix_block->v3->Y = a + b - c - d;
                
                pix_block->v4->Pb = pb;
                pix_block->v4->Pr = pr;
                pix_block->v4->Y = a + b + c + d;
        }

}

/* splitUpVidPix
 *      parameters: int i & j (index of the current vid_array element)
 *                  A2Methods_Uarray2 vid_array (2D array of vid component pixs)
 *                  void *elem (a void pointer to an element of the vid array)
 *                  void *four_pixs (a void pointer to a struct containing a 
 *                                   counter and a blocked array of individual
 *                                   video component pixels). *                  
 *      returns: Nothing
 *      effects: Seperates the struct containing 4 pixels into 4 seprate pixels 
 *               in a blocked array. This is an apply function used with blocked 
 *               mapping. 
 *
 *      invariants: Pixels are the same, but split into a blocked array
 */
extern void splitUpVidPix(int i, int j, A2Methods_UArray2 vid_array, void *elem,
                          void *four_pixs)
{
        struct block_data *four_pixels = (struct block_data *)(four_pixs);
        struct vid_pixel *vpix = (struct vid_pixel *)(elem);
        int counter = four_pixels->counter;
        struct vpixels *pixels = 
                (struct vpixels *)(UArray_at(four_pixels->avg_pixs, 
                                             counter / NUMBLOCKPIX));
        if (counter % NUMBLOCKPIX == 0) {
                vpix->Y = pixels->v1->Y; 
                vpix->Pb = pixels->v1->Pb; 
                vpix->Pr = pixels->v1->Pr; 
        } else if (counter % NUMBLOCKPIX == 1) {
                vpix->Y = pixels->v2->Y;
                vpix->Pb = pixels->v2->Pb;
                vpix->Pr = pixels->v2->Pr;              
        } else if (counter % NUMBLOCKPIX == 2) {
                vpix->Y = pixels->v3->Y;
                vpix->Pb = pixels->v3->Pb;
                vpix->Pr = pixels->v3->Pr;
        } else if (counter % NUMBLOCKPIX == 3) {
                vpix->Y = pixels->v4->Y;
                vpix->Pb = pixels->v4->Pb;
                vpix->Pr = pixels->v4->Pr;
        }
        four_pixels->counter++;
        (void)i; 
        (void)j;
        (void)vid_array; 
}

/* vid_to_float
 *      parameters: i and j are the indeces in the float_array
 *                  A2Methods_UArray2 float_array (empty blocked array 
 *                                                 to be filled with
 *                                                 pixels contining rgb 
 *                                                 float format pixels. 
 *                                                 )
 *          elem (a single pixel in float_array, which is being filled)
 *          vid_array (a void pointer to the blocked array contianing 
 *                             video componenet pixels )
 *      returns: Nothing
 *      effects: Fills float_array with rgb float value pixels calculated from 
 *               video component pixels. This is an apply function used with 
 *               block major mapping. 
 *
 *      invariants: Number of pixels stays the same
 */
extern void vid_to_float(int i, int j, A2Methods_UArray2 float_array, 
                         void *elem, void *vid_array)
{
        A2Methods_T methods = uarray2_methods_blocked; 
        struct float_pixel *new_pix = (struct float_pixel *)(elem);
        A2Methods_UArray2 vid_pixels = *(A2Methods_UArray2 *)vid_array;
        struct vid_pixel *vpix =
                (struct vid_pixel *)(methods->at(vid_pixels, i, j));
        new_pix->red = 1.0 * vpix->Y + 0.0 * vpix->Pb + 1.402 * vpix->Pr;
        if (new_pix->red > 1) {
                new_pix->red = 1;
        }
        if (new_pix->red < 0) {
                new_pix->red = 0;
        }
        new_pix->green = 1.0 * vpix->Y - 0.344136 * vpix->Pb 
                         - 0.714136 * vpix->Pr;
        if (new_pix->green > 1) {
                new_pix->green = 1;
        }
        if (new_pix->green < 0) {
                new_pix->green = 0;
        }
        new_pix->blue = 1.0 * vpix->Y + 1.772 * vpix->Pb;
        if (new_pix->blue > 1) {
                new_pix->blue = 1;
        }
        if (new_pix->blue < 0) {
                new_pix->blue = 0;
        }
        (void) float_array;
}    

/* unscale_floats
 *      parameters: i and j are the indeces in the float_array
 *                  A2Methods_UArray2 float_array (blocked array filled with
                                                   rgb pixels, of which the 
                                                   individual color values 
                                                   are floats.)
 *          elem (a single pixel in float_array)
 *          void *denom (a void pointer to the int denominator )
 *      returns: Nothing
 *      effects: Unscales the float red, green, and blue values for the 
 *               denominator 255
 *
 *      invariants: Number of pixels stays the same
 */
extern void unscale_floats(int i, int j, A2Methods_UArray2 float_array, 
                           void *elem, void *denom)
{
        struct float_pixel *new_pix = (struct float_pixel *)(elem);
        int denominator = *(int *)(denom);
        new_pix->red = new_pix->red * (float)denominator;
        new_pix->green = new_pix->green * (float)denominator;
        new_pix->blue = new_pix->blue * (float)denominator;
        (void) i;
        (void) j;
        (void) float_array;
}


/* floats_to_ints
 *      parameters: i and j are the indeces in int_pixels
 *                  elem (a single pixel in int_pixels)
 *                  void *float_pixels (blocked array filled with rgb pixels,
                                        of which the individual color values 
                                        are floats.)
 *      returns: Nothing
 *      effects: Fills int_pixels with rgb pixels in which the color values 
                 are ints.
 *
 *      invariants: Number of pixels stays the same.
 */
extern void floats_to_int(int i, int j, A2Methods_UArray2 int_pixels, 
                          void *elem, void *float_pixels)
{
        A2Methods_T methods = uarray2_methods_blocked; 
        struct Pnm_rgb *new_pix = (struct Pnm_rgb *)(elem);
        A2Methods_UArray2 sourceimage = *(A2Methods_UArray2 *)float_pixels;
        struct float_pixel *source = 
                (struct float_pixel *)(methods->at(sourceimage, i, j));
        new_pix->red = round_float(source->red);
        if (new_pix->red > DENOM)
                new_pix->red = DENOM;
        new_pix->green = round_float(source->green);
        if (new_pix->green > DENOM)
                new_pix->green = DENOM;
        new_pix->blue = round_float(source->blue);        
        if (new_pix->blue > DENOM)
                new_pix->blue = DENOM;        
        (void) int_pixels;         
}


/* free_vidpixels
 *      parameters: UArray_T pixels, a UArray containing structs of 4 pixels 
 *      returns: Nothing
 *      effects: Walks through the array, freeing the pixels the structs 
 *               point to.
 */
extern void free_vidpixels(UArray_T vid_pixels) 
{
        for (int i = 0; i < UArray_length(vid_pixels); i++) {
                struct vpixels *vpix = (struct vpixels *)(UArray_at(vid_pixels,
                                                                    i));
                FREE(vpix->v1);
                FREE(vpix->v2);
                FREE(vpix->v3);
                FREE(vpix->v4);
        }
}
