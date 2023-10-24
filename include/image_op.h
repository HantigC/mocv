#ifndef IMAGE_OP
#define IMAGE_OP
#include "image.h"
#include "mathx.h"
#include "image_stats.h"

typedef float (*op2f)(float op1, float op2);
typedef boolean (*float_eq_fn)(float x, float y);
typedef boolean (*cmp_color_fn)(color x);
typedef void (*maploc_img_fn)(image img, int y, int x);


void image_muls_(image img, float scalar);
image image_muls(image, float scalar);
void image_pow2_(image);
image image_pow2(image img);

void image_muls_channel_(image, float scalar, int channel);
image image_muls_channel(image, float scalar, int channel);

void image_min_max_norm_(image);
image image_min_max_norm(image);
image clip_values(image img, float min_value, float max_value);
void clip_values_(image img, float min_value, float max_value);

minmax image_get_min_max(image img, int channel);

image image_mask_scalar(image img, float_eq_fn fn, float comparee,
                        float scalar);
image image_mask_lt_scalar(image img, float x, float scalar);
image image_mask_le_scalar(image img, float x, float scalar);
image image_mask_gt_scalar(image img, float x, float scalar);
image image_mask_ge_scalar(image img, float x, float scalar);
image image_mask_eq_scalar(image img, float x, float scalar);

image img_mask_img(image img, float_eq_fn fn, image comparee, float scalar);
image img_mask_lt_img(image img, image comparee, float scalar);
image img_mask_le_img(image img, image comparee, float scalar);
image img_mask_gt_img(image img, image comparee, float scalar);
image img_mask_ge_img(image img, image comparee, float scalar);
image img_mask_eq_img(image img, image comparee, float scalar);

image img_where_scalar(image img, float_eq_fn, float comparee, float true_value,
                       float false_value);
image img_where_lt_scalar(image img, float comparee, float true_value,
                          float false_value);
image img_where_gt_scalar(image img, float comparee, float true_value,
                          float false_value);
image img_where_eq_scalar(image img, float comparee, float true_value,
                          float false_value);
image img_where_le_scalar(image img, float comparee, float true_value,
                          float false_value);
image img_where_ge_scalar(image img, float comparee, float true_value,
                          float false_value);

image img_where_img(image img, float_eq_fn, image comparee, float true_value,
                    float false_value);
image img_where_lt_img(image img, image comparee, float true_value,
                       float false_value);
image img_where_gt_img(image img, image comparee, float true_value,
                       float false_value);
image img_where_eq_img(image img, image comparee, float true_value,
                       float false_value);
image img_where_le_img(image img, image comparee, float true_value,
                       float false_value);
image img_where_ge_img(image img, image comparee, float true_value,
                       float false_value);

image img_blend_if_scalar(image img, float_eq_fn, float comparee,
                          image true_img, image false_img);

image img_blend_if_lt_scalar(image img, float comparee, image true_img,
                             image false_img);
image img_blend_if_gt_scalar(image img, float comparee, image true_img,
                             image false_img);
image img_blend_if_eq_scalar(image img, float comparee, image true_img,
                             image false_img);
image img_blend_if_le_scalar(image img, float comparee, image true_img,
                             image false_img);
image img_blend_if_ge_scalar(image img, float comparee, image true_img,
                             image false_img);
image img_blend(image st_image, image nd_image, float alpha);
void imgrgb_where_map_(image img, image dest, cmp_color_fn fn,
                       maploc_img_fn map_fn);

// Patch
image img_extract_patch_yxhw(image img, int y, int x, int h, int w);
image img_extract_patch_tlbr(image img, point2di tl, point2di br);

float accumulate_patch(image img, int y, int x, int h, int w, int from_ch,
                       int to_ch, op2f acc_fn, float init_acc);

float add_patch(image img, int y, int x, int h, int w, int from_ch, int to_ch);
float sub_patch(image img, int y, int x, int h, int w, int from_ch, int to_ch);
float mul_patch(image img, int y, int x, int h, int w, int from_ch, int to_ch);

point2di horizontal_scan(image first_image, image second_image, int y, int x,
                         int window_height, int window_width, op2f fn);
image patch_disparity(image first_image, image second_image, int window_height,
                      int window_width, op2f fn);

image ssd_disparity(image first_image, image second_image, int window_height,
                    int window_width);

image sad_disparity(image first_image, image second_image, int window_height,
                    int window_width);

float patch_ncc(image first_image, image second_image, point2di first_p,
                point2di second_p, int h_radius, int w_radius,
                mean_var *st_per_channel_stats);
point2di ncc_hscan(image first_image, image second_image, int y, int x,
                   int h_radius, int w_radius);
image ncc_disparity(image first_image, image second_image, int h_radius,
                    int w_radius);

#endif
