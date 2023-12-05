#pragma once

#include "common.hpp"

namespace Camera {
  extern s16 x;
  extern s16 y;

  void init();

  void update(s16 cursor_x, bool easing);

  s16 min_horizontal_strip();
  s16 max_horizontal_strip();
}; // namespace Camera
