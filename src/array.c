#include "array.h"
#include "mathx.h"
#include <math.h>
#include <stdlib.h>

void shuffle_int_array_(int *a, int length) {
    int swap, idx;
    for (int i = length - 1; i > 0; --i) {
        idx = rand() % i;
        swap = a[i];
        a[i] = a[idx];
        a[idx] = swap;
    }
}

int *int_range(int n) {
    int *a = (int *)calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) {
        a[i] = i;
    }
    return a;
}

float max_float(float *xs, int length) {
    float max_so_far = xs[0];
    for (int i = 1; i < length; i++) {
        max_so_far = MAX(xs[i], max_so_far);
    }
    return max_so_far;
}

array make_shaped_array(int nd, ...) {
    array a;
    a.nd = nd;
    a.shape = (int *)calloc(nd, sizeof(int));
    a.length = 1;

    va_list ap;

    va_start(ap, nd);
    for (int i = 0; i < nd; i++) {
        a.shape[i] = va_arg(ap, int);
        a.length *= a.shape[i];
    }
    va_end(ap);
    a.data = (float *)calloc(a.length, sizeof(float));
    return a;
}

int array_location(array a, int *coordinate) {
    int dim = a.nd;

    int i;
    int location = coordinate[a.nd - 1];
    int offset = a.shape[a.nd - 1];
    for (i = a.nd - 2; i >= 0; i--) {
        location += offset * coordinate[i];
        offset *= a.shape[i];
    }
    return location;
}
void extract_coorinate(int dim, va_list ap, int *coord) {
    for (int i = 0; i < dim; i++) {
        coord[i] = va_arg(ap, int);
    }
}

float array_at(array a, ...) {

    int coordinate[a.nd];
    va_list ap;
    va_start(ap, a);
    extract_coorinate(a.nd, ap, coordinate);
    va_end(ap);
    int location = array_location(a, coordinate);
    return a.data[location];
}

void array_set_at(float v, array a, ...) {
    int coordinate[a.nd];
    va_list ap;
    va_start(ap, a);
    extract_coorinate(a.nd, ap, coordinate);
    va_end(ap);
    int location = array_location(a, coordinate);
    a.data[location] = v;
}

void free_array(array a) { free(a.data); }
