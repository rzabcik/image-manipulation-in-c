#ifndef MEDIAN_FILTER_H
#define MEDIAN_FILTER_H

#include <math.h>
#include <stdlib.h> // silence implicit definition of free() warning.
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define RADIUS 3

float* find_cross_array (float** input, int M_in, int N_in, int row, int col,
    int width, int height, int* array_size);
float* find_pixel_array (float** input, int M_in, int N_in,
    int row, int col, int size, int* array_size);
float*** median_filter (float*** input, int M_in, int N_in);
float*** square_median_filter (float*** input, int M_in, int N_in);
uint8_t* square_median_filter_i(unsigned char* input, int height, int width);
uint8_t* square_median_filter_threaded(uint8_t* input, int height, int width, int nthreads);
float*** cross_median_filter (float*** input, int M_in, int N_in);

#endif
