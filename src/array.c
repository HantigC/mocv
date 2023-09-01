#include "array.h"
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
