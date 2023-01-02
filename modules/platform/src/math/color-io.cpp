#include "simplay/platform/math/color-io.h"

void sim::write_rgb_color(FILE* out, const rgb_color& c) {
  i32 r = (i32)(255.999 * c.x());
  i32 g = (i32)(255.999 * c.y());
  i32 b = (i32)(255.999 * c.z());
  fprintf(out, "%d %d %d\n", r, g, b);
}
