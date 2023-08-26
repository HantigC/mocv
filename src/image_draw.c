#include "image_draw.h"
#include "mathx.h"

void draw_rectangle_yxhw_(image *img, int y, int x, int h, int w,
                          color *color) {
    y = CLAMP(y, 0, img->height - 1);
    x = CLAMP(x, 0, img->width - 1);

    h = h + y + 1;
    w = w + x + 1;

    h = CLAMP(h, 0, img->height - 1);
    w = CLAMP(w, 0, img->width - 1);

    for (int i = y; i < h; i++) {
        set_color(img, i, x, color);
        set_color(img, i, w, color);
    }

    for (int j = x; j < w; j++) {
        set_color(img, y, j, color);
        set_color(img, h, j, color);
    }
}
void draw_rectangle_yxhw_rgb_(image *img, int y, int x, int h, int w,
                              rgb *color) {
    draw_rectangle_yxhw_(img, y, x, h, w, copy_rgb_color(color));
}
void fill_rectangle_yxhw_(image *img, int y, int x, int h, int w,
                          color *color) {
    y = CLAMP(y, 0, img->height - 1);
    x = CLAMP(x, 0, img->width - 1);

    h = h + y + 1;
    w = w + x + 1;

    h = CLAMP(h, 0, img->height - 1);
    w = CLAMP(w, 0, img->width - 1);

    for (int i = y; i < h; i++) {
        for (int j = x; j < w; j++) {
            set_color(img, i, j, color);
        }
    }
}

void fill_rectangle_yxhw_rgb_(image *img, int y, int x, int h, int w,
                              rgb *color) {
    fill_rectangle_yxhw_(img, y, x, h, w, copy_rgb_color(color));
}

void draw_rectangle_tlbr_(image *img, point2di *tl, point2di *br,
                          color *color) {
    int h = br->y - tl->y;
    int w = br->y - tl->y;
    draw_rectangle_yxhw_(img, tl->y, tl->x, h, w, color);
}
void fill_rectangle_tlbr_(image *img, point2di *tl, point2di *br,
                          color *color) {

    int h = br->y - tl->y;
    int w = br->x - tl->x;
    fill_rectangle_yxhw_(img, tl->y, tl->x, h, w, color);
}

void draw_rectangle_tlbr_rgb_(image *img, point2di *tl, point2di *br,
                              rgb *color) {

    int h = br->y - tl->y;
    int w = br->y - tl->y;
    draw_rectangle_yxhw_rgb_(img, tl->y, tl->x, h, w, color);
}
void fill_rectangle_tlbr_rgb_(image *img, point2di *tl, point2di *br,
                              rgb *color) {
    int h = br->y - tl->y;
    int w = br->y - tl->y;
    fill_rectangle_yxhw_rgb_(img, tl->y, tl->x, h, w, color);
}

void draw_y_thinkness_(image *img, int y, int x, int thickness, color *c) {
    int half = thickness / 2;
    for (int i = 0; i < thickness; i++) {
        set_color(img, CLAMP(y + i - half, 0, img->height), x, c);
    }
}

void draw_x_thinkness_(image *img, int y, int x, int thickness, color *c) {
    int half = thickness / 2;
    for (int i = 0; i < thickness; i++) {
        set_color(img, y, CLAMP(x + i - half, 0, img->width), c);
    }
}
typedef void (*draw_color_fn)(image *img, int y, int x, int thickness,
                              color *c);
void draw_line_yxyx_low_(image *img, int start_y, int start_x, int end_y,
                         int end_x, color *c, int thickness) {
    int dy = end_y - start_y;
    int dx = end_x - start_x;
    int inc_y = 1;
    if (dy < 0) {
        inc_y = -1;
        dy = -dy;
    }
    int acc = 2 * dy - dx;
    int x = start_x, y = start_y;
    draw_y_thinkness_(img, y, x, thickness, c);
    while (x < end_x) {
        x += 1;
        if (acc < 0) {
            acc += 2 * dy;
        } else {
            y += inc_y;
            acc += 2 * (dy - dx);
        }
        draw_y_thinkness_(img, y, x, thickness, c);
    }
}
void draw_line_yxyx_high_(image *img, int start_y, int start_x, int end_y,
                          int end_x, color *c, int thickness) {
    int dy = end_y - start_y;
    int dx = end_x - start_x;
    int inc_x = 1;
    if (dx < 0) {
        inc_x = -1;
        dx = -dx;
    }
    int acc = 2 * dx - dy;
    int x = start_x, y = start_y;
    draw_x_thinkness_(img, y, x, thickness, c);
    while (x < end_x) {
        y += 1;
        if (acc < 0) {
            acc += 2 * dx;
        } else {
            x += inc_x;
            acc += 2 * (dx - dy);
        }
        draw_x_thinkness_(img, y, x, thickness, c);
    }
}
void draw_line_yxyx_(image *img, int start_y, int start_x, int end_y, int end_x,
                     color *c, int thickness) {
    if (abs(end_y - start_y) < abs(end_x - start_x)) {
        if (start_x > end_x) {
            draw_line_yxyx_low_(img, end_y, end_x, start_y, start_x, c,
                                thickness);
        } else {
            draw_line_yxyx_low_(img, start_y, start_x, end_y, end_x, c,
                                thickness);
        }
    } else {

        if (start_y > end_y) {
            draw_line_yxyx_high_(img, end_y, end_x, start_y, start_x, c,
                                 thickness);
        } else {
            draw_line_yxyx_high_(img, start_y, start_x, end_y, end_x, c,
                                 thickness);
        }
    }
}

void draw_x_yx_(image *img, int y, int x, color *color, int length) {
    set_color(img, y, x, color);
    for (int i = 1; i < length; i++) {
        set_color(img, CLAMP(y - i, 0, img->height),
                  CLAMP(x + i, 0, img->width), color);
        set_color(img, CLAMP(y + i, 0, img->height),
                  CLAMP(x + i, 0, img->width), color);
        set_color(img, CLAMP(y + i, 0, img->height),
                  CLAMP(x - i, 0, img->width), color);
        set_color(img, CLAMP(y - i, 0, img->height),
                  CLAMP(x - i, 0, img->width), color);
    }
}
