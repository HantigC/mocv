#ifndef IMAGE_H
#define IMAGE_H
typedef struct image {
    float *data;
    int width;
    int height;
    int channels;
} image;

typedef struct rgb {
    float r, g, b;
} rgb;

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

float rgb_at(image *, int y, int x);
void clamp_image(image *);
image *image_to_gray(image *img) ;
#endif
