#include "optical_flow/lk.h"
#include "assert.h"
#include "image_draw.h"
#include "kernel.h"
#include "mathx.h"
#include "matrix.h"
#include <stdio.h>

image compute_image_dt(image image_t0, image image_t1) {
    image image_dt = make_image(image_t0.height, image_t0.width, 1);
    float pixel_dt;
    float p0, p1;

    for (int y = 0; y < image_t0.height; y++) {
        for (int x = 0; x < image_t0.width; x++) {
            p1 = get_pixel(image_t1, y, x, 0);
            p0 = get_pixel(image_t0, y, x, 0);
            pixel_dt = p1 - p0;
            set_pixel(image_dt, y, x, 0, pixel_dt);
        }
    }
    return image_dt;
}

point2df compute_flow_direction(image image_dx, image image_dy, image image_dt,
                                int y, int x, kernel krn) {
    point2df p = {0};
    int half_w = krn.width / 2;
    int half_h = krn.height / 2;
    matrix A = make_matrix(2, 2);
    matrix b = make_matrix(2, 1);
    double dx, dy, dt;
    for (int i = 0; i < krn.height; i++) {
        for (int j = 0; j < krn.width; j++) {
            dx = (double)get_pixel_safe(image_dx, y - half_h + i,
                                        x - half_w + j, 0);
            dy = (double)get_pixel_safe(image_dy, y - half_h + i,
                                        x - half_w + j, 0);
            dt = (double)get_pixel_safe(image_dt, y - half_h + i,
                                        x - half_w + j, 0);
            A.data[0][0] += dx * dx;
            A.data[0][1] += dx * dy;
            A.data[1][0] += dx * dy;
            A.data[1][1] += dy * dy;

            b.data[0][0] += -dx * dt;
            b.data[1][0] += -dy * dt;
        }
    }
    matrix Ainv = matrix_invert(A);
    if (Ainv.data == 0) {
        return p;
    }
    matrix a = matrix_mult_matrix(Ainv, b);
    free_matrix(A);
    free_matrix(b);
    free_matrix(Ainv);
    if (a.data != 0) {
        p.x = a.data[0][0];
        p.y = a.data[1][0];
        free_matrix(a);
    }
    return p;
}

static image compute_flow(image image_t0, image image_t1, kernel weight,
                          int stride) {
    assert(image_t0.channels == 1);
    assert(image_t1.channels == 1);
    int h = (image_t0.height - weight.height) / stride + 1;
    int w = (image_t0.width - weight.width) / stride + 1;
    image flow_image = make_image(h, w, 2);
    kernel sobel_x = kernel_make_sobelx();
    kernel sobel_y = kernel_make_sobely();

    image dx_image = kernel_convolve(image_t0, sobel_x, MIRROR, 0.0f);
    image dy_image = kernel_convolve(image_t0, sobel_y, MIRROR, 0.0f);
    image dt_image = compute_image_dt(image_t0, image_t1);
    point2df flow_direction;
    int half_w = weight.width / 2;
    int half_h = weight.height / 2;
    for (int y = half_h; y < image_t0.height - half_h; y += stride) {
        for (int x = half_w; x < image_t0.width - half_w; x += stride) {
            flow_direction = compute_flow_direction(dx_image, dy_image,
                                                    dt_image, y, x, weight);
            set_pixel(flow_image, (y - half_h) / stride, (x - half_w) / stride,
                      0, flow_direction.x);
            set_pixel(flow_image, (y - half_h) / stride, (x - half_w) / stride,
                      1, flow_direction.y);
        }
    }
    free_image(dx_image);
    free_image(dy_image);
    free_image(dt_image);
    free_kernel(sobel_x);
    free_kernel(sobel_y);
    return flow_image;
}

image extract_lk_flow(image image_t0, image image_t1, kernel weight,
                      int stride) {
    image flow = compute_flow(image_t0, image_t1, weight, stride);
    kernel gaus = kernel_make_gaus(3, 3, 2);
    kernel_convolve(flow, gaus, MIRROR, 0.0f);
    free_kernel(gaus);
    return flow;
}

rgb rgb_from_flow(float dy, float dx) {

    float angle = atan2(dy, dx);
    if (angle < 0) {
        angle = 6.28 + angle;
    }
    angle = 6 * (angle / 6.28);
    int index = floor(angle);
    if (angle < 0) {
        printf("dx=%f, dy=%f", dx, dy);
        printf("angle = %f", angle);
        printf("index = %d", index);
    }
    float f = angle - index;
    float r, g, b;
    if (index == 0) {
        r = 1;
        g = f;
        b = 0;
    } else if (index == 1) {
        r = 1 - f;
        g = 1;
        b = 0;
    } else if (index == 2) {
        r = 0;
        g = 1;
        b = f;
    } else if (index == 3) {
        r = 0;
        g = 1 - f;
        b = 1;
    } else if (index == 4) {
        r = f;
        g = 0;
        b = 1;
    } else {
        r = 1;
        g = 0;
        b = 1 - f;
    }
    return to_rgb(r, b, g);
}
void draw_flow_(image img, image flow_img, int scale, int stride, int offset) {
    int flow_w = img.width / flow_img.width;
    int flow_h = img.width / flow_img.width;
    float dy, dx;
    rgb r;
    int flow_y, flow_x;
    for (int y = offset; y < img.height - 1; y += stride) {
        for (int x = offset; x < img.width - 1; x += stride) {
            flow_y = y / flow_h;
            flow_x = x / flow_w;

            dx = get_pixel(flow_img, flow_y, flow_x, 0);
            dy = get_pixel(flow_img, flow_y, flow_x, 1);
            r = rgb_from_flow(dy, dx);

            // draw_x_yx_(&img, y, x, copy_rgb_color(&r), 3);
            draw_line_yxyx_safe_(img, y, x, y + scale * dy, x + scale * dx,
                                 rgb_to_color(r), 1);
        }
    }
}
