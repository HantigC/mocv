#ifndef KERNEL_H
#define KERNEL_H
#include "image.h"
#include "mathx.h"
#include <stdlib.h>
typedef struct kernel {
    int width;
    int height;
    float *data;
} kernel;

enum strategy {
    PAD_0,
    MIRROR,
    FULL,
    NO_BORDER,
};

image *kernel_convolve(image *img, kernel *kernel, enum strategy how,
                       float fill);
image *kernel_convolve_no_border(image *img, kernel *kernel);
float apply_kernel_at(image *img, kernel *krn, int y, int x);

kernel *make_empty_kernel(int height, int width);
kernel *make_kernel(int height, int width);
kernel *kernel_make_sobelx();
kernel *kernel_make_sobely();
kernel *kernel_make_gaus(int height, int width, float sigma);
float kernel_get_value(kernel *krnl, int y, int x);
void kernel_set_value(kernel *krnl, int y, int x, float value);
int kernel_length(kernel *krnl);
kernel* kernel_add(kernel *st_kernel, kernel *nd_kernel, int inplace);
void kernel_fill_inplace(kernel *kernek, float fill_value);

void free_kernel_content(kernel kernel);
void free_kernel(kernel *kernel);
#endif

