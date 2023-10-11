#include "image.h"
#include "rect.h"
#include "kernel.h"

rect meanshift(const image prob_img, const rect bbox, kernel krn, float eps,
               int iterations);
