#ifndef IMG_H
#define IMG_H
#include "image.h"
#include "list.h"
int show_image_cv(image *img, char *window_name, int no_scale, int wait_time);
int load_show_image_cv(const char *imagename, int channels, const char *name,
                       int ms);

int show_image_sequence_cv(list *image_sequence, const char *window_name,
                           int fps, int scale);
list *load_image_sequence(const char *filename);
#endif
