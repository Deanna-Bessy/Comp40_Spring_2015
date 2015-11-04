/* 
 * compress40.c
 * Deanna Bessy and Josh Berl, 2-26-15
 * HW4 - arith  
 *
 * Function caller for compression or decompression
 * Allocates arrays to store data
 * Frees memory after finished
 */

#include "compress40.h"
#include "compressMethods.h"
#include "a2blocked.h"
#include "a2methods.h"
#include "stdlib.h"
#include "assert.h"
#include "mem.h"
#include "compressiontypes.h"
#include "decompressMethods.h"

const int BLKSZE = 2; 
const int DENOMINATOR = 255;
/* compress40
 *      parameters: FILE *input (a ppm image)
 *      returns: nothing
 *      effect: reads ppm from input, trims height and width to be even
 *              allocates arrays and calls mapping functions to fill them
 *              and change them into values that can be used to write code
 *              words
 *              frees all memory used
 *      invariants: inputted file is not NULL, input is a ppm
 */
extern void compress40(FILE *input)
{

        A2Methods_T methods = uarray2_methods_blocked; 
        Pnm_ppm image = Pnm_ppmread(input, methods); 
        image->methods = methods;
        if (image->width % BLKSZE != 0) {
                image->width = image->width - 1;
        }
        if (image->height % BLKSZE != 0) {
                image->height = image->height - 1;
        }
        A2Methods_UArray2 all_float_pix =
                methods->new_with_blocksize(image->width, image->height,
                                            sizeof(struct float_pixel), BLKSZE);
        methods->map_block_major(all_float_pix, ints_to_float, &image->pixels); 
        methods->map_block_major(all_float_pix, scale_floats, 
                                 &image->denominator);
        A2Methods_UArray2 vidcomp_pixels = 
                methods->new_with_blocksize(image->width, image->height,
                                            sizeof(struct vid_pixel), BLKSZE);
        methods->map_block_major(vidcomp_pixels, float_to_vid, &all_float_pix);
        block_data avgVidCl = malloc(sizeof(struct block_data));
        avgVidCl->avg_pixs = UArray_new(((image->width * image->height) / 
                                         (BLKSZE * BLKSZE)),
                                        sizeof(struct vpixels));
        avgVidCl->counter = 0;
        methods->map_block_major(vidcomp_pixels, vid_to_4vid, avgVidCl);
        UArray_T block_avgs = UArray_new(((image->width * image->height) / 
                                          (BLKSZE * BLKSZE)), 
                                         sizeof(struct compblock));
        
        fvid_to_avgvid(block_avgs, avgVidCl->avg_pixs);
        codewords(block_avgs, image->width, image->height); 
        Pnm_ppmfree(&image);
        methods->free(&all_float_pix);
        methods->free(&vidcomp_pixels);
        UArray_free(&block_avgs);
        UArray_free(&(avgVidCl->avg_pixs));
        FREE(avgVidCl);
}

/* decompress40 
 *      parameters: FILE *input (characters that make up codewords with width, 
                                 height, and scannable header)
 *      returns: nothing
 *      effects: reads codewords from input, allocates arrays and calls mapping
                 functions to fill them, modifies data until it can be used to
                 write a ppm
                 frees all allocated memory after writing ppm
 *      invariants: inputted file has the correct header, input is not NULL
 */
extern void decompress40(FILE *input)
{
        A2Methods_T methods = uarray2_methods_blocked; 
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u\n", 
                          &width, &height);
        assert(read == 2);
        A2Methods_UArray2 array = 
                methods->new_with_blocksize(width, height,
                                            sizeof(struct Pnm_rgb), BLKSZE);
        struct Pnm_ppm pixmap = { .width = width, .height = height,
                                  .denominator = DENOMINATOR, .pixels = array,
                                  .methods = methods };
        block_data blockSplit_data = malloc(sizeof(struct block_data));
        blockSplit_data->avg_pixs = UArray_new(((width * height) / 
                                                (BLKSZE * BLKSZE)), 
                                               sizeof(struct vpixels));
        blockSplit_data->counter = 0;
        UArray_T block_avgs = UArray_new(((width * height) / 
                                          (BLKSZE * BLKSZE)), 
                                         sizeof(struct compblock));
        read_codewords(block_avgs, input);
        avgvid_to_fvid(block_avgs, blockSplit_data->avg_pixs);
        A2Methods_UArray2 vc_pixels = 
                methods->new_with_blocksize(width, height,
                                            sizeof(struct vid_pixel), BLKSZE);
        methods->map_block_major(vc_pixels, splitUpVidPix, blockSplit_data);    
        A2Methods_UArray2 float_pixels = 
                methods->new_with_blocksize(width, height,
                                            sizeof(struct float_pixel), BLKSZE);
        methods->map_block_major(float_pixels, vid_to_float, &vc_pixels);
        int denomptr = DENOMINATOR;
        methods->map_block_major(float_pixels, unscale_floats, &denomptr); 
        A2Methods_UArray2 int_pix = 
                methods->new_with_blocksize(width, height,
                                            sizeof(struct Pnm_rgb), BLKSZE);
        methods->map_block_major(int_pix, floats_to_int, &float_pixels); 
        pixmap.pixels = int_pix;
        Pnm_ppmwrite(stdout, &pixmap); 

        free_vidpixels(blockSplit_data->avg_pixs);
        UArray_free(&(blockSplit_data->avg_pixs));
        FREE(blockSplit_data);
        UArray_free(&block_avgs);
        methods->free(&int_pix);
        methods->free(&vc_pixels);
        methods->free(&float_pixels);
        methods->free(&array);
}
