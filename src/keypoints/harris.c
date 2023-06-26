#include "keypoints/harris.h"
#include "kernel.h"

kernel *extract_matrices(image *dx_img, image *dy_image, int neighbourhood_size,
                         int y, int x) {
    int half = neighbourhood_size / 2;
    float dx, dy;
    kernel *harris_kernel = make_kernel(2, 2);
    kernel_fill_inplace(harris_kernel, 0);
    float m00, m01, m10, m11;
    for (int i = -half; i < half + 1; ++i) {
        for (int j = -half; j < half + 1; ++j) {

            dy = get_pixel(dy_image, y + i, x + j, 0);
            dx = get_pixel(dx_img, y + i, x + j, 0);

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
list *detect_harris(image *img, int neighbourhood_size, float k) {
    kernel *sobel_x = kernel_make_sobelx();
    kernel *sobel_y = kernel_make_sobely();

    image *dx_img = kernel_convolve_no_border(img, sobel_x);
    image *dy_img = kernel_convolve_no_border(img, sobel_y);

    list *keyponts = list_make();
    kernel *harris_kernel;
    float m00, m01, m10, m11;
    float det, tr;
    int half = neighbourhood_size / 2;

    float R;
    for (int y = half; y < dy_img->height; ++y) {
        for (int x = half; x < dy_img->width; ++x) {
            harris_kernel =
                extract_matrices(dx_img, dy_img, neighbourhood_size, y, x);
            m00 = kernel_get_value(harris_kernel, 0, 0);
            m01 = kernel_get_value(harris_kernel, 0, 1);
            m10 = kernel_get_value(harris_kernel, 1, 0);
            m11 = kernel_get_value(harris_kernel, 1, 1);
            det = m00 * m11 - m01 * m10;
            tr = m00 + m11;
            R = det - k * tr;
            list_insert(keyponts, make_keypoint(make_point2di(x, y), R));
            free_kernel(harris_kernel);
        }
    }
    free_image(dx_img);
    free_image(dy_img);
    free_kernel(sobel_x);
    free_kernel(sobel_y);
    return keyponts;
}
