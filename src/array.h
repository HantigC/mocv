#ifndef ARRAY_H
#define ARRAY_H
typedef struct array {
    int length;
    void *data;
    int block_size;
} array;

void shuffle_int_array_(int *a, int length);
int *int_range(int n);
float max_float(float *xs, int length);

#endif
