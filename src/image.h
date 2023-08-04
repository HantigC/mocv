#ifndef IMAGE_H
#define IMAGE_H
#include "mathx.h"
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

// image creation
int image_area(image *);
image *make_empty_image(int height, int width, int channels);
image *make_image(int height, int width, int channels);
image *make_image_like(image *);

// load images
image *load_image(char *filename);
image *load_image_binary(const char *fname);

// save images
void save_image_binary(image im, const char *fname);
void save_png(image *im, const char *name);
void save_image(image *im, const char *name);

void free_image(image *im);
image *copy_image(image *img);

int image_size(image *img);
// data retrieving
float get_pixel(image *img, int y, int x, int c);
void set_pixel(image *img, int y, int x, int c, float v);
void set_get_pixel_mul(image *img, image *dest, int y, int x, int c, float v);

rgb* get_rgb(image *, int y, int x);
void set_rgb(image *, int y, int x, rgb *);
void clamp_image(image *);
image *image_to_gray(image *img);

rgb* make_rgb(float r, float g, float b);
color* make_empty_color();
color* make_gray_color(float gray_level);
color* copy_rgb_color(rgb *rgb);
color* make_rgb_color(float r, float g, float b);

ok set_color(image *img, int y, int x, color *c);


#endif
