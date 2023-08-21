#include "image_op.h"

void image_muls_(image *img, float scalar) {
    for (int i = 0; i < image_size(img); ++i) {
        img->data[i] = img->data[i] * scalar;
    }
}

image *image_muls(image *img, float scalar) {
    image *dest = make_image_like(img);
    for (int i = 0; i < image_size(img); ++i) {
        dest->data[i] = img->data[i] * scalar;
    }
    return dest;
}

void image_muls_channel_(image *img, float scalar, int channel) {
    for (int y = 0; y < img->height; ++y) {
        for (int x = 0; x < img->width; ++x) {
            set_get_pixel_mul(img, img, y, x, channel, scalar);
        }
    }
}

image *image_muls_channel(image *img, float scalar, int channel) {
    image *dest = make_image_like(img);
    for (int y = 0; y < img->height; ++y) {
        for (int x = 0; x < img->width; ++x) {
            set_get_pixel_mul(img, dest, y, x, channel, scalar);
        }
    }
    return dest;
}

minmax *image_get_min_max(image *img, int channel) {
    minmax *min_max = (minmax *)malloc(sizeof(minmax));
    min_max->max = min_max->min = get_pixel(img, 0, 0, channel);
    for (int y = 0; y < img->height; ++y) {
        for (int x = 0; x < img->width; ++x) {
            min_max->min = MIN(min_max->min, get_pixel(img, y, x, channel));
            min_max->max = MAX(min_max->max, get_pixel(img, y, x, channel));
        }
    }
    return min_max;
}

void image_min_max_norm_(image *img) {
    minmax *mm;

    float range, value;
    for (int c = 0; c < img->channels; ++c) {
        mm = image_get_min_max(img, c);
        range = mm->max - mm->min;
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                value = (get_pixel(img, y, x, c) - mm->min) / range;
                set_pixel(img, y, x, c, value);
            }
        }
    }
}

image *image_min_max_norm(image *img) {
    image *dest = make_image_like(img);

    minmax *mm;
    float range, value;
    for (int c = 0; c < img->channels; ++c) {
        mm = image_get_min_max(img, c);
        range = mm->max - mm->min;
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                value = (get_pixel(img, y, x, c) - mm->min) / range;
                set_pixel(dest, y, x, c, value);
            }
        }
    }

    return dest;
}

image *image_mask_cmp_scalar(image *img, cmp_fn fn, float comparee,
                             float scalar) {
    image *mask = make_image_like(img);
    float pixel;
    for (int c = 0; c < img->channels; c++) {
        for (int y = 0; y < img->height; y++) {
            for (int x = 0; x < img->width; x++) {
                pixel = get_pixel(img, y, x, c);
                if (fn(pixel, comparee)) {
                    set_pixel(mask, y, x, c, scalar);
                } else {
                    set_pixel(mask, y, x, c, pixel);
                }
            }
        }
    }
    return mask;
}

boolean lt(float x, float y) { return x < y; }

boolean gt(float x, float y) { return x > y; }
boolean eq(float x, float y) { return x == y; }
boolean le(float x, float y) { return x <= y; }
boolean ge(float x, float y) { return x >= y; }

image *image_mask_lt_scalar(image *img, float x, float scalar) {
    return image_mask_cmp_scalar(img, lt, x, scalar);
}
image *image_mask_le_scalar(image *img, float x, float scalar) {
    return image_mask_cmp_scalar(img, le, x, scalar);
}
image *image_mask_gt_scalar(image *img, float x, float scalar) {
    return image_mask_cmp_scalar(img, gt, x, scalar);
}
image *image_mask_ge_scalar(image *img, float x, float scalar) {

    return image_mask_cmp_scalar(img, ge, x, scalar);
}
image *image_mask_eq_scalar(image *img, float x, float scalar) {
    return image_mask_cmp_scalar(img, eq, x, scalar);
}
