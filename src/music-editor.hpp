#pragma once

#include "maestro.hpp"

class MusicEditor {
  static constexpr SongOpCode MIN_NOTE_PER_CHANNEL[] = {
      SongOpCode::C0, SongOpCode::C0, SongOpCode::C0, (SongOpCode)0x00,
      SongOpCode::C3};
  static constexpr SongOpCode MAX_NOTE_PER_CHANNEL[] = {
      SongOpCode::B7, SongOpCode::B7, SongOpCode::B7, (SongOpCode)0x0f,
      SongOpCode::B3};

  enum class EditorMode : u8 {
    Edit,
    Menu,
    SelectLoadSlot,
    SelectSaveSlot,
  };

  enum class MenuOption : u8 {
    PlayStop,
    Load,
    Save,
    New,
    Slower,
    Faster,
  };

  Maestro &maestro;

  EditorMode mode;
  MenuOption menu_option;

  u8 current_row;
  GGSound::Channel current_channel;
  SongOpCode note[Maestro::MAX_CHANNELS];
  u8 instrument_index[Maestro::MAX_CHANNELS];
  s16 min_loaded_strip, max_loaded_strip;

  bool is_playing;
  u8 playing_row;
  u8 playing_step_counter;

  u8 note_alt_sprite_timer;

  void render_sprites(s16 cursor_x, u8 cursor_y, s16 playing_x);
  void play_note();
  void load_strip(s16 strip, bool force);
  void editor_handler(u8 pressed);
  void menu_handler(u8 pressed);
  void save_handler(u8 pressed);
  void load_handler(u8 pressed);

public:
  MusicEditor(Maestro &maestro);
  ~MusicEditor();
  void loop();
};
