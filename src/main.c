#include "array.h"
#include "histogram.h"
#include "image.h"
#include "image_draw.h"
#include "image_op.h"
#include "image_stats.h"
#include "img.h"
#include "kernel.h"
#include "keypoints/harris.h"
#include "keypoints/keypoint.h"
#include "list.h"
#include "stdx.h"
#include "tracking.h"
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
void track_the_ball(list *image_sequence, rect start_bbox,
                    rgb_cube_hist rgb_3d_hist, rgb color) {

    point2di tl = get_tli(start_bbox);
    point2di br = get_bri(start_bbox);
    kernel gaus = kernel_make_ones(br.y - tl.y, br.x - tl.x);

    node *n = image_sequence->first;
    image *img;
    rect bbox = start_bbox;

    while (n) {
        img = n->item;
        image second_img256 = image_muls(*img, 255.0f);
        image back_proj = back_project(second_img256, rgb_3d_hist);
        rect new_rect = camshift(back_proj, bbox, 1.0f, 10, 3.0f);
        point2di new_tl = get_tli(new_rect);
        point2di new_br = get_bri(new_rect);
        bbox = new_rect;

        draw_rectangle_tlbr_rgb_(*img, new_tl, new_br, color);
        print_rect(new_rect);
        printf("\n");
        show_image_cv(img, "ball", -1, 0);
        free_image(second_img256);
        free_image(back_proj);
        n = n->next;
    }
    free_kernel(gaus);
}

void free_sequence(list *alist) {

    node *n = alist->first;
    image *img;

    while (n) {
        img = (image *)n->item;
        free_image(*img);
        n = n->next;
    }
    free_list(alist);
}

int main() {
    list *image_sequence =
        cv_load_image_sequence("./resources/bouncing_ball.mp4");
    list *sliced_seq = slice_at(image_sequence, 11, 100);
    image *first_image = item_at(image_sequence, 10);
    rgb red = {.r = 1.0f, .g = 0.0f, .b = 0.0f};
    point2di tl = {.y = 157, .x = 144};
    point2di br = {.y = 206, .x = 188};
    rect bbox = rect_from_2p(tl, br);

    image patch = img_extract_patch_tlbr(*first_image, tl, br);

    image patch255 = image_muls(patch, 255.0f);
    rgb_cube_hist rgb_3d_hist = compute_rgb_cube_hist(patch255, 8, 8, 8);

    track_the_ball(sliced_seq, bbox, rgb_3d_hist, red);
    show_image_sequence_cv(sliced_seq, "highway", 60, 0);
    free_image(patch255);
    free_image(patch);
    free_rgb_cube_hist(rgb_3d_hist);
    free_list(sliced_seq);
    free_sequence(image_sequence);
    return 0;
}
