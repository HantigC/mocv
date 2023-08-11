#include "histogram.h"
#include "image_draw.h"
#include "mathx.h"
#include "stdio.h"
#include <stdlib.h>

histogram *make_empty_hist() {
    histogram *hist = malloc(sizeof(histogram));
    return hist;
}

bucket *make_bucket(float start, float end, float count) {
    bucket *buck = (bucket *)malloc(sizeof(bucket));
    buck->start = start;
    buck->end = end;
    buck->count = count;
    return buck;
}

float histogram_total(histogram *hist) {
    float total = 0.0f;
    for (int bin = 0; bin < hist->bins; bin++) {
        total += hist->buckets[bin].count;
    }
    return total;
}

histogram *make_hist(int bins, float min_value, float max_value) {
    histogram *hist = make_empty_hist();
    init_hist(hist, bins, min_value, max_value);
    return hist;
}

void init_hist(histogram *hist, int bins, float min_value, float max_value) {
    hist->bins = bins;
    hist->buckets = (bucket *)calloc(bins, sizeof(bucket));
    hist->step = (max_value - min_value + 0.0000001f) / bins;
    for (int bin = 0; bin < hist->bins; bin++) {
        hist->buckets[bin].start = bin * hist->step;
        hist->buckets[bin].count = 0;
        hist->buckets[bin].end = bin * hist->step + hist->step;
    }
    hist->min_value = min_value;
    hist->max_value = max_value;
    hist->total = 0;
}

bucket *copy_bucket(bucket *src) {
    return make_bucket(src->start, src->end, src->count);
}

void add_to_hist(histogram *hist, float x) {
    int bin = (x - hist->min_value) / hist->step;
    bin = CLAMP(bin, 0, hist->bins - 1);
    hist->buckets[bin].count += 1.0f;
    hist->total += 1.0f;
}

histogram *copy_hist(histogram *hist) {
    histogram *dest = make_hist(hist->bins, hist->min_value, hist->max_value);
    for (int i = 0; i < hist->bins; i++) {
        dest->buckets[i] = *copy_bucket(&(hist->buckets[i]));
    }
    dest->step = hist->step;
    dest->min_value = hist->min_value;
    dest->max_value = hist->max_value;
    return dest;
}

void normalize_hist_(histogram *hist) {
    float total;
    total = hist->buckets[0].count;
    for (int i = 1; i < hist->bins; i++) {
        total += hist->buckets[1].count;
    }
    for (int i = 0; i < hist->bins; i++) {
        hist->buckets[i].count /= total;
    }
}
histogram *normalize_hist(histogram *hist);
image *render_histogram(histogram *hist, int height, int width, color *color) {
    image *img = make_image(height, width, 3);
    return render_histogram_(img, hist, color);
}

float histogram_max_count(histogram *hist){
    float max_count = hist->buckets[0].count;
    for (int bin = 1; bin < hist->bins; bin++) {
        max_count = MAX(max_count, hist->buckets[bin].count);
    }
    return max_count;
}

image *render_histogram_(image *img, histogram *hist, color *color) {
    float offset = 10.0f;
    float bin_width =(float)(img->width - 2.0f * offset) / hist->bins;
    int bin_height;
    float max_count = histogram_max_count(hist);
    point2di *tl = (point2di *)malloc(sizeof(point2di));
    point2di *br = (point2di *)malloc(sizeof(point2di));

    for (int bin = 0; bin < hist->bins; bin++) {
        tl->x = bin * bin_width + offset;
        bin_height = (img->height - offset) * hist->buckets[bin].count / max_count;
        tl->y = img->height - bin_height;
        br->x = bin * bin_width + bin_width + offset;
        br->y = img->height;
        fill_rectangle_tlbr_(img, tl, br, color);
    }
    free(tl);
    free(br);
    return img;
}

void print_bucket(bucket *buck) {
    printf("Bucket(start=%f, end=%f, count=%f)", buck->start, buck->end,
           buck->count);
}
void print_hist(histogram *hist) {
    printf("Histogram(bins=%d, step=%f, min_value=%f, max_value=%f, total=%f",
           hist->bins, hist->step, hist->min_value, hist->max_value,
           hist->total);
    for (int bin = 0; bin < hist->bins; bin++) {
        printf("bin[%d]:", bin);
        print_bucket(&hist->buckets[bin]);
        printf("\n");
    }
}
