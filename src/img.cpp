#ifdef OPENCV
#include "opencv2/opencv.hpp"
#include <stdlib.h>

extern "C" {
#include "image.h"
cv::Vec3b get_rgb_or_gray(image *img, int y, int x, float scalar) {
    if (img->channels == 3 || img->channels == 4) {
        return cv::Vec3b((unsigned char)(get_pixel(img, y, x, 2) * scalar),
                         (unsigned char)(get_pixel(img, y, x, 1) * scalar),
                         (unsigned char)(get_pixel(img, y, x, 0) * scalar));
    } else {
        return cv::Vec3b((unsigned char)(get_pixel(img, y, x, 0) * scalar),
                         (unsigned char)(get_pixel(img, y, x, 0) * scalar),
                         (unsigned char)(get_pixel(img, y, x, 0) * scalar));
    }
}

cv::Mat image_to_mat(image *im) {
    int i, j;
    cv::Mat m(im->height, im->width, CV_8UC3);
    for (j = 0; j < im->height; ++j) {
        for (i = 0; i < im->width; ++i) {
            m.at<cv::Vec3b>(j, i) = get_rgb_or_gray(im, j, i, 255.0f);
        }
    }
    return m;
}

cv::Mat image_to_mat_noscale(image *im) {
    int i, j;
    cv::Mat m(im->height, im->width, CV_8UC3);
    for (j = 0; j < im->height; ++j) {
        for (i = 0; i < im->width; ++i) {
            m.at<cv::Vec3b>(j, i) = get_rgb_or_gray(im, j, i, 1.0f);
        }
    }
    return m;
}
image *mat_to_image(cv::Mat m) {
    image *im = make_image(m.cols, m.rows, 3);
    int i, j;
    for (j = 0; j < im->height; ++j) {
        for (i = 0; i < im->width; ++i) {
            cv::Vec3b intensity = m.at<cv::Vec3b>(j, i);
            float blue = intensity.val[0] / 255.;
            float green = intensity.val[1] / 255.;
            float red = intensity.val[2] / 255.;
            set_pixel(im, j, i, 0, red);
            set_pixel(im, j, i, 1, green);
            set_pixel(im, j, i, 2, blue);
        }
    }
    return im;
}

image *load_image_cv(const char *filename, int channels) {
    int flag = -1;
    if (channels == 0)
        flag = -1;
    else if (channels == 1)
        flag = 0;
    else if (channels == 3)
        flag = 1;
    else {
        fprintf(stderr, "OpenCV can't force load with %d channels\n", channels);
    }
    cv::Mat m;
    m = cv::imread(filename, flag);
    if (!m.data) {
        fprintf(stderr, "Cannot load image \"%s\"\n", filename);
        char buff[256];
        sprintf(buff, "echo %s >> bad.list", filename);
        system(buff);
        return make_image(10, 10, 3);
        // exit(0);
    }
    image *im = mat_to_image(m);
    if (channels)
        assert(im->channels == channels);
    return im;
}
int show_image_cv(image *im, const char *name, int no_scale, int ms) {
    cv::Mat m;
    if (no_scale)
        m = image_to_mat_noscale(im);
    else
        m = image_to_mat(im);
    imshow(name, m);
    int c = cv::waitKey(ms);
    if (c != -1)
        c = c % 256;
    return c;
}

int load_show_image_cv(const char *imagename, int channels, const char *name,
                       int ms) {
    image *im = load_image_cv(imagename, channels);
    cv::Mat m = image_to_mat(im);
    imshow(name, m);
    int c = cv::waitKey(ms);
    if (c != -1)
        c = c % 256;
    return c;
}
}
#endif
