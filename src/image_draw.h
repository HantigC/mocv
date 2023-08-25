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

void draw_line_yxyx_(image *img, int start_y, int start_x, int end_y, int end_x);
void draw_x_yx_(image *img, int y, int x, color *color, int length);


#endif
