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
int min_int(int cnt, ...) {
    va_list ap;
    int minimum;
    va_start(ap, cnt);
    minimum = va_arg(ap, int);
    int current;
    for (int i = 1; i < cnt; i++) {
        current = va_arg(ap, int);
        minimum = MIN(minimum, current);
    }
    va_end(ap);
    return minimum;
}
int max_int(int cnt, ...) {
    va_list ap;
    int maximum;
    va_start(ap, cnt);
    maximum = va_arg(ap, int);
    int current;
    for (int i = 1; i < cnt; i++) {
        current = va_arg(ap, int);
        maximum = MAX(maximum, current);
    }
    va_end(ap);
    return maximum;
}

float euclidean_distancef(point2df sp, point2df ep) {
    float dx = sp.x - ep.x;
    float dy = sp.y - ep.y;
    return sqrt(dx * dx + dy * dy);
}

point2df compute_middlef(point2df sp, point2df ep){
    point2df middle = {.x = (sp.x + ep.x) / 2, .y = (sp.y + ep.y) / 2};
    return middle;
}
