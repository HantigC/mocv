#ifndef IMAGE_OP
#define IMAGE_OP
#include "image.h"
#include "mathx.h"

typedef boolean (*cmp_fn)(float x, float y);
void image_muls_(image *, float scalar);
image *image_muls(image *, float scalar);

void image_muls_channel_(image *, float scalar, int channel);
image *image_muls_channel(image *, float scalar, int channel);

void image_min_max_norm_(image *);
image *image_min_max_norm(image *);

minmax *image_get_min_max(image *img, int channel);
image *image_mask_scalar(image *img, image *mask, float scalar);
image *image_mask_cmp_scalar(image *img, cmp_fn fn, float comparee,
                             float scalar);
image *image_mask_lt_scalar(image *img, float x, float scalar);
image *image_mask_le_scalar(image *img, float x, float scalar);
image *image_mask_gt_scalar(image *img, float x, float scalar);
image *image_mask_ge_scalar(image *img, float x, float scalar);
image *image_mask_eq_scalar(image *img, float x, float scalar);
#endif
