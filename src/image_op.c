#include "image_op.h"
#include "img.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

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

image image_mask_scalar(image img,
                        float_eq_fn fn,
                        float comparee,
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

image img_mask_cmp_img(image img,
                       float_eq_fn fn,
                       image comparee,
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

image img_where_scalar(image img,
                       float_eq_fn fn,
                       float comparee,
                       float true_value,
                       float false_value) {
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

image img_where_lt_scalar(image img,
                          float comparee,
                          float true_value,
                          float false_value) {
    return img_where_scalar(img, lt, comparee, true_value, false_value);
}
image img_where_gt_scalar(image img,
                          float comparee,
                          float true_value,
                          float false_value) {

    return img_where_scalar(img, gt, comparee, true_value, false_value);
}
image img_where_eq_scalar(image img,
                          float comparee,
                          float true_value,
                          float false_value) {
    return img_where_scalar(img, eq, comparee, true_value, false_value);
}
image img_where_le_scalar(image img,
                          float comparee,
                          float true_value,
                          float false_value) {
    return img_where_scalar(img, le, comparee, true_value, false_value);
}
image img_where_ge_scalar(image img,
                          float comparee,
                          float true_value,
                          float false_value) {
    return img_where_scalar(img, ge, comparee, true_value, false_value);
}

image img_where_img(image img,
                    float_eq_fn fn,
                    image comparee,
                    float true_value,
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
image img_where_lt_img(image img,
                       image comparee,
                       float true_value,
                       float false_value) {
    return img_where_img(img, lt, comparee, true_value, false_value);
}

image img_where_gt_img(image img,
                       image comparee,
                       float true_value,
                       float false_value) {
    return img_where_img(img, gt, comparee, true_value, false_value);
}
image img_where_eq_img(image img,
                       image comparee,
                       float true_value,
                       float false_value) {
    return img_where_img(img, eq, comparee, true_value, false_value);
}
image img_where_le_img(image img,
                       image comparee,
                       float true_value,
                       float false_value) {
    return img_where_img(img, le, comparee, true_value, false_value);
}
image img_where_ge_img(image img,
                       image comparee,
                       float true_value,
                       float false_value) {
    return img_where_img(img, ge, comparee, true_value, false_value);
}

image img_blend_if_scalar(image img,
                          float_eq_fn fn,
                          float comparee,
                          image true_img,
                          image false_img) {
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

image img_blend_if_lt_scalar(image img,
                             float comparee,
                             image true_img,
                             image false_img) {
    return img_blend_if_scalar(img, lt, comparee, true_img, false_img);
}
image img_blend_if_gt_scalar(image img,
                             float comparee,
                             image true_img,
                             image false_img) {
    return img_blend_if_scalar(img, gt, comparee, true_img, false_img);
}
image img_blend_if_eq_scalar(image img,
                             float comparee,
                             image true_img,
                             image false_img) {
    return img_blend_if_scalar(img, eq, comparee, true_img, false_img);
}
image img_blend_if_le_scalar(image img,
                             float comparee,
                             image true_img,
                             image false_img) {
    return img_blend_if_scalar(img, le, comparee, true_img, false_img);
}
image img_blend_if_ge_scalar(image img,
                             float comparee,
                             image true_img,
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
                set_pixel(
                    blend, y, x, c, alpha * st_pixel + (1 - alpha) * nd_pixel);
            }
        }
    }
    return blend;
}

void imgrgb_where_map_(image img,
                       image dest,
                       cmp_color_fn fn,
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

float accumulate_patch(image img,
                       int y,
                       int x,
                       int h,
                       int w,
                       int from_ch,
                       int to_ch,
                       op2f acc_fn,
                       float init_acc) {
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

float patch_ncc(image first_image,
                image second_image,
                point2di first_p,
                point2di second_p,
                int h_radius,
                int w_radius,
                mean_var *st_per_channel_stats) {
    mean_var *nd_per_channel_stats =
        compute_stats(second_image, second_p.y, second_p.x, h_radius, w_radius);
    float e = 0.0f, esq = 0.0f;
    float st_pixel, nd_pixel;
    float pixel_count = 0.0f;
    float numerator;
    float denominator;
    float pixel_num = (h_radius + h_radius + 1) * (w_radius + w_radius + 1);
    float nc;
    float h_mean_acc = 0.0f;
    float c_st_pixel, c_nd_pixel;

    for (int c = 0; c < first_image.channels; c++) {
        numerator = 0.0f;
        pixel_count = 0.0f;
        for (int i = -h_radius; i <= h_radius; ++i) {
            for (int j = -w_radius; j <= w_radius; ++j) {
                st_pixel =
                    get_pixel(first_image, i + first_p.y, j + first_p.x, c);
                nd_pixel =
                    get_pixel(second_image, i + second_p.y, j + second_p.x, c);
                c_nd_pixel = (nd_pixel - nd_per_channel_stats[c].mean);
                c_st_pixel = (st_pixel - st_per_channel_stats[c].mean);
                numerator += c_st_pixel * c_nd_pixel;
            }
        }
        numerator /= pixel_num;
        denominator = st_per_channel_stats[c].var * nd_per_channel_stats[c].var;
        nc = numerator / sqrt(denominator);
        // nc = sqrt(nc);
        h_mean_acc = acc_mean(h_mean_acc, 1.0f / nc, c + 1);
    }
    free(nd_per_channel_stats);
    return 1.0f / h_mean_acc;
}

point2di ncc_hscan(image first_image,
                   image second_image,
                   int y,
                   int x,
                   int h_radius,
                   int w_radius) {
    float max_corr = FLT_MIN;
    point2di max_p = {.x = x, .y = y};
    point2di st_center = {.x = x, .y = y};
    point2di nd_center;
    mean_var *st_per_channel_stats =
        compute_stats(first_image, y, x, h_radius, w_radius);
    float corr_value;
    for (int nd_x = x; nd_x >= w_radius; nd_x--) {
        nd_center = (point2di){.x = nd_x, .y = y};
        corr_value = patch_ncc(first_image,
                               second_image,
                               st_center,
                               nd_center,
                               h_radius,
                               w_radius,
                               st_per_channel_stats);
        if (max_corr < corr_value) {
            max_corr = corr_value;
            max_p.x = nd_x;
            // print_point2di(max_p);
        }
    }
    free(st_per_channel_stats);
    return max_p;
}

image ncc_disparity(image first_image,
                    image second_image,
                    int h_radius,
                    int w_radius) {
    image disparity_img = make_image(first_image.height, first_image.width, 1);
    point2di best_p;
    for (int y = h_radius; y < first_image.height - h_radius; y++) {
        for (int x = w_radius; x < first_image.width - w_radius; x++) {
            best_p =
                ncc_hscan(first_image, second_image, y, x, h_radius, w_radius);
            set_pixel(disparity_img, y, x, 0, (x - best_p.x));
        }
    }
    return disparity_img;
}

point2di horizontal_scan(image left_image,
                         image right_image,
                         int y,
                         int x,
                         int window_height,
                         int window_width,
                         op2f fn) {
    float min_diff = FLT_MAX;
    point2di min_p = {.x = x, .y = y};
    int half_h = window_height / 2;
    int half_w = window_width / 2;
    float diff;
    float st_pixel, nd_pixel;
    float value;
    for (int nd_x = x; nd_x >= half_w; nd_x--) {
        diff = 0.0f;
        for (int wy = -half_h; wy <= half_h; wy++) {
            for (int wx = -half_w; wx <= half_w; wx++) {
                for (int c = 0; c < left_image.channels; c++) {
                    st_pixel = get_pixel(left_image, wy + y, wx + x, c);
                    nd_pixel = get_pixel(right_image, wy + y, wx + nd_x, c);
                    value = fn(st_pixel, nd_pixel);
                    diff += value;
                }
            }
        }
        if (diff < min_diff) {
            min_diff = diff;
            min_p.x = nd_x;
        }
    }
    return min_p;
}

image patch_disparity(image first_image,
                      image second_image,
                      int window_height,
                      int window_width,
                      op2f fn) {
    int half_h = window_height / 2;
    int half_w = window_width / 2;
    image disparity_img = make_image(first_image.height, first_image.width, 1);
    point2di best_p;
    for (int y = half_h; y < first_image.height - half_h; y++) {
        for (int x = half_w; x < first_image.width - half_w; x++) {
            best_p = horizontal_scan(first_image,
                                     second_image,
                                     y,
                                     x,
                                     window_height,
                                     window_width,
                                     fn);
            set_pixel(disparity_img, y, x, 0, (x - best_p.x));
        }
    }
    return disparity_img;
}

image ssd_disparity(image first_image,
                    image second_image,
                    int window_height,
                    int window_width) {
    return patch_disparity(
        first_image, second_image, window_height, window_width, ssd);
}

image sad_disparity(image first_image,
                    image second_image,
                    int window_height,
                    int window_width) {
    return patch_disparity(
        first_image, second_image, window_height, window_width, ssd);
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

int float_argmin(float *a, int n) {
    float min_v = a[0];
    int min_idx = 0;
    for (int i = 1; i < n; i++) {
        if (a[i] < min_v) {
            min_v = a[i];
            min_idx = i;
        }
    }
    return min_idx;
}

float window_ssd(image first_image,
                 image second_image,
                 point2di first_p,
                 point2di second_p,
                 tlbr_rect rect) {
    float ssd_val = 0.0f;
    float first_pixel, second_pixel;
    int cnt = 0;
    for (int c = 0; c < first_image.channels; c++) {
        for (int dy = rect.tl.y; dy <= rect.br.y; dy++) {
            for (int dx = rect.tl.x; dx <= rect.br.x; dx++) {
                first_pixel =
                    get_pixel(first_image, first_p.y + dy, first_p.x + dx, c);
                second_pixel = get_pixel(
                    second_image, second_p.y + dy, second_p.x + dx, c);
                ssd_val += ssd(first_pixel, second_pixel);
                cnt += 1;
            }
        }
    }
    return sqrt(ssd_val / cnt);
}

float window_sad(image first_image,
                 image second_image,
                 point2di first_p,
                 point2di second_p,
                 tlbr_rect rect) {
    float ssd_val = 0.0f;
    float first_pixel, second_pixel;
    int cnt = 0;
    for (int c = 0; c < first_image.channels; c++) {
        for (int dy = rect.tl.y; dy <= rect.br.y; dy++) {
            for (int dx = rect.tl.x; dx <= rect.br.x; dx++) {
                first_pixel =
                    get_pixel(first_image, first_p.y + dy, first_p.x + dx, c);
                second_pixel = get_pixel(
                    second_image, second_p.y + dy, second_p.x + dx, c);
                ssd_val += ssd(first_pixel, second_pixel);
                cnt += 1;
            }
        }
    }
    return ssd_val / cnt;
}

void dp_h_disparity(image first_image,
                    image second_image,
                    int first_y,
                    int second_y,
                    int radius_h,
                    int radius_w,
                    float oclusion_constant,
                    wfn fn,
                    image cost_table,
                    image disparity_array) {
    // array cost_table =
    //     make_shaped_array(3, 3, second_image.width + 1, first_image.width +
    //     1);

    point2di p = {.x = radius_w, .y = first_y};
    point2di st_p = {.x = radius_w, .y = first_y},
             nd_p = {.x = radius_w, .y = second_y};

    float disimilarity;
    set_pixel(cost_table, 0, 0, 0, 0.0f);
    float x_cost, y_cost, yx_cost;
    float min_disimilarity;

    // printf("%04d", 0);

    for (int x = 1; x < cost_table.width; x++) {
        x_cost = get_pixel(cost_table, 0, x - 1, 0) + oclusion_constant;
        // printf("|%04d", (int)x_cost);
        set_pixel(cost_table, 0, x, 0, x_cost);
        set_pixel(cost_table, 0, x, 1, 0);
        set_pixel(cost_table, 0, x, 2, -1);
    }
    for (int nd_x = 1; nd_x < cost_table.height; nd_x++) {
        x_cost = get_pixel(cost_table, nd_x - 1, 0, 0) + oclusion_constant;
        set_pixel(cost_table, nd_x, 0, 0, x_cost);
        set_pixel(cost_table, nd_x, 0, 1, -1);
        set_pixel(cost_table, nd_x, 0, 2, 0);
        for (int stx = 1; stx < nd_x; stx++) {

            x_cost =
                get_pixel(cost_table, nd_x, stx - 1, 0) + oclusion_constant;
            set_pixel(cost_table, nd_x, stx, 0, x_cost);
            set_pixel(cost_table, nd_x, stx, 1, 0);
            set_pixel(cost_table, nd_x, stx, 2, -1);
        }
    }
    int min_idx;
    int from_h_radius = MAX3(-first_y, -second_y, -radius_h);
    int to_h_radius = MIN3(
        first_image.height - first_y, second_image.height - second_y, radius_h);

    int from_w_radius;
    int to_w_radius;

    tlbr_rect window_rect = {.tl = {.y = from_h_radius},
                             .br = {.y = to_h_radius}};
    float values[3];

    // printf("\n");
    float min_cost;
    int min_nd_x, min_st_x;
    int nd_x;
    int st_x;
    for (nd_x = 1; nd_x < cost_table.height; nd_x++) {
        // printf("|%04d\n", (int)array_at(cost_table, idx(0, nd_x, 0)));
        min_cost = FLT_MAX;
        for (st_x = nd_x; st_x < cost_table.width; st_x++) {
            window_rect.tl.x = MAX3(-nd_x, -st_x, -radius_w);
            window_rect.br.x = MIN3(second_image.width - nd_x - 1,
                                    first_image.width - st_x - 1,
                                    radius_w);
            // print_tlbr_rect(window_rect);

            x_cost =
                get_pixel(cost_table, nd_x, st_x - 1, 0) + oclusion_constant;
            y_cost =
                get_pixel(cost_table, nd_x - 1, st_x, 0) + oclusion_constant;

            disimilarity = fn(first_image,
                              second_image,
                              (point2di){.x = st_x, .y = first_y},
                              (point2di){.x = nd_x, .y = second_y},
                              window_rect);
            yx_cost =
                get_pixel(cost_table, nd_x - 1, st_x - 1, 0) + disimilarity;

            values[0] = x_cost;
            values[1] = y_cost;
            values[2] = yx_cost;
            min_idx = float_argmin(values, 3);

            set_pixel(cost_table, nd_x, st_x, 0, values[min_idx]);
            if (min_cost > values[min_idx]) {
                min_cost = values[min_idx];
                min_nd_x = nd_x;
                min_st_x = st_x;
            }

            // printf("|%04d", (int)values[min_idx]);
            switch (min_idx) {
            case 0:
                set_pixel(cost_table, nd_x, st_x, 1, 0);
                set_pixel(cost_table, nd_x, st_x, 2, -1);
                break;
            case 1:
                set_pixel(cost_table, nd_x, st_x, 1, -1);
                set_pixel(cost_table, nd_x, st_x, 2, 0);
                break;
            case 2:
                set_pixel(cost_table, nd_x, st_x, 1, -1);
                set_pixel(cost_table, nd_x, st_x, 2, -1);
                break;
            }
        }
    }
    image ct_img = extract_channel(cost_table, 0);
    image_min_max_norm_(ct_img);
    image_muls_(ct_img, 255.0f);
    image ct_img3 = image_convert_1x3(ct_img);

    nd_x = cost_table.height - 1, st_x = cost_table.width - 1;
    rgb red = to_rgb(255.0f, 0.0f, 0.0f);
    int nd_dx = get_pixel(cost_table, nd_x, st_x, 1),
        st_dx = get_pixel(cost_table, nd_x, st_x, 2);
    // printf("here %d\n", first_y);
    // printf("here %d\n", first_image.width);

    // array disparity_array = make_shaped_array(1, first_image.width);
    while (nd_x > 0 || st_x > 0) {
        set_rgb(ct_img3, nd_x, st_x, red);
        if (nd_dx == 0 || st_dx == 0) {
            set_pixel(disparity_array, 0, st_x, 0, -1.0f);
        } else if (nd_dx == -1 && st_dx == -1) {
            set_pixel(disparity_array, 0, st_x, 0, st_x - nd_x);
        }

        nd_x += nd_dx;
        st_x += st_dx;
        nd_dx = get_pixel(cost_table, nd_x, st_x, 1);
        st_dx = get_pixel(cost_table, nd_x, st_x, 2);
        // printf("(nd_x, st_x, nd_dx, st_dx)=(%d, %d, %d, %d)\n",
        //        nd_x,
        //        st_x,
        //        nd_dx,
        //        st_dx);
        //   printf("(dy, dx)=(%1.0f, %1.0f)",
        //          array_at(cost_table, idx(1, nd_x, st_x)),
        //          array_at(cost_table, idx(2, nd_x, st_x)));
    }

    // show_image_cv(&ct_img3, "cost_table", 0, 0, 0);
    free_image(ct_img);
    free_image(ct_img3);
    //  printf("here %d\n", first_y);
    //   free_array(cost_table);
}

image dp_disparity(image first_image,
                   image second_image,
                   int radius_h,
                   int radius_w,
                   float oclusion_constant,
                   wfn fn) {
    image disparity_img = make_image(first_image.height, first_image.width, 1);
    image cost_table =
        make_image(second_image.width + 1, first_image.width + 1, 3);

    image disparity_array = make_image(1, first_image.width, 1);
    point2di best_p;
    for (int y = 0; y < first_image.height; y++) {
        printf("y=%d\n", y);
        dp_h_disparity(first_image,
                       second_image,
                       y,
                       y,
                       radius_h,
                       radius_w,
                       oclusion_constant,
                       fn,
                       cost_table,
                       disparity_array);
        for (int x = 1; x < disparity_array.width; x++) {
            set_pixel(disparity_img,
                      y,
                      x - 1,
                      0,
                      get_pixel(disparity_array, 0, x, 0));
        }
    }
    free_image(disparity_array);
    free_image(cost_table);
    return disparity_img;
}

image render_disparity(image disparity, rgb color) {
    image dest = make_image(disparity.height, disparity.width, 3);
    minmax mm = image_get_min_max(disparity, 0);
    float pixel;
    rgb r = to_rgb(255.0f, 0.0f, 0.0f);
    for (int y = 0; y < disparity.height; y++) {
        for (int x = 0; x < disparity.width; x++) {
            pixel = get_pixel(disparity, y, x, 0);
            if (pixel == -1.0f) {
                set_rgb(dest, y, x, color);
            } else {
                pixel = pixel / mm.max * 255.0f;
                set_rgb(dest, y, x, to_rgb(pixel, pixel, pixel));
            }
        }
    }
    return dest;
}
