#include "image_stats.h"
#include "image_op.h"

histogram *compute_1c_image_hist(image *img, int bins, int channel) {
    minmax *mm = image_get_min_max(img, 0);
    histogram *hist = make_hist(bins, mm->min, mm->max);
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            add_to_hist(hist, get_pixel(img, y, x, channel));
        }
    }
    return hist;
}

histogram *compute_gray_image_hist(image *img, int channel) {
    histogram *hist = make_hist(256, 0.0f, 255.0f);
    float pixel;
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            pixel = get_pixel(img, y, x, channel);
            add_to_hist(hist, pixel);
        }
    }
    return hist;
}
