#include "rect.h"
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
