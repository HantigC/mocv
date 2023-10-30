#ifndef RECT_H
#define RECT_H
#include "mathx.h"
typedef struct rect {
    int x;
    int y;
    int w;
    int h;
} rect;
typedef struct tlbr_rect {
    point2di tl;
    point2di br;
} tlbr_rect;

tlbr_rect make_tlbr_rect(int tly, int tlx, int bry, int brx);

rect rect_from_2p(point2di tl, point2di br);
point2di get_tli(rect);
point2di get_bri(rect);
rect rect_from_yxhw(int y, int x, int h, int w);
point2di rect_get_tl(rect r);
point2df rect_center(rect);
rect rect_from_cyxyhw(int cy, int cx, int h, int w);
void print_rect(rect);
void print_tlbr_rect(tlbr_rect);

#endif
