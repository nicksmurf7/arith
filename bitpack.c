/*
* * bitpack.c
* * Authors: Nick Murphy and Reed Kass-Mullet
* * Assignment: Comp40 Homework 4, arith
* * Due: October 26
* *
* */
#include <stdbool.h>
#include <stdint.h>
#include "except.h"
#include "assert.h"
#include "bitpack.h"
#include <math.h>

/* Bitpack_newu
 * Gets:  get a uint64_t word which will be packed
 *        unsigned width represents how many bits the
 *        value being packed in will require
 *        lsb is how much the value will be shiften
 *        uint64_t value isthe number value being packed
 * Returns: a uint64_t word where the unsigned value was packed into
 * Does: Bitpacks value into word according to the specifacations
 *       width and lsb
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, 
                        unsigned lsb, uint64_t value)
{
    if((width > 64) || (width + lsb > 64) ){
        RAISE(Bitpack_Overflow);
    }
    uint64_t mask = (uint64_t) (pow(2,32) - 1);
    uint64_t mask2 = (uint64_t) (pow(2,width) - 1) << lsb;
    uint64_t mask3 = mask - mask2;
    uint64_t repVal = value << lsb;
    uint64_t repWord = word & mask3;
    uint64_t final = repWord + repVal;
    return final;
}

/* Bitpack_news
 * Gets:  get a uint64_t word which will be packed
 *        unsigned width represents how many bits the
 *        value being packed in will require
 *        lsb is how much the value will be shiften
 *        uint64_t value isthe number value being packed
 * Returns: a uint64_t word where the signed value was packed into
 * Does: Bitpacks value into word according to the specifacations
 *       width and lsb
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, 
                    unsigned lsb, int64_t value)
{
    if( (width > 64) || (width + lsb > 64) ){
        RAISE(Bitpack_Overflow);
    }
    uint64_t mask = (uint64_t) (pow(2,32) - 1);
    uint64_t mask2 = (uint64_t) (pow(2, width) - 1) << lsb;
    uint64_t masker = mask - mask2;
    uint64_t maskVal = (uint64_t) (pow(2,width) - 1);
    uint64_t repVal = (value & maskVal) << lsb;
    uint64_t repWord = word & masker;
    uint64_t final = repWord + repVal;
    return final; 
}

/* Bitpack_getu
 * Gets:  uint64_t word is the word from which the value is being
 *        unpacked from. Width is the size of the word being pulled
 *        lsb is how far left the unsigned value being extracted is
 *        being pulled.
 * Returns: a uint64_t representing the extracted value
 * Does: Extracts the value from the word from the location specified
 *       by the parameters.
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width + lsb <= 64);
    uint64_t mask = (uint64_t) (pow(2,width) - 1) << lsb;
    uint64_t final = (word & mask) >> lsb;
    return final;
}

/* Bitpack_gets
 * Gets:  uint64_t word is the word from which the value is being
 *        unpacked from. Width is the size of the word being pulled
 *        lsb is how far left the signed value being extracted is being
 *        pulled.
 * Returns: a uint64_t representing the extracted value
 * Does: Extracts the value from the word from the location specified
 *       by the parameters.
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width + lsb <= 64);
    uint64_t mask = (uint64_t) (pow(2,width) - 1) << lsb;
    int64_t final = (word & mask) >> lsb;
    return final;
}

/* Bitpack_fitsu
 * Gets:  uint64_t n is word, width is the length of a potential value
 * Returns: a boolean representing if a value will fit
 * Does: Checks if a potential unsigned value will fit in the
 *       uint64_t passed in
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{  
    assert(width <= 64);
    uint64_t lim = pow(2,width) - 1;
    if(n > lim) {
        return false;
    } else {
        return true;
    }

}


/* Bitpack_fitsu
 * Gets:  uint64_t n is word, width is the length of a potential value
 * Returns: a boolean representing if a value will fit
 * Does: Checks if the potential signed value will fit in the
 *       uint64_t passed in
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{
    assert(width <= 64);
    int64_t lim = pow(2,width) - 1;
    int64_t nlim = -1 * pow(2,width);
    if(n >= nlim && n <= lim) {
        return true;
    } else {
        return false;
    }
}

