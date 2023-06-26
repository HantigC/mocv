#ifndef HARRIS_H
#define HARRIS_H
#include "image.h"
#include "list.h"
#include "mathx.h"
#include "keypoints/keypoint.h"


list *detect_harris(image *img, int neighbourhood_size, float k);
keypoint *make_keypoint(point2di *p, float confidence);
keypoint *make_empty_keypoint();

#endif
