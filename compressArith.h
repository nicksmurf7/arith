/*
* * compressArith.c
* * Authors: Nick Murphy and Reed Kass-Mullet
* * Assignment: Comp40 Homework 4, arith
* * Due: October 26
* *
* */

#include "compress40.h"
#include "arith40.h"
#include "a2methods.h"
#include <math.h>
#include "uarray.h"
#include "assert.h"
#include "except.h"
#include "bitpack.h"
#include "uarray2b.h"
#include "a2blocked.h"
#include "pnm.h"
#include "pnmrdr.h"
#include "uarray2.h"


float bit92float(int x);
float bit52float(int x);
int float25bit(float x);
int float29bit(float x);




void read_comp(int col, int row, UArray2_T array2, void *elem,
                void *cl);

int ppmdiff(UArray2b_T og, UArray2b_T pic);
void rgb_cvc_transform(int col, int row, UArray2b_T array2b, void *elem,
    void *cl);
void cvc_rgb_transform(int col, int row, UArray2b_T array2b, void *elem,
    void *cl);

static float round_arith(float number, float lower, float upper);

void quantize_and_pack(int col, int row, UArray2_T array2, void *elem,
        void *cl);
void dequantize_and_unpack(int col, int row, UArray2_T array2, void *elem,
        void *cl);
struct pixdata *quantize(struct pixdata *pack);
struct pixdata *discrete_cosine_transform(struct pixdata *pack);
void print_comp(int col, int row, UArray2_T array2, void *elem,
    void *cl);
/* BITPACKING FUNCTIONS */ 
//bool Bitpack_fitsu(uint64_t n, unsigned width);
//bool Bitpack_fitss( int64_t n, unsigned width);
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb);\
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb);
uint64_t Bitpack_newu(uint64_t word, unsigned width, 
                    unsigned lsb, uint64_t value);
uint64_t Bitpack_news(uint64_t word, unsigned width, 
                    unsigned lsb, int64_t value);

Except_T Bitpack_Overflow = { "Overflow packing bits" };
