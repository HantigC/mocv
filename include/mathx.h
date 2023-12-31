#ifndef MATHX_H
#define MATHX_H
#include <math.h>
#include <stdlib.h>

typedef enum boolean {
    FALSE,
    TRUE,
} boolean;

typedef enum ok {
    FAIL = -1,
    OK = 1,
} ok;

typedef struct point2di {
    int x;
    int y;
} point2di;

typedef struct point2df {
    float x;
    float y;
} point2df;

typedef struct minmax {
    float min;
    float max;
} minmax;

#define MAX(x, y) (x) < (y) ? (y) : (x)
#define ABS(x) (x) >= 0 ? (x) : (-x)
#define MIN(x, y) (x) < (y) ? (x) : (y)
#define MIN3(x, y, z) MIN((x), MIN((y), (z)))
#define MAX3(x, y, z) MAX((x), MAX((y), (z)))
#define CLAMP(x, min_v, max_v) (MIN(MAX((x), (min_v)), (max_v)))
#define IS_BETWEEN(x, min_v, max_v) (x < min_v ? 0 : ((x > max_v) ? 0 : 1))
int min_int(int cnt, ...);
int max_int(int cnt, ...);

float gaussian(float x, float mu, float sigma);
float *normalize(float *xs, int length, float normalizer, int inplace);
point2di *make_empty_point2di();
point2di *make_point2di(int x, int y);
float euclidean_distancef(point2df sp, point2df ep);
point2df compute_middlef(point2df sp, point2df ep);
void print_point2di(point2di p);

float ssd(float x, float y);
float sad(float x, float y);
float acc_mean(float prev_mean, float cur_item, float n);
#endif
