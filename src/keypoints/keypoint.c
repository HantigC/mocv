#include "keypoints/keypoint.h"

keypoint *make_empty_keypoint() { return (keypoint *)malloc(sizeof(keypoint)); }
keypoint *make_keypoint(point2di *p, float confidence) {
    keypoint *keypoint = make_empty_keypoint();
    keypoint->xy = p;
    keypoint->confidence = confidence;
    return keypoint;
}

image *render_keypoints(list *keypoints, image *img) {
    node *node = keypoints->first;
    image *kp_image = make_image(img->height, img->width, 1);
    keypoint *kp;
    while (node) {
        kp = (keypoint *)node->item;
        set_pixel(kp_image, kp->xy->y, kp->xy->x, 0, kp->confidence);
        node = node->next;
    }

    return kp_image;
}

float apply_neighbourhood(image *img, int y, int x, int hood_size) {
    int half = hood_size / 2;
    int start_x, start_y, end_y, end_x;
    start_y = MAX(y - half, 0);
    start_x = MAX(x - half, 0);

    end_y = MIN(y + half, img->height);
    end_x = MIN(x + half, img->width);

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

void apply_neighbourhood_(image *img, int y, int x, int hood_size) {
    set_pixel(img, y, x, 0, apply_neighbourhood(img, y, x, hood_size));
}

image *kp_nms(image *kp_img, int nms_hood) {
    image *dest = make_image_like(kp_img);
    float dest_pixel;
    for (int y = 0; y < kp_img->height; y++) {
        for (int x = 0; x < kp_img->width; x++) {
            dest_pixel = apply_neighbourhood(kp_img, y, x, nms_hood);
            set_pixel(dest, y, x, 0, dest_pixel);
        }
    }
    return dest;
}

void kp_nms_(image *kp_img, int nms_hood) {
    for (int y = 0; y < kp_img->height; y++) {
        for (int x = 0; x < kp_img->width; x++) {
            apply_neighbourhood_(kp_img, y, x, nms_hood);
        }
    }
}

simple_descriptor *extract_window(const image *img, int y, int x,
                                  int window_size) {
    simple_descriptor *descriptor =
        (simple_descriptor *)malloc(sizeof(simple_descriptor));
    descriptor->length = window_size * window_size * img->channels;
    descriptor->data = (float *)calloc(descriptor->length, sizeof(float));
    int count = 0;
    int half = window_size / 2;
    int cy, cx;
    for (int i = 0; i < window_size; i++) {
        for (int j = 0; j < window_size; j++) {
            cy = CLAMP(y - half + i, 0, img->height);
            cx = CLAMP(x - half + j, 0, img->width);
            for (int c = 0; c < img->channels; c++) {
                descriptor->data[count] = get_pixel(img, cy, cx, c);
                ++count;
            }
        }
    }
    return descriptor;
}

void extract_patch_descriptors(const image *img, list *keypoints,
                               int window_size) {
    node *node;
    keypoint *kp;
    for (node = keypoints->first; node; node = node->next) {
        kp = (keypoint *)node->item;
        kp->descriptor = extract_window(img, kp->xy->y, kp->xy->x, window_size);
    }
}
