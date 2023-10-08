#include "keypoints/keypoint.h"
#include "image_draw.h"
#include "stdx.h"

keypoint *make_empty_keypoint() { return (keypoint *)malloc(sizeof(keypoint)); }
keypoint *alloc_keypoint(point2di *p, float confidence) {
    keypoint *keypoint = make_empty_keypoint();
    keypoint->xy = p;
    keypoint->confidence = confidence;
    return keypoint;
}

image render_keypoint_pixels(list *keypoints, image img) {
    node *node = keypoints->first;
    image kp_image = make_image(img.height, img.width, 1);
    keypoint *kp;
    while (node) {
        kp = (keypoint *)node->item;
        set_pixel(kp_image, kp->xy->y, kp->xy->x, 0, kp->confidence);
        node = node->next;
    }

    return kp_image;
}

float apply_neighbourhood(image img, int y, int x, int hood_size) {
    int half = hood_size / 2;
    int start_x, start_y, end_y, end_x;
    start_y = MAX(y - half, 0);
    start_x = MAX(x - half, 0);

    end_y = MIN(y + half, img.height);
    end_x = MIN(x + half, img.width);

    float pixel = get_pixel(img, y, x, 0);
    for (int i = start_y; i < end_y; i++) {
        for (int j = start_x; j < end_x; j++) {
            if (pixel <= get_pixel(img, i, j, 0)) {
                if (i == y && j == x)
                    continue;
                return -99999;
            }
        }
    }
    return pixel;
}

void apply_neighbourhood_(image img, int y, int x, int hood_size) {
    set_pixel(img, y, x, 0, apply_neighbourhood(img, y, x, hood_size));
}

image kp_nms(image kp_img, int nms_hood) {
    image dest = make_image_like(kp_img);
    float dest_pixel;
    for (int y = 0; y < kp_img.height; y++) {
        for (int x = 0; x < kp_img.width; x++) {
            dest_pixel = apply_neighbourhood(kp_img, y, x, nms_hood);
            set_pixel(dest, y, x, 0, dest_pixel);
        }
    }
    return dest;
}

void kp_nms_(image kp_img, int nms_hood) {
    for (int y = 0; y < kp_img.height; y++) {
        for (int x = 0; x < kp_img.width; x++) {
            apply_neighbourhood_(kp_img, y, x, nms_hood);
        }
    }
}

simple_descriptor *extract_window(const image img, int y, int x,
                                  int window_size) {
    simple_descriptor *descriptor =
        (simple_descriptor *)malloc(sizeof(simple_descriptor));
    descriptor->length = window_size * window_size * img.channels;
    descriptor->data = (float *)calloc(descriptor->length, sizeof(float));
    int count = 0;
    int half = window_size / 2;
    int cy, cx;
    for (int i = 0; i < window_size; i++) {
        for (int j = 0; j < window_size; j++) {
            cy = CLAMP(y - half + i, 0, img.height);
            cx = CLAMP(x - half + j, 0, img.width);
            for (int c = 0; c < img.channels; c++) {
                descriptor->data[count] = get_pixel_safe(img, cy, cx, c);
                ++count;
            }
        }
    }
    return descriptor;
}

void extract_patch_descriptors_(const image img, list *keypoints,
                                int window_size) {
    node *node;
    keypoint *kp;
    for (node = keypoints->first; node; node = node->next) {
        kp = (keypoint *)node->item;
        kp->descriptor = extract_window(img, kp->xy->y, kp->xy->x, window_size);
    }
}

match *alloc_empty_match() {
    match *m = (match *)malloc(sizeof(match));
    return m;
}

match *alloc_match(void *st_kp, void *nd_kp, float distace, int st_index,
                  int nd_index) {
    match *m = alloc_empty_match();
    m->st_kp = st_kp;
    m->nd_kp = nd_kp;
    m->distance = distace;
    m->st_index = st_index;
    m->nd_index = nd_index;
    return m;
}

enum CMP match_cmp(void *x, void *y) {
    match *xp = (match *)x;
    match *yp = (match *)y;
    if (xp->distance < yp->distance) {
        return LT;
    } else if (xp->distance == yp->distance) {
        return EQ;
    }
    return GT;
}

list *match_keypoints(list *st_keypoints, list *nd_keypoints, distance_fn fn) {
    list *matches_list = list_make();
    keypoint **st_kps = (keypoint **)list_to_array(st_keypoints);
    keypoint **nd_kps = (keypoint **)list_to_array(nd_keypoints);
    float min_distance = -1.0f;
    int min_index = 0;
    float distance;
    for (int st_cnt = 0; st_cnt < st_keypoints->length; st_cnt++) {
        min_distance = (float)0x7FFFFFFF;
        min_index = 0;
        for (int nd_cnt = 0; nd_cnt < nd_keypoints->length; nd_cnt++) {
            distance = fn(st_kps[st_cnt], nd_kps[nd_cnt]);
            if (min_distance > distance) {
                min_distance = distance;
                min_index = nd_cnt;
            }
        }
        list_insert(matches_list, alloc_match(st_kps[st_cnt], nd_kps[min_index],
                                             min_distance, st_cnt, min_index));
    }
    free(st_kps);
    free(nd_kps);
    match **matches = (match **)list_to_array(matches_list);
    qsort_vector_(match_cmp, (void **)matches, matches_list->length);
    int *seen = (int *)calloc(nd_keypoints->length, sizeof(int));
    list *injective_matches = list_make();
    int nd_index;
    for (int i = 0; i < matches_list->length; i++) {
        nd_index = matches[i]->nd_index;
        if (seen[nd_index] != 1)
            list_insert(injective_matches, matches[i]);
        else {
            free(matches[i]);
        }
        seen[nd_index] = 1;
    }
    free(matches);
    free(seen);

    return injective_matches;
}

void render_keyppoints_(image img, list *keypoints, color c, int length) {
    node *node = keypoints->first;
    keypoint *kp;
    while (node) {
        kp = (keypoint *)node->item;
        draw_x_pointi_(img, *kp->xy, c, length);
        node = node->next;
    }
}

list *collect_point_from_kps(list *keypoints) {
    node *node = keypoints->first;
    list *points_list = list_make();
    keypoint *kp;
    while (node) {
        kp = (keypoint *)node->item;
        list_insert(points_list, kp->xy);
        node = node->next;
    }
    return points_list;
}
