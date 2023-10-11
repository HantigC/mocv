#include "image_stats.h"
#include "image_draw.h"
#include "image_op.h"
#include <stdio.h>

histogram compute_1c_image_hist(image img, int bins, int channel, float eps) {
    minmax mm = image_get_min_max(img, 0);
    histogram hist = make_hist(bins, mm.min, mm.max, eps);
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            add_to_hist(&hist, get_pixel(img, y, x, channel));
        }
    }
    return hist;
}

histogram compute_gray_image_hist(image img, int channel) {
    histogram hist = make_hist(256, 0.0f, 255.0f, 1.0f);
    float pixel;
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            pixel = get_pixel(img, y, x, channel);
            add_to_hist(&hist, pixel);
        }
    }
    return hist;
}

histogram compute_rgb_image_hist(image img) {
    histogram hist = make_hist(3 * 256, 0.0f, 3 * 256.0f - 1.0f, 1.0f);
    float pixel;
    for (int c = 0; c < img.channels; c++) {
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                pixel = get_pixel(img, y, x, c);
                add_to_hist(&hist, c * 255.0f + pixel);
            }
        }
    }
    return hist;
}

image render_rgb_histogram(histogram rgb_histogram, int height, int width) {
    image img = make_image(height, width, 3);

    int offset = img.width * 0.01f;
    float bin_width =
        (float)(img.width - 2.0f * offset) / (float)rgb_histogram.bins;

    rgb red = {1.0f, 0.0f, 0.0f};
    rgb green = {0.0f, 0.0f, 1.0f};
    rgb blue = {0.0f, 1.0f, 0.0f};

    int max_count = histogram_max_count(rgb_histogram);
    render_histogram_at_(img, rgb_histogram, 0, 256, max_count, bin_width,
                         offset, red);
    render_histogram_at_(img, rgb_histogram, 256, 2 * 256, max_count, bin_width,
                         offset, green);
    render_histogram_at_(img, rgb_histogram, 2 * 256, 3 * 256, max_count,
                         bin_width, offset, blue);
    return img;
}

rgb_cube_hist make_3d_rgb_hist(int red_bins, int green_bins, int blue_bins) {
    rgb_cube_hist rgb_3d = {0};
    rgb_3d.red_bins = red_bins;
    rgb_3d.green_bins = green_bins;
    rgb_3d.blue_bins = blue_bins;
    rgb_3d.counts =
        (float *)calloc(red_bins * blue_bins * green_bins, sizeof(float));
    return rgb_3d;
}

int compute_bin_position(rgb_cube_hist rgb_hist, rgb rgb_color) {

    float red_step = 256.0f / rgb_hist.red_bins;
    float blue_step = 256.0f / rgb_hist.blue_bins;
    float green_step = 256.0f / rgb_hist.green_bins;
    int red_bin = rgb_color.r / red_step;
    int green_bin = rgb_color.g / green_step;
    int blue_bin = rgb_color.b / blue_step;
    int pos = red_bin * rgb_hist.blue_bins * rgb_hist.green_bins +
              green_bin * rgb_hist.blue_bins + blue_bin;
    if (pos >= rgb_hist.red_bins * rgb_hist.blue_bins * rgb_hist.green_bins) {
        printf("pos  %d", pos);
        printf("red_bin  %d", red_bin);
        printf("blue_bin  %d", blue_bin);
        printf("green_bin  %d", green_bin);
    }

    return pos;
}

void add_rgb_to_hist(rgb_cube_hist *rgb_hist, rgb rgb_color) {
    int pos = compute_bin_position(*rgb_hist, rgb_color);
    rgb_hist->counts[pos] += 1.0f;
    rgb_hist->total_count += 1;
}

float get_rgb_probability(rgb_cube_hist rgb_hist, rgb rgb_color,
                          int normalize) {
    int pos = compute_bin_position(rgb_hist, rgb_color);
    float proba = rgb_hist.counts[pos];
    if (normalize == 1) {
        proba = proba / rgb_hist.total_count;
    }
    return proba;
}

rgb_cube_hist compute_rgb_cube_hist(image img, int red_bins, int green_bins,
                                    int blue_bins) {
    rgb_cube_hist rgb_hist = make_3d_rgb_hist(red_bins, green_bins, blue_bins);
    rgb rgb_color;
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            rgb_color = get_rgb(img, y, x);
            add_rgb_to_hist(&rgb_hist, rgb_color);
        }
    }
    return rgb_hist;
}

image back_project(image img, rgb_cube_hist rgb_hist) {
    image img_back = make_image(img.height, img.width, 1);
    rgb rgb_color;
    float prob;
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            rgb_color = get_rgb(img, y, x);
            prob = get_rgb_probability(rgb_hist, rgb_color, 1);
            set_pixel(img_back, y, x, 0, prob);
        }
    }
    return img_back;
}

int rgb_cube_length(rgb_cube_hist rgb_hist) {
    return rgb_hist.red_bins * rgb_hist.green_bins * rgb_hist.blue_bins;
}
image render_rgb_cube_histogram(rgb_cube_hist rgb_hist, int height, int width,
                                rgb color) {
    return render_hist_counter(rgb_hist.counts, rgb_cube_length(rgb_hist),
                               height, width, color);
}

void print_rgb_cube_hist(rgb_cube_hist rgb_hist) {

    printf("RGB Histogram(bins=%d)\n", rgb_cube_length(rgb_hist));
    float prob;

    float red_step = 256.0f / rgb_hist.red_bins;
    float blue_step = 256.0f / rgb_hist.blue_bins;
    float green_step = 256.0f / rgb_hist.green_bins;
    for (float red = 0; red < 256; red+=red_step) {
        for (float green = 0; green < 256; green+=green_step) {
            for (float blue = 0; blue < 256; blue+=blue_step) {
                prob =
                    get_rgb_probability(rgb_hist, to_rgb(red, green, blue), 0);
                printf("(%f, %f, %f)=%d", red, green, blue, (int)prob);
                printf("\n");
            }
        }
    }
}

void free_rgb_cube_hist(rgb_cube_hist h){
    free(h.counts);

}
