#ifndef KEYPOINT_H
#define KEYPOINT_H
#include "mathx.h"
#include "list.h"
#include "image.h"

typedef struct keypoint {
    point2di *xy;
    float confidence;
} keypoint;

keypoint *make_empty_keypoint();
keypoint *make_keypoint(point2di *p, float confidence);
image *render_keypoints(list *keypoints, image *img);
#endif /* ifndef KEYPOINT_H */
