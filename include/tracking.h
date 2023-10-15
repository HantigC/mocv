#include "image.h"
#include "kernel.h"
#include "rect.h"

rect meanshift(image prob_img, rect bbox, float eps, int iterations,
               float expansion);

rect camshift(image prob_img, rect bbox, float eps, int iterations,
              float expansion);
