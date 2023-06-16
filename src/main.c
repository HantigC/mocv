#include "image.h"
#include "img.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    image *img = load_image("resources/lena.jpeg");
    show_image_cv(img, "merge", 0);
}
