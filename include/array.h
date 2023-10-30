#ifndef ARRAY_H
#define ARRAY_H
#include <stdarg.h>

typedef struct array {
    int length;
    int nd;
    int *shape;
    float *data;
} array;

array make_shaped_array(int nd, ...);
void free_array(array a);

void shuffle_int_array_(int *a, int length);
int *int_range(int n);
float max_float(float *xs, int length);
int array_location(array a, int *coordinate);

float array_at(array a, ...);
void array_set_at(float v, array a, ...);

#define ARRAY_AT(a, ...) array_at(a, __VA_ARGS__)
#define ARRAY_SET_AT(a, v, ...) array_set_at(v, a, __VA_ARGS__)
#define idx(...) __VA_ARGS__

#endif /* ARRAY_H */
