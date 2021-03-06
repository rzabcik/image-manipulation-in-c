
#include "lodepng.h"
#include "ppm.h"
#include "array_utility.h"
#include "median_filter.h"
#include "timer.h"
#include <stdio.h>
#include <stdint.h>

float*** load_image(char* filename, unsigned* width, unsigned* height) {

    unsigned error;
    unsigned char* image;

    error = lodepng_decode24_file(&image, width, height, filename);
    if (error) {
        printf("Error (%u) loading file: %s\n", error,
            lodepng_error_text(error));
        exit(1);
    }

    float*** input = png_to_ppm(image, *width, *height);
    free(image);

    return input;

}

int main(int argc, char** argv) {

    if (argc < 2) {
        printf("Usage: %s [PNG filename]\n", argv[0]);
        exit(1);
    }

    // Load an image
    unsigned error;
    unsigned char* image;
    unsigned width, height;

    error = lodepng_decode24_file(&image, &width, &height, argv[1]);
    if (error) {
        printf("Error (%u) loading file: %s\n", error,
            lodepng_error_text(error));
        exit(1);
    }

    // Perform the median filter on it once (will have a runner run this multiple times so we can see distribution instead of losing that data by averaging here)
    Timer t;
    timer_start(&t);
    uint8_t* output = square_median_filter_i(image, height, width);
    timer_stop(&t);
    lodepng_encode24_file("/u/ryanz/perf/mfoutput.png", output, width, height);

    // Print operation, filename, image width, image height, runtime in nanoseconds.
    printf("median_filter,%s,%d,%d,%llu\n", argv[1], width, height, timer_duration_ns(&t));

    free(image);

}
