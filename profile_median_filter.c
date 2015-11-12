
#include "lodepng.h"
#include "ppm.h"
#include "array_utility.h"
#include "median_filter.h"
#include "timer.h"
#include <stdio.h>

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
    unsigned width, height;
    float*** image = load_image(argv[1], &width, &height);
    int repetitions = 100;
    unsigned long long total_duration = 0;
    Timer t;
    int i = 0;
    for (i = 0; i < repetitions; ++i) {
        timer_start(&t);
        float*** output = square_median_filter(image, height, width);
        timer_stop(&t);
        total_duration += timer_duration_ns(&t);
        dealloc3df(output, 3, height, width);
    }
    total_duration /= repetitions;

    // Print filename, image width, image height, average runtime in nanoseconds.
    printf("%s,%d,%d,%llu\n", argv[1], width, height, total_duration);

}
