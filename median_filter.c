#include "median_filter.h"
#include "array_utility.h"
#include "kernel_filter.h"

// Helper function to find the pixel array for a particular input image.
float* find_pixel_array (float** input, int M_in, int N_in,
    int row, int col, int size, int* array_size)
{
    // Declare array size.
    *array_size = size * size;

    // Declare the pixel array.
    float* array = alloc1df(*array_size);
    int index = 0;

    // Find the coordinates of the center pixel in the pixel matrix.
    int center = (int) floor((double) size / 2);

    // Fill in the surrounding edges relative to the center pixel.
    int i, j;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            int pixel_row = row + (i - center);
            int pixel_col = col + (j - center);

            if (pixel_row < 0) {
                pixel_row = 0;
            }

            if (pixel_row > (M_in - 1)) {
                pixel_row = M_in - 1;
            }

            if (pixel_col < 0) {
                pixel_col = 0;
            }

            if (pixel_col > (N_in - 1)) {
                pixel_col = N_in - 1;
            }

            array[index++] = input[pixel_row][pixel_col];
        }
    }

    return array;
}

// Helper function to find the cross array for a particular input image.
float* find_cross_array (float** input, int M_in, int N_in,
    int row, int col, int width, int height, int* array_size)
{
    // Declare the pixel matrix and the index on which we're currently iterating.
    *array_size = width * 2 + height * 2;
    float* cross_array = alloc1df(*array_size);
    int current_index = 0;

    // Find the horizontal elements of the cross.
    int i, j;
    for (i = 0; i < width * 2; i++) {
        int pixel_row = row + (i - width);
        int pixel_col = col;

        if (pixel_row < 0) {
            pixel_row = 0;
        }

        if (pixel_row > (M_in - 1)) {
            pixel_row = M_in - 1;
        }

        cross_array[current_index++] = input[pixel_row][pixel_col];
    }

    // Find the vertical elements of the cross.
    for (j = 0; j < height * 2; j++) {
        int pixel_row = row;
        int pixel_col = col + (j - height);

        if (pixel_col < 0) {
            pixel_col = 0;
        }

        if (pixel_col > (N_in - 1)) {
            pixel_col = N_in - 1;
        }

        cross_array[current_index++] = input[pixel_row][pixel_col];
    }

    return cross_array;
}

float*** median_filter (float*** input, int M_in, int N_in)
{
    // Prompt the user.
    printf("What shape would you like to use for the median filter's window?");
    printf(" (1) for cross, (2) for square.\n");
    int option;
    scanf("%d", &option);

    float*** output;
    switch (option) {
        case 2:
            printf("Performing a square-shaped median filter...\n");
            output = square_median_filter(input, M_in, N_in);
            break;
        default:
            printf("Performing a cross-shaped median filter...\n");
            output = cross_median_filter(input, M_in, N_in);
            break;
    }

    return output;
}

float*** square_median_filter (float*** input, int height, int width)
{
    // Declare output.
    float*** output = alloc3df(3, height, width);

    // Decide the window of a 2D median filter to be an 3 by 3 pixel array.
    int size = 3;

    int i, j, k, l, m;
    for (k = 0; k < 3; k++) {
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                int array_size;
                float* square = find_pixel_array(input[k], height, width, i,
                    j, size, &array_size);

                // Sort the resulting set of pixels.
                for (m = 0; m < array_size - 1; m++) {
                    for (l = m + 1; l < array_size; l++) {
                        if (square[m] > square[l]) {
                            float swap = square[m];
                            square[m] = square[l];
                            square[l] = swap;
                        }
                    }
                }

                // Replace the pixel that we're evaluating with the median
                // pixel.
                int center = (int) floor((double) array_size / 2);
                output[k][i][j] = square[center];
                free(square);
            }
        }
    }

    return output;
}

inline int get_color_bounded(int x, int y, int color, uint8_t* input, int height, int width) {
    if (x < 0) {
        x = 0;
    }
    if (x > width - 1) {
        x = width - 1;
    }
    if (y < 0) {
        y = 0;
    }
    if (y > height - 1) {
        y = height - 1;
    }
    return input[3 * y * width + 3 * x + color];
}

/** Find the median of the histogram. Only works when all histogram values are ≥ 0 and the histogram has exactly 256 bins
 */
int histogram_median(uint8_t histogram[256], uint8_t* median, int* ltmedian) {
    int head_pos = 0;
    int tail_pos = 255;
    int head_sum = 0;
    int tail_sum = 0;
    while (head_pos < tail_pos + 1) {
        while (histogram[head_pos] == 0){
            head_pos++;
        }
        head_sum += histogram[head_pos++];
        while (tail_sum < head_sum) {
            while (histogram[tail_pos] == 0){
                tail_pos--;
            }
            tail_sum += histogram[tail_pos--];
        }
    }
    *median = head_pos;
    *ltmedian = head_sum;
}

int running_median(uint8_t histogram[256], uint8_t* leftcol, uint8_t* rightcol, uint8_t* median, int* ltmedian) {
    int th = ((2 * RADIUS + 1) * (2 * RADIUS + 1))/2;
    for (int i = 0; i < 2 * RADIUS + 1; i++) {
        uint8_t e = leftcol[i];
        histogram[e]--;
        if (e < *median) (*ltmedian)--;
        e = rightcol[i];
        histogram[e]++;
        if (e < *median) (*ltmedian)++;
    }
    if (*ltmedian > th){
        do {
            (*median)--;
            *ltmedian -= histogram[*median];
        } while (*ltmedian > th);
    } else {
        while (*ltmedian + histogram[*median] <= th) {
            *ltmedian += histogram[*median];
            (*median)++;
        }
    }
}

/**
 * A version of square median filter that uses histograms instead of
 * sorting to find medians. Requires the integer image data from lodepng
 * and NOT the float*** PPM format that is used elsewhere.
 */
uint8_t* square_median_filter_i(unsigned char* input, int height, int width) {
    int radius = RADIUS;
    uint8_t* output = malloc(3 * sizeof(uint8_t) * height * width);
    uint8_t* histogram_r = malloc(sizeof(uint8_t) * 255);
    uint8_t* histogram_g = malloc(sizeof(uint8_t) * 255);
    uint8_t* histogram_b = malloc(sizeof(uint8_t) * 255);
    uint8_t median_r = 0;
    uint8_t median_g = 0;
    uint8_t median_b = 0;
    int ltmedian_r = 0;
    int ltmedian_g = 0;
    int ltmedian_b = 0;
    uint8_t* leftcol = malloc(sizeof(uint8_t) * (2 * radius + 1));
    uint8_t* rightcol = malloc(sizeof(uint8_t) * (2 * radius + 1));
    for (int wy = 0; wy < height; wy++) {
        // Set up array histogram for first window
        int wx = 0;
        // zero out histograms
        memset(histogram_r, 0, 255);
        memset(histogram_g, 0, 255);
        memset(histogram_b, 0, 255);
        for (int x = wx - radius; x <= wx + radius; x++) {
            for (int y = wy - radius; y <= wy + radius; y++){
                histogram_r[get_color_bounded(x, y, 0, input, height, width)]++;
                histogram_g[get_color_bounded(x, y, 1, input, height, width)]++;
                histogram_b[get_color_bounded(x, y, 2, input, height, width)]++;
            }
        }
        histogram_median(histogram_r, &median_r, &ltmedian_r);
        histogram_median(histogram_g, &median_g, &ltmedian_g);
        histogram_median(histogram_b, &median_b, &ltmedian_b);
        // output these medians to image
        output[3 * wy * width + 3 * wx + 0] = median_r;
        output[3 * wy * width + 3 * wx + 1] = median_g;
        output[3 * wy * width + 3 * wx + 2] = median_b;
        for (wx = 1; wx < width; wx++) {
            // put (wx - radius - 1, wy - radius) (wx - radius - 1, wy + radius) into leftcol
            for (int i = 0; i < 2 * radius + 1; i++){
                leftcol[i] = get_color_bounded(wx - radius - 1, wy - radius + i, 0, input, height, width);
            }
            // put (wx + radius, wy - radius) (wx + radius, wy + radius) into rightcol
            for (int i = 0; i < 2 * radius + 1; i++) {
                rightcol[i] = get_color_bounded(wx + radius, wy - radius + i, 0, input, height, width);
            }
            running_median(histogram_r, leftcol, rightcol, &median_r, &ltmedian_r);
            // output median to image
            output[3 * wy * width + 3 * wx + 0] = median_r;
            for (int i = 0; i < 2 * radius + 1; i++){
                leftcol[i] = get_color_bounded(wx - radius - 1, wy - radius + i, 1, input, height, width);
            }
            for (int i = 0; i < 2 * radius + 1; i++) {
                rightcol[i] = get_color_bounded(wx + radius, wy - radius + i, 1, input, height, width);
            }
            running_median(histogram_g, leftcol, rightcol, &median_g, &ltmedian_g);
            // output median to image
            output[3 * wy * width + 3 * wx + 1] = median_g;
            for (int i = 0; i < 2 * radius + 1; i++){
                leftcol[i] = get_color_bounded(wx - radius - 1, wy - radius + i, 2, input, height, width);
            }
            for (int i = 0; i < 2 * radius + 1; i++) {
                rightcol[i] = get_color_bounded(wx + radius, wy - radius + i, 2, input, height, width);
            }
            running_median(histogram_b, leftcol, rightcol, &median_b, &ltmedian_b);
            // output median to image
            output[3 * wy * width + 3 * wx + 2] = median_b;
        }
    }
    free(leftcol);
    free(rightcol);
    free(histogram_r);
    free(histogram_g);
    free(histogram_b);
    return output;
}

float*** cross_median_filter (float*** input, int M_in, int N_in)
{
    // Declare output.
    float*** output = alloc3df(3, M_in, N_in);

    // Decide the window of a 2D median filter to be a cross that extends
    // 4 pixels W and E and 4 pixels N and S.
    int width = 4;
    int height = 4;

    int i, j, k, l, m;
    for (k = 0; k < 3; k++) {
        for (i = 0; i < M_in; i++) {
            for (j = 0; j < N_in; j++) {
                int array_size;
                float* cross = find_cross_array(input[k], M_in, N_in, i, j,
                    width, height, &array_size);

                // Sort the resulting set of pixels.
                for (m = 0; m < array_size - 1; m++) {
                    for (l = m + 1; l < array_size; l++) {
                        if (cross[m] > cross[l]) {
                            float swap = cross[m];
                            cross[m] = cross[l];
                            cross[l] = swap;
                        }
                    }
                }

                // Replace the pixel that we're evaluating with the median
                // pixel.
                int center = (int) floor((double) array_size / 2);
                output[k][i][j] = cross[center];
            }
        }
    }

    return output;
}
