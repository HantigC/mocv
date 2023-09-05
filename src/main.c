#include "array.h"
#include "image.h"
#include "image_draw.h"
#include "image_op.h"
#include "image_stats.h"
#include "img.h"
#include "kernel.h"
#include "keypoints/harris.h"
#include "keypoints/keypoint.h"
#include "list.h"
#include "panorama.h"
#include "stdx.h"
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
    while (n) {
        list_insert(keypoints_list, foo(n->item));
        n = n->next;
    }
    return keypoints_list;
}

list *load_images(list *filenames_list) {
    list *images_list = list_make();
    node *n = filenames_list->first;
    while (n) {
        list_insert(images_list, load_image((char *)n->item));
        n = n->next;
    }
    return images_list;
}

int main() {

    color *red = make_red_unit();
    color *green = make_green_unit();

    image *reiner1 = load_image("resources/field1.jpeg");
    image *reiner2 = load_image("resources/field2.jpeg");

    list *kps = foo(reiner1);
    list *kps2 = foo(reiner2);
    list *matches = match_keypoints(kps, kps2, l1_d);
    // render_keyppoints_(reiner1, kps, red, 5);
    // render_keyppoints_(reiner2, kps2, red, 5);
    printf("%d\n", matches->length);
    image *b2b_img =
        render_matches(reiner1, reiner2, matches, red, 5, green, 1);

    list *filename_list = list_make();
    list_insert(filename_list, "resources/Rainier2.png");
    list_insert(filename_list, "resources/Rainier1.png");
    list_insert(filename_list, "resources/Rainier3.png");
    // list_insert(filename_list, "resources/Rainier4.png");
    // list_insert(filename_list, "resources/Rainier1.png");
    list *images_list = load_images(filename_list);
    list *keypoints_list = extract_keypoint_images(images_list);
    image *all_combined =
        combine_pano(images_list, keypoints_list, l1_d, 20, 50, 100, 10);

    show_image_cv(reiner1, "reiner1", 0, 1);
    show_image_cv(reiner2, "reiner2", 0, 1);
    show_image_cv(b2b_img, "b2b_img", 0, 0);
    show_image_cv(all_combined, "all_combined", 0, 0);
    free_image(reiner1);
}
