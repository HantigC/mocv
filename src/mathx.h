#ifndef MATHX_H
#define MATHX_H
#include <stdlib.h>
#include <math.h>
typedef struct point2di {
    int x;
    int y;
} point2di;

#define MAX(x, y) x < y ? y : x
#define MIN(x, y) x < y ? x : y
#define CLAMP(x, min_v, max_v) (MIN(MAX(x, min_v), max_v))
#define IS_BETWEEN(x, min_v, max_v) (x < min_v ? 0 : ((x > max_v) ? 0 : 1))

float gaussian(float x, float mu, float sigma);
float *normalize(float *xs, int length, float normalizer, int inplace);
point2di *make_empty_point2di();
point2di *make_point2di(int x, int y);
#endif
