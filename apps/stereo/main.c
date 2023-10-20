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
            y, x, get_pixel(*img, y, x, 0) * 50);
        for (int i = 1; i < img->channels; i++) {
            printf(", %f", get_pixel(*img, y, x, i) * 50);
        }
        printf(")\n");
    }
}

int main(int argc, char *argv[]) {
    image img1 = load_image("./apps/stereo/resources/bowling/view0.png");
    image img5 = load_image("./apps/stereo/resources/bowling/view1.png");
    rgb red = to_rgb(1.0f, 0.0f, 0.0f);
    int window_h = 15, window_w = 15;
    int y = 50, x = 60;
    draw_rectangle_yxhw_rgb_(img1, y, x, window_h, window_w, red);
    point2di p = horizontal_scan(img1, img5, y + window_h / 2, x + window_w / 2,
                                 window_h, window_w, sad);
    draw_rectangle_yxhw_rgb_(img5, p.y - window_h / 2, p.x - window_w / 2,
                             window_h, window_w, red);
    print_point2di(p);
    show_image_cv(&img1, "img1", 1, 0, 0);
    show_image_cv(&img5, "img2", 1, 0, 0);
    printf("compute stereo\n");

    clock_t t;
    t = clock();
    image stereo_disparity = sad_disparity(img1, img5, 9, 9);
    clip_values_(stereo_disparity, 0, 50);
    image stereo_disparity_pow = image_pow2(stereo_disparity);

    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC;
    printf("time taken to compute stereo: %f\n", time_taken);
    image_min_max_norm_(stereo_disparity);
    image_min_max_norm_(stereo_disparity_pow);
    show_image_cv(&stereo_disparity, "stereo_disparity", 1, click_on_disparity,
                  &stereo_disparity);
    show_image_cv(&stereo_disparity_pow, "stereo_disparity_pow", 0, 0, 0);
    return EXIT_SUCCESS;
}
