#ifndef IMG_H
#define IMG_H
#include "image.h"
int show_image_cv(image *, char *, int);
int load_show_image_cv(const char *imagename, int channels, const char *name,
                       int ms);
#endif
