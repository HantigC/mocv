#include "mathx.h"
float gaussian(float x, float mu, float sigma) {
    float a = (x - mu) / sigma;
    return exp(-0.5f * a * a);
}

float *normalize(float *xs, int length, float normalizer, int inplace) {
    float *dest;
    if (inplace) {
        dest = xs;
    } else {
        dest = (float *)calloc(length, sizeof(float));
    }
    for (int i = 0; i < length; ++i) {
        dest[i] = xs[i] / normalizer;
    }
    return dest;
}

point2di *make_empty_point2di() { return (point2di *)malloc(sizeof(point2di)); }
point2di *make_point2di(int x, int y) {
    point2di *p = (point2di *)malloc(sizeof(point2di));
    p->x = x;
    p->y = y;
    return p;
}
