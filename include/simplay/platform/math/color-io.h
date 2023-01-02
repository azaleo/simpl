#pragma once

#include <stdio.h>

#include "vec3.h"

namespace sim {
  void write_rgb_color(FILE* out, const rgb_color& c);
}
