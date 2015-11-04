/*
 * compressiontypes.h
 * Deanna Bessy and Josh Berl, 2-26-15
 * HW4 - arith
 * 
 * Data structures that are used by compressMethods
 * and decompressMethods to represent data about the 
 * image as it goes through stagesof compression/
 * decompression.
 */
#ifndef COMPRESSIONTYPES
#define COMPRESSIONTYPES

typedef struct float_pixel {
        float red;
        float green;
        float blue;
} *float_pixel;

typedef struct vid_pixel {
        float Y;
        float Pb;
        float Pr;
} *vid_pixel;

typedef struct vpixels {
        vid_pixel v1;
        vid_pixel v2;
        vid_pixel v3;
        vid_pixel v4;
} *vpixels;

typedef struct block_data {
        int counter; 
        UArray_T avg_pixs; 
} *block_data; 

typedef struct compblock {
        unsigned pbavg;
        unsigned pravg;
        unsigned a;
        int b;
        int c;
        int d;
} *compblock;

#endif
