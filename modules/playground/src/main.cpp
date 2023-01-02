#include <stdio.h>

#include <simplay/platform/core.h>
#include <simplay/platform/math/color-io.h>
#include <simplay/platform/math/vec3.h>

namespace sim {
  constexpr i32 IMG_W = 256;
  constexpr i32 IMG_H = 256;
}

int main() {
  using namespace sim;
  
  printf("P3\n%d %d\n255\n", IMG_W, IMG_H);
  for (int y = IMG_H - 1; y > 0; --y) {
    fprintf(stderr, "\rScanlines remaining: %d ", y);
    fflush(stderr);
    
    for (int x = 0; x < IMG_W; ++x) {
      rgb_color c((f64)x / (IMG_W - 1), (f64)y / (IMG_H - 1), 0.25);
      write_rgb_color(stdout, c);
    }
  }
  fprintf(stderr, "\rDone\n");
}
