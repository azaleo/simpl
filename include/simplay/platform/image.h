#pragma once

#include "vec3.h"

namespace sim {
  struct FloatImage {
    Color3* pixels;
    u32 w, h;

    FloatImage() : pixels(nullptr), w(0), h(0) {}

    Color3& get(u32 x, u32 y) { return pixels[y*w + x]; }
    const Color3& get(u32 x, u32 y) const { return pixels[y*w + x]; }

    void init(u32 new_w, u32 new_h);
    void release();

    void save_png(const char* out_path);
  };
}
