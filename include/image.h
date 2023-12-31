#ifndef IMAGE_H
#define IMAGE_H
#include "array.h"
#include "mathx.h"
#include "rect.h"
typedef struct image {
    float *data;
    int width;
    int height;
    int channels;
} image;

typedef struct rgb {
    float r, g, b;
} rgb;

typedef struct gray {
    float level;
} gray;

typedef struct color {
    float *data;
    int channels;
} color;
// print image
void print_image(image);

// image creation
int image_area(image);
image make_empty_image(int height, int width, int channels);
image make_image(int height, int width, int channels);
image make_image_like(const image);
image *make_alloc_image(int height, int width, int channels);

rect image_image_bounding(image img1, image img2);

// load images
image load_image(const char *filename);
image load_image_binary(const char *fname);
image *load_alloc_image(const char *filename);

// save images
void save_image_binary(image im, const char *fname);
void save_png(image im, const char *name);
void save_image(image im, const char *name);

void free_image(image im);
image copy_image(const image img);

int image_size(const image img);
image image_convert_1x3(const image img);

// data retrieving
float get_pixel(const image img, int y, int x, int c);
void set_pixel(image img, int y, int x, int c, float value);

void set_pixel_safe(image img, int y, int x, int c, float v);
void set_get_pixel_mul(image img, image dest, int y, int x, int c, float v);
float get_pixel_safe(const image img, int y, int x, int c);

ok set_color(image img, int y, int x, color color);
color get_color(const image img, int y, int x);
void get_color_(const image img, int y, int x, color c);

ok set_color_safe(image img, int y, int x, color color);
color get_color_safe(const image img, int y, int x);
void get_color_safe_(const image img, int y, int x, color c);

rgb get_rgb(image img, int y, int x);

void set_rgb(image dest, int y, int x, rgb c);
rgb to_rgb(float r, float g, float b);
image image_to_gray(const image img);

// color creation
rgb *alloc_rgb(float r, float g, float b);
color *alloc_empty_color();
color make_gray_color(float gray_level);
void copy_rgb_color(const rgb rgb, color c);
color make_rgb_color(float r, float g, float b);
color *alloc_rgb_color(float r, float g, float b);

color make_color(int channels);
color rgb_to_color(rgb);
void free_color(color c);
image extract_channel(image img, int channel);

image image_from_array(array array, int channel_num, ...) ;

#endif
