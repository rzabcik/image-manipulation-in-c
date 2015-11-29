
#include "lodepng.h"
#include "ppm.h"
#include "rotate.h"
#include "array_utility.h"
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

    // Rotate it once (will have a runner run this multiple times so we can see distribution instead of losing that data by averaging here)
    const float angle_deg = 15;
    Timer t;
    timer_start(&t);
    float*** output = rotate_degrees(image, height, width, angle_deg, 0);
    timer_stop(&t);

    // Print operation, filename, image width, image height, runtime in nanoseconds.
    printf("rotate,%s,%d,%d,%llu\n", argv[1], width, height, timer_duration_ns(&t));

    return 0;

}
