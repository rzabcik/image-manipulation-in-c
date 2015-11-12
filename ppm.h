
#include <stdlib.h>
#include "array_utility.h"

float*** png_to_ppm (unsigned char* image, unsigned width, unsigned height)
{
    float*** img = alloc3df(3, height, width);

    int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            img[0][i][j] = image[3 * i * width + 3 * j + 0]; // Red.
            img[1][i][j] = image[3 * i * width + 3 * j + 1]; // Green.
            img[2][i][j] = image[3 * i * width + 3 * j + 2]; // Blue.
        }
    }

      return img;
}

unsigned char* ppm_to_png (float*** image, unsigned width, unsigned height)
{
    unsigned char* img = (unsigned char*) malloc(3 * width * height *
        sizeof(unsigned char));

    // Perform linear stretch before converting to PNG.
    int i, j, k;

    for (k = 0; k < 3; k++) {
        float min = 9e9;
        float max = -min;

        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                float v = image[k][i][j];
                if (min > v) {
                    min = v;
                }
                if (max < v) {
                    max = v;
                }
            }
        }

        float v = 254.99 / (max - min);

        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                img[3 * i * width + 3 * j + k] = (image[k][i][j] - min) * v;
            }
        }
    }

      return img;
}
