#ifndef ROTATE_H
#define ROTATE_H

#define Pi 3.141592653589

#include "bilinear_interpolation.h"

typedef struct {
	int n_threads_used;
} rotate_degrees_debug_info;

float*** rotate_degrees (float*** input, int M_in, int N_in, float rotation_deg, int n_threads, rotate_degrees_debug_info* debug);
float*** rotate (float*** input, int M_in, int N_in);

#endif
