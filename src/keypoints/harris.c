#include "keypoints/harris.h"
#include "kernel.h"

kernel *extract_matrices(image *dx_img, image *dy_img, kernel *hood_kernel,
                         int y, int x) {
    int half_height = hood_kernel->height / 2;
    int half_width = hood_kernel->width / 2;
    float dx, dy;
    kernel *harris_kernel = make_kernel(2, 2);
    float m00, m01, m10, m11;
    int ky, kx;
    float weigth;
    for (int i = 0; i < hood_kernel->height; ++i) {
        for (int j = 0; j < hood_kernel->width; ++j) {
            ky = CLAMP(y - half_height + i, 0, dy_img->height);
            kx = CLAMP(x - half_width + j, 0, dy_img->width);
            weigth = kernel_get_value(hood_kernel, i, j);

            dy = weigth * get_pixel(dy_img, ky, kx, 0);
            dx = weigth * get_pixel(dx_img, ky, kx, 0);
            m00 = dx * dx;
            m01 = m10 = dy * dx;
            m11 = dy * dy;
            kernel_set_value(harris_kernel, 0, 0,
                             kernel_get_value(harris_kernel, 0, 0) + m00);
            kernel_set_value(harris_kernel, 0, 1,
                             kernel_get_value(harris_kernel, 0, 1) + m01);
            kernel_set_value(harris_kernel, 1, 0,
                             kernel_get_value(harris_kernel, 1, 0) + m10);
            kernel_set_value(harris_kernel, 1, 1,
                             kernel_get_value(harris_kernel, 1, 1) + m11);
        }
    }
    return harris_kernel;
}

image *compute_cornerness(image *img, kernel *hood_kernel, float alpha) {
    image *cornerness_img = make_image(img->height, img->width, 1);
    // compute Ix Iy
    kernel *sobel_x = kernel_make_sobelx();
    kernel *sobel_y = kernel_make_sobely();
    image *dx_img = kernel_convolve(img, sobel_x, MIRROR, 0.0f);
    image *dy_img = kernel_convolve(img, sobel_y, MIRROR, 0.0f);

    kernel *harris_kernel;
    float m00, m01, m10, m11;
    float det, tr;

    float R;
    for (int y = 0; y < dy_img->height; ++y) {
        for (int x = 0; x < dy_img->width; ++x) {
            harris_kernel = extract_matrices(dx_img, dy_img, hood_kernel, y, x);
            m00 = kernel_get_value(harris_kernel, 0, 0);
            m01 = kernel_get_value(harris_kernel, 0, 1);
            m10 = kernel_get_value(harris_kernel, 1, 0);
            m11 = kernel_get_value(harris_kernel, 1, 1);
            det = m00 * m11 - m01 * m10;
            tr = m00 + m11;
            R = det - alpha * tr;
            set_pixel(cornerness_img, y, x, 0, R);
            free_kernel(harris_kernel);
        }
    }
    free_image(dx_img);
    free_image(dy_img);

    free_kernel(sobel_x);
    free_kernel(sobel_y);
    return cornerness_img;
}

image *extract_cornerness(image *img, kernel *hood_kernel, float alpha) {
    if (hood_kernel == NULL) {
        hood_kernel = kernel_make_ones(5, 5);
    }
    image *cornerness_img = compute_cornerness(img, hood_kernel, alpha);
    return cornerness_img;
}

image *compute_harris_corner(image *img, kernel *hood_kernel, float alpha,
                             int nms_hood) {
    image *cornerness_img = extract_cornerness(img, hood_kernel, alpha);
    if (nms_hood > 1) {
        kp_nms_(cornerness_img, nms_hood);
    }
    return cornerness_img;
}
list *detect_harris_keypoints(image *img, kernel *hood_kernel, float alpha,
                              int nms_hood, float threshold) {
    list *kps_list = list_make();
    image *cornerness_img =
        compute_harris_corner(img, hood_kernel, alpha, nms_hood);
    float confidence;
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            confidence = get_pixel(cornerness_img, y, x, 0);
            if (get_pixel(cornerness_img, y, x, 0) > threshold) {
                list_insert(kps_list,
                            make_keypoint(make_point2di(x, y), confidence));
            }
        }
    }
    return kps_list;
}
