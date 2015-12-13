
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

#define max(a, b) ((a) > (b) ? (a) : (b))

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
    debug.times_per_thread = (unsigned long long*)calloc(n_threads, sizeof(unsigned long long));
    Timer t;
    timer_start(&t);
    float*** output = rotate_degrees(image, height, width, angle_deg, n_threads, &debug);
    timer_stop(&t);

    // get max per-thread time
    unsigned long long maxPerThreadTime = 0;
    int i;
    for (i = 0; i < n_threads; ++i) {
        maxPerThreadTime = max(maxPerThreadTime, debug.times_per_thread[i]);
    }

    // Print operation, filename, image width, image height, runtime in nanoseconds, number of threads used
    // As per recommendation, we report the maximum per-thread time instead of the time it takes overall since pthread_join delays.
    printf("rotate,%s,%d,%d,%llu,%d\n", argv[1], width, height, maxPerThreadTime/*timer_duration_ns(&t)*/, debug.n_threads_used);

    return 0;

}
