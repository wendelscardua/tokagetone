#pragma once

#include "maestro.hpp"

class MusicEditor {
  Maestro &maestro;

  u8 current_row;
  GGSound::Channel current_channel;
  SongOpCode note[Maestro::MAX_CHANNELS];
  u8 instrument_index[Maestro::MAX_CHANNELS];

  s16 x_scroll;

  void render_sprites();

public:
  MusicEditor(Maestro &maestro);
  ~MusicEditor();
  void loop();
};
