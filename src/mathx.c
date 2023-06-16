#include "mathx.h"
float gaussian(float x, float mu, float sigma) {
    float a = (x - mu) / sigma;
    return exp(-0.5f * a * a);
}
