/*
* * process_image.c
* * Authors: Nick Murphy and Reed Kass-Mullet
* * Assignment: Comp40 Homework 4, arith
* * Due: October 26
* *
* */
#include "process_image.h"


/* prep
 * Gets:  a Pmn_ppm pic which contains the photo being compressed
 * Returns: a Pnm_ppm containing the trimmed image
 * Does: trims a column and/or a row if the dimensions of the image
 *       are odd.
 */
Pnm_ppm prep(Pnm_ppm pic)
{
    UArray2b_T array = pic->pixels;
    if (pic->width % 2 != 0) {
        UArray2b_T shaved = UArray2b_new(pic->width - 1, pic->height, 
                            (sizeof (struct Pnm_rgb)), 2);
        UArray2b_map(array, shave_column, shaved);
        UArray2b_free(&array);
        array = shaved;
        pic->width = pic->width - 1;
    } if (pic->height % 2 != 0) {
        UArray2b_T shaved = UArray2b_new(pic->width, pic->height - 1, 
                                (sizeof (struct Pnm_rgb)), 2);
        UArray2b_map(array, shave_row, shaved);
        UArray2b_free(&array);
        array = shaved;
        pic->height = pic->height - 1;
    }
    pic->pixels = array;
    return pic;
}

/* shave_column
 * Gets:  the parameters for a UArray2b map.
 *        cl is a pointer to a UArray2b where values are
 *        copied to.
 *        elem represents the pixel stored at a point in
 *        the photo
 * Returns: nothing
 * Does: copies all values from an array to another array
 *       except for the furthest right column
 */
void shave_column(int col, int row, UArray2b_T array2b, void *elem,
                    void *cl)
{
    if (col < UArray2b_width(array2b) - 1) {
        *(Pnm_rgb) UArray2b_at((UArray2b_T)(cl), col, row) = *(Pnm_rgb) elem;
    } else {
    }
}

/* shave_row
 * Gets:  the parameters for a UArray2b map.
 *        cl is a pointer to a UArray2b where values are
 *        copied to.
 *        elem represents the pixel stored at a point in
 *        the photo
 * Returns: nothing
 * Does: copies all values from an array to another array
 *       except for the furthest down row
 */
void shave_row(int col, int row, UArray2b_T array2b, void *elem,
                void *cl)
{
    if (row < UArray2b_height(array2b) - 1) {   
        *(Pnm_rgb) UArray2b_at((UArray2b_T)(cl), col, row) = *(Pnm_rgb) elem;
    }

    else {
    }
}

