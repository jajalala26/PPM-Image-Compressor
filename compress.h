/*******************************************************************************
 *
 *                                  compress.h
 *
 *      Assignment: arith
 *      Authors:    Jared Lee (jalee04) and Coby Keren (jkeren01)
 *      Date:       10/24/23
 *
 *      This is the header file for decompress.c. It serves as interface,
 *      declaring the functions that are implemented in decompress.c. The 
 *      functions declared in compress.h perform the various steps 
 *      taken to compress a full-color portable pixmap image to the CS 40 
 *      compressed image format.
 *
 ******************************************************************************/

#ifndef COMPRESS_INCLUDED //TODO: CHECK
#define COMPRESS_INCLUDED

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

Pnm_ppm read_n_trim(FILE *inputfd);
void read_n_trim_app(int col, int row, A2Methods_UArray2 array2, void *elem, 
                     void *cl);
UArray2b_T rgb_to_comp_vid(Pnm_ppm original_image);
void rgb_to_comp_vid_app(int col, int row, A2Methods_UArray2 array2, 
                         void *elem, void *cl);
void populate_word_info(UArray2b_T comp_vid_image, UArray2_T word_info_arr);
void populate_word_info_app(int col, int row, A2Methods_UArray2 array2, 
                            void *elem, void *cl);
void pop_pb_pr(comp_vid comp_vid_pixel, meth_bundle word_info_bundle, 
               int col, int row);
UArray2_T init_word_info_arr(UArray2b_T comp_vid_image);
void init_word_info_arr_app(int col, int row, A2Methods_UArray2 array2, 
                            void *elem, void *cl);
void pop_abcd(comp_vid comp_vid_pixel, meth_bundle word_info_bundle, 
               int col, int row);
void finalize_word_info(UArray2_T word_info_arr);
void finalize_word_info_app(int col, int row, A2Methods_UArray2 array2, 
                            void *elem, void *cl);
int quantize_bcd(float val);
void pack_n_print(UArray2_T word_info_arr);
void pack_n_print_app(int col, int row, A2Methods_UArray2 array2, 
                      void *elem, void *cl);

#endif

