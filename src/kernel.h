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

image *convolve(image *img, kernel *kernel);

kernel *make_empty_kernel(int height, int width);
kernel *make_kernel(int height, int width);
kernel *make_sobelx();
kernel *make_sobely();
kernel *make_gaus(int height, int width);
#endif
