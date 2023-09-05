#ifndef PANORAMA_H
#define PANORAMA_H
#include "image.h"
#include "keypoints/keypoint.h"
#include "list.h"
#include "matrix.h"

image *combine_images_on_x(image *img_st, image *img_nd);

image *render_matches(image *st_img, image *nd_img, list *matches, color *c,
                      int length, color *line_color, int thikness);

list *project_points(matrix m, list *point_list);
point2di project_point(matrix m, point2di p);

matrix RANSAC(match **matches, int nm, float thresh, int k, int cutoff,
              int no_points);
matrix compute_homography(match **matches, int *indeces, int n);
image *combine_on_homography(matrix H, image *st_image, image *nd_image);
image *combine_pano(list *image_list, list *keypoints_list, distance_fn fn,
                    int aligment_thresh, int iterations, int cutoff, int no_points) ;

list *extract_kps_from_matches(list kp_list1, list kp_list2, matrix H,
                               point2di t);
#endif
