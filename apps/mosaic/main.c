#include "histogram.h"
#include "image.h"
#include "img.h"
#include "keypoints/harris.h"
#include "keypoints/keypoint.h"
#include "list.h"
#include "panorama.h"

float l1_d(void *k1, void *k2) {
    keypoint *kp1 = (keypoint *)k1;
    keypoint *kp2 = (keypoint *)k2;
    simple_descriptor *d1 = (simple_descriptor *)kp1->descriptor;
    simple_descriptor *d2 = (simple_descriptor *)kp2->descriptor;
    float total = 0.0f;
    float x;
    for (int i = 0; i < d1->length; i++) {
        x = d1->data[i] - d2->data[i];
        total += ABS(x);
    }
    return total / d1->length;
}

list *foo(image img) {
    image gray = image_to_gray(img);

    kernel gaus = kernel_make_gaus(3, 3, 5.0f);
    image smoothed_img = kernel_convolve(gray, gaus, MIRROR, 255.0f);
    kernel harris_kernel = kernel_make_gaus(3, 3, 2.0f);
    kernel_mul_scalar_(harris_kernel, 9.0f);
    list *harris_kps_list =
        detect_harris_keypoints(smoothed_img, harris_kernel, 0.06, 7, 20.0f);
    extract_patch_descriptors_(img, harris_kps_list, 7);

    free_image(gray);
    free_image(smoothed_img);
    free_kernel(gaus);
    free_kernel(harris_kernel);

    return harris_kps_list;
}

list *extract_keypoint_images(list *images_list) {
    list *keypoints_list = list_make();
    node *n = images_list->first;
    image *img;
    while (n) {
        img = n->item;
        list_insert(keypoints_list, foo(*img));
        n = n->next;
    }
    return keypoints_list;
}

int main() {
    image reiner1 = load_image("apps/mosaic/resources/Rainier1.png");
    image reiner2 = load_image("apps/mosaic/resources/Rainier2.png");
    image reiner3 = load_image("apps/mosaic/resources/Rainier3.png");
    image reiner6 = load_image("apps/mosaic/resources/Rainier6.png");
    list *images_list = list_make();
    list_insert(images_list, &reiner2);
    list_insert(images_list, &reiner1);
    list_insert(images_list, &reiner6);
    list_insert(images_list, &reiner3);
    list *keypoints_list = extract_keypoint_images(images_list);
    image panorama =
        combine_pano(images_list, keypoints_list, l1_d, 1.0f, 20, 100, 6);
    show_image_cv(&reiner1, "Reiner1", 1, 0);
    show_image_cv(&reiner2, "Reiner2", 1, 0);
    show_image_cv(&reiner3, "Reiner3", 1, 0);
    show_image_cv(&reiner6, "Reiner5", 1, 0);
    show_image_cv(&panorama, "panorama", 0, 0);
    save_png(panorama, "apps/mosaic/resources/mosaic");

    return 0;
}
