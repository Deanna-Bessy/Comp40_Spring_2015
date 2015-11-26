/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  !! This is a hw from the class comp40 at Tufts! If you're in comp40 right now,!!
  !! stop right there. You should not be viewing this code. Course policy and   !! 
  !! academic integrity makes that a big no-no. Bugs have intentionally not been!! 
  !! fixed to discourage plagarism                                              !!
  !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/ 

/*
 * Margaret Chapman and Deanna Bessy
 * February 3, 2015
 * sudoku.c
 *
 * This program takes a 9x9 greymap image of a sudoku puzzle. If the sudoku
 * is solved correctly, the program exits normally, and if it is not solved 
 * correctly, it exits in an error state. 
 *
 */

#include <uarray2.h>
#include <bit2.h>
#include <stdlib.h>
#include <stdio.h>
#include <mem.h>
#include <pnmrdr.h> 
#include <assert.h>

/*
 * Contains 3 Bit2_Ts that can check for duplicate numbers in each col, row or 
 * square.
 */
struct duplicate_check {
    Bit2_T rows;
    Bit2_T cols;
    Bit2_T squares; 
};
typedef struct duplicate_check *duplicate_check;

void data_to_array(FILE *fp, UArray2_T uarray2);
void get_data(int col, int row, UArray2_T uarray2, void *member, void *pnmrdr); 
void check_sudoku(int col, int row, UArray2_T uarray2, void *member, 
                  void *dup_check);

/*
 * Calls functions that read in data from an open file handle created here or
 * from standard input. Then calls functions that check if the sudoku puzzle
 * is correctly solved. Returns with an exit(0) if correct, and exit(1) 
 * otherwise;
 */
int main(int argc, char *argv[]) 
{
        duplicate_check check_bitmaps = malloc(sizeof(*check_bitmaps));
        assert(check_bitmaps != NULL);
        check_bitmaps -> rows = Bit2_new(9, 9);
        check_bitmaps -> cols = Bit2_new(9, 9);
        check_bitmaps -> squares = Bit2_new(9, 9);

        UArray2_T sudoku = UArray2_new(9, 9, sizeof(int)); 
        if (argc == 1) {
                data_to_array(stdin, sudoku); 
        }
        else if (argc == 2) {
                FILE *fp = fopen(argv[1], "rb"); 
                if (fp == NULL) {
                        fprintf(stderr, "%s: %s %s %s\n", argv[0], 
                                "Could not open file ", argv[1], "for reading");
                exit(EXIT_FAILURE);  
                }
                data_to_array(fp, sudoku); 
                fclose(fp); 
        } else {
                assert(argc <= 2); 
        } 

        UArray2_map_row_major(sudoku, check_sudoku, (void *) check_bitmaps);

        /* Free all members */
        Bit2_free(&(check_bitmaps -> rows));
        Bit2_free(&(check_bitmaps -> cols));
        Bit2_free(&(check_bitmaps -> squares)); 
        FREE(check_bitmaps);
        UArray2_free(&sudoku);

        exit(0);
}
/*
 * Put input data from a portable graymap or standard input into a Uarray2_T
 */
void data_to_array(FILE *fp, UArray2_T uarray2)
{
        Pnmrdr_T s_reader; 
        Pnmrdr_mapdata s_data; 

        s_reader = Pnmrdr_new(fp); 
        s_data = Pnmrdr_data(s_reader); 

        assert(s_data.type == Pnmrdr_gray); 
        assert(s_data.width >= 9);
        assert(s_data.height >= 9);     

        /* Use get_data to populate uarray2 with correct values */
        UArray2_map_row_major(uarray2, get_data, (void *) s_reader);

        Pnmrdr_free(& s_reader);
}

/*
 * Used with UArray2_map_row_major to populate uarray2 with the numeric values
 * given by the portable graymap or standard input.
 */
void get_data(int col, int row, UArray2_T uarray2, void *member, void *pnmrdr)
{
        (void)uarray2;

        Pnmrdr_T rdr = (Pnmrdr_T) pnmrdr;
        int width = Pnmrdr_data(rdr).width;

        *(int *) member = Pnmrdr_get(rdr);

        /* The graymap height > 9. Pass over extra input */
        if(col == 8) {
                for(int i = 0; i < (int)(width - 9); i++) {
                Pnmrdr_get(rdr);
                }
        }
        /* The graymap width > 9. Pass over extra input */
        if(Pnmrdr_data(rdr).height > 9 && row == 8 && col == 8) {
                for(int i = 0; i < (int)((Pnmrdr_data(rdr).height - 9) * width);
                    i ++) {
                        Pnmrdr_get(rdr);
                }
        }
}

/*
 * Used with UArray2_map_row_major to check a 9 x 9 UArray_T to see if it is a 
 * correctly solved sudoku puzzle.
 */
void check_sudoku(int col, int row, UArray2_T uarray2, void *member, 
                  void *dup_check)
{
        /* Determine the correct square for the current member */
        int square;
        if (row < 3) {
                if (col < 3) square = 0;
                else if (col < 6) square = 1;
                else square = 2;
        } else if (row < 6) {
                if (col < 3) square = 3;
                else if (col < 6) square = 4;
                else square = 5;
        } else {
                if (col < 3) square = 6;
                else if (col < 6) square = 7;
                else square = 8;
        }
        /* Each value must be a number between 1 and 9 */
        assert(*(int *) member > 0 && *(int *) member <= 9);

        /* Check that there are no duplicates in each row, column or square. */
        if(Bit2_put(((duplicate_check)dup_check) -> cols, col, 
           *(int *) member - 1, 1) == 1){
                exit(1);
        }
        if(Bit2_put(((duplicate_check)dup_check) -> rows, row, 
           *(int *) member - 1, 1) == 1){
                exit(1);
        }
        if(Bit2_put(((duplicate_check)dup_check) -> squares, square, 
           *(int *) member - 1, 1) == 1){
                exit(1);
        }
        (void)uarray2;
}
