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

kernel *kernel_make_gaus(int height, int width, float sigma) {
    kernel *gaus = make_kernel(height, width);
    int center_x = width / 2;
    int center_y = height / 2;
    int centered_x = 0.0f;
    int centered_y = 0.0f;
    float acc = 0.0f;
    float value = 0.0f;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            centered_y = y - center_y;
            centered_x = x - center_x;
            value = gaussian(centered_y * centered_y + centered_x * centered_x,
                             0, sigma);

            kernel_set_value(gaus, y, x, value);
            acc += value;
        }
    }
    normalize(gaus->data, kernel_length(gaus), acc, 1);
    return gaus;
}
int kernel_compute_location(kernel *krn, int y, int x) {
    return y * krn->width + x;
}

float kernel_get_value(kernel *krnl, int y, int x) {
    int location = kernel_compute_location(krnl, y, x);
    return krnl->data[location];
}
void kernel_set_value(kernel *krnl, int y, int x, float value) {
    int location = kernel_compute_location(krnl, y, x);
    krnl->data[location] = value;
}

kernel *kernel_make_sobelx() {
    kernel *sobel = (kernel *)malloc(sizeof(kernel));
    sobel->width = 3;
    sobel->height = 3;
    sobel->data = (float *)calloc(9, sizeof(float));
    // 1st row
    kernel_set_value(sobel, 0, 0, -1);
    kernel_set_value(sobel, 0, 1, 0);
    kernel_set_value(sobel, 0, 2, 1);

    // 2nd row
    kernel_set_value(sobel, 1, 0, -2);
    kernel_set_value(sobel, 1, 1, 0);
    kernel_set_value(sobel, 1, 2, 2);

    // 3rd row
    kernel_set_value(sobel, 2, 0, -1);
    kernel_set_value(sobel, 2, 1, 0);
    kernel_set_value(sobel, 2, 2, 1);
    return sobel;
}

kernel *kernel_make_sobely() {
    kernel *sobel = (kernel *)malloc(sizeof(kernel));
    sobel->width = 3;
    sobel->height = 3;
    sobel->data = (float *)calloc(9, sizeof(float));
    // 1st row
    kernel_set_value(sobel, 0, 0, -1);
    kernel_set_value(sobel, 0, 1, -2);
    kernel_set_value(sobel, 0, 2, -1);

    // 2nd row
    kernel_set_value(sobel, 1, 0, 0);
    kernel_set_value(sobel, 1, 1, 0);
    kernel_set_value(sobel, 1, 2, 0);

    // 3rd row
    kernel_set_value(sobel, 2, 0, 1);
    kernel_set_value(sobel, 2, 1, 2);
    kernel_set_value(sobel, 2, 2, 1);
    return sobel;
}

float apply_kernel_at(image *img, kernel *krn, int y, int x) {
    float acc = 0.0f;
    int half_h = (int)krn->height / 2;
    int half_w = (int)krn->width / 2;
    for (int i = 0; i < krn->height; ++i) {
        for (int j = 0; j < krn->width; ++j) {
            acc += kernel_get_value(krn, i, j) *
                   get_pixel(img, i - half_h + y, j - half_w + x, 0);
        }
    }
    return acc;
}

float fill_apply_kernel_at(image *img, kernel *krn, int y, int x,
                           enum strategy how, float fill) {
    float acc = 0.0f;
    int half_h = (int)krn->height / 2;
    int half_w = (int)krn->width / 2;
    int coord_x = 0;
    int coord_y = 0;
    float pixel_value = 0;
    for (int i = 0; i < krn->height; ++i) {
        for (int j = 0; j < krn->width; ++j) {
            coord_y = i - half_h + y;
            coord_x = j - half_w + x;
            if (!IS_BETWEEN(coord_y, 0, img->height - 1) ||
                !IS_BETWEEN(coord_x, 0, img->width - 1)) {
                switch (how) {
                case PAD_0:
                    pixel_value = 0.0f;
                    break;
                case MIRROR:
                    coord_y = CLAMP(coord_y, 0, img->height - 1);
                    coord_x = CLAMP(coord_x, 0, img->width - 1);
                    pixel_value = get_pixel(img, coord_y, coord_x, 0);
                    break;
                default:
                    continue;
                    break;
                }
            } else {
                pixel_value = get_pixel(img, coord_y, coord_x, 0);
            }

            acc += kernel_get_value(krn, i, j) * pixel_value;
        }
    }
    return acc;
}

image *kernel_convolve_no_border(image *img, kernel *kernel) {

    int half_w = kernel->width / 2;
    int half_h = kernel->height / 2;
    image *dest =
        make_image(img->height - half_h + 1, img->width - half_w + 1, 1);
    for (int y = 0; y < img->height - half_h; ++y) {
        for (int x = 0; x < img->width - half_w; ++x) {
            set_pixel(dest, y, x, 0,
                      apply_kernel_at(img, kernel, y + half_h, x + half_w));
        }
    }
    return dest;
}

image *kernel_convolve(image *img, kernel *kernel, enum strategy how,
                       float fill) {
    image *dest = make_image_like(img);
    for (int y = 1; y < img->height - 1; ++y) {
        for (int x = 1; x < img->width - 1; ++x) {
            set_pixel(dest, y, x, 0, apply_kernel_at(img, kernel, y, x));
        }
    }
    return dest;
}

int kernel_length(kernel *krnl) { return krnl->width * krnl->height; }
kernel *kernel_add(kernel *st_kernel, kernel *nd_kernel, int inplace) {
    kernel *dest;
    if (inplace) {
        dest = st_kernel;
    } else {
        dest = (kernel *)malloc(sizeof(kernel));
    }
    for (int i = 0; i < kernel_length(st_kernel); ++i) {
        dest->data[i] = st_kernel->data[i] + nd_kernel->data[i];
    }
    return dest;
}

void kernel_fill_inplace(kernel *kernel, float fill_value) {
    for (int i = 0; i < kernel_length(kernel); ++i) {
        kernel->data[i] = fill_value;
    }
}

void free_kernel_content(kernel kernel) {
    free(kernel.data);
}
void free_kernel(kernel *kernel){
    free(kernel->data);
    free(kernel);
}
