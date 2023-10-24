#ifdef OPENCV
#include "opencv2/opencv.hpp"
#include <stdlib.h>

extern "C" {
#include "image.h"
#include "img.h"
#include "list.h"

void print_click_mouse_callback(int event, int x, int y, int flags,
                                void *userdata) {
    image *img = (image *)userdata;
    // Check if the left button is pressed
    if (event == cv::EVENT_LBUTTONDOWN) {
        printf(
            "Left button of the mouse is clicked - position (y=%d, x=%d)=(%f",
            y, x, get_pixel(*img, y, x, 0));
        for (int i = 1; i < img->channels; i++) {
            printf(", %f", get_pixel(*img, y, x, i));
        }
        printf(")\n");
    }
}

image *mat_to_image(cv::Mat m) {
    image *im = make_alloc_image(m.rows, m.cols, 3);
    int i, j;
    for (j = 0; j < im->height; ++j) {
        for (i = 0; i < im->width; ++i) {
            cv::Vec3b intensity = m.at<cv::Vec3b>(j, i);
            float blue = intensity.val[0] / 255.;
            float green = intensity.val[1] / 255.;
            float red = intensity.val[2] / 255.;
            set_pixel(*im, j, i, 0, red);
            set_pixel(*im, j, i, 1, green);
            set_pixel(*im, j, i, 2, blue);
        }
    }
    return im;
}

cv::Vec3b get_rgb_or_gray(image *img, int y, int x) {
    if (img->channels == 3 || img->channels == 4) {
        return cv::Vec3b((unsigned char)(get_pixel(*img, y, x, 2)),
                         (unsigned char)(get_pixel(*img, y, x, 1)),
                         (unsigned char)(get_pixel(*img, y, x, 0)));
    } else {
        return cv::Vec3b((unsigned char)(get_pixel(*img, y, x, 0)),
                         (unsigned char)(get_pixel(*img, y, x, 0)),
                         (unsigned char)(get_pixel(*img, y, x, 0)));
    }
}

cv::Mat image_to_mat(image *im) {
    int i, j;
    cv::Mat m(im->height, im->width, CV_8UC3);
    for (j = 0; j < im->height; ++j) {
        for (i = 0; i < im->width; ++i) {
            m.at<cv::Vec3b>(j, i) = get_rgb_or_gray(im, j, i);
        }
    }
    return m;
}

cv::Mat image_to_mat_noscale(image *im) {
    int i, j;
    cv::Mat m(im->height, im->width, CV_8UC3);
    for (j = 0; j < im->height; ++j) {
        for (i = 0; i < im->width; ++i) {
            m.at<cv::Vec3b>(j, i) = get_rgb_or_gray(im, j, i);
        }
    }
    return m;
}

list *cv_load_image_sequence(const char *filename) {
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
        exit(0);
    }
    image *im = mat_to_image(m);
    if (channels)
        assert(im->channels == channels);
    return im;
}

void _show_image_cv(image *im, const char *name) {
    cv::Mat m;
    m = image_to_mat(im);
    imshow(name, m);
}

int show_image_cv(image *im, const char *name, int ms, mouse_callback callback,
                  void *user_data) {
    cv::Mat m;
    m = image_to_mat(im);
    imshow(name, m);
    if (callback) {
        cv::setMouseCallback(name, callback, user_data);
    } else {
        cv::setMouseCallback(name, print_click_mouse_callback, im);
    }
    int c = cv::waitKey(ms);
    if (c != -1)
        c = c % 256;
    return c;
}

int show_image_sequence_cv(list *image_sequence, const char *window_name,
                           int fps, mouse_callback callback) {

    cv::namedWindow(window_name, cv::WINDOW_NORMAL);
    node *iter = image_sequence->first;
    image *img;
    int wwait_time = (int)1000.0 / fps;

    if (callback) {
        cv::setMouseCallback(window_name, callback, 0);
    } else {
        cv::setMouseCallback(window_name, print_click_mouse_callback, 0);
    }
    while (iter) {
        img = (image *)iter->item;
        _show_image_cv(img, window_name);
        iter = iter->next;
        cv::waitKey(wwait_time);
    }

    cv::destroyWindow(window_name);
    return 0;
}

void save_sequence_cv(list *image_sequence, char *name, int fps) {
    image *img = (image *)image_sequence->first->item;

    cv::VideoWriter out(name, cv::VideoWriter::fourcc('M', 'P', '4', 'V'), fps,
                        cv::Size(img->width, img->height));

    node *n = image_sequence->first;
    while (n) {
        img = (image *)n->item;
        cv::Mat frame = image_to_mat(img);

        if (frame.empty()) {
            std::cerr << "Frame corrupted" << std::endl;
            break;
        }

        out.write(frame);
        n = n->next;
    }

    out.release();
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
