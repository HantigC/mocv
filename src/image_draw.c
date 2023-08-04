#include "image_draw.h"
#include "mathx.h"

void draw_rectangle_yxhw_(image *img, int y, int x, int h, int w,
                          color *color) {
    y = CLAMP(y, 0, img->height);
    x = CLAMP(x, 0, img->width);

    for (int i = y; i < h + y; i++) {
        set_color(img, i, x, color);
        set_color(img, i, x + w, color);
    }

    for (int j = x; j < w + x; j++) {
        set_color(img, y, j, color);
        set_color(img, y + h, j, color);
    }
}
void draw_rectangle_yxhw_rgb_(image *img, int y, int x, int h, int w,
                              rgb *color) {
    draw_rectangle_yxhw_(img, y, x, h, w, copy_rgb_color(color));
}
void fill_rectangle_yxhw_(image *img, int y, int x, int h, int w,
                          color *color) {
    y = CLAMP(y, 0, img->height);
    x = CLAMP(x, 0, img->width);
    for (int i = y; i < h + y + 1; i++) {
        for (int j = x; j < w + x + 1; j++) {
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
    int w = br->y - tl->y;
    fill_rectangle_yxhw_(img, tl->y, tl->x, h, w, color);
}

void draw_rectangle_tlbr_rgb_(image *img, point2di *tl, point2di *br,
                              rgb *color) {

    int h = br->y - tl->y;
    int w = br->y - tl->y;
    draw_rectangle_yxhw_rgb_(img, tl->y, tl->x, h, w, color);
}
void fill_rectangle_tlbr_rgb_(image *img, point2di *tl, point2di *br,
                              rgb *color){
    int h = br->y - tl->y;
    int w = br->y - tl->y;
    fill_rectangle_yxhw_rgb_(img, tl->y, tl->x, h, w, color);
}
