#ifndef RECT_H
#define RECT_H
#include "mathx.h"
typedef struct rect {
    int x;
    int y;
    int w;
    int h;
} rect;

rect rect_from_2p(point2di tl, point2di br);
rect rect_from_yxhw(int y, int x, int h, int w);
point2di rect_get_tl(rect r);
#endif

