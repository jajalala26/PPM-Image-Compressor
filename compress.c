/*******************************************************************************
 *
 *                                  compress.c
 *
 *      Assignment: arith
 *      Authors:    Jared Lee (jalee04) and Coby Keren (jkeren01)
 *      Date:       10/24/23
 *
 *      This file contains the functions for the compress.c program, which
 *      is executed in the compress40 function in 40image.c. The functions   
 *      in compress.c perform the various steps taken to compress a 
 *      full-color portable pixmap image , including trimming the PPM,
 *      quantizing the RGB values, and more. Once the image is compressed
 *      to the CS 40 compressed image format, it is written to standard output.
 *      Errors related to image file formats are handled using the pnm.h 
 *      library.
 *
 ******************************************************************************/

#include "compress.h"

/********** read_n_trim ********
 *
 * Given a Pnm_ppm struct, trims the image to have even dimensions
 * if necessary
 *
 * Inputs:
 *      FILE *fp: Pointer to a FILE stream for the input file.
 * 
 * Return:
 *      Pnm_ppm holding the trimmed image
 * 
 * Expects:
 *      The input file (inputfd) to be a valid pointer to an open input file.
 *      Properly formatted ppm in the input file 
 * 
 * Notes:
 *      Memory is allocated if the image is trimmed, it is freed elsewhere
 ************************/
Pnm_ppm read_n_trim(FILE *fp)
{
        A2Methods_T methods = uarray2_methods_blocked;
        Pnm_ppm image = Pnm_ppmread(fp, methods);
        bool changed = false;
        
        if (image->width % 2 == 1) {
                image->width = image->width - 1;
                changed = true;
        }

        if (image->height % 2 == 1) {
                image->height = image->height - 1;
                changed = true; 
        }

        /* if image is trimmed allocate new array with updated dimensions */
        if (changed) {
                int blocksize = 2;
                UArray2b_T new_pixels = methods->new_with_blocksize(image->width,
                                                     image->height, 
                                                     sizeof(struct Pnm_rgb),
                                                     blocksize);
                
                methods->map_block_major(new_pixels, read_n_trim_app, image);
                methods->free(&(image->pixels));
                image->pixels = new_pixels;
        }
         
        return image;
}

/********** read_n_trim_app ********
 *
 * Populate an array representing a trimmed image with data from the original
 * image
 *
 * Inputs:
 *      int col:                   Column index
 *      int row:                   Row index
 *      A2Methods_UArray2 array2:  The new array being populated
 *      void *elem:                The current element in the array
 *      void * cl;                 The original image array from which data
 *                                 is copied
 * 
 * Expects:
 *      All pointers to not be null
 *      The array being mapped over to have dimensions <= the dimensions of
 *      the array from which data is copied
 * 
 * Notes:
 *      Upon completion the new array allocated in read_n_trim is fully 
 *      populated
 ************************/
void read_n_trim_app(int col, int row, A2Methods_UArray2 array2, void *elem, 
                        void *cl)
{
        (void) array2;
        Pnm_rgb new_pix_cell = elem;
        Pnm_ppm image = cl;
        assert(new_pix_cell);
        assert(image);

        Pnm_rgb old_pix_cell = image->methods->at(image->pixels, col, row);
        *new_pix_cell = *old_pix_cell;
        
}

/********** rgb_to_comp_vid ********
 *
 * Given an image in rgb format, populate an array that represents that 
 * image in component video format
 *
 * Inputs:
 *      Pnm_ppm original image: The image being converted to component video
 * 
 * Return:
 *      UArray2b_T that represents the image in component video format
 * 
 * Expects:
 *      A valid Pnm_ppm object
 * 
 * Notes:
 *      Memory is allocated for the component video array, it is freed elsewhere
 *      A mapping function is called to populate the component video array
 ************************/
UArray2b_T rgb_to_comp_vid(Pnm_ppm original_image)
{
        A2Methods_T methods = uarray2_methods_blocked;
        int blocksize = 2;
        /* allocate 2d blocked array of comp_vid structs */
        UArray2b_T comp_vid_array = methods->new_with_blocksize(
                                                original_image->width,
                                                original_image->height,
                                                sizeof(struct comp_vid),
                                                blocksize);
        
        meth_bundle comp_vid_info = NEW(comp_vid_info);
        comp_vid_info->array = comp_vid_array;
        comp_vid_info->methods = methods;
        comp_vid_info->denominator = original_image->denominator;

        methods->map_block_major(original_image->pixels, rgb_to_comp_vid_app, 
                                 comp_vid_info);

        comp_vid_array = comp_vid_info->array;
        FREE(comp_vid_info);
        return comp_vid_array;
}

/********** rgb_to_comp_vid_app ********
 * 
 * Converts a pixel from rgb to component video format and stores
 * it in its respective location in the image array
 *
 * Inputs:
 *      int col:                   Column index
 *      int row:                   Row index
 *      A2Methods_UArray2 array2:  the original array holding rbg structs
 *      void *elem:                The current element in the array
 *      void * cl;                 A meth_bundle struct holding the component
 *                                 video array, its denominator, and methods
 * 
 * Expects:
 *      All pointers to not be null, the array being mapped to be in
 *      rgb format
 * 
 * Notes:
 *      Upon completion the component video array is fully populated
 *      A new comp_vid struct is allocated to store data and later freed
 ************************/
void rgb_to_comp_vid_app(int col, int row, A2Methods_UArray2 array2, 
                         void *elem, void *cl)
{
       
        (void) array2;
        Pnm_rgb rgb_vals = elem;
        meth_bundle comp_vid_info = cl;
        assert(rgb_vals);
        assert(comp_vid_info);

        float denom = (float)comp_vid_info->denominator;

        float r = (float)rgb_vals->red / denom;
        float g = (float)rgb_vals->green / denom;
        float b = (float)rgb_vals->blue / denom;

        float y = 0.299 * r + 0.587 * g + 0.114 * b;
        float pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
        float pr = 0.5 * r - 0.418688 * g - 0.081312 * b;


        comp_vid comp_vid_cell = NEW(comp_vid_cell);
        comp_vid_cell->y = y;
        comp_vid_cell->pb = pb;
        comp_vid_cell->pr = pr;
        
        comp_vid array_cell = comp_vid_info->methods->at(comp_vid_info->array, 
                                                         col, row); 
        *array_cell = *comp_vid_cell;

        FREE(comp_vid_cell);
}


/********** init_word_info_arr ********
 *
 * Allocates and initializes an array of word_info structs that will later
 * hold all the data that needs to be packed into a 32 bit word
 *
 * Inputs:
 *      UArray2b_T comp_vid_image: The image being compressed
 * 
 * Return:
 *      UArray2_T that holds all the word data for an image
 * 
 * Expects:
 *      Comp_vid_image to be an array of comp_vid structs
 * 
 * Notes:
 *      Memory is allocated for the word_info array, it is freed elsewhere
 *      A mapping function is called to initialize the word_info array
 ************************/
UArray2_T init_word_info_arr(UArray2b_T comp_vid_image) 
{
        A2Methods_T methods = uarray2_methods_blocked;
        int blocksize = 2;
        /* height and width are halved as each element now represents a 
           2x2 block */
        int width_in_blocks = methods->width(comp_vid_image) / blocksize;
        int height_in_blocks = methods->height(comp_vid_image) / blocksize;
        
        methods =  uarray2_methods_plain;
        
        UArray2_T word_info_arr = methods->new(width_in_blocks, 
                                               height_in_blocks, 
                                               sizeof(struct word_info));

        methods->map_row_major(word_info_arr, init_word_info_arr_app, NULL);

        return word_info_arr;
}


/********** init_word_info_arr_app ********
 * 
 * Initializes all word_info structs in an array to be 0
 *
 * Inputs:
 *      int col:                   Column index
 *      int row:                   Row index
 *      A2Methods_UArray2 array2:  The word_info array
 *      void *elem:                The current element in the array
 *      void * cl;                 NULL
 * 
 * Expects:
 *      Elem to not be null
 * 
 * Notes:
 *      Upon completion the word_info array is fully initialized
 ************************/
void init_word_info_arr_app(int col, int row, A2Methods_UArray2 array2, 
                            void *elem, void *cl)
{
        (void) col;
        (void) row;
        (void) array2;
        (void) cl;
        word_info word_data = NEW(word_data);
        word_data->avg_pb = 0;
        word_data->avg_pr = 0;
        word_data->a = 0;
        word_data->b = 0;
        word_data->c = 0;
        word_data->d = 0;

        word_info info_cell = elem;
        assert(info_cell);
        *info_cell = *word_data;
        FREE(word_data);
}


/********** populate_word_info ********
 *
 * Populates an array of word_info structs with data extracted from
 * a component video image
 *
 * Inputs:
 *      UArray2b_T comp_vid_image: The image being compressed
 * 
 * Return:
 *      UArray2_T that holds the all the word data for an image
 * 
 * Expects:
 *      Comp_vid_image to be an array of comp_vid structs
 * 
 * Notes:
 *      Memory is allocated for the word_info array, it is freed elsewhere
 *      A mapping function is called to initialize the word_info array
 ************************/
void populate_word_info(UArray2b_T comp_vid_image, UArray2_T word_info_arr)
{
        A2Methods_T blocked_methods = uarray2_methods_blocked;
        A2Methods_T plain_methods = uarray2_methods_plain;

        meth_bundle word_info_bundle = NEW(word_info_bundle);
        word_info_bundle->methods = plain_methods;
        word_info_bundle->array = word_info_arr;
        word_info_bundle->denominator = 0;

        blocked_methods->map_block_major(comp_vid_image, populate_word_info_app, 
                                         word_info_bundle);
        
        word_info_arr = word_info_bundle->array;
        FREE(word_info_bundle);
}


/********** populate_word_info_app ********
 * 
 * Populates a word_info element with data extracted from a component video
 * array
 *
 * Inputs:
 *      int col:                   Column index
 *      int row:                   Row index
 *      A2Methods_UArray2 array2:  The comp vid array being mapped over
 *      void *elem:                The current element in the array
 *      void * cl;                 Meth_bundle holding the word info array, 
 *                                 the denominator which is used as a counter,
 *                                 and the methods
 * 
 * Expects:
 *      No pointers to be null, the array being mapped to be in component
 *      video format
 * 
 * Notes:
 *      Upon completion the word_info array is populated, but is not complete
 *      as some final operations need to be made
 ************************/
void populate_word_info_app(int col, int row, A2Methods_UArray2 array2, 
                            void *elem, void *cl)
{
        (void) array2;
        meth_bundle word_info_bundle = cl;
        comp_vid comp_vid_pixel = elem;
        assert(word_info_bundle);
        assert(comp_vid_pixel);
        
        pop_pb_pr(comp_vid_pixel, word_info_bundle, col, row);
        pop_abcd(comp_vid_pixel, word_info_bundle, col, row);

        /* adjust counter so we know which element in a 2x2 block we are on */
        word_info_bundle->denominator++;
        if (word_info_bundle->denominator == 4){
                word_info_bundle->denominator = 0;
        }
        
}

/********** pop_pb_pr ********
 * 
 * populates the avg_pb and avg_pr fields in an in instance of a curr_word 
 *      struct with the four pb and pr values in a given 2x2 block
 *
 * Inputs:
 *      int col:                       Column index
 *      int row:                       Row index
 *      comp_vid comp_vid_pixel:       An instance of a comp_vid struct 
 *                                     representing the values of a pixel
 *      meth_bundle word_info_bundle:  holds array of word_info structs and 
 *                                     methods
 * 
 * Expects:
 *      Elem to not be null
 * 
 * Notes:
 *      Upon completion avg_pb and avg_pr have the sums of the pb, pr values
 *      of a 2x2 block
 ************************/
void pop_pb_pr(comp_vid comp_vid_pixel, meth_bundle word_info_bundle, 
               int col, int row)
{

        int bsz = 2;
        UArray2_T word_info_array = word_info_bundle->array;

        word_info curr_word = word_info_bundle->methods->at(word_info_array,
                                                            col / bsz, 
                                                            row / bsz);
        
        curr_word->avg_pb += comp_vid_pixel->pb;
        curr_word->avg_pr += comp_vid_pixel->pr;
        
}

/********** pop_abcd ********
 * 
 * populates the a,b,c, and d fields in an in instance of a word_info 
 *      struct with the corresponding Y values 
 *
 * Inputs:
 *      int col:                       Column index
 *      int row:                       Row index
 *      comp_vid comp_vid_pixel:       An instance of a comp_vid struct 
 *                                     representing the values of a pixel
 *      meth_bundle word_info_bundle:  holds array of word_info structs and 
 *                                     methods
 * 
 * Expects:
 *      Elem to not be null
 * 
 * Notes:
 *      Upon completion a,b,c, andd will have the correct sums of the 4 Y
 *      values of a 2x2 block
 ************************/
void pop_abcd(comp_vid comp_vid_pixel, meth_bundle word_info_bundle, 
               int col, int row)
{

        int bsz = 2;
        UArray2_T word_info_array = word_info_bundle->array;

        word_info curr_word = word_info_bundle->methods->at(word_info_array,
                                                            col / bsz, 
                                                            row / bsz);

        if ((int)word_info_bundle->denominator == 0) {
                curr_word->a += comp_vid_pixel->y;
                curr_word->b -= comp_vid_pixel->y;
                curr_word->c -= comp_vid_pixel->y;
                curr_word->d += comp_vid_pixel->y;
        } else if ((int)word_info_bundle->denominator == 1) {
                curr_word->a += comp_vid_pixel->y;
                curr_word->b += comp_vid_pixel->y;
                curr_word->c -= comp_vid_pixel->y;
                curr_word->d -= comp_vid_pixel->y;
        } else if ((int)word_info_bundle->denominator == 2) {
                curr_word->a += comp_vid_pixel->y;
                curr_word->b -= comp_vid_pixel->y;
                curr_word->c += comp_vid_pixel->y;
                curr_word->d -= comp_vid_pixel->y;
        } else {
                curr_word->a += comp_vid_pixel->y;
                curr_word->b += comp_vid_pixel->y;
                curr_word->c += comp_vid_pixel->y;
                curr_word->d += comp_vid_pixel->y;
        }
}

/********** finalize_word_info ********
 * 
 * Sets A2Methods_T and calls maps over the word info array while calling
 *      finalize_word_info_app
 *
 * Inputs:
 *      UArray2_T word_info_arr: an array containing word_info structs
 * 
 * Expects:
 *      Elem to not be null
 * 
 * Notes:
 *      Upon completion, word_info structs will contain the correct quantized
 *      values for each field
 ************************/
void finalize_word_info(UArray2_T word_info_arr)
{
        A2Methods_T methods = uarray2_methods_plain;

        methods->map_row_major(word_info_arr, finalize_word_info_app, NULL);
}

/********** final_word_info_app ********
 * 
 * populates the word_info structs with the quantized values for each field
 *
 * Inputs:
 *      int col:                   Column index
 *      int row:                   Row index
 *      A2Methods_UArray2 array2:  The comp vid array being mapped over
 *      void *elem:                The current element in the array
 *      void * cl;                 Meth_bundle holding the word info array, 
 *                                 the denominator which is used as a counter,
 *                                 and the methods
 * 
 * Expects:
 *      Elem to not be null
 * 
 * Notes:
 *      Upon completion, word_info structs will contain the correct quantized
 *      values for each field
 *      Calls quantize_bcd()
 ************************/
void finalize_word_info_app(int col, int row, A2Methods_UArray2 array2, 
                            void *elem, void *cl)
{
        (void) col;
        (void) row;
        (void) array2;
        (void) cl;

        word_info cell = elem;
        assert(cell);

        cell->avg_pb = Arith40_index_of_chroma(cell->avg_pb / 4.0);
        cell->avg_pr = Arith40_index_of_chroma(cell->avg_pr / 4.0);
        cell->a = (unsigned)((cell->a / 4.0) * 511);
        cell->b = quantize_bcd(cell->b);
        cell->c = quantize_bcd(cell->c);
        cell->d = quantize_bcd(cell->d);
        
}

/********** quantize_bcd ********
 * 
 * quantizes a given value and returns the resulting integer
 *
 * Inputs:
 *      float val: the value to be quantized (b,c,d)
 * 
 * Expects:
 *      Elem to not be null
 ************************/
int quantize_bcd(float val)
{
        val = val / 4.0;
        if (val > 0.3) {
                val = 0.3;
        } else if (val < -0.3) {
                val = -0.3;
        }
        
        return val *= 50;
}

/********** pack_n_print ********
 * 
 * Sets A2Methods_T and calls maps over the word info array while calling
 *      pack_n_print_app
 *
 * Inputs:
 *      UArray2_T word_info_arr: an array containing word_info structs
 * 
 * Expects:
 *      Elem to not be null
 * 
 * Notes:
 *      Upon completion, data in the fields of a word_info_struct will be
 *      packed into a word, and then that word will be printed
 ************************/
void pack_n_print(UArray2_T word_info_arr)
{
        A2Methods_T plain_methods = uarray2_methods_plain;

        plain_methods->map_row_major(word_info_arr, pack_n_print_app, NULL);
        
}

/********** pack_n_print_app ********
 * 
 * packs fields from word_info_struct into a word, and then print that word
 *
 * Inputs:
 *      int col:                   Column index
 *      int row:                   Row index
 *      A2Methods_UArray2 array2:  The comp vid array being mapped over
 *      void *elem:                The current element in the array
 *      void * cl;                 Meth_bundle holding the word info array, 
 *                                 the denominator which is used as a counter,
 *                                 and the methods
 * 
 * Expects:
 *      Elem to not be null
 * 
 * Notes:
 *      Upon completion, data in the fields of a word_info_struct will be
 *      packed into a word, and then that word will be printed
 ************************/
void pack_n_print_app(int col, int row, A2Methods_UArray2 array2, 
                      void *elem, void *cl)
{
        (void) col;
        (void) row;
        (void) array2;
        (void) cl;
        word_info word_data = elem;
        assert(word_data);
        uint64_t word = 0;

        assert(Bitpack_fitsu(word_data->avg_pr, 4));
        word = Bitpack_newu(word, 4, 0, word_data->avg_pr);

        assert(Bitpack_fitsu(word_data->avg_pb, 4));
        word = Bitpack_newu(word, 4, 4, word_data->avg_pb);

        assert(Bitpack_fitss(word_data->d, 5));
        word = Bitpack_news(word, 5, 8, word_data->d);

        assert(Bitpack_fitss(word_data->c, 5));
        word = Bitpack_news(word, 5, 13, word_data->c);

        assert(Bitpack_fitss(word_data->b, 5));
        word = Bitpack_news(word, 5, 18, word_data->b);

        assert(Bitpack_fitsu(word_data->a, 9));
        word = Bitpack_news(word, 9, 23, word_data->a);

        uint64_t finalword;
        for (int i = 3; i >= 0; i--) {
                finalword = Bitpack_getu(word, 8, 8 * i);
                putchar(finalword);
        }
}