#ifndef IMAGE_STATS_H
#define IMAGE_STATS_H
#include "histogram.h"
#include "image.h"
typedef struct rgb_cube_hist {
    float *counts;
    int red_bins, green_bins, blue_bins;
    int total_count;
} rgb_cube_hist;

typedef struct mean_var {
    float mean;
    float var;

} mean_var;

mean_var *compute_stats(image first_image, int y, int x, int h_radius,
                        int w_radius);

void compute_stats_(image first_image, int y, int x, int h_radius, int w_radius,
                    mean_var *per_channel_stats);
rgb_cube_hist make_3d_rgb_hist(int red_bins, int green_bins, int blue_bins);
rgb_cube_hist compute_rgb_cube_hist(image img, int red_bins, int green_bins,
                                    int blue_bins);

histogram compute_1c_image_hist(image img, int bins, int channel, float eps);
histogram compute_rgb_image_hist(image img);
histogram compute_gray_image_hist(image img, int channel);

int rgb_cube_length(rgb_cube_hist rgb_hist);
rgb_cube_hist compute_rgb_cube_hist(image img, int red_bins, int green_bins,
                                    int blue_bins);
image render_rgb_cube_histogram(rgb_cube_hist rgb_hist, int height, int width,
                                rgb color);

image back_project(image img, rgb_cube_hist rgb_hist);

image render_rgb_histogram(histogram rgb_histogram, int height, int width);

void print_rgb_cube_hist(rgb_cube_hist rgb_hist);
void free_rgb_cube_hist(rgb_cube_hist);
#endif
