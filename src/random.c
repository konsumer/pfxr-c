#include "../include/pfxr.h"
#include <time.h>

static uint32_t pfxr_random_uint32(pfxr_random_t* rng);

void pfxr_random_init(pfxr_random_t* rng, uint32_t seed) {
    if (seed == 0) {
        seed = (uint32_t)time(NULL);
    }
    
    rng->seed = seed;
    rng->x = seed & 0xffffffff;
    rng->y = 362436069;
    rng->z = 521288629;
    rng->w = 88675123;
    
    // Warm up the generator (same as TypeScript version)
    for (int i = 0; i < 32; i++) {
        pfxr_random_uint32(rng);
    }
}

static uint32_t pfxr_random_uint32(pfxr_random_t* rng) {
    uint32_t t = rng->x ^ ((rng->x << 11) & 0xffffffff);
    rng->x = rng->y;
    rng->y = rng->z;
    rng->z = rng->w;
    rng->w = rng->w ^ (rng->w >> 19) ^ (t ^ (t >> 8));
    return rng->w + 0x80000000;
}

float pfxr_random_float(pfxr_random_t* rng, float min, float max) {
    if (max < min) {
        float temp = min;
        min = max;
        max = temp;
    }
    
    uint32_t random_val = pfxr_random_uint32(rng);
    float normalized = (float)random_val / (float)0xffffffff;
    return min + (max - min) * normalized;
}

int pfxr_random_bool(pfxr_random_t* rng, float true_probability) {
    return pfxr_random_float(rng, 0.0f, 1.0f) < true_probability;
}

int pfxr_random_choice(pfxr_random_t* rng, const int* choices, int count) {
    if (count <= 0) return 0;
    int index = (int)pfxr_random_float(rng, 0.0f, (float)count);
    if (index >= count) index = count - 1;
    return choices[index];
}