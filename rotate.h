#ifndef ROTATE_H
#define ROTATE_H

#define Pi 3.141592653589

#include "bilinear_interpolation.h"

float*** rotate_degrees (float*** input, int M_in, int N_in, float rotation_deg, int n_threads);
float*** rotate (float*** input, int M_in, int N_in);

#endif
