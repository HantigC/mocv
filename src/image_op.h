#ifndef IMAGE_OP
#define IMAGE_OP
#include "image.h"
#include "mathx.h"

void image_muls_(image *, float scalar);
image *image_muls(image *, float scalar);

void image_muls_channel_(image *, float scalar, int channel);
image *image_muls_channel(image *, float scalar, int channel);

void image_min_max_norm_(image *);
image *image_min_max_norm(image *);

minmax *image_get_min_max(image *img, int channel);
#endif
