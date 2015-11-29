
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
        printf("Usage: %s [PNG filename] [opt: number of threads to use]\n", argv[0]);
        exit(1);
    }

    int n_threads = 0;
    if (argc >= 3) {
        n_threads = atoi(argv[2]);
    }

    // Load an image
    unsigned width, height;
    float*** image = load_image(argv[1], &width, &height);

    // Rotate it once (will have a runner run this multiple times so we can see distribution instead of losing that data by averaging here)
    const float angle_deg = 15;
    rotate_degrees_debug_info debug;
    Timer t;
    timer_start(&t);
    float*** output = rotate_degrees(image, height, width, angle_deg, n_threads, &debug);
    timer_stop(&t);

    // Print operation, filename, image width, image height, runtime in nanoseconds, number of threads used.
    printf("rotate,%s,%d,%d,%llu,%d\n", argv[1], width, height, timer_duration_ns(&t), debug.n_threads_used);

    return 0;

}
