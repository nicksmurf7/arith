/*
* * process_image.h
* * Authors: Nick Murphy and Reed Kass-Mullet
* * Assignment: Comp40 Homework 4, arith
* * Due: October 26
* *
* */
#include "arith40.h"
#include "a2methods.h"
#include "uarray.h"
#include "assert.h"
#include "except.h"
#include "bitpack.h"
#include "uarray2b.h"
#include "a2blocked.h"
#include "pnm.h"
#include "pnmrdr.h"
#include "uarray2.h"


void shave_column(int col, int row, UArray2b_T array2b, void *elem,
void *cl);
void shave_row(int col, int row, UArray2b_T array2b, void *elem,
void *cl);
Pnm_ppm prep(Pnm_ppm pic);
