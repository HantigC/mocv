#ifndef LK_H
#define LK_H
#include "image.h"
#include "kernel.h"
image extract_lk_flow(image image_t0, image image_t1, kernel weight,
                      int stride);

image *compute_image_dt(image image_t0, image image_t1);
void draw_flow_(image img, image flow_img, int scale, int stride, int offset);
#endif
