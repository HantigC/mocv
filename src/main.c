#include "image.h"
#include "img.h"
#include "kernel.h"
#include "keypoints/harris.h"
#include "keypoints/keypoint.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

int filter_harris_keypoints(void *void_keypoint) {
    keypoint *keypoint = (struct keypoint *)void_keypoint;
    return keypoint->confidence > 0;
}

int main() {
    image *global_img = make_image(3, 3, 1);
    kernel *krn = make_kernel(3, 3);
    image *img = load_image("resources/chess-board.png");
    image *gray = image_to_gray(img);
    kernel *gaus = kernel_make_gaus(7, 7, 5.0f);
    kernel *sobel_x = kernel_make_sobelx();
    kernel *sobel_y = kernel_make_sobely();

    image *smoothed_img = kernel_convolve_no_border(gray, gaus);

    image *x_grad = kernel_convolve_no_border(gray, sobel_x);
    image *y_grad = kernel_convolve_no_border(gray, sobel_y);

    image *sx_grad = kernel_convolve_no_border(smoothed_img, sobel_x);
    image *sy_grad = kernel_convolve_no_border(smoothed_img, sobel_y);
    list *harris_kps = detect_harris(smoothed_img, 3, 0.06);
    list *harris_corners = list_filter(filter_harris_keypoints, harris_kps);

    image *kp_image = render_keypoints(harris_corners, smoothed_img);

    show_image_cv(gray, "merge", 0);
    show_image_cv(smoothed_img, "smoothed_img", 0);
    show_image_cv(x_grad, "x_grad", 0);
    show_image_cv(y_grad, "y_grad", 0);

    show_image_cv(sx_grad, "sx_grad", 0);
    show_image_cv(sy_grad, "sy_grad", 0);
    show_image_cv(kp_image, "kp_image", 0);
    free_image(global_img);
    free_image(img);
    free_image(gray);
    free_image(smoothed_img);

    free_image(x_grad);
    free_image(y_grad);
    free_image(sx_grad);
    free_image(sy_grad);
    free_image(kp_image);
    free_list(harris_corners);
}
