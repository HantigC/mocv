#ifndef KEYPOINT_H
#define KEYPOINT_H
#include "image.h"
#include "list.h"
#include "mathx.h"

typedef struct keypoint {
    point2di *xy;
    float confidence;
    void *descriptor;
} keypoint;

typedef struct simple_descriptor {
    float *data;
    int length;
} simple_descriptor;

typedef struct match {
    void *st_kp, *nd_kp;
    float ditance;
} match;

typedef float (*distance_fn)(void *, void *);
keypoint *make_empty_keypoint();
keypoint *make_keypoint(point2di *p, float confidence);
image *render_keypoint_pixels(list *keypoints, image *img);

image *kp_nms(image *kp_img, int nms_hood);
void kp_nms_(image *kp_img, int nms_hood);

void extract_patch_descriptors_(const image *img, list *keypoints,
                                int window_size);

match *make_empty_match();
match *make_match(void *st_kp, void *nd_kp, float distace);
list *match_keypoints(list *st_keypoints, list *nd_keypointsm, distance_fn fn);
void render_keyppoint_(image *img, list *descritors, color *c, int length);

image *render_matches(image *st_img, image *nd_img, list *matches, color *c,
                      int length, color *line_color, int thikness);

#endif /* ifndef KEYPOINT_H */
