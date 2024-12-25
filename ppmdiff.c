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
#include <assert.h>
#include <math.h>

int main(int argc, char *argv[])
{
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);
        
        if (argc != 3) {
                fprintf(stderr, "Requires 2 inputs\n");
                exit(1);
        }
        FILE *input1, *input2;
        input1 = fopen(argv[1], "r");
        input2 = fopen(argv[2], "r");
        assert(input1 && input2);

        Pnm_ppm image = Pnm_ppmread(input1, methods);
        Pnm_ppm image_2 = Pnm_ppmread(input2, methods);

        if(!(abs((int)(image->height - image_2->height)) <= 1 && 
                abs((int)(image->width - image_2->width)) <= 1)) {
                        fprintf(stderr, 
                                "width or height difference greater than 1\n");
                        exit(1);

        }
        
        int small_width = ((image->width < image_2->width) ? image->width : 
                                image_2->width);
        int small_height = ((image->height < image_2->height) ? image->height : 
                                image_2->height);
        
        double numerator = 0;
        float denominator = 3 * small_width * small_height;
        float maxval_1 = image->denominator;
        float maxval_2 = image_2->denominator;
        
        for (int i = 0; i < small_width; i++) {
                for (int j = 0; j < small_height; j++) {
                        Pnm_rgb pixel1 = (image->methods->at(image->pixels, i, 
                                        j));
                        Pnm_rgb pixel2 = (image_2->methods->at(image_2->pixels, 
                                        i, j));
                        numerator += 
                                pow(((pixel1->red)/maxval_1 - (pixel2->red)/maxval_2), 2) +
                                pow(((pixel1->green)/maxval_1 - (pixel2->green)/maxval_2), 2) +
                                pow(((pixel1->blue)/maxval_1 - (pixel2->blue)/maxval_2), 2);
                }  
        }
        float root_mean_sq_dif = sqrt((numerator)/(denominator));
        printf("E = %.4f\n", root_mean_sq_dif);
}