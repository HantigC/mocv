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
    bucket *buckets;
    int bins;
    float step;
    float min_value;
    float max_value;
    float total;
} histogram;

bucket *copy_bucket(bucket *);
histogram *make_hist(int bins, float min_value, float max_value);
histogram *make_empty_hist();
histogram *copy_hist(histogram *);
void init_hist(histogram *, int bins, float min_value, float max_value);
bucket *make_bucket(float start, float end, float count);

void normalize_hist_(histogram *hist);
histogram *normalize_hist(histogram *hist);
float histogram_total(histogram *hist);

image *render_histogram(histogram *, int height, int width, color *color);
image *render_histogram_(image *img, histogram *hist, color *color);
void print_hist(histogram *hist);
void add_to_hist(histogram *hist, float x);
#endif
