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

boolean is_over(color *gray) { return gray->data[0] > 25.0f; }

void draw_an_x(image *img, int y, int x) {
    draw_x_yx_(img, y, x, make_rgb_color(1.0f, 0.0f, 0.0f), 6);
}

int filter_harris_keypoints(void *void_keypoint) {
    keypoint *keypoint = (struct keypoint *)void_keypoint;
    return keypoint->confidence >= 0.0f && keypoint->confidence <= 100.0f;
}

void display_kp(void *v_kp) {
    keypoint *kp = (keypoint *)v_kp;
    printf("keypoint(x=%d, y=%d, confidence=%f)", kp->xy->x, kp->xy->y,
           kp->confidence);
}
list *foo(image *img) {
    image *gray = image_to_gray(img);

    kernel *gaus = kernel_make_gaus(3, 3, 5.0f);
    image *smoothed_img = kernel_convolve(gray, gaus, MIRROR, 255.0f);
    kernel *harris_kernel = kernel_make_gaus(3, 3, 2.0f);
    kernel_mul_scalar_(harris_kernel, 9.0f);
    list *harris_kps_list =
        detect_harris_keypoints(smoothed_img, harris_kernel, 0.06, 5, 30.0f);
    extract_patch_descriptors_(img, harris_kps_list, 5);

    free_image(gray);
    free_image(smoothed_img);
    free_kernel(gaus);
    free_kernel(harris_kernel);

    return harris_kps_list;
}

float l1_d(void *k1, void *k2) {
    keypoint *kp1 = (keypoint *)k1;
    keypoint *kp2 = (keypoint *)k2;
    simple_descriptor *d1 = (simple_descriptor *)kp1->descriptor;
    simple_descriptor *d2 = (simple_descriptor *)kp2->descriptor;
    float total = 0.0f;
    float x;
    for (int i = 0; i < d1->length; i++) {
        x = d1->data[i] - d2->data[i];
        total += ABS(x);
    }
    return total / d1->length;
}

int main() {
    image *reiner1 = load_image("resources/Rainier1.png");
    image *reiner2 = load_image("resources/Rainier2.png");

    // image *img_sm = kernel_convolve(img, gaus, MIRROR, 255.0f);

    list *harris_kps1 = foo(reiner1);
    list *harris_kps2 = foo(reiner2);
    // render_keyppoint_(reiner1, harris_kps1, make_red_unit(), 5);
    // image *combined = combine_images_on_x(reiner1, reiner2);
    list *matches = match_keypoints(harris_kps1, harris_kps2, l1_d);
    image *combined = render_matches(reiner1, reiner2, matches, make_red_unit(),
                                     5, make_green_unit(), 1);

    // display_list(harris_kps, display_kp);
    // display_list(harris_corners, display_kp);

    // image *kp_image = render_keypoints(harris_kps, smoothed_img);
    // image *kp_image2 = render_keypoints(harris_corners, smoothed_img);

    // fill_rectangle_yxhw_(img, 10, 10, 50, 20, make_rgb_color(1.0f, 0.0f,
    // 0.0f));

    // draw_rectangle_yxhw_(img, 10, 50, 100, 200,
    //                      make_rgb_color(1.0f, 0.0f, 0.0f));
    // show_image_cv(img_sm, "img_sm", 0, 1);
    // show_image_cv(gray, "gray", 1, 1);
    show_image_cv(combined, "combined", 0, 1);
    // show_image_cv(gray3, "gray3", 0, 1);
    // image *new_image = image_mask_lt_scalar(gray, 255.0f, 255.0f);
    // show_image_cv(new_image, "new_image", 1, 0);
    // image *sme = image_convert_1x3(smoothed_img);
    // show_image_cv(smoothed_img, "smoothed_img", 0, 1);
    // show_image_cv(sme, "sme", 0, 0);

    // histogram *hist = compute_gray_image_hist(gray, 0);
    // image *hist_imgs =
    //     render_histogram(hist, 300, 400, make_rgb_color(0.0f, 0.0f, 255.0f));
    // show_image_cv(hist_imgs, "hist", 1, 1);
    // show_image_cv(x_grad, "x_grad", 0, 1);
    // show_image_cv(y_grad, "y_grad", 0, 1);

    // show_image_cv(sx_grad, "sx_grad", 0, 1);
    // show_image_cv(sy_grad, "sy_grad", 0, 0);
    //  show_image_cv(harris_kps, "harris_kps", 1, 1);
    show_image_cv(reiner1, "image", 0, 0);
    free_image(reiner1);
}
