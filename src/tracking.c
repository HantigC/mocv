#ifndef TRACKING_H
#define TRACKING_H
#include "tracking.h"
#include <float.h>

point2df compute_weighted_center(const image img, rect r, float *weight_sum,
                                 float expansion) {

    float acc = 0.0f;

    point2df c = rect_center(r);
    int half_h = expansion * r.h / 2, half_w = expansion * r.w / 2;

    int y_min = CLAMP(c.y - half_h, 0, img.height - 1);
    int y_max = CLAMP(c.y + half_h, 0, img.height - 1);
    int x_min = CLAMP(c.x - half_w, 0, img.width - 1);
    int x_max = CLAMP(c.x + half_w, 0, img.width - 1);
    float cx = 0.0f, cy = 0.0f;
    float p;
    for (int i = y_min; i <= y_max; ++i) {
        for (int j = x_min; j <= x_max; ++j) {
            p = get_pixel(img, i, j, 0);
            cx += j * p;
            cy += i * p;
            acc += p;
        }
    }
    if (weight_sum != 0) {
        *weight_sum = acc;
    }
    point2df new_c = {.y = cy / acc, .x = cx / acc};
    return new_c;
}

rect meanshift(image prob_img, rect bbox, float eps, int iterations,
               float expansion) {
    point2df prev_center;
    point2df curr_center = rect_center(bbox);
    float prev_error;
    float error = FLT_MAX;
    while (error > eps) {
        prev_center = curr_center;
        prev_error = error;
        curr_center = compute_weighted_center(prob_img, bbox, 0, expansion);
        error = euclidean_distancef(prev_center, curr_center);
        if (error > prev_error) {
            curr_center = compute_middlef(prev_center, curr_center);
        }
    }

    return rect_from_cyxyhw(curr_center.y, curr_center.x, bbox.h, bbox.w);
}

rect camshift(image prob_img, rect bbox, float eps, int iterations,
              float expansion) {
    point2df prev_center;
    point2df curr_center;
    float prev_error;
    float error = FLT_MAX;
    int start_from = iterations > 0 ? 0 : -2;
    int increase_by = iterations > 0 ? 1 : -1;
    float acc;
    rect est_bbox = bbox;
    float box_size;
    for (int i = start_from; i < iterations; i += increase_by) {

        curr_center = rect_center(est_bbox);
        prev_center = curr_center;
        prev_error = error;
        curr_center =
            compute_weighted_center(prob_img, est_bbox, &acc, expansion);
        error = euclidean_distancef(prev_center, curr_center);
        if (error < eps) {
            break;
        }
        if (error > prev_error) {
            curr_center = compute_middlef(prev_center, curr_center);
        }
        box_size = sqrt(acc) + (est_bbox.h + est_bbox.w) / 4.0f;

        est_bbox =
            rect_from_cyxyhw(curr_center.y, curr_center.x, box_size, box_size);
    }
    return est_bbox;
}

#endif /* ifndef TRACKING_H */
