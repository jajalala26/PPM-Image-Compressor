/*******************************************************************************
 *
 *                                  40image.c
 *
 *      Assignment: arith
 *      Authors:    Jared Lee (jalee04) and Coby Keren (jkeren01)
 *      Date:       10/24/23
 *
 *      This file contains the main and compress40 and decompress40 functions 
 *      for the 40image.c program. 40image.c is an image compression and 
 *      decompression program that supports conversions between full-color 
 *      portable pixmap images and compressed binary image files. It reads an
 *      input PPM image from standard input or a specified file, performs the 
 *      specified conversion, and writes the result to standard output in 
 *      binary or PPM format.
 *
 ******************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pnm.h>
#include "compress.h"
#include "decompress.h"
#include "assert.h"
#include "compress40.h"

static void (*compress_or_decompress)(FILE *input) = compress40;

/********** main ********
 *
 * This is the driver for the Arith program
 *
 * Inputs:
 *      int argc:       the number of command line arguments
 *      char *argv[]:   array containing the command line arguments
 * 
 * Return:
 *      EXIT_SUCCESS upon successulf completion of the cod
 * 
 * Expects:
 *      Inputs that follow expected usage
 * 
 ************************/
int main(int argc, char *argv[])
{
        int i;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-c") == 0) {
                        compress_or_decompress = compress40;
                } else if (strcmp(argv[i], "-d") == 0) {
                        compress_or_decompress = decompress40;
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n",
                                argv[0], argv[i]);
                        exit(1);
                } else if (argc - i > 2) {
                        fprintf(stderr, "Usage: %s -d [filename]\n"
                                "       %s -c [filename]\n",
                                argv[0], argv[0]);
                        exit(1);
                } else {
                        break;
                }
        }
        assert(argc - i <= 1);    /* at most one file on command line */
        if (i < argc) {
                FILE *fp = fopen(argv[i], "r");
                assert(fp != NULL);
                compress_or_decompress(fp);
                fclose(fp);
        } else {
                compress_or_decompress(stdin);
        }

        return EXIT_SUCCESS; 
}

/********** compress40 ********
 *
 * This function compresses a PPM image file to CS40 compressed format
 *
 * Inputs:
 *      FILE *fp: pointer to a file holding a PPM image
 * 
 * Expects:
 *     The file to hold a properly formatted PPM image
 * 
 * Notes:
 *      Calls compress.c functions that allocated and free memory
 *      Writes compressed image to stdout
 ************************/
void compress40(FILE *fp)
{
        A2Methods_T plain_methods = uarray2_methods_plain;
        A2Methods_T blocked_methods = uarray2_methods_blocked;

        Pnm_ppm image = read_n_trim(fp);
        UArray2b_T compressed_image = rgb_to_comp_vid(image);
        UArray2_T word_info_arr = init_word_info_arr(compressed_image);
        populate_word_info(compressed_image, word_info_arr);
        finalize_word_info(word_info_arr);

        printf("COMP40 Compressed image format 2\n%u %u\n", image->width,
                                                          image->height);
        pack_n_print(word_info_arr);

        A2Methods_UArray2 A2compressed_image = compressed_image;
        A2Methods_UArray2 A2word_info_arr = word_info_arr;
        blocked_methods->free(&A2compressed_image);
        plain_methods->free(&A2word_info_arr);
        Pnm_ppmfree(&image);
}

/********** decompress40 ********
 *
 * This function decompresses a CS40 compressed format file to a PPM image
 *
 * Inputs:
 *      FILE *fp: pointer to a CS40 compressed format
 * 
 * Expects:
 *     The file to hold a properly formatted compressed image file
 * 
 * Notes:
 *      Calls decompress.c functions that allocated and free memory
 *      Writes decompressed image to stdout
 ************************/
void decompress40(FILE *fp)
{
        A2Methods_T plain_methods = uarray2_methods_plain;
        A2Methods_T blocked_methods = uarray2_methods_blocked;

        unsigned height, width;
        int read = fscanf(fp, "COMP40 Compressed image format 2\n%u %u", 
                          &width, &height);
        assert(read == 2);
        int c = getc(fp);
        assert(c == '\n');

        UArray2_T word_info_arr = unpack_n_store(fp, width, height);
        UArray2b_T decompressed_image = word_info_to_comp_vid(word_info_arr);
        UArray2b_T final_decomp_image = comp_vid_to_rgb(decompressed_image);

        Pnm_ppm final_image = NEW(final_image);
        final_image->width = width;
        final_image->height = height;
        final_image->denominator = 255;
        final_image->methods = blocked_methods;
        final_image->pixels = final_decomp_image;

        Pnm_ppmwrite(stdout, final_image);
        
        A2Methods_UArray2 A2word_info_arr = word_info_arr;
        A2Methods_UArray2 A2decompressed_image = decompressed_image;
        
        blocked_methods->free(&A2decompressed_image);
        plain_methods->free(&A2word_info_arr);
        Pnm_ppmfree(&final_image);
}