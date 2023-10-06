#ifdef OPENCV
#include "opencv2/opencv.hpp"
#include <stdlib.h>

extern "C" {
#include "image.h"
#include "list.h"

image *mat_to_image(cv::Mat m) {
    image *im = make_image(m.rows, m.cols, 3);
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

list *load_image_sequence(const char *filename) {
    list *image_list = list_make();

    cv::VideoCapture cap(filename);

    // Check if video opened successfully
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not read video file" << std::endl;
        exit(-1);
    }

    image *img;

    while (true) {
        cv::Mat frame;

        // Capture the next frame
        cap >> frame;

        // Break the loop if the video is over
        if (frame.empty())
            break;

        img = mat_to_image(frame);
        list_insert(image_list, img);
    }
    cap.release();
    return image_list;
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

int show_image_sequence_cv(list *image_sequence, const char *window_name,
                           int fps, int scale) {

    cv::namedWindow(window_name, cv::WINDOW_NORMAL);
    node *iter = image_sequence->first;
    image *img;
    int wwait_time = (int)1000.0 / fps;
    while (iter) {
        img = (image *)iter->item;
        show_image_cv(img, window_name, scale, wwait_time);
        iter = iter->next;
    }

    cv::destroyWindow(window_name);
    return 0;
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
