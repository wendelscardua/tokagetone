#pragma once

#include "maestro.hpp"

class MusicEditor {
  Maestro &maestro;

public:
  MusicEditor(Maestro &maestro);
  ~MusicEditor();
  void loop();
  void render_sprites();
};
