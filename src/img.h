#ifndef IMG_H
#define IMG_H
#include "image.h"
int show_image_cv(image *img, char *window_name, int no_scale, int wait_time);
int load_show_image_cv(const char *imagename, int channels, const char *name,
                       int ms);
#endif
