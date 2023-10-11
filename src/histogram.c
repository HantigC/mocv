#include "histogram.h"
#include "array.h"
#include "image_draw.h"
#include "mathx.h"
#include "stdio.h"
#include <stdlib.h>

histogram *alloc_empty_hist() {
    histogram *hist = malloc(sizeof(histogram));
    return hist;
}

float histogram_total(histogram hist) {
    float total = 0.0f;
    for (int bin = 0; bin < hist.bins; bin++) {
        total += hist.counts[bin];
    }
    return total;
}

histogram make_hist(int bins, float min_value, float max_value, float eps) {
    histogram hist = {0};
    init_hist(&hist, bins, min_value, max_value, eps);
    return hist;
}

void init_hist(histogram *hist, int bins, float min_value, float max_value,
               float eps) {
    hist->bins = bins;
    hist->counts = (float *)calloc(bins, sizeof(float));
    hist->step = (max_value - min_value + eps) / bins;
    hist->eps = eps;
    hist->min_value = min_value;
    hist->max_value = max_value;
    hist->total_count = 0;
}

void add_to_hist(histogram *hist, float x) {
    int bin = (x - hist->min_value) / hist->step;
    bin = CLAMP(bin, 0, hist->bins - 1);
    hist->counts[bin] += 1.0f;
    hist->total_count += 1;
}

histogram copy_hist(histogram hist) {
    histogram dest = hist;
    hist.counts = (float *)calloc(hist.bins, sizeof(float));
    for (int i = 0; i < hist.bins; i++) {
        dest.counts[i] = hist.counts[i];
    }
    return dest;
}

void normalize_hist_(histogram hist) {
    for (int i = 0; i < hist.bins; i++) {
        hist.counts[i] /= hist.total_count;
    }
}
histogram normalize_hist(histogram hist) {
    histogram dest = copy_hist(hist);
    normalize_hist_(dest);
    return dest;
}
image render_histogram(histogram hist, int height, int width, rgb color) {
    image img = make_image(height, width, 3);
    render_histogram_(img, hist, color);
    return img;
}

int histogram_max_count(histogram hist) {
    int max_count = hist.counts[0];
    for (int bin = 1; bin < hist.bins; bin++) {
        max_count = MAX(max_count, hist.counts[bin]);
    }
    return max_count;
}

void render_hist_counter_(float *counts, int bins, image img, rgb color) {

    int offset = img.width * 0.01f;
    float bin_width = (float)(img.width - 2.0f * offset) / bins;

    int bin_height;
    point2di tl;
    point2di br;

    float max_count = max_float(counts, bins);
    for (int bin = 0; bin < bins; bin++) {
        tl.x = bin * bin_width + offset;
        bin_height = (img.height - offset) * counts[bin] / max_count;
        tl.y = img.height - bin_height;
        br.x = bin * bin_width + bin_width + offset;
        br.y = img.height;
        fill_rectangle_tlbr_rgb_(img, tl, br, color);
    }
}

image render_hist_counter(float *counts, int bins, int height, int width,
                         rgb color) {
    image img = make_image(height, width, 3);
    render_hist_counter_(counts, bins, img, color);
    return img;
}
void render_histogram_(image img, histogram hist, rgb color) {
    render_hist_counter_(hist.counts, hist.bins, img, color);
}

void print_hist(histogram hist) {
    printf("Histogram(bins=%d, step=%f, min_value=%f, max_value=%f, total=%d",
           hist.bins, hist.step, hist.min_value, hist.max_value,
           hist.total_count);
    for (int bin = 0; bin < hist.bins; bin++) {
        printf("bin[%d]=%f:", bin, hist.counts[bin]);
        printf("\n");
    }
}

void render_histogram_at_(image img, histogram hist, int start_bin, int end_bin,
                          int max_count, float bin_width, int offset,
                          rgb color) {
    point2di tl;
    point2di br;
    int bin_height;
    for (int bin = start_bin; bin < end_bin; bin++) {
        tl.x = bin * bin_width + offset;
        bin_height =
            (img.height - offset) * hist.counts[bin] / (float)max_count;
        tl.y = img.height - bin_height;
        br.x = bin * bin_width + bin_width + offset;
        br.y = img.height;
        fill_rectangle_tlbr_rgb_(img, tl, br, color);
    }
}
