#ifndef IMAGE_STATS_H
#define IMAGE_STATS_H
#include "histogram.h"
#include "image.h"
histogram compute_1c_image_hist(image img, int bins, int channel, float eps);
histogram compute_rgb_image_hist(image img);
histogram compute_gray_image_hist(image img, int channel);

image render_rgb_histogram(histogram rgb_histogram, int height, int width);
#endif
