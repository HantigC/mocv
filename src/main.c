#include "array.h"
#include "image.h"
#include "image_draw.h"
#include "image_op.h"
#include "histogram.h"
#include "image_stats.h"
#include "img.h"
#include "kernel.h"
#include "keypoints/harris.h"
#include "keypoints/keypoint.h"
#include "list.h"
#include "optical_flow/lk.h"
#include "panorama.h"
#include "stdx.h"
#include <stdio.h>
#include <stdlib.h>

boolean is_over(color *gray) { return gray->data[0] > 25.0f; }

void draw_an_x(image img, int y, int x) {
    color red = make_rgb_color(1.0f, 0.0f, 0.0f);
    draw_x_yx_(img, y, x, red, 6);
    free_color(red);
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
list *foo(image img) {
    image gray = image_to_gray(img);

    kernel gaus = kernel_make_gaus(3, 3, 5.0f);
    image smoothed_img = kernel_convolve(gray, gaus, MIRROR, 255.0f);
    kernel harris_kernel = kernel_make_gaus(3, 3, 2.0f);
    kernel_mul_scalar_(harris_kernel, 9.0f);
    list *harris_kps_list =
        detect_harris_keypoints(smoothed_img, harris_kernel, 0.06, 7, 20.0f);
    extract_patch_descriptors_(img, harris_kps_list, 7);

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

int **just_make_it() {
    int **a = (int **)calloc(6, sizeof(int *));
    for (int i = 0; i < 6; i++) {
        a[i] = (int *)calloc(1, sizeof(int));
    }
    a[0][0] = 4;
    a[1][0] = 2;
    a[2][0] = 5;
    a[3][0] = 1;
    a[4][0] = 6;
    a[5][0] = 3;
    return a;
}

enum CMP int_cmp(void *x, void *y) {
    int *xp = (int *)x;
    int *yp = (int *)y;
    if (*xp < *yp) {
        return LT;
    } else if (*xp == *yp) {
        return EQ;
    }
    return GT;
}

list *extract_keypoint_images(list *images_list) {
    list *keypoints_list = list_make();
    node *n = images_list->first;
    image *img;
    while (n) {
        img = n->item;
        list_insert(keypoints_list, foo(*img));
        n = n->next;
    }
    return keypoints_list;
}

list *load_images(list *filenames_list) {
    list *images_list = list_make();
    node *n = filenames_list->first;
    while (n) {
        list_insert(images_list, load_alloc_image((char *)n->item));
        n = n->next;
    }
    return images_list;
}
int main() {
    list *image_sequence =
        cv_load_image_sequence("./resources/person_walking.mp4");
    list *sliced_seq = slice_at(image_sequence, 40, 100);
    image *first_image = item_at(image_sequence, 40);
    rgb red = {.r = 1.0f, .g = 0.0f, .b = 0.0f};
    color red_color = rgb_to_color(red);
    point2di tl = {.y = 193, .x = 518};
    point2di br = {.y = 344, .x = 601};
    image patch = img_extract_patch_tlbr(*first_image, tl, br);

    draw_rectangle_tlbr_rgb_(*first_image, tl, br, red);
    print_image(*first_image);
    printf("\n");
    show_image_cv(first_image, "window", 1, 0);
    print_image(patch);
    printf("\n");
    show_image_cv(&patch, "patch", 1, 0);
    image patch255 = image_muls(patch, 255.0f);
    histogram rgb_hist = compute_rgb_image_hist(patch255);
    image hist_img = render_rgb_histogram(rgb_hist, 480, 640);
    print_hist(rgb_hist);
    show_image_cv(&hist_img, "histogram", 0, 0);



    printf("\n");
    show_image_sequence_cv(sliced_seq, "highway", 24, 0);
    return 0;
}
