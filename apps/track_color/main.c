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
        image_min_max_norm_(back_proj);

        point2di new_tl = get_tli(new_rect);
        point2di new_br = get_bri(new_rect);
        bbox = new_rect;

        draw_rectangle_tlbr_rgb_(*img, new_tl, new_br, color);
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
        cv_load_image_sequence("apps/track_color/resources/bouncing_ball.mp4");
    list *sliced_seq = slice_at(image_sequence, 11, 200);
    image *first_image = item_at(image_sequence, 10);
    rgb red = {.r = 1.0f, .g = 0.0f, .b = 0.0f};
    point2di tl = {.y = 157, .x = 144};
    point2di br = {.y = 206, .x = 188};
    rect bbox = rect_from_2p(tl, br);

    image patch = img_extract_patch_tlbr(*first_image, tl, br);

    image patch255 = image_muls(patch, 255.0f);
    rgb_cube_hist rgb_3d_hist = compute_rgb_cube_hist(patch255, 8, 8, 8);

    track_the_ball(sliced_seq, bbox, rgb_3d_hist, red);
    show_image_sequence_cv(sliced_seq, "bouncing_ball", 60, 0);
    free_image(patch255);
    free_image(patch);
    free_rgb_cube_hist(rgb_3d_hist);
    free_list(sliced_seq);
    free_sequence(image_sequence);
    return 0;
}
