#include "stdx.h"

void swap(void **x, void **y) {
    void *aux = *x;
    *x = *y;
    *y = aux;
}

void _qsort_(cmp_fn fn, void **vector, int start, int end) {
    if (start >= end || start < 0) {
        return;
    }
    int count = start - 1;
    for (int i = start; i < end; i++) {
        switch (fn(vector[i], vector[end])) {
        case LT:
            count++;
            if (i > count) {
                swap(&vector[i], &vector[count]);
            }
            break;
        default:
            break;
        }
    }
    count++;
    swap(&vector[end], &vector[count]);
    _qsort_(fn, vector, start, count - 1);
    _qsort_(fn, vector, count + 1, end);
}

void qsort_vector_(cmp_fn fn, void **vector, int length) {
    _qsort_(fn, vector, 0, length - 1);
}
