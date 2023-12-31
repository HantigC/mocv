#include "image.h"
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void print_image(image img) {
    printf("Image(width=%d, height=%d, channels=%d)",
           img.width,
           img.height,
           img.channels);
}

image make_empty_image(int height, int width, int channels) {
    image img = {0};
    img.width = width;
    img.height = height;
    img.channels = channels;
    return img;
}

image make_image(int height, int width, int channels) {
    image img = {0};
    img.data = (float *)calloc(width * height * channels, sizeof(float));
    img.width = width;
    img.height = height;
    img.channels = channels;
    return img;
}

image make_image_like(const image img) {
    return make_image(img.height, img.width, img.channels);
}

void save_image_stb(image im, const char *name, int png) {
    char buff[256];
    unsigned char *data = (unsigned char *)calloc(
        im.width * im.height * im.channels, sizeof(char));
    int i, k;
    for (k = 0; k < im.channels; ++k) {
        for (i = 0; i < im.width * im.height; ++i) {
            data[i * im.channels + k] = (unsigned char)roundf(
                (255 * im.data[i + k * im.width * im.height]));
        }
    }
    int success = 0;
    if (png) {
        sprintf(buff, "%s.png", name);
        success = stbi_write_png(buff,
                                 im.width,
                                 im.height,
                                 im.channels,
                                 data,
                                 im.width * im.channels);
    } else {
        sprintf(buff, "%s.jpg", name);
        success =
            stbi_write_jpg(buff, im.width, im.height, im.channels, data, 100);
    }
    free(data);
    if (!success)
        fprintf(stderr, "Failed to write image %s\n", buff);
}

void save_png(image im, const char *name) { save_image_stb(im, name, 1); }
void save_image(image im, const char *name) { save_image_stb(im, name, 0); }

image *make_alloc_image(int height, int width, int channels) {
    image *img = (image *)malloc(sizeof(image));
    img->width = width;
    img->height = height;
    img->channels = channels;
    img->data = calloc(height * width * channels, sizeof(float));
    return img;
}
image *load_image_stb(const char *filename, int channels) {
    int w, h, c;
    unsigned char *data = stbi_load(filename, &w, &h, &c, channels);
    if (!data) {
        fprintf(stderr,
                "Cannot load image \"%s\"\nSTB Reason: %s\n",
                filename,
                stbi_failure_reason());
        exit(0);
    }
    if (channels)
        c = channels;
    int i, j, k;
    image *im = make_alloc_image(h, w, c);
    for (k = 0; k < c; ++k) {
        for (j = 0; j < h; ++j) {
            for (i = 0; i < w; ++i) {
                int src_index = k + c * i + c * w * j;
                set_pixel(*im, j, i, k, (float)data[src_index]);
            }
        }
    }
    if (im->channels == 4)
        im->channels = 3;
    free(data);
    return im;
}

image *load_alloc_image(const char *filename) {
    return load_image_stb(filename, 0);
}

image load_image(const char *filename) {
    image out = *load_image_stb(filename, 0);
    return out;
}

void save_image_binary(image im, const char *fname) {
    FILE *fp = fopen(fname, "wb");
    fwrite(&im.width, sizeof(int), 1, fp);
    fwrite(&im.width, sizeof(int), 1, fp);
    fwrite(&im.width, sizeof(int), 1, fp);
    fwrite(im.data, sizeof(float), im.width * im.width * im.channels, fp);
    fclose(fp);
}

image load_image_binary(const char *fname) {
    int w = 0;
    int h = 0;
    int c = 0;
    FILE *fp = fopen(fname, "rb");
    fread(&w, sizeof(int), 1, fp);
    fread(&h, sizeof(int), 1, fp);
    fread(&c, sizeof(int), 1, fp);
    image im = make_image(h, w, c);
    fread(im.data, sizeof(float), im.width * im.height * im.channels, fp);
    return im;
}

void free_image(image im) { free(im.data); }

image copy_image(const image img) {
    image dest_img = make_empty_image(img.width, img.height, img.channels);
    for (int i = 0; i < image_size(img); ++i) {
        dest_img.data[i] = img.data[i];
    }
    return dest_img;
}

int compute_location(const image img, int y, int x, int c) {
    return c * img.height * img.width + y * img.width + x;
}

int image_size(const image img) {
    return img.width * img.height * img.channels;
}

rgb get_rgb(image img, int y, int x) {
    rgb c;
    c.r = get_pixel(img, y, x, 0);
    c.g = get_pixel(img, y, x, 1);
    c.b = get_pixel(img, y, x, 2);
    return c;
}

void set_rgb(image img, int y, int x, rgb c) {
    set_pixel(img, y, x, 0, c.r);
    set_pixel(img, y, x, 1, c.g);
    set_pixel(img, y, x, 2, c.b);
}

void set_pixel(image img, int y, int x, int c, float value) {
    int location = compute_location(img, y, x, c);
    img.data[location] = value;
}

float get_pixel(const image img, int y, int x, int c) {
    int location = compute_location(img, y, x, c);
    return img.data[location];
}

float get_pixel_safe(const image img, int y, int x, int c) {
    y = CLAMP(y, 0, img.height - 1);
    x = CLAMP(x, 0, img.width - 1);
    return get_pixel(img, y, x, c);
}

void set_get_pixel_mul(image img, image dest, int y, int x, int c, float v) {
    set_pixel(dest, y, x, c, get_pixel(img, y, x, c) * v);
}

image image_to_gray(const image img) {
    image des = make_image(img.height, img.width, 1);
    float gray_value;
    for (int y = 0; y < des.height; ++y) {
        for (int x = 0; x < des.width; ++x) {
            gray_value = 0.299f * get_pixel(img, y, x, 0) +
                         0.587f * get_pixel(img, y, x, 1) +
                         0.114f * get_pixel(img, y, x, 2);
            set_pixel(des, y, x, 0, gray_value);
        }
    }
    return des;
}

color *alloc_empty_color() {
    color *c = (color *)malloc(sizeof(color));
    return c;
}

color make_color(int channels) {
    color c;
    c.channels = channels;
    c.data = (float *)calloc(channels, sizeof(float));
    return c;
}

color rgb_to_color(rgb r) {
    color c = make_color(3);
    c.data[0] = r.r;
    c.data[1] = r.g;
    c.data[2] = r.b;
    return c;
}

void free_color(color c) { free(c.data); }

color make_gray_color(float gray_level) {
    color c = {0};
    c.channels = 1;
    c.data = (float *)malloc(sizeof(float));
    c.data[0] = gray_level;
    return c;
}

void copy_rgb_color(const rgb rgb, color c) {
    c.data[0] = rgb.r;
    c.data[1] = rgb.g;
    c.data[2] = rgb.b;
}

color make_rgb_color(float r, float g, float b) {
    color c = make_color(3);
    c.data[0] = r;
    c.data[1] = g;
    c.data[2] = b;
    return c;
}
color *alloc_rgb_color(float r, float g, float b) {

    color *c = alloc_empty_color();
    c->channels = 3;
    c->data = (float *)calloc(sizeof(float), 3);
    c->data[0] = r;
    c->data[1] = g;
    c->data[2] = b;
    return c;
}
void set_pixel_safe(image img, int y, int x, int c, float v) {
    y = CLAMP(y, 0, img.height - 1);
    x = CLAMP(x, 0, img.width - 1);
    set_pixel(img, y, x, c, v);
}
ok set_color_safe(image img, int y, int x, color color) {
    if (img.channels != color.channels)
        return FAIL;
    for (int c = 0; c < color.channels; c++) {
        set_pixel_safe(img, y, x, c, color.data[c]);
    }
    return OK;
}

ok set_color(image img, int y, int x, color color) {
    if (img.channels != color.channels)
        return FAIL;
    for (int c = 0; c < color.channels; c++) {
        set_pixel(img, y, x, c, color.data[c]);
    }
    return OK;
}

color get_color(const image img, int y, int x) {
    color c = make_color(img.channels);
    for (int i = 0; i < img.channels; i++) {
        c.data[i] = get_pixel(img, y, x, i);
    }
    return c;
}

color get_color_safe(const image img, int y, int x) {
    color c = make_color(img.channels);
    for (int i = 0; i < img.channels; i++) {
        c.data[i] = get_pixel_safe(img, y, x, i);
    }
    return c;
}

void get_color_(const image img, int y, int x, color c) {
    for (int i = 0; i < img.channels; i++) {
        c.data[i] = get_pixel(img, y, x, i);
    }
}
void get_color_safe_(const image img, int y, int x, color c) {
    for (int i = 0; i < img.channels; i++) {
        c.data[i] = get_pixel_safe(img, y, x, i);
    }
}

rgb *alloc_rgb(float r, float g, float b) {
    rgb *color = (rgb *)malloc(sizeof(rgb));
    color->r = r;
    color->g = g;
    color->b = b;
    return color;
}

image image_convert_1x3(const image img) {
    image dest = make_image(img.height, img.width, 3);
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            set_pixel(dest, y, x, 0, get_pixel(img, y, x, 0));
            set_pixel(dest, y, x, 1, get_pixel(img, y, x, 0));
            set_pixel(dest, y, x, 2, get_pixel(img, y, x, 0));
        }
    }
    return dest;
}

rgb to_rgb(float r, float g, float b) {
    rgb rg = {.r = r, .g = g, .b = b};
    return rg;
}

image extract_channel(image img, int channel) {
    image dest = make_image(img.height, img.width, 1);
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            set_pixel(dest, y, x, 0, get_pixel(img, y, x, channel));
        }
    }
    return dest;
}

image image_from_array(array a, int channel_num, ...) {
    image dest = make_image(a.shape[0], a.shape[1], channel_num);
    int channel;
    va_list va;
    va_start(va, channel_num);
    for (int c = 0; c < channel_num; c++) {
        channel = va_arg(va, int);
        for (int y = 0; y < a.shape[0]; y++) {
            for (int x = 0; x < a.shape[1]; x++) {
                set_pixel(dest, y, x, c, array_at(a, idx(y, x, channel)));
            }
        }
    }
    va_end(va);
    return dest;
}
