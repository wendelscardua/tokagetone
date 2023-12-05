#pragma once

#include "maestro.hpp"

class MusicEditor {
  static constexpr SongOpCode MIN_NOTE_PER_CHANNEL[] = {
      SongOpCode::C0, SongOpCode::C0, SongOpCode::C0, (SongOpCode)0x00,
      SongOpCode::C3};
  static constexpr SongOpCode MAX_NOTE_PER_CHANNEL[] = {
      SongOpCode::B7, SongOpCode::B7, SongOpCode::B7, (SongOpCode)0x0f,
      SongOpCode::G3};

  Maestro &maestro;

  u8 current_row;
  GGSound::Channel current_channel;
  SongOpCode note[Maestro::MAX_CHANNELS];
  u8 instrument_index[Maestro::MAX_CHANNELS];
  s16 min_loaded_strip, max_loaded_strip;

  void render_sprites(s16 cursor_x, u8 cursor_y);
  void play_note();
  void load_strip(s16 strip, bool force);

public:
  MusicEditor(Maestro &maestro);
  ~MusicEditor();
  void loop();
};
