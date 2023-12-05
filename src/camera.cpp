#include "camera.hpp"
#include "maestro.hpp"
#include <nesdoug.h>

namespace Camera {
  static constexpr u8 horizontal_margin = 0x60;

  s16 x;

  s16 max_x;

  void init() {
    Camera::max_x = (Maestro::MAX_ROWS + 2) * 16 - 256;

    Camera::x = 0;

    set_scroll_x((u16)x);
  }

  void update(s16 cursor_x, bool easing) {
    s16 box_x_left = x + horizontal_margin;
    s16 box_x_right = x + 256 - horizontal_margin;

    s16 target_x = x;
    if (cursor_x < box_x_left) {
      target_x = cursor_x - horizontal_margin;
      if (target_x < 0)
        target_x = 0;
    }
    if (cursor_x > box_x_right) {
      target_x = cursor_x - 256 + horizontal_margin;
      if (target_x < 0)
        x = 0;
      else if (target_x > max_x)
        target_x = max_x;
    }

    if (easing) {
      x += (target_x - x) >> 4;
    } else {
      x = target_x;
    }

    set_scroll_x((u16)x);
  }

  s16 min_horizontal_strip() { return (x >> 4) - 1; }

  s16 max_horizontal_strip() { return (x >> 4) + 16; }

  s16 min_vertical_strip() { return (y >> 4) - 1; }

  s16 max_vertical_strip() { return (y >> 4) + 15; }
} // namespace Camera
