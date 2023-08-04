#include "image.h"
#include "image_draw.h"
#include "image_op.h"
#include "image_stats.h"
#include "img.h"
#include "kernel.h"
#include "keypoints/harris.h"
#include "keypoints/keypoint.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

int filter_harris_keypoints(void *void_keypoint) {
    keypoint *keypoint = (struct keypoint *)void_keypoint;
    return keypoint->confidence > 0.0f;
}

int main() {
    image *img = load_image("resources/chess-board.png");

    image *img256 = image_muls(img, 255.0f);
    image *gray = image_to_gray(img256);
    kernel *gaus = kernel_make_gaus(7, 7, 5.0f);
    kernel *sobel_x = kernel_make_sobelx();
    kernel *sobel_y = kernel_make_sobely();

    image *smoothed_img = kernel_convolve_no_border(gray, gaus);

    image *x_grad = kernel_convolve_no_border(gray, sobel_x);
    image *y_grad = kernel_convolve_no_border(gray, sobel_y);

    image *sx_grad = kernel_convolve_no_border(smoothed_img, sobel_x);
    image *sy_grad = kernel_convolve_no_border(smoothed_img, sobel_y);
    list *harris_kps = detect_harris(smoothed_img, 3, 0.06);
    // list *harris_corners = list_filter(filter_harris_keypoints, harris_kps);

    image *kp_image = render_keypoints(harris_kps, smoothed_img);
    image_min_max_norm_(kp_image);

    //fill_rectangle_yxhw_(img, 10, 10, 3000, 20,
    //                     make_rgb_color(1.0f, 0.0f, 0.0f));
    show_image_cv(image_muls(gray, 1.0f / (255.0f)), "gray", 0);
    show_image_cv(img, "image", 0);
    show_image_cv(image_muls(smoothed_img, 1.0f / (255.0f)), "smoothed_img", 1);
    histogram *hist = compute_gray_image_hist(gray, 0);
    print_hist(hist);
    image *hist_imgs =
        render_histogram(hist, 225, 500, make_rgb_color(0.0f, 0.0f, 1.0f));

    show_image_cv(hist_imgs, "hist", 1);
    show_image_cv(image_muls(x_grad, 1.0f / (255.0f)), "x_grad", 1);
    show_image_cv(image_muls(y_grad, 1.0f / (255.0f)), "y_grad", 1);

    show_image_cv(image_muls(sx_grad, 1.0f / 255.f), "sx_grad", 1);
    show_image_cv(image_muls(sy_grad, 1.0f / 255.f), "sy_grad", 1);
    show_image_cv(image_muls(kp_image, 1.0f / 255.f), "kp_image", 0);
    free_image(img);
    free_image(gray);
    free_image(smoothed_img);

    free_image(x_grad);
    free_image(y_grad);
    free_image(sx_grad);
    free_image(sy_grad);
    free_image(kp_image);
    // free_list(harris_kps);
}
