#include "panorama.h"
#include "array.h"
#include "image_draw.h"
#include "rect.h"
#include <assert.h>

image *combine_images_on_x(image *img_st, image *img_nd) {
    int height = MAX(img_st->height, img_nd->height);
    image *img =
        make_image(height, img_st->width + img_nd->width, img_st->channels);
    for (int c = 0; c < img_st->channels; c++) {
        for (int y = 0; y < img_st->height; y++) {
            for (int x = 0; x < img_st->width; x++) {
                set_pixel(img, y, x, c, get_pixel(img_st, y, x, c));
            }
        }
    }

    for (int c = 0; c < img_nd->channels; c++) {
        for (int y = 0; y < img_nd->height; y++) {
            for (int x = 0; x < img_nd->width; x++) {
                set_pixel(img, y, img_st->width + x, c,
                          get_pixel(img_nd, y, x, c));
            }
        }
    }
    return img;
}

image *render_matches(image *st_img, image *nd_img, list *matches, color *c,
                      int length, color *line_color, int thikness) {
    image *img = combine_images_on_x(st_img, nd_img);
    keypoint *st_kp, *nd_kp;
    match *kp_match;
    node *node = matches->first;
    while (node) {
        kp_match = (match *)node->item;
        st_kp = kp_match->st_kp;
        nd_kp = kp_match->nd_kp;
        draw_x_pointi_(img, st_kp->xy, c, length);
        draw_x_yx_(img, nd_kp->xy->y, nd_kp->xy->x + st_img->width, c, length);
        draw_line_yxyx_(img, st_kp->xy->y, st_kp->xy->x, nd_kp->xy->y,
                        nd_kp->xy->x + nd_img->width, line_color, thikness);
        node = node->next;
    }
    return img;
}

point2di project_point(matrix m, point2di p) {
    matrix a = make_matrix(3, 1);
    a.data[0][0] = p.x;
    a.data[1][0] = p.y;
    a.data[2][0] = 1;
    matrix result = matrix_mult_matrix(m, a);
    free_matrix(a);
    point2di p_res;
    p_res.x = result.data[0][0] / result.data[2][0];
    p_res.y = result.data[1][0] / result.data[2][0];
    return p_res;
}

list *project_points(matrix m, list *point_list) {
    node *node = point_list->first;
    list *points_list_res = list_make();
    point2di pp, *p;
    while (node) {
        p = (point2di *)node->item;
        pp = project_point(m, *p);
        list_insert(points_list_res, make_point2di(pp.x, pp.y));
        node = node->next;
    }
    return points_list_res;
}
float l2_distance_2di(point2di p1, point2di p2) {
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);
}
matrix compute_homography(match **matches, int *indeces, int n) {
    matrix M = make_matrix(2 * n, 8);
    matrix b = make_matrix(2 * n, 1);
    keypoint *kp1, *kp2;
    double *row;
    for (int i = 0; i < n; i++) {
        kp1 = (keypoint *)matches[i]->st_kp;
        kp2 = (keypoint *)matches[i]->nd_kp;
        row = M.data[2 * i];
        b.data[2 * i][0] = kp2->xy->x;
        row[0] = kp1->xy->x;
        row[1] = kp1->xy->y;
        row[2] = 1;
        row[3] = 0;
        row[4] = 0;
        row[5] = 0;
        row[6] = -kp1->xy->x * kp2->xy->x;
        row[7] = -kp1->xy->y * kp2->xy->x;

        row = M.data[2 * i + 1];
        row[0] = 0;
        row[1] = 0;
        row[2] = 0;
        row[3] = kp1->xy->x;
        row[4] = kp1->xy->y;
        row[5] = 1;
        row[6] = -kp1->xy->x * kp2->xy->y;
        row[7] = -kp1->xy->y * kp2->xy->y;

        b.data[2 * i + 1][0] = kp2->xy->y;
    }
    matrix a = solve_system(M, b);
    matrix none = {0};
    if (!a.data) {
        return none;
    }
    free_matrix(M);
    free_matrix(b);
    matrix H = make_matrix(3, 3);
    H.data[0][0] = a.data[0][0];
    H.data[0][1] = a.data[1][0];
    H.data[0][2] = a.data[2][0];
    H.data[1][0] = a.data[3][0];
    H.data[1][1] = a.data[4][0];
    H.data[1][2] = a.data[5][0];
    H.data[2][0] = a.data[6][0];
    H.data[2][1] = a.data[7][0];
    H.data[2][2] = 1;
    free_matrix(a);
    return H;
}
int count_inliers(matrix H, match **matches, int nm, float threshold) {
    keypoint *kp1, *kp2;
    point2di pp;

    int total = 0;
    for (int i = 0; i < nm; i++) {
        kp1 = (keypoint *)matches[i]->st_kp;
        pp = project_point(H, *kp1->xy);
        kp2 = (keypoint *)matches[i]->nd_kp;
        if (l2_distance_2di(*kp2->xy, pp) <= threshold) {
            total++;
        }
    }
    return total;
}

matrix RANSAC(match **matches, int nm, float thresh, int k, int cutoff) {
    int *range = int_range(nm);
    matrix H, best_H;
    shuffle_int_array_(range, nm);
    int num_inliners, best_num_inliners;
    best_H = compute_homography(matches, range, 5);
    for (int i = 1; i < k; i++) {
        shuffle_int_array_(range, nm);
        H = compute_homography(matches, range, 5);
        num_inliners = count_inliers(H, matches, nm, thresh);
        if (num_inliners > best_num_inliners) {
            best_num_inliners = num_inliners;
            free_matrix(best_H);
            best_H = H;
        }
        if (best_num_inliners > cutoff) {
            return best_H;
        }
    }
    return best_H;
}

rect _extract_bounds(image img1, image img2, matrix H) {

    matrix Hinv = matrix_invert(H);
    point2di pinv00 = project_point(Hinv, *make_point2di(0, 0));
    point2di pinv0w = project_point(Hinv, *make_point2di(img2.width, 0));
    point2di pinvh0 = project_point(Hinv, *make_point2di(0, img2.height));
    point2di pinvhw =
        project_point(Hinv, *make_point2di(img2.width, img2.height));

    point2di pv00 = *make_point2di(0, 0);
    point2di pv0w = *make_point2di(img1.width, 0);
    point2di pvh0 = *make_point2di(0, img1.height);
    point2di pvhw = *make_point2di(img1.width, img1.height);
    int min_x = min_int(8, pinv00.x, pinvhw.x, pinvh0.x, pinv0w.x, pv00.x,
                        pvhw.x, pvh0.x, pv0w.x);
    int min_y = min_int(8, pinv00.y, pinvhw.y, pinvh0.y, pinv0w.y, pv00.y,
                        pvhw.y, pvh0.y, pv0w.y);

    int max_x = max_int(8, pinv00.x, pinvhw.x, pinvh0.x, pinv0w.x, pv00.x,
                        pvhw.x, pvh0.x, pv0w.x);
    int max_y = max_int(8, pinv00.y, pinvhw.y, pinvh0.y, pinv0w.y, pv00.y,
                        pvhw.y, pvh0.y, pv0w.y);
    int height = max_y - min_y;
    int width = max_x - min_x;
    return rect_from_yxhw(min_y, min_x, height, width);
}

image *combine_on_homography(matrix H, image *st_image, image *nd_image) {
    rect bbox = _extract_bounds(*st_image, *nd_image, H);
    image *combination = make_image(bbox.h, bbox.w, st_image->channels);
    color *c = make_red_unit();
    for (int y = 0; y < st_image->height; y++) {
        for (int x = 0; x < st_image->width; x++) {
            get_color_(st_image, y, x, c);
            set_color(combination, y - bbox.y, x - bbox.x, c);
        }
    }

    point2di pp;
    for (int y = 0; y < combination->height; y++) {
        for (int x = 0; x < combination->width; x++) {
            pp = project_point(H, *make_point2di(x + bbox.x, y + bbox.y));
            if (pp.x >= 0 && pp.x < nd_image->width && pp.y >= 0 &&
                pp.y < nd_image->height) {
                get_color_(nd_image, pp.y, pp.x, c);
                set_color(combination, y, x, c);
            }
        }
    }
    return combination;
}

float l1_norm(float *as, float *bs, int an, int bn) {
    assert(an == bn);
    float total = 0.0f;
    float x;
    for (int i = 0; i < an; i++) {
        x = as[i] - bs[i];
        total += ABS(x);
    }
    return total / an;
}

void shift_(list kp_list, point2di tl) {
    node *n = kp_list.first;
    keypoint *kp;
    while (n) {
        kp = (keypoint *)n->item;
        kp->xy->y -= tl.y;
        kp->xy->x -= tl.x;
        n = n->next;
    }
}

void project_and_shift_(list kp_list, matrix H, point2di tl) {

    node *n = kp_list.first;
    keypoint *kp;
    matrix Hinv = matrix_invert(H);
    point2di pp;
    while (n) {
        kp = (keypoint *)n->item;
        pp = project_point(Hinv, *kp->xy);
        kp->xy->y = pp.y - tl.y;
        kp->xy->x = pp.x - tl.x;
        n = n->next;
    }
}
list *extract_kps_from_matches(list kp_list1, list kp_list2, matrix H,
                               point2di t) {
    node *n = kp_list2.first;
    list *kp_list = copy_list(&kp_list1);
    shift_(*kp_list, t);
    keypoint *kp;
    matrix Hinv = matrix_invert(H);
    point2di pp;
    while (n) {
        kp = (keypoint *)n->item;
        pp = project_point(Hinv, *kp->xy);
        kp->xy->y = pp.y - t.y;
        kp->xy->x = pp.x - t.x;
        list_insert(kp_list, kp);
        n = n->next;
    }
    return kp_list;
}

image *combine_pano(list *image_list, list *keypoints_list, distance_fn fn,
                    int aligment_thresh, int iterations, int cutoff) {
    assert(image_list->length == keypoints_list->length);
    image **images = (image **)list_to_array(image_list);
    list **keypoints = (list **)list_to_array(keypoints_list);
    image *combined = images[0];
    list *matches_list;
    match **matches;
    list *matched_kps = keypoints[0];
    rect bbox;
    for (int i = 1; i < image_list->length; i++) {
        matches_list = match_keypoints(matched_kps, keypoints[i], fn);
        matches = (match **)list_to_array(matches_list);
        matrix H = RANSAC(matches, matches_list->length, aligment_thresh,
                          iterations, cutoff);
        bbox = _extract_bounds(*combined, *images[i], H);
        combined = combine_on_homography(H, combined, images[i]);
        matched_kps = extract_kps_from_matches(*matched_kps, *keypoints[i], H,
                                               rect_get_tl(bbox));
    }
    return combined;
}
