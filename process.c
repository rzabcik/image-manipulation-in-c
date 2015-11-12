#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array_utility.h"
#include "rotate.h"
#include "resize.h"
#include "histograms.h"
#include "contrast_enhancement.h"
#include "manipulate_hsi.h"
#include "histogram_equalization.h"
#include "kernel_filter.h"
#include "median_filter.h"
#include "gradient_filter.h"
#include "fourier_filter.h"
#include "edge_detection.h"
#include "lodepng.h"
#include "ppm.h"

int main ()
{
    int action = 1;

    char filename[256];
    char saved_filename[256];

    printf("Type in the name of the PNG file you would like to process.\n");
    scanf("%s", filename);

    setbuf(stdout, NULL);

    // Load the PNG using LodePNG library.
    printf("Loading the file into memory...\n\n\n");
    unsigned error;
    unsigned char* image;
    unsigned width, height;

    error = lodepng_decode24_file(&image, &width, &height, filename);
    if (error) {
        printf("Error (%u) loading file: %s\n", error,
            lodepng_error_text(error));
        exit(1);
    }

    // Then use array to load into a three-dimensional array of R, G,
    // and B values.
    float*** input = png_to_ppm(image, width, height);
    free(image);

    int i, j;
    int output_height, output_width;

    // Begin coding the user prompt menu.
    while (action > 0) {
        float*** output = (float ***) malloc(3 * sizeof(float**));

        printf("Select how you would like to process %s: \n", filename);
        printf("1) Generate a density histogram for the image.\n");
        printf("2) Generate a cumulative histogram for the image.\n");
        printf("3) Improve the image's contrast.\n");
        printf("4) Resize an image's width and height by certain factors.\n");
        printf("5) Rotate an image by some angle (in degrees).\n");
        printf("6) Perform histogram equalization on the image.\n");
        printf("7) Rotate the image's hue.\n");
        printf("8) Increase the image's color saturation.\n");
        printf("9) Increase the image's intensity.\n");
        printf("10) Perform low-pass filtering on an image using a kernel.\n");
        printf("11) Perform high-pass filtering on an image using a kernel.");
        printf("\n");
        printf("12) Perform low-pass or high-pass filtering using a Fourier");
        printf(" transform.\n");
        printf("13) Create a gradient image that accentuates changes in");
        printf(" color based on the given image.\n");
        printf("14) Clean up an image using a median filter.\n");
        printf("15) Detect edges in an image.\n");


        printf("Type in the number of the action you're interested in ");
        printf("(-1 to exit): ");
        scanf("%d", &action);
        printf("----------------------------------------------------\n\n\n");

        // Act according to what the user selected.
        switch (action) {
            case 1:
                output_height = output_width = 256;
                for (i = 0; i < 3; i++) {
                  output[i] = generate_density_histogram(input[i],
                      output_height, height, width);
                }

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_density_histogram.png");
              break;
            case 2:
                output_height = output_width = 256;
                for (i = 0; i < 3; i++) {
                  output[i] = generate_cumulative_histogram(input[i],
                      output_height, height, width);
                }

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_cumu_histogram.png");
                break;
            case 3:
                output_height = height;
                output_width = width;
                output = enhance_contrast(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_with_contrast.png");
                break;
            case 4:
                output = resize(input, height, width, &output_height,
                   &output_width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_resized.png");
                break;
            case 5:
                output_height = height;
                output_width = width;
                output = rotate(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_rotated.png");
                break;
            case 6:
                output_height = height;
                output_width = width;
                output = equalize(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_equalized.png");
                break;
            case 7:
                output_height = height;
                output_width = width;
                output = rotate_hue(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_rotated_hue.png");
                break;
            case 8:
                output_height = height;
                output_width = width;
                output = increase_saturation(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_increased_saturation.png");
                break;
            case 9:
                output_height = height;
                output_width = width;
                output = increase_intensity(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_increased_intensity.png");
                break;
            case 10:
                output_height = height;
                output_width = width;
                output = lowpass_filter(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_lowpass_filter.png");
                break;
            case 11:
                output_height = height;
                output_width = width;
                output = highpass_filter(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_highpass_filter.png");
                break;
            case 12:
                output_height = height;
                output_width = width;
                output = fourier_filter(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_fourier_filter.png");
                break;
            case 13:
                output_height = height;
                output_width = width;
                output = gradient_filter(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_gradient_image.png");
                break;
            case 14:
                output_height = height;
                output_width = width;
                output = median_filter(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_median_filter.png");
                break;
            case 15:
                output_height = height;
                output_width = width;
                output = detect_edges(input, height, width);

                strncpy(saved_filename, filename, 256);
                saved_filename[strlen(saved_filename) - 4] = 0;
                strcat(saved_filename, "_detected_edges.png");
                break;
            default:
                break;
        }

        if (action > 0) {
            // Save the file and indicate what the file was saved as.
            unsigned char* output_png = ppm_to_png(output, output_width,
                output_height);
            dealloc3df(output, 3, output_height, output_width);
            error = lodepng_encode24_file(saved_filename, output_png,
                output_width, output_height);
            if (error) {
                printf("Error (%u) saving file: %s\n\n\n", error,
                    lodepng_error_text(error));
            } else {
                printf("The file was successfully saved as %s.\n\n\n",
                    saved_filename);
            }
        }
    }

    dealloc3df(input, 3, height, width);
}
