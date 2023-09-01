#include "panorama.h"
#include "array.h"
#include "image_draw.h"

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
    keypoint *kp;
    point2di pp;
    while (node) {
        kp = (keypoint *)node->item;
        pp = project_point(m, *kp->xy);
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
    for (int i = 0; i < n; i++) {
        kp1 = (keypoint *)matches[i]->st_kp;
        kp2 = (keypoint *)matches[i]->nd_kp;
        M.data[2 * i][0] = kp1->xy->x;
        M.data[2 * i][1] = kp1->xy->y;
        M.data[2 * i][2] = 1;
        M.data[2 * i][3] = 0;
        M.data[2 * i][4] = 0;
        M.data[2 * i][5] = 0;
        M.data[2 * i][6] = -kp1->xy->x * kp2->xy->x;
        M.data[2 * i][7] = -kp1->xy->y * kp2->xy->x;
        b.data[2 * i][0] = kp2->xy->x;

        M.data[2 * i + 1][0] = 0;
        M.data[2 * i + 1][1] = 0;
        M.data[2 * i + 1][2] = 0;
        M.data[2 * i + 1][3] = kp1->xy->x;
        M.data[2 * i + 1][4] = kp1->xy->y;
        M.data[2 * i + 1][5] = 1;
        M.data[2 * i + 1][6] = -kp1->xy->x * kp2->xy->y;
        M.data[2 * i + 1][7] = -kp1->xy->y * kp2->xy->y;
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