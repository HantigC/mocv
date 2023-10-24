#include "image.h"
#include "image_draw.h"
#include "image_op.h"
#include "img.h"
#include <stdio.h>
#include <time.h>

void click_on_disparity(int event, int x, int y, int flags, void *userdata) {
    image *img = (image *)userdata;
    // Check if the left button is pressed
    if (event == 1) {
        printf(
            "Left button of the mouse is clicked - position (y=%d, x=%d)=(%f",
            y, x, get_pixel(*img, y, x, 0) * 20);
        for (int i = 1; i < img->channels; i++) {
            printf(", %f", get_pixel(*img, y, x, i) * 20);
        }
        printf(")\n");
    }
}

int main(int argc, char *argv[]) {
    image img1 = load_image("./apps/stereo/resources/tsukuba/img1.png");
    image img2 = load_image("./apps/stereo/resources/tsukuba/img2.png");
    rgb red = to_rgb(255.0f, 0.0f, 0.0f);
    int h_radius = 3, w_radius = 3;
    int y = 134, x = 240;

    printf("compute ncc disparity\n");
    clock_t t;
    t = clock();
    image ncc_disparity_img = ncc_disparity(img1, img2, 4, 4);
    clip_values_(ncc_disparity_img, 0, 20);
    t = clock() - t;

    printf("time taken to compute ncc disparity: %f\n",
           ((double)t) / CLOCKS_PER_SEC);
    t = clock();
    printf("compute ssd disparity\n");
    image ssd_disparity_img = ssd_disparity(img1, img2, 9, 9);
    clip_values_(ssd_disparity_img, 0, 20);
    t = clock() - t;
    printf("time taken to compute ssd disparity: %f\n",
           ((double)t) / CLOCKS_PER_SEC);

    t = clock();
    printf("compute sad disparity\n");
    image sad_disparity_img = sad_disparity(img1, img2, 9, 9);
    clip_values_(sad_disparity_img, 0, 20);
    t = clock() - t;
    printf("time taken to compute ssd disparity: %f\n",
           ((double)t) / CLOCKS_PER_SEC);
    image_min_max_norm_(ncc_disparity_img);
    image_muls_(ncc_disparity_img, 255.0);
    image_min_max_norm_(ssd_disparity_img);
    image_muls_(ssd_disparity_img, 255.0);
    image_min_max_norm_(sad_disparity_img);
    image_muls_(sad_disparity_img, 255.0);
    show_image_cv(&ncc_disparity_img, "stereo_disparity", 1, 0, 0);
    show_image_cv(&ssd_disparity_img, "ssd_disparity_img", 1, 0, 0);
    show_image_cv(&sad_disparity_img, "sad_disparity_img", 0, 0, 0);
    return EXIT_SUCCESS;
}
