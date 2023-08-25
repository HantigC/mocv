#ifndef HARRIS_H
#define HARRIS_H
#include "image.h"
#include "kernel.h"
#include "keypoints/keypoint.h"
#include "list.h"
#include "mathx.h"

image *compute_harris_corner(image *img, kernel *hood_kernel, float alpha,
                     int nms_hood);
image *extract_cornerness(image *img, kernel *hood_kernel, float alpha);
list *detect_harris_keypoints(image *img, kernel *hood_kernel, float alpha,
                     int nms_hood, float threshold);
#endif
