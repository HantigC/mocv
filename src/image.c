#include "image.h"
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

image *make_empty_image(int height, int width, int channels) {
    image *img = (image *)malloc(sizeof(image));
    img->width = width;
    img->height = height;
    img->channels = channels;
    return img;
}

image *make_image(int height, int width, int channels) {
    image *img = make_empty_image(height, width, channels);
    img->data = (float *)calloc(width * height * channels, sizeof(float));
    return img;
}

image *make_image_like(image *img) {
    return make_image(img->height, img->width, img->channels);
}

void save_image_stb(image *im, const char *name, int png) {
    char buff[256];
    unsigned char *data = (unsigned char *)calloc(
        im->width * im->height * im->channels, sizeof(char));
    int i, k;
    for (k = 0; k < im->channels; ++k) {
        for (i = 0; i < im->width * im->height; ++i) {
            data[i * im->channels + k] = (unsigned char)roundf(
                (255 * im->data[i + k * im->width * im->height]));
        }
    }
    int success = 0;
    if (png) {
        sprintf(buff, "%s.png", name);
        success = stbi_write_png(buff, im->width, im->height, im->channels,
                                 data, im->width * im->channels);
    } else {
        sprintf(buff, "%s.jpg", name);
        success = stbi_write_jpg(buff, im->width, im->height, im->channels,
                                 data, 100);
    }
    free(data);
    if (!success)
        fprintf(stderr, "Failed to write image %s\n", buff);
}

void save_png(image *im, const char *name) { save_image_stb(im, name, 1); }

void save_image(image *im, const char *name) { save_image_stb(im, name, 0); }

image *load_image_stb(char *filename, int channels) {
    int w, h, c;
    unsigned char *data = stbi_load(filename, &w, &h, &c, channels);
    if (!data) {
        fprintf(stderr, "Cannot load image \"%s\"\nSTB Reason: %s\n", filename,
                stbi_failure_reason());
        exit(0);
    }
    if (channels)
        c = channels;
    int i, j, k;
    image *im = make_image(h, w, c);
    for (k = 0; k < c; ++k) {
        for (j = 0; j < h; ++j) {
            for (i = 0; i < w; ++i) {
                int src_index = k + c * i + c * w * j;
                set_pixel(im, j, i, k, (float)data[src_index] / 255.);
            }
        }
    }
    if (im->channels == 4)
        im->channels = 3;
    free(data);
    return im;
}

image *load_image(char *filename) {
    image *out = load_image_stb(filename, 0);
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

image *load_image_binary(const char *fname) {
    int w = 0;
    int h = 0;
    int c = 0;
    FILE *fp = fopen(fname, "rb");
    fread(&w, sizeof(int), 1, fp);
    fread(&h, sizeof(int), 1, fp);
    fread(&c, sizeof(int), 1, fp);
    image *im = make_image(h, w, c);
    fread(im->data, sizeof(float), im->width * im->height * im->channels, fp);
    return im;
}

void free_image(image *im) {
    free(im->data);
    free(im);
}

image *copy_image(image *img) {
    image *dest_img = make_empty_image(img->width, img->height, img->channels);
    for (int i = 0; i < image_size(img); ++i) {
        dest_img->data[i] = img->data[i];
    }
    return dest_img;
}

int compute_location(image *img, int y, int x, int c) {
    return y * img->width * img->channels + x * img->channels + c;
}

int image_size(image *img) { return img->width * img->height * img->channels; }

void set_pixel(image *img, int y, int x, int c, float value) {
    int location = compute_location(img, y, x, c);
    img->data[location] = value;
}

float get_pixel(image *img, int y, int x, int c) {
    int location = compute_location(img, y, x, c);
    return img->data[location];
}

void set_get_pixel_mul(image *img, image *dest, int y, int x, int c, float v){
    set_pixel(dest, y, x, c, get_pixel(img, y, x, c) * v);
}
image *image_to_gray(image *img) {
    image *des = make_image(img->height, img->width, 1);
    float mean = 0.0f;
    for (int y = 0; y < img->height; ++y) {
        for (int x = 0; x < img->width; ++x) {
            mean = (get_pixel(img, y, x, 0) + get_pixel(img, y, x, 1) +
                    get_pixel(img, y, x, 2)) /
                   3.0f;
            set_pixel(des, y, x, 0, mean);
        }
    }
    return des;
}
