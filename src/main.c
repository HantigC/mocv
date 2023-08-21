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
    return keypoint->confidence >= 0.0f && keypoint->confidence <= 100.0f;
}

void display_kp(void *v_kp) {
    keypoint *kp = (keypoint *)v_kp;
    printf("keypoint(x=%d, y=%d, confidence=%f)", kp->xy->x, kp->xy->y,
           kp->confidence);
}

int main() {
    image *img = load_image("resources/lena.jpeg");

    image *gray = image_to_gray(img);
    image *gray3 = image_convert_1x3(gray);
    kernel *gaus = kernel_make_gaus(3, 3, 5.0f);
    kernel *sobel_x = kernel_make_sobelx();
    kernel *sobel_y = kernel_make_sobely();

    image *smoothed_img = kernel_convolve(gray, gaus, MIRROR, 255.0f);

    image *x_grad = kernel_convolve(gray, sobel_x, MIRROR, 255.0f);
    image *y_grad = kernel_convolve(gray, sobel_y, MIRROR, 255.0f);

    image *sx_grad = kernel_convolve(smoothed_img, sobel_x, MIRROR, 255.0f);
    // image *img_sm = kernel_convolve(img, gaus, MIRROR, 255.0f);
    image *sy_grad = kernel_convolve(smoothed_img, sobel_y, MIRROR, 255.0f);

    list *harris_kps = detect_harris(smoothed_img, 3, 0.06);
    list *harris_corners = list_filter(filter_harris_keypoints, harris_kps);

    // display_list(harris_kps, display_kp);
    // display_list(harris_corners, display_kp);

    image *kp_image = render_keypoints(harris_kps, smoothed_img);
    image *kp_image2 = render_keypoints(harris_corners, smoothed_img);

    image_min_max_norm_(kp_image);
    image_muls_channel_(kp_image, 255.0f, 0);

    image_min_max_norm_(kp_image2);
    image_muls_channel_(kp_image2, 255.0f, 0);

    // fill_rectangle_yxhw_(img, 10, 10, 50, 20, make_rgb_color(1.0f, 0.0f,
    // 0.0f));

    // draw_rectangle_yxhw_(img, 10, 50, 100, 200,
    //                      make_rgb_color(1.0f, 0.0f, 0.0f));
    // show_image_cv(img_sm, "img_sm", 0, 1);
    image_muls_(gray, 255.0f);
    image *blend_image = img_blend(gray, kp_image, 0.5f);
    show_image_cv(gray, "gray", 1, 1);
    show_image_cv(blend_image, "blend_image", 1, 1);
    show_image_cv(gray3, "gray3", 0, 1);
    show_image_cv(img, "image", 0, 1);
    image *new_image = image_mask_lt_scalar(gray, 255.0f, 255.0f);
    show_image_cv(new_image, "new_image", 1, 0);
    image *sme = image_convert_1x3(smoothed_img);
    show_image_cv(smoothed_img, "smoothed_img", 0, 1);
    show_image_cv(sme, "sme", 0, 0);

    histogram *hist = compute_gray_image_hist(gray, 0);
    image *hist_imgs =
        render_histogram(hist, 300, 400, make_rgb_color(0.0f, 0.0f, 255.0f));
    show_image_cv(hist_imgs, "hist", 1, 1);
    show_image_cv(x_grad, "x_grad", 0, 1);
    show_image_cv(y_grad, "y_grad", 0, 1);

    show_image_cv(sx_grad, "sx_grad", 0, 1);
    show_image_cv(sy_grad, "sy_grad", 0, 0);
    show_image_cv(kp_image, "kp_image", 1, 0);
    show_image_cv(kp_image2, "kp_image2", 1, 0);
    free_image(img);
    free_image(gray);
    free_image(smoothed_img);

    free_image(x_grad);
    free_image(y_grad);
    free_image(sx_grad);
    free_image(sy_grad);
    free_image(kp_image);
    free_image(kp_image2);
    free_list(harris_kps);
}
