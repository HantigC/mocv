#ifndef TRACKING_H
#define TRACKING_H
#include "tracking.h"
#include <float.h>

point2df compute_weighted_center(const image img, const kernel krn,
                                 point2df c) {

    float acc = 0.0f;
    int half_h = krn.height / 2;
    int half_w = krn.width / 2;
    int y_min = CLAMP(c.y - half_h, 0, img.height - 1);
    int y_max = CLAMP(c.y + half_h, 0, img.height - 1);
    int x_min = CLAMP(c.x - half_w, 0, img.width - 1);
    int x_max = CLAMP(c.x + half_w, 0, img.width - 1);
    float cx = 0.0f, cy = 0.0f;
    float p;
    for (int i = y_min; i <= y_max; ++i) {
        for (int j = x_min; j <= x_max; ++j) {
            p = kernel_get_value(krn, i - y_min, j - x_min) *
                get_pixel(img, i, j, 0);
            cx += j * p;
            cy += i * p;
            acc += p;
        }
    }
    point2df new_c = {.y = cy / acc, .x = cx / acc};
    return new_c;
}

rect meanshift(const image prob_img, const rect bbox, kernel krn, float eps,
               int iterations) {
    point2df prev_center;
    point2df curr_center = rect_center(bbox);
    float prev_error;
    float error = FLT_MAX;
    while (error > eps) {
        prev_center = curr_center;
        prev_error = error;
        curr_center = compute_weighted_center(prob_img, krn, curr_center);
        error = euclidean_distancef(prev_center, curr_center);
        if (error > prev_error) {
            curr_center = compute_middlef(prev_center, curr_center);
        }
    }

    return rect_from_cyxyhw(curr_center.y, curr_center.x, bbox.h, bbox.w);
}

#endif /* ifndef TRACKING_H */
