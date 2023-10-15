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
point2di get_tli(rect);
point2di get_bri(rect);
rect rect_from_yxhw(int y, int x, int h, int w);
point2di rect_get_tl(rect r);
point2df rect_center(rect);
rect rect_from_cyxyhw(int cy, int cx, int h, int w);
void print_rect(rect);

#endif
