#ifndef IMAGE_DRAW_H
#define IMAGE_DRAW_H
#include "image.h"
#include "list.h"
#include "mathx.h"

void draw_rectangle_yxhw_(image img, int y, int x, int h, int w, color color);
void fill_rectangle_yxhw_(image img, int y, int x, int h, int w, color color);

void fill_rectangle_yxhw_rgb_(image img, int y, int x, int h, int w,
                              rgb rgb_color);
void draw_rectangle_yxhw_rgb_(image img, int y, int x, int h, int w,
                              rgb color);

void draw_rectangle_tlbr_(image img, point2di tl, point2di br, color color);
void fill_rectangle_tlbr_(image img, point2di tl, point2di br, color color);

void draw_rectangle_tlbr_rgb_(image img, point2di tl, point2di br,
                              rgb rgb_color);
void fill_rectangle_tlbr_rgb_(image img, point2di tl, point2di br,
                              rgb rgb_color);

void draw_x_yx_(image img, int y, int x, color color, int length);
void draw_line_yxyx_(image img, int start_y, int start_x, int end_y, int end_x,
                     color c, int thickness);

void draw_line_yxyx_safe_(image img, int start_y, int start_x, int end_y, int end_x,
                          color c, int thickness);

void draw_line_pp_(image img, point2di start_p, point2di end_p, color c,
                   int thickness);

void draw_line_pp_safe_(image img, point2di start_p, point2di end_p,
                        color c, int thickness);
void draw_x_pointi_(image img, point2di p, color color, int length);
void draw_xs_pointi_(image img, list *points, color color, int length);

#endif
