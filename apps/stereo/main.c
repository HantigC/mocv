#include "array.h"
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
            y,
            x,
            get_pixel(*img, y, x, 0) * 20);
        for (int i = 1; i < img->channels; i++) {
            printf(", %f", get_pixel(*img, y, x, i) * 20);
        }
        printf(")\n");
    }
}

int main(int argc, char *argv[]) {
    image img1 = load_image("./apps/stereo/resources/bowling/view1.png");
    image img2 = load_image("./apps/stereo/resources/bowling/view2.png");
    rgb red = to_rgb(100.0f, 0.0f, 0.0f);
    int h_radius = 1, w_radius = 1;
    int y = 134, x = 240;
    image disparity =
        dp_disparity(img1, img2, h_radius, w_radius, 0.5, window_ncc);
    image disparity_map = render_disparity(disparity, red);

    // show_image_cv(&img1, "img1", 1, 0, 0);
    // printf("show them");
    // show_image_cv(&img2, "img2", 1, 0, 0);
    // printf("show them");
    show_image_cv(&img1, "img1", 1, 0, 0);
    show_image_cv(&img2, "img2", 1, 0, 0);
    show_image_cv(&disparity_map, "disparity_map", 0, 0, 0);
    return EXIT_SUCCESS;
}
