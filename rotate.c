#include "rotate.h"
#include "array_utility.h"
#include "bilinear_interpolation.h"

#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

#define MULTITHREAD_THRESHOLD 250000

typedef struct {
    float*** input;
    float*** output;
    int outX;
    int outY;
    int outW;
    int outH;
    int M_in;
    int N_in;
    float* xform;
} inv_affine_transform_region_args;
void inv_affine_transform_region(inv_affine_transform_region_args* args);

float*** rotate_degrees (float*** input, int M_in, int N_in, float rotation_deg, int n_threads, rotate_degrees_debug_info* debug)
{
    // If n_threads is 0, make a guess about how many threads to use.
    if (n_threads <= 0) {
        if (M_in * N_in > MULTITHREAD_THRESHOLD) {
            // get number of CPU cores; see http://stackoverflow.com/a/150971
            n_threads = sysconf(_SC_NPROCESSORS_ONLN);
        } else {
            // For small images, threading overhead outweights computation cost.
            n_threads = 1;
        }
    }
    if (debug) {
        debug->n_threads_used = n_threads;
    }

    // compute matrix
    float angle = rotation_deg * (float) Pi / 180;
    float xform[4];
    xform[0] = cosf(angle);
    xform[1] = sinf(angle);
    xform[2] = -xform[1];
    xform[3] = xform[0];

    // start threads
    float*** output = alloc3df(3, M_in, N_in);
    if (n_threads == 1) {
        inv_affine_transform_region_args args = (inv_affine_transform_region_args){ input, output, 0, 0, N_in, M_in, M_in, N_in, xform };
        inv_affine_transform_region(&args);
    } else {
        pthread_t threads[n_threads];
        inv_affine_transform_region_args args[n_threads];
        int i = 0;
        int rowsPerThread = M_in / n_threads;
        for (i = 0; i < n_threads; ++i) {
            int startY = rowsPerThread * i;
            int regionHeight = rowsPerThread;
            if (i == n_threads - 1) {
                regionHeight += M_in % n_threads;
            }
            args[i] = (inv_affine_transform_region_args){ input, output, 0, startY, N_in, regionHeight, M_in, N_in, xform };
            pthread_create(&threads[i], NULL, (void * (*)(void *))inv_affine_transform_region, (void*)&args[i]);
        }
        for (i = 0; i < n_threads; ++i) {
            pthread_join(threads[i], NULL);
        }
    }

    return output;
}

void inv_affine_transform_region(inv_affine_transform_region_args* args) {

    // Extract args.
    float*** input = args->input;
    float*** output = args->output;
    int outX = args->outX;
    int outY = args->outY;
    int outW = args->outW;
    int outH = args->outH;
    int M_in = args->M_in;
    int N_in = args->N_in;
    float* xform = args->xform;

    // Define the center of the image.
    int vertical_center = floor(M_in / 2);
    int horizontal_center = floor(N_in / 2);

    // Loop through each pixel of the new image, select the new vertical
    // and horizontal positions, and interpolate the image to make the change.
    int i, j, k;
    for (i = outY; i < outY + outH; ++i) {
        for (j = outX; j < outX + outW; ++j) {
            // Figure out how rotated we want the image.
            float vertical_position = (float) xform[0] *
                (i - vertical_center) + xform[1] * (j - horizontal_center)
                + vertical_center;
            float horizontal_position = (float) xform[2] *
                (i - vertical_center) + xform[3] * (j - horizontal_center)
                + horizontal_center;

            // Figure out the four locations (and then, four pixels)
            // that we must interpolate from the original image.
            int top = floor(vertical_position);
            int bottom = top + 1;
            int left = floor(horizontal_position);
            int right = left + 1;

            // Check if any of the four locations are invalid. If they are,
            // skip interpolating this pixel. Otherwise, interpolate the
            // pixel according to the dimensions set above and set the
            // resulting pixel.
            if (top >= 0 && bottom < M_in && left >= 0 && right < N_in ) {
                for (k = 0; k < 3; k++) {
                    float interpolated = bilinearly_interpolate(top, bottom,
                        left, right, horizontal_position, vertical_position,
                        input[k]);
                        output[k][i][j] = interpolated;
                }
            }
        }
    }

}

float*** rotate (float*** input, int M_in, int N_in)
{
    // Prompt the user.
    float rotation_factor;
    printf("By how many degrees CCW do you want to rotate your image?\n");
    scanf("%f", &rotation_factor);
    printf("Rotating the input file...\n");

    return rotate_degrees(input, M_in, N_in, rotation_factor, 0, NULL);
}
