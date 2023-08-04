#ifndef IMAGE_DRAW_H
#define IMAGE_DRAW_H
#include "image.h"
#include "mathx.h"

void draw_rectangle_yxhw_(image *img, int y, int x, int h, int w, color *color);
void fill_rectangle_yxhw_(image *img, int y, int x, int h, int w, color *color);

void draw_rectangle_yxhw_rgb_(image *img, int y, int x, int h, int w, rgb *color);
void fill_rectangle_yxhw_rgb_(image *img, int y, int x, int h, int w, rgb *color);


void draw_rectangle_tlbr_(image *img, point2di *tl, point2di *br, color *color);
void fill_rectangle_tlbr_(image *img, point2di *tl, point2di *br, color *color);

void draw_rectangle_tlbr_rgb_(image *img, point2di *tl, point2di *br, rgb *color);
void fill_rectangle_tlbr_rgb_(image *img, point2di *tl, point2di *br, rgb *color);

#endif
