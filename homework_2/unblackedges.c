/* 
 * Deanna Bessy & Margaret Chapman
 * February 3, 2015
 * unblackedges.c
 *
 * This program takes a plain portable bitmap file and returns a file identical 
 * to the original, except all "black edge" pixels have been changed to white.
 *
 */

#include <bit2.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <mem.h>
#include <pnmrdr.h> 
#include <assert.h>
#include <stack.h> 

struct pixel_info { 
    int col; 
    int row; 
    int old_col; 
    int old_row; 
};
typedef struct pixel_info *pixel_info; 

Bit2_T data_to_bit(FILE *fp); 
void print_pbitmap(int col, int row, Bit2_T i_bitmap, int bit, void *nothing); 
void get_data(int col, int row, Bit2_T i_bitmap, int bit, void *pnmrdr); 
void find_blackedges(Bit2_T i_bitmap);

/*
 * Calls functions that read in data from an open file handle created here 
 * or from standard input. Then calls functions that find and remove black 
 * edges in the image and print the result to standard output. 
 */
int main(int argc, char *argv[]) 
{       
        Bit2_T image;
        if (argc == 1) {
            image = data_to_bit(stdin); 
        }
        else if (argc == 2) {
                FILE *fp = fopen(argv[1], "rb"); 
                if (fp == NULL) {
                        fprintf(stderr, "%s: %s %s %s\n", argv[0], 
                                "Could not open file ", argv[1], "for reading");
                exit(EXIT_FAILURE);  
                }
                image = data_to_bit(fp); 
                fclose(fp); 
        } else {
                fprintf (stderr, "Too many inputs");
                exit(EXIT_FAILURE); 
        }

        find_blackedges(image); 

        Bit2_map_row_major(image, print_pbitmap, NULL); 
        printf("\n");

        Bit2_free(&image);
        exit(0);
}

/*
 * Reads in the portable bitmap and stores the pixel values in a Bit2_T
 */
Bit2_T data_to_bit(FILE *fp)
{
        Pnmrdr_T i_reader; 
        Pnmrdr_mapdata i_data;

        i_reader = Pnmrdr_new(fp); 
        i_data = Pnmrdr_data(i_reader); 
         
        if(i_data.type != Pnmrdr_bit) {
                fprintf(stderr, "File given not a portable bitmap.\n"); 
                Pnmrdr_free(& i_reader); 
                fclose(fp); 
                exit(EXIT_FAILURE); 
        }
        /* Create empty Bit2_T */
        Bit2_T image = Bit2_new(i_data.width, i_data.height); 

        /* Populate the Bit2_T using mapping & get_data function */
        Bit2_map_row_major(image, get_data, (void*) i_reader); 
        Pnmrdr_free(&i_reader);

        return image; 
}

/*
 * Takes a single bit from the Pnmrdr and stores it in the bitmap.
 */
void get_data(int col, int row, Bit2_T i_bitmap, int bit, void *pnmrdr)
{
        int c = Pnmrdr_get((Pnmrdr_T) pnmrdr); 
        Bit2_put(i_bitmap, col, row, c); 

        (void)bit;
}

/*
 * A function used by Bit2_map_row_major to print the result in correct 
 * portable bitmap form. 
 */
void print_pbitmap(int col, int row, Bit2_T i_bitmap, int bit, void *nothing)
{
        /* Prints header for plain pbm file if on first line */
        if (col == 0) {
            if (row == 0) {
                printf("P1\n"); 
                printf("# image without black edges\n");
                printf("%d %d", Bit2_width(i_bitmap), Bit2_height(i_bitmap));

            }
                printf("\n"); 
        }
        printf ("%d ", bit);  
        (void)nothing; 
} 

/*
 * Searches through the bitmap to find black edge pixels and change them to 
 * white.
 */
void find_blackedges(Bit2_T i_bitmap) 
{
        int col = 0; 
        int row  = 0; 
        Stack_T neighbors = Stack_new(); 
        
        /* Search top row for black pixels */
        for (; col < Bit2_width(i_bitmap); col++) {
                /* change pixel to white and add pixel below to stack */
                if (Bit2_get(i_bitmap, col, row) == 1) {
                        Bit2_put(i_bitmap, col, row, 0); 
                        pixel_info down = malloc (sizeof(*down)); 
                        assert (down != NULL); 
                        down -> row = row + 1; 
                        down -> col = col;
                        down -> old_row = row; 
                        down -> old_col = col; 
                        Stack_push(neighbors, down);
                }
        }
        col--;
        /* Search right most column for black pixels */
        for (; row < Bit2_height(i_bitmap); row++) {
                /* change pixel to white and add pixel to the left to stack */
                if (Bit2_get(i_bitmap, col, row) == 1) {
                        Bit2_put(i_bitmap, col, row, 0);
                        pixel_info left = malloc (sizeof(*left)); 
                        assert(left != NULL); 
                        left -> row = row; 
                        left -> col = col - 1;
                        left -> old_row = row; 
                        left -> old_col = col; 
                        Stack_push(neighbors, left); 
                }
        }
        row--; 
        /* Search bottom row for black pixels */
        for (; col >= 0; col--) {
                /* change pixel to white and add pixel above to stack */
                if (Bit2_get(i_bitmap, col, row) == 1) {
                        Bit2_put(i_bitmap, col, row, 0);
                        pixel_info up = malloc (sizeof(*up));  
                        assert(up != NULL); 
                        up -> row = row + 1; 
                        up -> col = col;
                        up -> old_row = row; 
                        up -> old_col = col; 
                        Stack_push(neighbors, up);
                }
        }
        col++; 
        /* Search left most column for black pixels */
        for (; row >= 0; row--) {
                /* change pixel to white and add pixel to the right to stack */
                if (Bit2_get(i_bitmap, col, row) == 1) {
                        Bit2_put(i_bitmap, col, row, 0);
                        pixel_info right = malloc(sizeof(*right)); 
                        assert (right != NULL); 
                        right -> row = row; 
                        right -> col = col + 1;
                        right -> old_row = row; 
                        right -> old_col = col; 
                        Stack_push(neighbors, right);
                }
        }

        /* 
         * Find and change all black edge pixels that are not on the outter 
         * most edge of the bitmap.
         */
        while(!Stack_empty(neighbors)) {
                pixel_info working = Stack_pop(neighbors);
                /* check that the pixel is in bounds */ 
                if(( working -> col) < 0 || 
                   (working -> col) > Bit2_width(i_bitmap) - 1 || 
                   (working -> row) < 0 ||
                   (working -> row) > Bit2_height(i_bitmap) - 1) {
                        FREE(working); 
                        continue; 
                }
                /* check if the bit is white */
                else if (Bit2_get(i_bitmap, working -> col, working -> row)
                          == 0) {
                        FREE(working); 
                        continue; 
                }
                /* change the pixel to white and add neighbors to the stack */
                else {
                        Bit2_put(i_bitmap, working -> col, working -> row, 0); 
                        pixel_info left2 = malloc (sizeof(*left2)); 
                        assert(left2 != NULL); 
                        left2 -> row = working -> row; 
                        left2 -> col = working -> col - 1;
                        left2 -> old_row = working -> row; 
                        left2 -> old_col = working -> col; 
                        if( left2 -> col != working -> old_col) {
                                Stack_push(neighbors, left2); 
                        } else {
                            FREE(left2);
                        }
                        pixel_info up2 = malloc (sizeof(*up2));
                        assert(up2 != NULL); 
                        up2 -> row = working -> row - 1; 
                        up2 -> col = working -> col;
                        up2 -> old_row = working -> row; 
                        up2 -> old_col = working -> col; 
                        if (up2 -> row != working -> old_row) {
                                Stack_push(neighbors, up2);
                        } else {
                            FREE(up2);
                        }
                        pixel_info right2 = malloc(sizeof(*right2)); 
                        assert (right2 != NULL); 
                        right2 -> row = working -> row ; 
                        right2 -> col = working -> col + 1;
                        right2 -> old_row = working -> row; 
                        right2 -> old_col = working -> col; 
                        if(right2 -> col != working -> old_col) {
                                Stack_push(neighbors, right2);
                        } else{
                            FREE(right2);
                        }
                        pixel_info down2 = malloc (sizeof(*down2)); 
                        assert (down2 != NULL); 
                        down2 -> row = working -> row + 1; 
                        down2 -> col = working -> col;
                        down2 -> old_row = working -> row; 
                        down2 -> old_col = working ->col; 
                        if (down2 -> row != working -> old_row) {
                                Stack_push(neighbors, down2); 
                        } else {
                            FREE(down2);
                        }
                        FREE(working); 
                } /* else */                  
        } /* while */
        Stack_free(&neighbors); 
} 

