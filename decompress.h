/*******************************************************************************
 *
 *                                  decompress.h
 *
 *      Assignment: arith
 *      Authors:    Jared Lee (jalee04) and Coby Keren (jkeren01)
 *      Date:       10/24/23
 *
 *      This is the header file for decompress.c. It serves as interface,
 *      declaring the functions that are implemented in decompress.c. These 
 *      functions are used to decompress and image from CS40 compressed image
 *      format to ppm format.
 *
 ******************************************************************************/

#ifndef DECOMPRESS_INCLUDED
#define DECOMPRESS_INCLUDED

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
#include <arith40.h>
#include "struct_def.h"
#include "bitpack.h"


UArray2b_T comp_vid_to_rgb(UArray2b_T comp_vid_array);
void comp_vid_to_rgb_app(int col, int row, A2Methods_UArray2 array2, 
                         void *elem, void *cl);
unsigned quantize_rgb(float color);
UArray2b_T word_info_to_comp_vid(UArray2_T word_info_arr);
void word_info_to_comp_vid_app(int col, int row, A2Methods_UArray2 array2, 
                               void *elem, void *cl);
void populate_comp_vid_cell(float y1, float y2, float y3, float y4, float pb,
                            float pr, UArray2b_T comp_vid_arr, int col, 
                            int row);
comp_vid new_comp_vid_cell(float pb, float pr, float y);
UArray2_T unpack_n_store(FILE *input, unsigned width, unsigned height);
void unpack_n_store_app(int col, int row,A2Methods_UArray2 array2, 
                        void *elem, void *cl);
word_info make_new_word_data(uint64_t word);
#endif