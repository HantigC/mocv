#include "image_stats.h"
#include "image_draw.h"
#include "image_op.h"

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
