#include "image_op.h"

#include <float.h>
void image_muls_(image img, float scalar) {
    for (int i = 0; i < image_size(img); ++i) {
        img.data[i] = img.data[i] * scalar;
    }
}

void image_pow2_(image img) {

    for (int i = 0; i < image_size(img); ++i) {
        img.data[i] = img.data[i] * img.data[i];
    }
}

image image_pow2(image img) {

    image dest = make_image_like(img);
    for (int i = 0; i < image_size(img); ++i) {
        dest.data[i] = img.data[i] * img.data[i];
    }
    return dest;
}
image image_muls(image img, float scalar) {
    image dest = make_image_like(img);
    for (int i = 0; i < image_size(img); ++i) {
        dest.data[i] = img.data[i] * scalar;
    }
    return dest;
}

void image_muls_channel_(image img, float scalar, int channel) {
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            set_get_pixel_mul(img, img, y, x, channel, scalar);
        }
    }
}

image image_muls_channel(image img, float scalar, int channel) {
    image dest = make_image_like(img);
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            set_get_pixel_mul(img, dest, y, x, channel, scalar);
        }
    }
    return dest;
}

minmax image_get_min_max(image img, int channel) {
    minmax min_max;
    min_max.max = min_max.min = get_pixel(img, 0, 0, channel);
    for (int y = 0; y < img.height; ++y) {
        for (int x = 0; x < img.width; ++x) {
            min_max.min = MIN(min_max.min, get_pixel(img, y, x, channel));
            min_max.max = MAX(min_max.max, get_pixel(img, y, x, channel));
        }
    }
    return min_max;
}

void image_min_max_norm_(image img) {
    minmax mm;
    float range, value;
    for (int c = 0; c < img.channels; ++c) {
        mm = image_get_min_max(img, c);
        range = mm.max - mm.min;
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                value = (get_pixel(img, y, x, c) - mm.min) / range;
                set_pixel(img, y, x, c, value);
            }
        }
    }
}

image image_min_max_norm(image img) {
    image dest = make_image_like(img);
    minmax mm;
    float range, value;
    for (int c = 0; c < img.channels; ++c) {
        mm = image_get_min_max(img, c);
        range = mm.max - mm.min;
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                value = (get_pixel(img, y, x, c) - mm.min) / range;
                set_pixel(dest, y, x, c, value);
            }
        }
    }

    return dest;
}

image image_mask_scalar(image img, float_eq_fn fn, float comparee,
                        float scalar) {
    image mask = make_image_like(img);
    float pixel;
    for (int c = 0; c < img.channels; c++) {
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
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

image img_mask_cmp_img(image img, float_eq_fn fn, image comparee,
                       float scalar) {
    image mask = make_image_like(img);
    float pixel1, pixel2;
    for (int c = 0; c < img.channels; c++) {
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                pixel1 = get_pixel(img, y, x, c);
                pixel2 = get_pixel(comparee, y, x, c);
                if (fn(pixel1, pixel2)) {
                    set_pixel(mask, y, x, c, scalar);
                } else {
                    set_pixel(mask, y, x, c, pixel1);
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

image image_mask_lt_scalar(image img, float x, float scalar) {
    return image_mask_scalar(img, lt, x, scalar);
}
image image_mask_le_scalar(image img, float x, float scalar) {
    return image_mask_scalar(img, le, x, scalar);
}
image image_mask_gt_scalar(image img, float x, float scalar) {
    return image_mask_scalar(img, gt, x, scalar);
}
image image_mask_ge_scalar(image img, float x, float scalar) {

    return image_mask_scalar(img, ge, x, scalar);
}
image image_mask_eq_scalar(image img, float x, float scalar) {
    return image_mask_scalar(img, eq, x, scalar);
}

image img_mask_lt_img(image img, image comparee, float scalar) {
    return img_mask_cmp_img(img, lt, comparee, scalar);
}
image img_mask_le_img(image img, image comparee, float scalar) {
    return img_mask_cmp_img(img, le, comparee, scalar);
}
image img_mask_gt_img(image img, image comparee, float scalar) {
    return img_mask_cmp_img(img, gt, comparee, scalar);
}
image img_mask_ge_img(image img, image comparee, float scalar) {
    return img_mask_cmp_img(img, ge, comparee, scalar);
}

image img_mask_eq_img(image img, image comparee, float scalar) {
    return img_mask_cmp_img(img, eq, comparee, scalar);
}

image img_where_scalar(image img, float_eq_fn fn, float comparee,
                       float true_value, float false_value) {
    image mask = make_image_like(img);
    float pixel;
    for (int c = 0; c < img.channels; c++) {
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                pixel = get_pixel(img, y, x, c);
                if (fn(pixel, comparee)) {
                    set_pixel(mask, y, x, c, true_value);
                } else {
                    set_pixel(mask, y, x, c, false_value);
                }
            }
        }
    }
    return mask;
}

image img_where_lt_scalar(image img, float comparee, float true_value,
                          float false_value) {
    return img_where_scalar(img, lt, comparee, true_value, false_value);
}
image img_where_gt_scalar(image img, float comparee, float true_value,
                          float false_value) {

    return img_where_scalar(img, gt, comparee, true_value, false_value);
}
image img_where_eq_scalar(image img, float comparee, float true_value,
                          float false_value) {
    return img_where_scalar(img, eq, comparee, true_value, false_value);
}
image img_where_le_scalar(image img, float comparee, float true_value,
                          float false_value) {
    return img_where_scalar(img, le, comparee, true_value, false_value);
}
image img_where_ge_scalar(image img, float comparee, float true_value,
                          float false_value) {
    return img_where_scalar(img, ge, comparee, true_value, false_value);
}

image img_where_img(image img, float_eq_fn fn, image comparee, float true_value,
                    float false_value) {
    image mask = make_image_like(img);
    float pixel1, pixel2;
    for (int c = 0; c < img.channels; c++) {
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                pixel1 = get_pixel(img, y, x, c);
                pixel2 = get_pixel(comparee, y, x, c);
                if (fn(pixel1, pixel2)) {
                    set_pixel(mask, y, x, c, true_value);
                } else {
                    set_pixel(mask, y, x, c, false_value);
                }
            }
        }
    }
    return mask;
}
image img_where_lt_img(image img, image comparee, float true_value,
                       float false_value) {
    return img_where_img(img, lt, comparee, true_value, false_value);
}

image img_where_gt_img(image img, image comparee, float true_value,
                       float false_value) {
    return img_where_img(img, gt, comparee, true_value, false_value);
}
image img_where_eq_img(image img, image comparee, float true_value,
                       float false_value) {
    return img_where_img(img, eq, comparee, true_value, false_value);
}
image img_where_le_img(image img, image comparee, float true_value,
                       float false_value) {
    return img_where_img(img, le, comparee, true_value, false_value);
}
image img_where_ge_img(image img, image comparee, float true_value,
                       float false_value) {
    return img_where_img(img, ge, comparee, true_value, false_value);
}

image img_blend_if_scalar(image img, float_eq_fn fn, float comparee,
                          image true_img, image false_img) {
    image mask = make_image_like(img);
    float pixel1;
    for (int c = 0; c < img.channels; c++) {
        for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
                pixel1 = get_pixel(img, y, x, c);
                if (fn(pixel1, comparee)) {
                    set_pixel(mask, y, x, c, get_pixel(true_img, y, x, c));
                } else {
                    set_pixel(mask, y, x, c, get_pixel(false_img, y, x, c));
                }
            }
        }
    }
    return mask;
}

image img_blend_if_lt_scalar(image img, float comparee, image true_img,
                             image false_img) {
    return img_blend_if_scalar(img, lt, comparee, true_img, false_img);
}
image img_blend_if_gt_scalar(image img, float comparee, image true_img,
                             image false_img) {
    return img_blend_if_scalar(img, gt, comparee, true_img, false_img);
}
image img_blend_if_eq_scalar(image img, float comparee, image true_img,
                             image false_img) {
    return img_blend_if_scalar(img, eq, comparee, true_img, false_img);
}
image img_blend_if_le_scalar(image img, float comparee, image true_img,
                             image false_img) {
    return img_blend_if_scalar(img, le, comparee, true_img, false_img);
}
image img_blend_if_ge_scalar(image img, float comparee, image true_img,
                             image false_img) {
    return img_blend_if_scalar(img, ge, comparee, true_img, false_img);
}

image img_blend(image st_image, image nd_image, float alpha) {
    image blend = make_image_like(st_image);
    float st_pixel, nd_pixel;
    for (int c = 0; c < st_image.channels; c++) {
        for (int y = 0; y < st_image.height; y++) {
            for (int x = 0; x < st_image.width; x++) {
                st_pixel = get_pixel(st_image, y, x, c);
                nd_pixel = get_pixel(nd_image, y, x, c);
                set_pixel(blend, y, x, c,
                          alpha * st_pixel + (1 - alpha) * nd_pixel);
            }
        }
    }
    return blend;
}

void imgrgb_where_map_(image img, image dest, cmp_color_fn fn,
                       maploc_img_fn map_fn) {
    color c = make_color(img.channels);
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            get_color_(img, y, x, c);
            if (fn(c)) {
                map_fn(dest, y, x);
            }
        }
    }
    free_color(c);
}

image img_extract_patch_yxhw(image img, int y, int x, int h, int w) {
    image dest = make_image(h, w, img.channels);
    float pixel;
    for (int c = 0; c < img.channels; c++) {
        for (int j = y; j < y + h; j++) {
            for (int i = x; i < x + w; i++) {
                pixel = get_pixel(img, j, i, c);
                set_pixel(dest, j - y, i - x, c, pixel);
            }
        }
    }
    return dest;
}

image img_extract_patch_tlbr(image img, point2di tl, point2di br) {
    int w = br.x - tl.x;
    int h = br.y - tl.y;
    return img_extract_patch_yxhw(img, tl.y, tl.x, h, w);
}

float accumulate_patch(image img, int y, int x, int h, int w, int from_ch,
                       int to_ch, op2f acc_fn, float init_acc) {
    float acc = init_acc;
    for (int c = from_ch; c < to_ch; c++) {
        for (int i = y; i < y + h; i++) {
            for (int j = x; j < x + w; j++) {
                acc = acc_fn(acc, get_pixel(img, i, j, c));
            }
        }
    }
    return acc;
}

float add_op(float x, float y) { return x + y; }
float sub_op(float x, float y) { return x - y; }
float mul_op(float x, float y) { return x * y; }

float add_patch(image img, int y, int x, int h, int w, int from_ch, int to_ch) {
    return accumulate_patch(img, y, x, h, w, from_ch, to_ch, add_op, 0.0f);
}
float sub_patch(image img, int y, int x, int h, int w, int from_ch, int to_ch) {
    return accumulate_patch(img, y, x, h, w, from_ch, to_ch, sub_op, 0.0f);
}
float mul_patch(image img, int y, int x, int h, int w, int from_ch, int to_ch) {
    return accumulate_patch(img, y, x, h, w, from_ch, to_ch, mul_op, 1.0f);
}

float ssd(image first_image, image second_image, rect st_rect, rect nd_rect) {
    float acc = 0.0f;
    float value;

    for (int c = 0; c < first_image.channels; c++) {
        for (int y = 0; y < st_rect.h; y++) {
            for (int x = 0; x < st_rect.w; x++) {
                value =
                    get_pixel(first_image, y + st_rect.y, x + st_rect.x, c) -
                    get_pixel(second_image, y + nd_rect.y, x + nd_rect.x, c);
                acc += value * value;
            }
        }
    }
    return acc;
}

point2di horizontal_scan(image first_image, image second_image, int y, int x,
                         int window_height, int window_width) {
    float min_diff = FLT_MAX;
    point2di min_p = {.x = x, .y = y};
    int half_h = window_height / 2;
    int half_w = window_width / 2;
    float ssd_diff;

    float value;

    for (int nd_x = x; nd_x < second_image.width - half_w; nd_x++) {
        ssd_diff = 0.0f;
        for (int wy = -half_h; wy <= half_h; wy++) {
            for (int wx = -half_w; wx <= half_w; wx++) {
                for (int c = 0; c < first_image.channels; c++) {
                    value = get_pixel(first_image, wy + y, wx + x, c) -
                            get_pixel(second_image, wy + y, wx + nd_x, c);
                    ssd_diff += value * value;
                }
            }
        }
        if (ssd_diff < min_diff) {
            min_diff = ssd_diff;
            min_p.x = nd_x;
        }
    }
    return min_p;
}
image patch_disparity(image first_image, image second_image, int window_height,
                      int window_width) {

    int half_h = window_height / 2;
    int half_w = window_width / 2;
    image disparity_img = make_image(first_image.height, first_image.width, 1);
    point2di best_p;

    for (int y = half_h; y < first_image.height - half_h; y++) {
        for (int x = half_w; x < first_image.width - half_w; x++) {
            best_p = horizontal_scan(first_image, second_image, y, x,
                                     window_height, window_width);
            set_pixel(disparity_img, y, x, 0, (best_p.x - x));
        }
    }
    return disparity_img;
}

image clip_values(image img, float min_value, float max_value) {

    image dest = make_image_like(img);
    for (int i = 0; i < image_size(img); ++i) {
        dest.data[i] = CLAMP(img.data[i], min_value, max_value);
    }
    return dest;
}
void clip_values_(image img, float min_value, float max_value) {
    for (int i = 0; i < image_size(img); ++i) {
        img.data[i] = CLAMP(img.data[i], min_value, max_value);
    }
}
