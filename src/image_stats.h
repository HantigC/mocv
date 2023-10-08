#ifndef IMAGE_STATS_H
#define IMAGE_STATS_H
#include "histogram.h"
#include "image.h"
histogram *compute_1c_image_hist(image img, int bins, int channel);
histogram *compute_gray_image_hist(image img, int channel);
#endif
