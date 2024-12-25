/*******************************************************************************
 *
 *                                  bitpack.c
 *
 *      Assignment: arith
 *      Authors:    Jared Lee (jalee04) and Coby Keren (jkeren01)
 *      Date:       10/24/23
 *
 *      This file contains the functions for the bitpack.c program
 *      The functions in bitpack.c perform the various steps taken to
 *      pack or extract information from a 64 bit int, including masking
 *      specific data fields for editing or retrieval.
 *
 ******************************************************************************/

#include <math.h>

#include "bitpack.h"
#include "assert.h"


/********** Bitpack_fitsu ********
 *
 * checks if a given uint64_t will fit in a given width
 *
 * Inputs:
 *      uint64_t n:     given value to pack
 *      unsigned width: number of bits aloted to n
 * 
 * Return:
 *      a boolean representing whether n will fit in the width
 * 
 * Expects:
 *      width to be less than or equal to 64
 *
 ************************/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= 64);
        return n <= pow(2, width) - 1;
}

/********** Bitpack_fitss ********
 *
 * checks if a given int64_t will fit in a given width
 *
 * Inputs:
 *      uint64_t n:     given value to pack
 *      unsigned width: number of bits aloted to n
 * 
 * Return:
 *      a boolean representing whether n will fit in the width
 * 
 * Expects:
 *      width to be less than or equal to 64
 *
 ************************/
bool Bitpack_fitss(int64_t n, unsigned width)
{
        assert(width <= 64);
        return (n <= pow(2, width-1) - 1) && (n >= -(pow(2, width-1)));
}

/********** Bitpack_getu ********
 *
 * extracts an unsigned value from a word 
 *
 * Inputs:
 *      uint64_t n:     given value to pack
 *      unsigned width: number of bits aloted to n
 *      unsigned lsb:   the location of the least significant bit
 * 
 * Return:
 *      the extracted value as a uint64_t
 * 
 * Expects:
 *      width and width + lsb to be less than or equal to 64
 *
 ************************/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        if (width == 0) {
                return 0;
        }

        uint64_t mask = (((1 << width) - 1) << lsb);
        uint64_t extracted_val = word & mask;

        extracted_val = extracted_val >> lsb;

        return extracted_val;
}

/********** Bitpack_gets ********
 *
 * extracts an signed value from a word 
 *
 * Inputs:
 *      uint64_t n:     given value to pack
 *      unsigned width: number of bits aloted to n
 *      unsigned lsb:   the location of the least significant bit
 * 
 * Return:
 *      the extracted value as a int64_t
 * 
 * Expects:
 *      width and width + lsb to be less than or equal to 64
 *
 ************************/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        assert(width + lsb <= 64);

        if (width == 0) {
                return 0;
        }
        
        int64_t mask = (((1 << width) - 1) << lsb);
        int64_t extracted_val = word & mask;

        extracted_val = extracted_val << (64 - width - lsb);
        extracted_val = extracted_val >> (64 - width);

        return extracted_val;
}

/********** Bitpack_newu ********
 *
 * generates a new word with an updated field
 *
 * Inputs:
 *      uint64_t n:     given value to pack
 *      unsigned width: number of bits aloted to n
 *      unsigned lsb:   the location of the least significant bit
 *      uint64_t value: the value to be inserted into the word
 * 
 * Return:
 *      the new word with the updated field
 * 
 * Expects:
 *      width and width + lsb to be less than or equal to 64
 *
 ************************/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                      uint64_t value)
{
        assert(width <= 64);
        assert(width + lsb <= 64);
        
        uint64_t mask = ~(((1 << width) - 1) << lsb);
        word = word & mask;

        value = value << lsb;

        uint64_t new_word = word | value;

        return new_word;
        
}

/********** Bitpack_news ********
 *
 * generates a new word with an updated field
 *
 * Inputs:
 *      uint64_t n:     given value to pack
 *      unsigned width: number of bits aloted to n
 *      unsigned lsb:   the location of the least significant bit
 *      int64_t value: the value to be inserted into the word
 * 
 * Return:
 *      the new word with the updated field
 * 
 * Expects:
 *      width and width + lsb to be less than or equal to 64
 *
 ************************/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                      int64_t value)
{
        assert(width <= 64);
        assert(width + lsb <= 64);
        
        uint64_t mask = ~(((1 << width) - 1) << lsb);
        word = word & mask;

        uint64_t val_mask = ((1 << width) - 1);
        value = value & val_mask;
        value = value << lsb;

        uint64_t new_word = word | value;

        return new_word;

}

