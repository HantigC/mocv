#ifndef STDX_H
#define STDX_H
enum CMP {
    LT,
    EQ,
    GT,
};
typedef enum CMP (*cmp_fn)(void *, void*);
void qsort_vector_(cmp_fn fn, void **vector, int length);

#endif
