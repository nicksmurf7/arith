#include <stdio.h>
#include "uarray2b.h"
#include "a2blocked.h"
#include "pnm.h"
#include "pnmrdr.h"
#include "uarray2.h"

/*
 * The two functions below are functions you should implement.
 * They should take their input from the parameter and should
 *  write their output to stdout
 */
extern void compress40  (FILE *input);  /* reads PPM, writes compressed image */
extern void decompress40(FILE *input);  /* reads compressed image, writes PPM */
