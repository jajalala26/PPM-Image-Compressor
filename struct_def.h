/*******************************************************************************
 *
 *                                  struct_def.h
 *
 *      Assignment: arith
 *      Authors:    Jared Lee (jalee04) and Coby Keren (jkeren01)
 *      Date:       10/24/23
 *
 *      This file declares and defines the structs used in compress.c and
 *      decompress.c. These structs include one for storing component video
 *      format data, one for storing code word data, and one for grouping 
 *      an array that represents an image with its corresponding methods
 *      and denominator. 
 *
 ******************************************************************************/

#ifndef STRUCT_DEF_INCLUDED
#define STRUCT_DEF_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pnm.h>
#include <a2methods.h>
#include <uarray.h>
#include <uarray2.h>
#include <uarray2b.h>
#include <a2plain.h>
#include <a2blocked.h>
#include <assert.h>
#include <math.h>
#include <mem.h>

typedef struct comp_vid {
        float y;
        float pb;
        float pr;
} *comp_vid;

typedef struct meth_bundle{
        A2Methods_UArray2 array;
        A2Methods_T methods;
        float denominator;
} *meth_bundle;

typedef struct word_info {
        float avg_pb;
        float avg_pr;
        float a;
        float b;
        float c;
        float d;
} *word_info;

#endif