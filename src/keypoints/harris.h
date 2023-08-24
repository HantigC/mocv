#ifndef HARRIS_H
#define HARRIS_H
#include "image.h"
#include "kernel.h"
#include "keypoints/keypoint.h"
#include "list.h"
#include "mathx.h"

image *detect_harris(image *img, kernel *hood_kernel, float alpha,
                     int nms_hood);
image *extract_cornerness(image *img, kernel *hood_kernel, float alpha);

keypoint *make_keypoint(point2di *p, float confidence);
keypoint *make_empty_keypoint();

#endif
