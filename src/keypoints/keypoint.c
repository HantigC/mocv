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
