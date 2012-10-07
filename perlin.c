#include "perlin.h"
#include <math.h>

static int perm[256] = {
    151,160,137,91,90,15,
    131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
    190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
    88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
    77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
    102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
    135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
    5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
    223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
    129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
    251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
    49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

static inline float perm_ref(int index) {
    return perm[index & 0xff];
}

static inline float fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

static inline float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

static inline float grad3d(int hash, float x, float y, float z) {
    int h = hash & 0xf;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) ? u : -u) + ((h & 2) ? v : -v);
}

float perlin_noise_3d(float x, float y, float z) {
    float floor_x = floorf(x);
    float floor_y = floorf(y);
    float floor_z = floorf(z);

    int ix = floor_x;
    int iy = floor_y;
    int iz = floor_z;

    x -= floor_x;
    y -= floor_y;
    z -= floor_z;

    float u = fade(x);
    float v = fade(y);
    float w = fade(z);

    int ia  = perm_ref(ix  ) + iy;
    int ib  = perm_ref(ix+1) + iy;
    int iaa = perm_ref(ia  ) + iz;
    int iba = perm_ref(ib  ) + iz;
    int iab = perm_ref(ia+1) + iz;
    int ibb = perm_ref(ib+1) + iz;

    return lerp(w, lerp(v, lerp(u, grad3d(perm_ref(iaa  ), x  , y  , z  ),
                                   grad3d(perm_ref(iba  ), x-1, y  , z  )),
                           lerp(u, grad3d(perm_ref(iab  ), x  , y-1, z  ),
                                   grad3d(perm_ref(ibb  ), x-1, y-1, z  ))),
                   lerp(v, lerp(u, grad3d(perm_ref(iaa+1), x  , y  , z-1),
                                   grad3d(perm_ref(iba+1), x-1, y  , z-1)),
                           lerp(u, grad3d(perm_ref(iab+1), x  , y-1, z-1),
                                   grad3d(perm_ref(ibb+1), x-1, y-1, z-1))));
}

float perlin_fbm_3d(float x, float y, float z, int octave) {
    float f = 0.0f;
    float w = 0.5f;
    for (int i = 0; i < octave; i++) {
        f += w * perlin_noise_3d(x, y, z);
        x *= 2.0f;
        y *= 2.0f;
        z *= 2.0f;
        w *= 0.5f;
    }
    return f;
}
