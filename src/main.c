#include "image.h"
#include "kernel.h"
#include "img.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    image *global_img = make_image(3, 3, 1);
    kernel *krn = make_kernel(3, 3);
    image *img = load_image("resources/lena.jpeg");
    image *gray = image_to_gray(img);
    kernel *gaus = kernel_make_gaus(7, 7, 5.0f);
    kernel *sobel_x = kernel_make_sobelx();
    kernel *sobel_y = kernel_make_sobely();

    image *smoothed_img = kernel_convolve_no_border(gray, gaus);

    image *x_grad = kernel_convolve_no_border(gray, sobel_x);
    image *y_grad = kernel_convolve_no_border(gray, sobel_y);

    show_image_cv(gray, "merge", 0);
    show_image_cv(smoothed_img, "smoothed_img", 0);
    show_image_cv(x_grad, "x_grad", 0);
    show_image_cv(y_grad, "y_grad", 0);
}
