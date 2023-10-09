#ifndef HISTOGRAM_H
#define HISTOGRAM_H
#include "image.h"
#include "list.h"
typedef struct bucket {
    float start;
    float end;
    float count;
} bucket;

typedef struct histogram {
    int *counts;
    int bins;
    float step;
    float min_value;
    float max_value;
    int total_count;
    float eps;
} histogram;

histogram make_hist(int bins, float min_value, float max_value, float eps);
histogram *alloc_empty_hist();
histogram copy_hist(histogram);
void init_hist(histogram *, int bins, float min_value, float max_value,
               float eps);
bucket *alloc_bucket(float start, float end, float count);

void normalize_hist_(histogram hist);
histogram normalize_hist(histogram hist);
float histogram_total(histogram hist);

image render_histogram(histogram, int height, int width, rgb color);
void render_histogram_(image img, histogram hist, rgb color);
void print_hist(histogram hist);
void add_to_hist(histogram *hist, float x);

int histogram_max_count(histogram hist);
void render_histogram_at_(image img, histogram hist, int start_bin, int end_bin,
                          int max_count, float bin_width, int offset,
                          rgb color);
#endif
