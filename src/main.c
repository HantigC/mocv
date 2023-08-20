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

void display_kp(void *v_kp) {
    keypoint *kp = (keypoint *)v_kp;
    printf("keypoint(x=%d, y=%d, confidence=%f)", kp->xy->x, kp->xy->y,
           kp->confidence);
}

int main() {
    image *img = load_image("resources/chess.png");

    image *img256 = image_muls(img, 255.0f);
    image *gray = image_to_gray(img256);
    image *gray3 = image_convert_1x3(gray);
    kernel *gaus = kernel_make_gaus(3, 3, 5.0f);
    kernel *sobel_x = kernel_make_sobelx();
    kernel *sobel_y = kernel_make_sobely();

    image *smoothed_img = kernel_convolve(gray, gaus,  MIRROR, 255.0f);

    image *x_grad = kernel_convolve_no_border(gray, sobel_x);
    image *y_grad = kernel_convolve_no_border(gray, sobel_y);

    image *sx_grad = kernel_convolve_no_border(smoothed_img, sobel_x);
    image *img_sm = kernel_convolve_no_border(img, gaus);
    image *sy_grad = kernel_convolve_no_border(smoothed_img, sobel_y);
    list *harris_kps = detect_harris(smoothed_img, 3, 0.06);
    // list *harris_corners = list_filter(filter_harris_keypoints, harris_kps);

    image *kp_image = render_keypoints(harris_kps, smoothed_img);
    image_min_max_norm_(kp_image);
    image_muls_channel_(kp_image, 255.0f, 0);
    // display_list(harris_kps, display_kp);

    // fill_rectangle_yxhw_(img, 10, 10, 50, 20, make_rgb_color(1.0f, 0.0f, 0.0f));

    // draw_rectangle_yxhw_(img, 10, 50, 100, 200,
    //                      make_rgb_color(1.0f, 0.0f, 0.0f));
    show_image_cv(img_sm, "img_sm", 0, 1);
    show_image_cv(gray, "gray", 1, 1);
    show_image_cv(gray3, "gray3", 1, 1);
    show_image_cv(img, "image", 0, 0);
    image *sme = image_convert_1x3(smoothed_img);
    show_image_cv(smoothed_img, "smoothed_img", 1, 1);
    show_image_cv(sme, "sme", 1, 0);
    histogram *hist = compute_gray_image_hist(gray, 0);
    histogram *kp_hist = compute_1c_image_hist(kp_image, 256, 0);
    // print_hist(hist);
    // print_hist(kp_hist);
    image *hist_imgs =
        render_histogram(hist, 300, 400, make_rgb_color(0.0f, 0.0f, 255.0f));
    show_image_cv(hist_imgs, "hist", 1, 1);
    image *kp_hist_imgs =
        render_histogram(kp_hist, 300, 400, make_rgb_color(0.0f, 255.0f, 0.0f));
    show_image_cv(kp_hist_imgs, "kp_hist", 1, 1);
    show_image_cv(x_grad, "x_grad", 1, 1);
    show_image_cv(y_grad, "y_grad", 1, 1);

    show_image_cv(sx_grad, "sx_grad", 1, 1);
    show_image_cv(sy_grad, "sy_grad", 1, 0);
    show_image_cv(kp_image, "kp_image", 0, 0);
    free_image(img);
    free_image(gray);
    free_image(smoothed_img);

    free_image(x_grad);
    free_image(y_grad);
    free_image(sx_grad);
    free_image(sy_grad);
    // free_image(kp_image);
    free_list(harris_kps);
}
