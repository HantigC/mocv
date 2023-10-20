#ifndef IMG_H
#define IMG_H
#include "image.h"
#include "list.h"

typedef void (*mouse_callback)(int event, int x, int y, int flags,
                               void *user_data);
int show_image_cv(image *im, const char *name, int ms, mouse_callback callback,
                  void *user_data);
int load_show_image_cv(const char *imagename, int channels, const char *name,
                       int ms);

int show_image_sequence_cv(list *image_sequence, const char *window_name,
                           int fps, mouse_callback callback);
list *cv_load_image_sequence(const char *filename);
void save_sequence_cv(list *image_sequence, char *name, int fps);
#endif
