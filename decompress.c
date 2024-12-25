/*******************************************************************************
 *
 *                                decompress.c
 *
 *      Assignment: arith
 *      Authors:    Jared Lee (jalee04) and Coby Keren (jkeren01)
 *      Date:       10/24/23
 *
 *      This file defines the functions for the decompress.c program, which
 *      is executed in the decompress40 function. The functions serve to 
 *      execute steps in the proccess of decompressing a file from CS40 format
 *      to ppm format. The decompressed image is converted from COMP40 
 *      compressed image format to component video format and then finally to 
 *      rgb format, at which point the image is written to standard output. 
 *
 ******************************************************************************/

#include "decompress.h"

/********** comp_vid_to_rgb ********
 *
 * Given an image in component video format, allocated and populate an array 
 * that represents that image in rgb format
 *
 * Inputs:
 *      UArray2b_T comp_vid_array: The image being converted from component
 *                                 video to rgb
 * 
 * Return:
 *      UArray2b_T that represents the image in rgb format
 * 
 * Expects:
 *      A properly formatted array holding comp_vid structs 
 * 
 * Notes:
 *      Memory is allocated for the rgb array, it is freed elsewhere
 *      A mapping function is called to perform the conversion and populate
 *      the rgb array
 ************************/
UArray2b_T comp_vid_to_rgb(UArray2b_T comp_vid_array)
{
        A2Methods_T methods = uarray2_methods_blocked;
        int blocksize = 2;
        UArray2b_T rgb_array = methods->new_with_blocksize(
                                                methods->width(comp_vid_array),
                                                methods->height(comp_vid_array),
                                                sizeof(struct Pnm_rgb),
                                                blocksize);
        meth_bundle rgb_info = NEW(rgb_info);
        rgb_info->array = rgb_array;
        rgb_info->methods = methods;

        methods->map_block_major(comp_vid_array, comp_vid_to_rgb_app, rgb_info);

        rgb_array = rgb_info->array;
        FREE(rgb_info);
        
        return rgb_array;
}

/********** comp_vid_to_rgb_app ********
 * 
 * Converts a pixel from component video to rgb format and stores
 * it in its respective location in the image array
 *
 * Inputs:
 *      int col:                   Column index
 *      int row:                   Row index
 *      A2Methods_UArray2 array2:  the comp_vid array
 *      void *elem:                The current element in the array
 *      void * cl;                 A meth_bundle struct holding the rgb array,
 *                                 and methods
 * 
 * Expects:
 *      All pointers to not be null, the array being mapped to be in
 *      component video format
 * 
 * Notes:
 *      Upon completion the rgb array is fully populated
 *      A new rgb struct is allocated to store data and later freed
 ************************/
void comp_vid_to_rgb_app(int col, int row, A2Methods_UArray2 array2, 
                         void *elem, void *cl)
{
        (void) array2;
        comp_vid comp_vid_vals = elem;
        meth_bundle rgb_info = cl;
        assert(comp_vid_vals);
        assert(rgb_info);
         
        float y = comp_vid_vals->y;
        float pb = comp_vid_vals->pb;
        float pr = comp_vid_vals->pr;

        float r = 1.0 * y + 0.0 * pb + 1.402 * pr;
        float g = 1.0 * y - 0.344136 * pb - 0.714136 * pr;
        float b = 1.0 * y + 1.772 * pb + 0.0 * pr;
        
        unsigned red = quantize_rgb(r);
        unsigned green = quantize_rgb(g);
        unsigned blue = quantize_rgb(b);
                
        Pnm_rgb rgb_cell = NEW(rgb_cell);
        rgb_cell->red = red;
        rgb_cell->green = green;
        rgb_cell->blue = blue;
        
        Pnm_rgb array_cell = rgb_info->methods->at(rgb_info->array, col, row);

        *array_cell = *rgb_cell;

        FREE(rgb_cell);
}

/********** quantize_rgb ********
 * 
 * Quantizes an rgb value by multiplying it by a denominator of 255 and
 * truncating the remainder
 *
 * Inputs:
 *      float color:  the value being quantized
 * 
 * Returns:
 *      Unsigned representing the quantized color value
 * 
 * Expects:
 *      color to be a value between 0 and 1
 * 
 * Notes:
 *      prepares rgb values to correspond with the final image denominator
 *      of 255 which is assigned later
 ************************/
unsigned quantize_rgb(float color)
{
        unsigned denominator = 255;
        if (color > 1) {
                color = 1;
        }
        if (color < 0) {
                color = 0;
        }
        
        unsigned quantized_color = color * denominator;
        return quantized_color; 
}


/********** word_info_to_comp_vid ********
 *
 * Allocates and populates an array of comp_Vid structs with data
 * extacted from an array of word_info structs
 *
 * Inputs:
 *      UArray2_T word_info_arr: An array holding all the word_info data
 * 
 * Return:
 *      UArray2b_T that represents an image in component video format
 * 
 * Expects:
 *      word_info_arr to be an array of word_info structs
 * 
 * Notes:
 *      Memory is allocated for the comp_vid array, it is freed elsewhere
 *      A mapping function is called to initialize the comp_vid array
 ************************/
UArray2b_T word_info_to_comp_vid(UArray2_T word_info_arr)
{
        A2Methods_T methods_b = uarray2_methods_blocked;
        A2Methods_T methods_p = uarray2_methods_plain;
        
        int blocksize = 2;
        int new_width = methods_p->width(word_info_arr) * 2;
        int new_height = methods_p->height(word_info_arr) * 2;
        
        UArray2b_T comp_vid_arr = methods_b->new_with_blocksize(new_width,
                                                                new_height,
                                                                sizeof(struct 
                                                                comp_vid),
                                                                blocksize);

        methods_p->map_row_major(word_info_arr, word_info_to_comp_vid_app,
                                 comp_vid_arr);

        return comp_vid_arr;
}

/********** word_info_to_comp_vid_app ********
 * 
 * Extracts data from the word_info array, computes and populates
 * comp_vid data
 *
 * Inputs:
 *      int col:                   Column index
 *      int row:                   Row index
 *      A2Methods_UArray2 array2:  The word_info array
 *      void *elem:                The current element in the array
 *      void * cl;                 the comp_vid array being populated
 * 
 * Expects:
 *      Elem to not be null
 * 
 * Notes:
 *      Upon completion the word_info array is fully initialized
 *      calls a helper function to populate comp_vid struct with the
 *      extracted data
 ************************/
void word_info_to_comp_vid_app(int col, int row, A2Methods_UArray2 array2, 
                               void *elem, void *cl)
{
        (void) array2;

        UArray2b_T comp_vid_arr = cl;
        word_info word_data_cell = elem;
        assert(comp_vid_arr);
        assert(word_data_cell);

        float a = word_data_cell->a / 511.0;
        float b = word_data_cell->b / 50.0;
        float c = word_data_cell->c / 50.0;
        float d = word_data_cell->d / 50.0;

        
        float y1 = a - b - c + d;
        float y2 = a - b + c - d;
        float y3 = a + b - c - d;
        float y4 = a + b + c + d;
        float pb =  Arith40_chroma_of_index((unsigned) word_data_cell->avg_pb);
        float pr =  Arith40_chroma_of_index((unsigned) word_data_cell->avg_pr);

        populate_comp_vid_cell(y1, y2, y3, y4, pb, pr, comp_vid_arr, col, row);
        
}


/********** populate_comp_vid_cell ********
 * 
 * Populates a comp_vid struct with data extracted from the
 * word_info array
 *
 * Inputs:
 *      float y1:                  first y value 
 *      float y2:                  second y value 
 *      float y3:                  third y value 
 *      float y4:                  fourth y value 
 *      float pb:                  pb value 
 *      float pr:                  pr value 
 *      UArray2b_T comp_vid_arr:   comp_vid array
 *      int col:                   Column index of element being populated
 *      int row:                   Row index of element being populated
 * 
 * Expects:
 *      comp_vid_arr to be an array of comp_vid structs
 * 
 * Notes:
 *      allocates a new comp_vid struct for storage which is later freed
 ************************/
void populate_comp_vid_cell(float y1, float y2, float y3, float y4, float pb,
                            float pr, UArray2b_T comp_vid_arr, int col, int row)
{
        A2Methods_T blocked_methods = uarray2_methods_blocked;

        int col1 = col * 2;
        int col2 = col * 2 + 1;
        int row1 = row * 2;
        int row2 = row * 2 + 1;
        
        comp_vid cell1 = blocked_methods->at(comp_vid_arr, col1, row1);
        comp_vid cell2 = blocked_methods->at(comp_vid_arr, col2, row1);
        comp_vid cell3 = blocked_methods->at(comp_vid_arr, col1, row2);
        comp_vid cell4 = blocked_methods->at(comp_vid_arr, col2, row2);

        comp_vid cell1_data = new_comp_vid_cell(pb, pr, y1);
        comp_vid cell2_data = new_comp_vid_cell(pb, pr, y2);
        comp_vid cell3_data = new_comp_vid_cell(pb, pr, y3);
        comp_vid cell4_data = new_comp_vid_cell(pb, pr, y4);

        *cell1 = *cell1_data;
        *cell2 = *cell2_data;
        *cell3 = *cell3_data;
        *cell4 = *cell4_data;

        FREE(cell1_data);
        FREE(cell2_data);
        FREE(cell3_data);
        FREE(cell4_data);
}

/********** new_comp_vid_cell ********
 * 
 * Allocates and populates a new comp_vid struct
 *
 * Inputs:
 *      float y:   y value 
 *      float pb:  pb value 
 *      float pr:  pr value 
 * 
 * Returns:
 *      new initialized comp_vid struct
 * 
 * Expects:
 *      comp_vid_arr to be an array of comp_vid structs
 * 
 * Notes:
 *      allocates a new comp_vid struct for storage which is later freed
 ************************/
comp_vid new_comp_vid_cell(float pb, float pr, float y)
{
        comp_vid new_cell = NEW(new_cell);
        new_cell->pb = pb;
        new_cell->pr = pr;
        new_cell->y = y;

        return new_cell;
}

/********** unpack_n_store ********
 * 
 * Given a file containing a compressed image, unpacks data into an
 *      array of word_info structs
 *
 * Inputs:
 *      FILE *input:      file containing the compressed image
 *      unsigned width:   the width of the image
 *      unsigned height:  the height of the image   
 * 
 * Expects:
 *      The file to be a compressed image properly formatted to the CS40
 *      standards
 *      The width and height to be that of the original image prior to
 *      compression
 * 
 * Notes:
 *      Calls mapping function to populate each word_info struct
 ************************/
UArray2_T unpack_n_store(FILE *input, unsigned width, unsigned height)
{

        A2Methods_T plain_methods = uarray2_methods_plain;
        UArray2_T word_info_arr = plain_methods->new(width / 2, height / 2,
                                                     sizeof(struct word_info));

        plain_methods->map_row_major(word_info_arr, unpack_n_store_app, input);
        
        return word_info_arr;
}

/********** unpack_n_store_app ********
 * 
 * Packs four characters into a word, then unpacks word into a word_info struct
 * 
 * Inputs:
 *      int col:                   Column index
 *      int row:                   Row index
 *      A2Methods_UArray2 array2:  The word_info array being mapped over
 *      void *elem:                The current element in the array
 *      void * cl;                 The file stream holding the compressed image
 * 
 * Expects:
 *      The file to be a compressed image properly formatted to the CS40
 *      standards
 * 
 * Notes:
 *      Calls make_new_word_data to apply bitpack functions
 ************************/
void unpack_n_store_app(int col, int row,A2Methods_UArray2 array2, 
                        void *elem, void *cl)
{
        (void) col;
        (void) row;
        (void) array2;
        FILE *input = cl;
        assert(input);
        word_info word_data = elem;
        assert(word_data);

        uint64_t word = 0;
        int to_pack;

        /* pack 32 bit word with data from 4 chars */
        for (int i = 3; i >= 0; i--) {
                to_pack = getc(input);
                word = Bitpack_newu(word, 8, 8 * i, to_pack);
        }
        
        word_info new_word_data = make_new_word_data(word);

        *word_data = *new_word_data;

        FREE(new_word_data);
}

/********** make_new_word_data ********
 * 
 * Allocates and populates a new word_info struct
 * 
 * Inputs:
 *      uint64_t word:  code word holding data to be packed in word_info struct
 * 
 * Returns:
 *      word_info struct holding data for one code word
 * 
 * Expects:
 *      the word to be packed with data prior to being passed to this function
 * 
 * Notes:
 *      Calls bitpack functions to unpack data into word_info struct
 ************************/
word_info make_new_word_data(uint64_t word)
{
        word_info new_word_data = NEW(new_word_data);

        new_word_data->a = Bitpack_getu(word, 9, 23);
        new_word_data->b = Bitpack_gets(word, 5, 18);
        new_word_data->c = Bitpack_gets(word, 5, 13);
        new_word_data->d = Bitpack_gets(word, 5, 8);
        new_word_data->avg_pb = Bitpack_getu(word, 4, 4);
        new_word_data->avg_pr = Bitpack_getu(word, 4, 0);

        return new_word_data;
}
