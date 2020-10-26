/*
* The two fus40.c
* * Authors: Nick Murphy and Reed Kass-Mullet
* * Assignment: Comp40 Homework 4, arith
* * Due: October 26
* *
* *
* */

#include <stdlib.h>
#include <stdio.h>
#include "compress40.h"
#include "uarray2b.h"
#include "a2blocked.h"
#include "arith40.h"
#include "a2methods.h"
#include <math.h>
#include "uarray.h"
#include "assert.h"
#include "pnm.h"
#include "pnmrdr.h"
#include "uarray2.h"
#include "except.h"
#include "assert.h"


typedef struct Pnm_cvc {
float y, pb, pr;
} *Pnm_cvc;


typedef struct lifesaver {
unsigned den;
UArray2b_T cv_image;
} *lifesaver;



typedef struct pixdata {
struct Pnm_cvc tl;
struct Pnm_cvc tr;
struct Pnm_cvc bl;
struct Pnm_cvc br;
float a;
float b;
float c;
float d;
unsigned ipb;
unsigned ipr;
} *pixdata;


void shave_column(int col, int row, UArray2b_T array2b, void *elem,
void *cl);
void shave_row(int col, int row, UArray2b_T array2b, void *elem,
void *cl);
Pnm_ppm prep(Pnm_ppm pic);
int ppmdiff(UArray2b_T og, UArray2b_T pic);
void rgb_cvc_transform(int col, int row, UArray2b_T array2b, void *elem,
    void *cl);
void cvc_rgb_transform(int col, int row, UArray2b_T array2b, void *elem,
    void *cl);

static float round_arith(float number, float lower, float upper);

Pnm_ppm unpack(FILE *fp);
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
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value);
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, int64_t value);

Except_T Bitpack_Overflow = { "Overflow packing bits" };

int float25bit(float x);
int float29bit(float x);
void make_word(int col, int row, UArray2_T array2, void *elem,
void *cl);



extern void decompress40(FILE *input)

{
    (void) input;
// (1) READ IN compressed file

// (2) unquantize all pixels into cvc

// (3) transform from cvc to rgb
   // UArray2b_map(pixel_array, cvc_rgb_transform, &(pic->denominator));
// (4) make PPM file and place rgb array into photo

// (5) properly output file

}


/* reads PPM, writes compressed image */

extern void compress40 (FILE *input)
{
// (1) READ IN FILE
    Pnm_ppm pic = Pnm_ppmread(input, uarray2_methods_blocked);

// (2) TRIM FILE
    pic = prep(pic);

// (3) Array is array of rgb pixels
    UArray2b_T pixel_array = pic->pixels;

    /*
    for(int i = 0; i < (int)pic->width; i++) {
        for(int j = 0; j < (int) pic->height; j++){
            printf("r g b = [%d %d %d]", ((struct Pnm_rgb *) UArray2b_at(array, i, j))->red,
                ((struct Pnm_rgb *) UArray2b_at(array, i, j))->green,
                ((struct Pnm_rgb *) UArray2b_at(array, i, j))->blue);
            printf(" at col, row [%d %d]\n", i, j);
        }
    }
    */

    //printf("pic dimensions: col, row [%d %d]\n", pic->width, pic->height);
    /* Pic is trimmed and copied as a UArray2b */

    unsigned den = pic->denominator;
    (void) den;

    fprintf(stdout, "Write after image\n");
    FILE *writeToB = fopen("beforetrans.ppm", "w");
    Pnm_ppmwrite(writeToB, pic);
    fclose(writeToB);

// (4) Transform from rgb to cvc
    UArray2b_map(pixel_array, rgb_cvc_transform, &(pic->denominator));

//TEST transform & reverse transform
    UArray2b_map(pixel_array, cvc_rgb_transform, &(pic->denominator));
    pic->pixels = pixel_array;
    fprintf(stdout, "Write after image\n");
    FILE *writeToA = fopen("aftertrans.ppm", "w");
    Pnm_ppmwrite(writeToA, pic);
    fclose(writeToA);
    /*
    printf("pixel data:\n");
    for(int i = 0; i < (int)pic->width; i++) {
        for(int j = 0; j < (int) pic->height; j++){
            printf("y pb pr = [%f %f %f]", ((struct Pnm_cvc *) UArray2b_at(array, i, j))->y,
                ((struct Pnm_cvc *) UArray2b_at(array, i, j))->pb,
                ((struct Pnm_cvc *) UArray2b_at(array, i, j))->pr);
            printf(" at col, row [%d %d]\n", i, j);
        }
    }
    */

    struct lifesaver closure;
    UArray2_T output = UArray2_new(UArray2b_width(pixel_array) / 2, UArray2b_height(pixel_array) / 2, 80);

    closure.cv_image = pixel_array;
    closure.den = pic->denominator;

// (5) quantize & pitpack all pixel groups, save codes into output
    UArray2_map_row_major(output, quantize_and_pack, &closure);

    //UArray2_T words = UArray2_new(UArray2_width(output), UArray2_height(output), 64);
    //printf("printing all words:\n");

// (6) Print out Compressed file to stdout
    fprintf(stdout, "COMP40 Compressed image format 2\n%u %u\n",
        pic->width, pic->height);
    UArray2_map_row_major(output, print_comp, NULL);
    //UArray2_map_row_major(words, make_word, output);
    //output is an array of codewords for compressed info
    //bitpack this bih

    UArray2_free(&output);
    FILE *writeTo2 = fopen("afterCVC.ppm", "w");
    Pnm_ppmwrite(writeTo2, pic);
    fclose(writeTo2);
    Pnm_ppmfree(&pic);
}

void print_comp(int col, int row, UArray2_T array2, void *elem,
void *cl)
{

(void) array2;
(void) col;
(void) row;
(void) cl;
uint64_t word = *(uint64_t*) elem;
printf("WORD: %ld\n", word);
char ch1 = Bitpack_getu(word, 8, 24);
//printf("Test: %c\n", ch1);
char ch2 = Bitpack_getu(word, 8, 16);
//printf("Test: %c\n", ch2);
char ch3 = Bitpack_getu(word, 8, 8);
//printf("Test: %c\n", ch3);
char ch4 = Bitpack_getu(word, 8, 0);
//printf("Test: %c\n", ch4);
//putchar(ch1);
//putchar(ch2);
//putchar(ch3);
//putchar(ch4);
(void) ch1;
(void) ch2;
(void) ch3;
(void) ch4;
}

//Takes four pixels and converts to word
void quantize_and_pack(int col, int row, UArray2_T array2, void *elem,
        void *cl)
{

    struct pixdata *to_quantize;
    to_quantize->tl = *(struct Pnm_cvc*) UArray2b_at(((struct lifesaver*) cl)->cv_image, col * 2, row* 2);
    to_quantize->tr = *(struct Pnm_cvc*) UArray2b_at(((struct lifesaver*) cl)->cv_image, col * 2 + 1, row* 2);
    to_quantize->bl = *(struct Pnm_cvc*) UArray2b_at(((struct lifesaver*) cl)->cv_image, col * 2, row* 2 + 1);
    to_quantize->br = *(struct Pnm_cvc*) UArray2b_at(((struct lifesaver*) cl)->cv_image, col * 2 + 1, row * 2 + 1);

    struct pixdata *check = discrete_cosine_transform(quantize(to_quantize));

    *(struct pixdata*) UArray2_at(array2, row, col) = *check;
    printf("lessgo\n");
    uint64_t word = 0;
    printf("Passing in: [A B C D IPB IPR] == [%f %f %f %f %d %d]", check->a, check->b, check->c, check->d, check->ipb, check->ipr);
    word = Bitpack_newu(word, 9, 23, float29bit(check->a));
    word = Bitpack_news(word, 5, 18, float25bit(check->b));
    word = Bitpack_news(word, 5, 13, float25bit(check->c));
    word = Bitpack_news(word, 5, 8, float25bit(check->d));
    word = Bitpack_newu(word, 4, 4, check->ipb);
    word = Bitpack_newu(word, 4, 0, check->ipr);
    printf("WRODRD at %d %d, %ld\n", col, row, word);
    *(uint64_t *) elem = word;
    printf("WORD SHOULD BE 69 and IT IS :: %ld \n", *(uint64_t*)elem);
    printf("test at: (%d, %d) %ld\n", col, row, *(uint64_t*)UArray2_at(array2, col, row)); 
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width + lsb <= 64);
    uint64_t mask = (uint64_t) (pow(2,width) - 1) << lsb;
    uint64_t final = (word & mask) >> lsb;
    return final;
}

//Takes word and converts to four pixels
void dequantize_and_unpack(int col, int row, UArray2_T array2, void *elem,
        void *cl)
{
     uint64_t word = *(uint64_t*)elem;
     struct pixdata *dequantize;
     pixdata->a = Bitpack_getu(word, 9, 23);
     pixdata->b = Bitpack_getu(word, 5, 18);
     pixdata->c = Bitpack_getu(word, 5, 13);
     pixdata->d = Bitpack_getu(word, 5, 8);
     pixdata->pb = Arith40_chroma_of_index(Bitpack_getu(word, 4, 4));
     pixdata->pr = Arith40_chroma_of_index(Bitpack_getu(word, 4, 0));
     //Data has been unpacked
     //We must unquantize that bih

    struct Pnm_cvc tl;
    struct Pnm_cvc tr;
    struct Pnm_cvc bl;
    struct Pnm_cvc br;
    tl.y = pixdata->a − pixdata->b − pixdata->c + pixdata->d;
    tr.y = pixdata->a − pixdata->b + pixdata->c − pixdata->d;
    bl.y = pixdata->a + pixdata->b − pixdata->c − pixdata->d;
    br.y = pixdata->a + pixdata->b + pixdata->c + pixdata->d;

    tl.pb = pixdata->pb;
    tl.pb = pixdata->pr;
    tr.pb = pixdata->pb;
    tr.pb = pixdata->pr;
    bl.pb = pixdata->pb;
    bl.pb = pixdata->pr;
    br.pb = pixdata->pb;
    br.pb = pixdata->pr;

    //Places pixels in array accessed via the cl variable of the map
    *(struct cvc*)UArray2_at((Uarray2_T) cl, col, row) = tl;
    *(struct cvc*)UArray2_at((Uarray2_T) cl, col, row) = tr;
    *(struct cvc*)UArray2_at((Uarray2_T) cl, col, row) = bl;
    *(struct cvc*)UArray2_at((Uarray2_T) cl, col, row) = br;
}


int float29bit(float x) 
{
printf("\n");
printf("FLOAT29BIT Turning %f into %u\n", x, (unsigned) (x * 511));
x = (unsigned) (x * 511);
x = round(x /* * 63*/);
return x;
}

int float25bit(float x) 
{
if(x < -0.3) {
// printf("FLOAT25BIT Turning %f into -15\n", x);
return -15;
} if (x > 0.3) {
//printf("FLOAT25BIT Turning %f into 15\n", x);
return 15;
} else {
//printf("FLOAT25BIT Turning %f into %d\n", x, (int) (x/0.02));
return (int) (x/0.02);
}
}

uint64_t Bitpack_newu(uint64_t word, unsigned width, 
unsigned lsb, uint64_t value)
{
//printf("BITPACKING NEWU %ld and %ld using %u\n", word, value, lsb);
if((width > 64) || (width + lsb > 64) ){
RAISE(Bitpack_Overflow);
}
uint64_t mask = (uint64_t) (pow(2,32) - 1);
uint64_t mask2 = (uint64_t) (pow(2,width) - 1) << lsb;
uint64_t mask3 = mask - mask2;
uint64_t repVal = value << lsb;
//printf("repVal::: %ld\n", repVal);
uint64_t repWord = word & mask3;
uint64_t final = repWord + repVal;
//printf("Bitpacked newu final::: %ld\n", final);
return final;

/*
printf("lsb %d\n", lsb);
printf("BITPACKING NEWU %ld and %ld using %u and ", word, value, lsb);
uint64_t mask = (pow(2,32) - 1);
mask = mask << 31;
printf("mask1 %ld\n", mask);
uint64_t maskhelp = (pow(2,32) - 1);
mask = mask | maskhelp;
uint64_t mask2 = (pow(2, width) - 1);
printf("mask1 %ld\n", mask);
mask2 = mask2 << lsb;
printf("mask2 %ld\n", mask2);
mask = mask & ~mask2;
printf("Newu mask used == %ld\n", mask);
value = value << lsb;
word = word & mask;
word = word | value;

uint64_t mask3 = 1;
mask3 = mask3 << 63;
printf("mask3 %ld\n", mask3);
printf("Newu mask == %ld\n", mask);
printf("\n word == %ld\n", word);
*/
return word;
}

uint64_t Bitpack_news(uint64_t word, unsigned width, 
unsigned lsb, int64_t value)
{
if((width > 64) || (width + lsb > 64)){
RAISE(Bitpack_Overflow);
}
//printf("BITPACKING NEWS %ld and %ld using %u\n", word, value, lsb);
uint64_t mask = (uint64_t) (pow(2,32) - 1);
uint64_t mask2 = (uint64_t) (pow(2, width) - 1) << lsb;
uint64_t masker = mask - mask2;
uint64_t maskVal = (uint64_t) (pow(2,width) - 1);
uint64_t repVal = (value & maskVal) << lsb;
//printf("Val::: %ld\n", value);
//printf("repVal::: %ld\n", repVal);
uint64_t repWord = word & masker;
uint64_t final = repWord + repVal;
/*
printf("BITPACKING NEWS %ld and %ld using %u---->", word, value, lsb);
uint64_t mask = (pow(2,32) - 1);
uint64_t mask2 = (pow(2,32) - 1);
mask2 = mask2 << (64 - width);
mask2 = mask2 >> (64 - lsb);
mask = mask - mask2;
value = (uint64_t) value << lsb;
word = mask & value;
mask = mask << lsb;
word = word & (~mask);
mask2 = value << (64-width);
mask2 = mask2 >> (64 - (width + lsb));
printf("News mask == %ld\n", mask);
//word = word | mask2;
printf("poop ====== %ld\n", word);
*/
//printf("Bitpacked news final::: %ld\n", final);
return final; 

}



struct pixdata *quantize(struct pixdata *pack)
{
        float avg_pb = (pack->tl).pb + pack->tr.pb + pack->br.pb + pack->bl.pb;
        avg_pb = avg_pb/4;
        float avg_pr = pack->tl.pr + pack->tr.pr + pack->br.pr + pack->bl.pr;
        avg_pr = avg_pr/4;
        unsigned rep_pb = Arith40_index_of_chroma(avg_pb);
        unsigned rep_pr = Arith40_index_of_chroma(avg_pr);
        pack->ipb = rep_pb;
        pack->ipr = rep_pr;
        printf("[pb pr] == [%d %d]\n", rep_pb, rep_pr);
        return pack;
}

struct pixdata *discrete_cosine_transform(struct pixdata *pack)
{
    float y1, y2, y3, y4, a, b, c, d;
    y1 = pack->tl.y;
    y2 = pack->tr.y;
    y3 = pack->bl.y;
    y4 = pack->br.y;
    printf("[y1 y2 y3 y4] == [%f %f %f %f]\n", y1, y2, y3, y4);
    a = (y4 + y3 + y2 + y1) / 4;
    b = (y4 + y3 - y2 - y1) / 4;
    c = (y4 - y3 + y2 - y1) / 4;
    d = (y4 - y3 - y2 + y1) / 4;
    printf("[A B C D] == [%f %f %f %f]\n", a, b, c, d);
    pack->a = a;
    pack->b = b;
    pack->c = c;
    pack->d = d;
    printf("[A B C D] == [%f %f %f %f]\n", pack->a, pack->b, pack->c, pack->d);
    return pack;
}


Pnm_ppm prep(Pnm_ppm pic)

{
        UArray2b_T og = pic->pixels;
        UArray2b_T array = pic->pixels;
        int d = pic->denominator;
        int h = pic->height;
        int w = pic->width;
        printf("[Denom, height, width] == [%d, %d, %d]\n", (int) d, (int) h, (int) w);
        //fprintf(stdout, "Write before image\n");
        FILE *writeTo = fopen("before.ppm", "w");
        Pnm_ppmwrite(writeTo, pic);
        fclose(writeTo);
        if(pic->width % 2 != 0) {
        UArray2b_T shaved = UArray2b_new(pic->width - 1, pic->height, (sizeof (struct Pnm_rgb)), 2);
        UArray2b_map(array, shave_column, shaved);
        array = shaved;
        pic->width = pic->width - 1;
        }
        if(pic->height % 2 != 0){
                UArray2b_T shaved = UArray2b_new(pic->width, pic->height - 1, (sizeof (struct Pnm_rgb)), 2);
                UArray2b_map(array, shave_row, shaved);
                array = shaved;
                pic->height = pic->height - 1;
        }
        pic->pixels = array;
        //printf("POST [Denom, height, width] = [%d, %d, %d]\n", d, h, w);
        fprintf(stdout, "Write after image\n");
        FILE *writeTo2 = fopen("after.ppm", "w");
        Pnm_ppmwrite(writeTo2, pic);
        fclose(writeTo2);
        double diff = ppmdiff(og,pic->pixels);
        printf("PPMDiff returned: %f\n", diff);
        return pic;
}

//RGB TO CVC TRANSFORMS =====================================================

void rgb_cvc_transform(int col, int row, UArray2b_T array2b, void *elem,
    void *cl)
{
    struct Pnm_rgb rgb = *((struct Pnm_rgb*) elem);
    unsigned den = *((unsigned*) cl);
    struct Pnm_cvc *cvc = malloc(sizeof(*cvc));
    float r, g, b;
    r = (rgb.red * 1.0) / (den);
    g = (rgb.green * 1.0) / (den);
    b = (rgb.blue * 1.0) / (den);
    cvc->y = (0.299 * r + 0.587 * g + 0.114 * b);
    cvc->pb = (-0.168736 * r - 0.331264 * g + 0.5 * b);
    cvc->pr = (0.5 * r - 0.418688 * g - 0.081312 * b);
    cvc->y = round_arith(cvc->y, 0, 1);
    cvc->pb = round_arith(cvc->pb, -0.5, 0.5);
    cvc->pr = round_arith(cvc->pr, -0.5, 0.5);
    *(struct Pnm_cvc*) UArray2b_at(array2b, col, row) = *cvc;
}

void cvc_rgb_transform(int col, int row, UArray2b_T array2b, void *elem,
    void *cl)
{

    struct Pnm_cvc cvc = *((struct Pnm_cvc*) elem);
    unsigned den = *((unsigned*) cl);
    float y, pr, pb;
    int r, g, b;
    y = cvc.y;
    pr = cvc.pr;
    pb = cvc.pb;

    r = den * (1.0 * y + 1.204 * pr);
    g = den * (1.0 * y - .0344136 * pb - .714136 * pr);
    b = den * (1.0 * y + 1.772 * pb);
    
    if (r < 0) {
        r = 0;
    } else if (r > den) {
        r = den;
    }
    if (g < 0) {
        g = 0;
    } else if (g > den) {
        g = den;
    }
    if (b < 0) {
        b = 0;
    } else if (b > den) {
        b = den;
    }
    struct Pnm_rgb rgb;
    rgb.red = r;
    rgb.blue = b;
    rgb.green = g;
    *(struct Pnm_rgb*) UArray2b_at(array2b, col, row) = rgb;
}

//==========================================================================


static float round_arith(float number, float lower, float upper)

{

if (number > upper) {

number = upper;

} else if (number < lower) {

number = lower;

}

return number;

}



void shave_column(int col, int row, UArray2b_T array2b, void *elem,

void *cl)

{

//printf("made it in shave col: [row, col] == [%d %d] \n", row, col);

if (col < UArray2b_width(array2b) - 1) {

*(Pnm_rgb) UArray2b_at((UArray2b_T)(cl), col, row) = *(Pnm_rgb)elem;

}

else {

//printf("shaved col @ %d\n", col);

}

}


void shave_row(int col, int row, UArray2b_T array2b, void *elem,

void *cl)

{

//printf("made it in shave row: [row, col] == [%d %d] \n", row, col);

if (row < UArray2b_height(array2b) - 1) {

*(Pnm_rgb) UArray2b_at((UArray2b_T)(cl), col, row) = *(Pnm_rgb)elem;

}

else {

//printf("shaved row @ %d\n", row);

}

}










/* reads compressed image, writes PPM */



Pnm_ppm unpack(FILE *fp)

{

unsigned h, w, den;

int r = fscanf(fp, "COMP40 Compressed image format 2\n%u %u", &w, &h);

printf("R == %d\n", r);

assert(r == 2);

int c = getc(fp);

assert(c == '\n');

UArray2b_T array = UArray2b_new_64K_block(w, h, 2);

den = 65000;

Pnm_ppm pixelmap = malloc(sizeof(*pixelmap));

assert(pixelmap != NULL);

struct Pnm_ppm pic = { .width = w, .height = h,

.denominator = den,

.pixels = array,

.methods = uarray2_methods_blocked

};

*pixelmap = pic;

return pixelmap;

}


/*

struct Pnm_rgb float2int(int col, int row, UArray2b_T array2b, void *elem,

void *cl)

{

float r, g, b;

r =

g =

b =

struct Pnm_rgb rgb = *((struct Pnm_rgb*) elem);

unsigned den = *((unsigned*) cl);

struct Pnm_cvc *cvc = malloc(sizeof(*cvc));

cvc->y = (0.299 * rgb.red + 0.587 * rgb.green + 0.114 * rgb.blue)/den;

cvc->pb = (-0.168736 * rgb.red - 0.331264 * rgb.green + 0.5 * rgb.blue)/den;

cvc->pr = (0.5 * rgb.red - 0.418688 * rgb.green - 0.081312 * rgb.blue)/den;

cvc->y = round_arith(cvc->y, 0, 1);

cvc->pb = round_arith(cvc->pb, -0.5, 0.5);

cvc->pr = round_arith(cvc->pr, -0.5, 0.5);

*(struct Pnm_cvc*) UArray2b_at(array2b, col, row) = *cvc;

}

*/























int ppmdiff(UArray2b_T og, UArray2b_T array2b)

{

int diff = 0;

int h = UArray2b_height(array2b);

int w = UArray2b_width(array2b);

for(int i = 0; i < w; ++i) {

for(int j = 0; j < h; ++j) {

int r = 0;

int b = 0;

int g = 0;

r = ((struct Pnm_rgb *)UArray2b_at(og, i,j)) -> red;

b = ((struct Pnm_rgb *)UArray2b_at(og, i,j)) -> blue;

g = ((struct Pnm_rgb *)UArray2b_at(og, i,j)) -> green;

int r2 = 0;

int b2 = 0;

int g2 = 0;

r2 = ((struct Pnm_rgb *)UArray2b_at(array2b, i,j)) ->red;

b2 = ((struct Pnm_rgb *)UArray2b_at(array2b, i,j)) ->blue;

g2 = ((struct Pnm_rgb *)UArray2b_at(array2b, i,j)) ->green;

if((r != r2) || (b != b2) || (g != g2)){

//printf("Found a difference\n");

diff++;

}

}

}

printf("Diff == %d\n", diff);

printf("NumPix == %d\n", (h * w));

double avg = (diff)/(h * w);

return avg;

}



/*
//BITPACK FILE ===============================================================-=-=====

bool Bitpack_fitsu(uint64_t n, unsigned width);

bool Bitpack_fitss( int64_t n, unsigned width);

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb);

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb);

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value);

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, int64_t value);

extern Except_T Bitpack_Overflow;
*/






