#include "rect.h"
#include <stdio.h>

rect rect_from_yxhw(int y, int x, int h, int w) {
    rect r = {.y = y, .x = x, .h = h, .w = w};
    return r;
}

rect rect_from_2p(point2di tl, point2di br) {
    return rect_from_yxhw(tl.x, tl.y, br.y - tl.y, br.x - tl.x);
}

point2di rect_get_tl(rect r) {
    point2di p = {.x = r.x, .y = r.y};
    return p;
}

rect rect_from_cyxyhw(int cy, int cx, int h, int w) {
    int tl_x = cx - w / 2, tl_y = cy - h / 2;
    rect r = {.x = tl_x, .y = tl_y, .h = h, .w = w};
    return r;
}

point2df rect_center(rect r) {
    float half_h = r.h / 2.0f, half_w = r.w / 2.0f;
    point2df c = {.x = r.x + half_w, .y = r.y + half_h};
    return c;
}

point2di get_tli(rect r) {
    point2di tl = {.x = r.x, .y = r.y};
    return tl;
}
point2di get_bri(rect r) {
    point2di br = {.x = r.x + r.w, .y = r.y + r.h};
    return br;
}

void print_rect(rect r) {
    printf("Rect(y=%d, x=%d, h=%d, w=%d)", r.y, r.x, r.h, r.w);
}
