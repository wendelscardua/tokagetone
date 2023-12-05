#pragma once

#include "common.hpp"

namespace Camera {
  extern s16 x;
  extern s16 y;

  enum class StripType : u8 {
    LeftMargin1,
    LeftMargin2,
    MusicRow,
    MusicFourthRow,
    RightMargin1,
    RightMargin2,
  };

  void init();

  void update(s16 cursor_x, bool easing);

  s16 min_horizontal_strip();
  s16 max_horizontal_strip();
  s16 last_strip();
  StripType strip_type(s16 strip);
}; // namespace Camera
