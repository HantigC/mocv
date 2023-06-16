#include "kernel.h"

kernel *make_empty_kernel(int height, int width) {
    kernel *ker = (kernel *)malloc(sizeof(kernel));
    ker->width = width;
    ker->height = height;
    return ker;
}

kernel *make_kernel(int height, int width) {
    kernel *ker = make_empty_kernel(height, width);
    ker->data = (float *)calloc(width * height, sizeof(float));
    return ker;
}

kernel *make_gaus(int height, int width) {
    kernel *gaus = make_kernel(height, width);
    return gaus;
}

kernel *make_sobelx() {
    kernel *sobel = (kernel *)malloc(sizeof(kernel));
    sobel->width = 3;
    sobel->height = 3;
    sobel->data = (float *)calloc(9, sizeof(float));
    return sobel;
}
