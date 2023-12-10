#include "music-editor.hpp"
#include "bank-helper.hpp"
#include "banked-asset-helpers.hpp"
#include "camera.hpp"
#include "common.hpp"
#include "ggsound.hpp"
#include "maestro.hpp"
#include "metasprites.hpp"
#include <mapper.h>
#include <nesdoug.h>
#include <neslib.h>
#include <string.h>

const u8 irq_buffer[] = {0xcf, 0xfd, 0xf5, 0x00, 0xf0, 0b10001000, 0xff};
const u8 irq_alt_buffer[] = {0xcf, 0xfd, 0xf5, 0x00, 0xf0, 0b10001001, 0xff};

MusicEditor::MusicEditor(Maestro &maestro)
    : maestro(maestro), mode(EditorMode::Edit), current_slot(0), current_row(0),
      current_channel(GGSound::Channel::Square_1),
      note{SongOpCode::C3, SongOpCode::C3, SongOpCode::C4, (SongOpCode)0x00,
           SongOpCode::C3},
      instrument_index{0, 0, 0, 0, 0}, is_playing(false), playing_row(0),
      playing_step_counter(0), note_alt_sprite_timer(0) {
  load_music_editor_assets();

  pal_bright(0);

  oam_clear();

  reset_editor();

  ppu_on_all();

  set_irq_ptr(irq_buffer);

  pal_fade_to(0, 4);
}

MusicEditor::~MusicEditor() {
  pal_fade_to(4, 0);
  ppu_off();
  disable_irq();
}

// TODO: revise this
const Instrument square_instruments[] = {
    Instrument::Powerchord,     Instrument::StarPlink,
    Instrument::Voice3Heavy,    Instrument::Voice4,
    Instrument::StarPlinkEcho,  Instrument::Voice3,
    Instrument::Plink,          Instrument::Voice2,
    Instrument::Voice3Echo,     Instrument::Click50,
    Instrument::Click00,        Instrument::Click15,
    Instrument::Voice3HeavyOct, Instrument::BullPlink,
    Instrument::SussGlue,       Instrument::Voice4Echo,
    Instrument::TriangleSnare,  Instrument::Voice1,
    Instrument::HiHats,         Instrument::Perc3,
    Instrument::Perc2,          Instrument::Voice5,
    Instrument::PlinkEcho,      Instrument::Perc1,
    Instrument::HeavyMajArp2,
};
const Instrument triangle_instruments[] = {
    Instrument::Click00,    Instrument::TriangleSnare, Instrument::Perc1,
    Instrument::BassKick,   Instrument::Plink,         Instrument::HiHats,
    Instrument::Voice1,     Instrument::Click15,       Instrument::Perc2,
    Instrument::Voice4,     Instrument::StarPlinkEcho, Instrument::Voice3,
    Instrument::Voice2,     Instrument::Perc3,         Instrument::PlinkEcho,
    Instrument::Voice3Slide};
const Instrument noise_instruments[] = {
    Instrument::HiHats,  Instrument::Perc2,          Instrument::KickHatsCombo,
    Instrument::Click00, Instrument::SnareHatsCombo, Instrument::Perc3,
    Instrument::Perc1,   Instrument::Click50};
const Instrument dpcm_instruments[] = {Instrument::AEIOU};

const Instrument *instruments[] = {
    (Instrument *)&square_instruments[0], (Instrument *)&square_instruments[0],
    (Instrument *)&triangle_instruments[0], (Instrument *)&noise_instruments[0],
    (Instrument *)&dpcm_instruments[0]};

const u8 max_instruments[] = {
    sizeof(square_instruments), sizeof(square_instruments),
    sizeof(triangle_instruments), sizeof(noise_instruments),
    sizeof(dpcm_instruments)};

const u8 note_height[] = {
    0xc0, // C0
    0xb0, // CS0
    0xa0, // D0
    0x90, // DS0
    0x80, // E0
    0x70, // F0
    0x60, // FS0
    0x50, // G0
    0x40, // GS0
    0x30, // A0
    0x20, // AS0
    0x10, // B0
    0xc0, // C1
    0xb0, // CS1
    0xa0, // D1
    0x90, // DS1
    0x80, // E1
    0x70, // F1
    0x60, // FS1
    0x50, // G1
    0x40, // GS1
    0x30, // A1
    0x20, // AS1
    0x10, // B1
    0xc0, // C2
    0xb0, // CS2
    0xa0, // D2
    0x90, // DS2
    0x80, // E2
    0x70, // F2
    0x60, // FS2
    0x50, // G2
    0x40, // GS2
    0x30, // A2
    0x20, // AS2
    0x10, // B2
    0xc0, // C3
    0xb0, // CS3
    0xa0, // D3
    0x90, // DS3
    0x80, // E3
    0x70, // F3
    0x60, // FS3
    0x50, // G3
    0x40, // GS3
    0x30, // A3
    0x20, // AS3
    0x10, // B3
    0xc0, // C4
    0xb0, // CS4
    0xa0, // D4
    0x90, // DS4
    0x80, // E4
    0x70, // F4
    0x60, // FS4
    0x50, // G4
    0x40, // GS4
    0x30, // A4
    0x20, // AS4
    0x10, // B4
    0xc0, // C5
    0xb0, // CS5
    0xa0, // D5
    0x90, // DS5
    0x80, // E5
    0x70, // F5
    0x60, // FS5
    0x50, // G5
    0x40, // GS5
    0x30, // A5
    0x20, // AS5
    0x10, // B5
    0xc0, // C6
    0xb0, // CS6
    0xa0, // D6
    0x90, // DS6
    0x80, // E6
    0x70, // F6
    0x60, // FS6
    0x50, // G6
    0x40, // GS6
    0x30, // A6
    0x20, // AS6
    0x10, // B6
    0xc0, // C7
    0xb0, // CS7
    0xa0, // D7
    0x90, // DS7
    0x80, // E7
    0x70, // F7
    0x60, // FS7
    0x50, // G7
    0x40, // GS7
    0x30, // A7
    0x20, // AS7
    0x10, // B7
};

const u8 octaveless_note[] = {
    0x00, // C0
    0x01, // CS0
    0x02, // D0
    0x03, // DS0
    0x04, // E0
    0x05, // F0
    0x06, // FS0
    0x07, // G0
    0x08, // GS0
    0x09, // A0
    0x0a, // AS0
    0x0b, // B0
    0x00, // C1
    0x01, // CS1
    0x02, // D1
    0x03, // DS1
    0x04, // E1
    0x05, // F1
    0x06, // FS1
    0x07, // G1
    0x08, // GS1
    0x09, // A1
    0x0a, // AS1
    0x0b, // B1
    0x00, // C2
    0x01, // CS2
    0x02, // D2
    0x03, // DS2
    0x04, // E2
    0x05, // F2
    0x06, // FS2
    0x07, // G2
    0x08, // GS2
    0x09, // A2
    0x0a, // AS2
    0x0b, // B2
    0x00, // C3
    0x01, // CS3
    0x02, // D3
    0x03, // DS3
    0x04, // E3
    0x05, // F3
    0x06, // FS3
    0x07, // G3
    0x08, // GS3
    0x09, // A3
    0x0a, // AS3
    0x0b, // B3
    0x00, // C4
    0x01, // CS4
    0x02, // D4
    0x03, // DS4
    0x04, // E4
    0x05, // F4
    0x06, // FS4
    0x07, // G4
    0x08, // GS4
    0x09, // A4
    0x0a, // AS4
    0x0b, // B4
    0x00, // C5
    0x01, // CS5
    0x02, // D5
    0x03, // DS5
    0x04, // E5
    0x05, // F5
    0x06, // FS5
    0x07, // G5
    0x08, // GS5
    0x09, // A5
    0x0a, // AS5
    0x0b, // B5
    0x00, // C6
    0x01, // CS6
    0x02, // D6
    0x03, // DS6
    0x04, // E6
    0x05, // F6
    0x06, // FS6
    0x07, // G6
    0x08, // GS6
    0x09, // A6
    0x0a, // AS6
    0x0b, // B6
    0x00, // C7
    0x01, // CS7
    0x02, // D7
    0x03, // DS7
    0x04, // E7
    0x05, // F7
    0x06, // FS7
    0x07, // G7
    0x08, // GS7
    0x09, // A7
    0x0a, // AS7
    0x0b, // B7
};

const u8 note_octave[] = {
    0x00, // C0
    0x00, // CS0
    0x00, // D0
    0x00, // DS0
    0x00, // E0
    0x00, // F0
    0x00, // FS0
    0x00, // G0
    0x00, // GS0
    0x00, // A0
    0x00, // AS0
    0x00, // B0
    0x01, // C1
    0x01, // CS1
    0x01, // D1
    0x01, // DS1
    0x01, // E1
    0x01, // F1
    0x01, // FS1
    0x01, // G1
    0x01, // GS1
    0x01, // A1
    0x01, // AS1
    0x01, // B1
    0x02, // C2
    0x02, // CS2
    0x02, // D2
    0x02, // DS2
    0x02, // E2
    0x02, // F2
    0x02, // FS2
    0x02, // G2
    0x02, // GS2
    0x02, // A2
    0x02, // AS2
    0x02, // B2
    0x03, // C3
    0x03, // CS3
    0x03, // D3
    0x03, // DS3
    0x03, // E3
    0x03, // F3
    0x03, // FS3
    0x03, // G3
    0x03, // GS3
    0x03, // A3
    0x03, // AS3
    0x03, // B3
    0x04, // C4
    0x04, // CS4
    0x04, // D4
    0x04, // DS4
    0x04, // E4
    0x04, // F4
    0x04, // FS4
    0x04, // G4
    0x04, // GS4
    0x04, // A4
    0x04, // AS4
    0x04, // B4
    0x05, // C5
    0x05, // CS5
    0x05, // D5
    0x05, // DS5
    0x05, // E5
    0x05, // F5
    0x05, // FS5
    0x05, // G5
    0x05, // GS5
    0x05, // A5
    0x05, // AS5
    0x05, // B5
    0x06, // C6
    0x06, // CS6
    0x06, // D6
    0x06, // DS6
    0x06, // E6
    0x06, // F6
    0x06, // FS6
    0x06, // G6
    0x06, // GS6
    0x06, // A6
    0x06, // AS6
    0x06, // B6
    0x07, // C7
    0x07, // CS7
    0x07, // D7
    0x07, // DS7
    0x07, // E7
    0x07, // F7
    0x07, // FS7
    0x07, // G7
    0x07, // GS7
    0x07, // A7
    0x07, // AS7
    0x07, // B7
};

const void *note_metasprites[] = {
    metasprite_note_c,  metasprite_note_cs, metasprite_note_d,
    metasprite_note_ds, metasprite_note_e,  metasprite_note_f,
    metasprite_note_fs, metasprite_note_g,  metasprite_note_gs,
    metasprite_note_a,  metasprite_note_as, metasprite_note_b,
};

const void *octave_metasprites[] = {
    metasprite_oct0, metasprite_oct1, metasprite_oct2, metasprite_oct3,
    metasprite_oct4, metasprite_oct5, metasprite_oct6, metasprite_oct7,
};

const u8 menu_option_x_coords[] = {0x20, 0x40, 0x50, 0x60, 0x80, 0xd0};
const u8 speed_x_coords[] = {0xa0 + 0 * 3, 0xa0 + 1 * 3,  0xa0 + 2 * 3,
                             0xa0 + 3 * 3, 0xa0 + 4 * 3,  0xa0 + 5 * 3,
                             0xa0 + 6 * 3, 0xa0 + 7 * 3,  0xa0 + 8 * 3,
                             0xa0 + 9 * 3, 0xa0 + 10 * 3, 0xa0 + 11 * 3};

void MusicEditor::reset_editor() {
  scroll(0, 0);

  Camera::init();

  s16 min_strip = Camera::min_horizontal_strip() >= 0
                      ? (u8)Camera::min_horizontal_strip()
                      : 0;
  s16 max_strip = Camera::max_horizontal_strip() <= Camera::last_strip()
                      ? (u8)Camera::max_horizontal_strip()
                      : Camera::last_strip();

  min_loaded_strip = max_strip;
  max_loaded_strip = min_strip;

  for (auto strip = min_strip; strip <= max_strip; strip++) {
    load_strip(strip, true);
    flush_vram_update2();
  }
  playing_row = 0;
  playing_step_counter = 0;
  current_row = 0;
}

void MusicEditor::stop_music() {
  is_playing = false;
  banked_lambda(GET_BANK(instrument_list), []() { GGSound::stop(); });
  multi_vram_buffer_horz((u8[]){0xe0, 0xe1}, 2, NTADR_A(4, 27));
  multi_vram_buffer_horz((u8[]){0xf0, 0xf1}, 2, NTADR_A(4, 28));
}

void MusicEditor::play_music() {
  is_playing = true;
  maestro.update_streams();
  banked_play_song(Song::Synthetic);
  playing_row = 0;
  playing_step_counter = 0;
  multi_vram_buffer_horz((u8[]){0xe2, 0xe3}, 2, NTADR_A(4, 27));
  multi_vram_buffer_horz((u8[]){0xf2, 0xf3}, 2, NTADR_A(4, 28));
}

void MusicEditor::editor_handler(u8 pressed) {

  if (pressed & (PAD_START)) {
    mode = EditorMode::Menu;
    menu_option = MenuOption::PlayStop;
  }
  if (pressed & (PAD_A)) {
    const Entry new_entry = {
        note[(u8)current_channel],
        instruments[(u8)current_channel][instrument_index[(u8)current_channel]],
    };
    Entry &current_entry =
        maestro.rows[current_row].channel_entry(current_channel);
    if (current_entry == new_entry) {
      current_entry = {SongOpCode::None, Instrument::None};
    } else {
      current_entry = new_entry;
    }
    load_strip(Camera::music_margin + current_row, true);
    play_note();
  }
  if (pressed & (PAD_B)) {
    if (current_channel == GGSound::Channel::DPCM) {
      current_channel = GGSound::Channel::Square_1;
    } else {
      current_channel = (GGSound::Channel)(((u8)current_channel) + 1);
    }
    play_note();
  }
  if (pressed & (PAD_UP)) {
    if (note[(u8)current_channel] ==
        MAX_NOTE_PER_CHANNEL[(u8)current_channel]) {
      note[(u8)current_channel] = MIN_NOTE_PER_CHANNEL[(u8)current_channel];
    } else {
      note[(u8)current_channel] =
          (SongOpCode)((u8)note[(u8)current_channel] + 1);
    }
    play_note();
  }
  if (pressed & (PAD_DOWN)) {
    if (note[(u8)current_channel] ==
        MIN_NOTE_PER_CHANNEL[(u8)current_channel]) {
      note[(u8)current_channel] = MAX_NOTE_PER_CHANNEL[(u8)current_channel];
    } else {
      note[(u8)current_channel] =
          (SongOpCode)((u8)note[(u8)current_channel] - 1);
    }
    play_note();
  }
  if (pressed & (PAD_LEFT)) {
    if (current_row > 0) {
      current_row--;
    }
    play_note();
  }
  if (pressed & (PAD_RIGHT)) {
    if (current_row < Maestro::MAX_ROWS - 1) {
      current_row++;
    }
    play_note();
  }

  if (pressed & (PAD_SELECT)) {
    instrument_index[(u8)current_channel]++;
    if (instrument_index[(u8)current_channel] ==
        max_instruments[(u8)current_channel]) {
      instrument_index[(u8)current_channel] = 0;
    }
    play_note();
  }
}

void MusicEditor::menu_handler(u8 pressed) {
  const MenuOption next_option[] = {
      MenuOption::Load,   MenuOption::Save,   MenuOption::New,
      MenuOption::Slower, MenuOption::Faster, MenuOption::PlayStop,

  };

  const MenuOption previous_option[] = {
      MenuOption::Faster, MenuOption::PlayStop, MenuOption::Load,
      MenuOption::Save,   MenuOption::New,      MenuOption::Slower,
  };

  if (pressed & (PAD_START | PAD_B)) {
    mode = EditorMode::Edit;
  } else if (pressed & (PAD_SELECT | PAD_DOWN | PAD_RIGHT)) {
    menu_option = next_option[(u8)menu_option];
  } else if (pressed & (PAD_UP | PAD_LEFT)) {
    menu_option = previous_option[(u8)menu_option];
  } else if (pressed & PAD_A) {
    switch (menu_option) {
    case MenuOption::PlayStop:
      if (is_playing) {
        stop_music();
      } else {
        play_music();
      }
      break;
    case MenuOption::Load:
      mode = MusicEditor::EditorMode::SelectLoadSlot;
      set_irq_ptr(irq_alt_buffer);
      break;
    case MenuOption::Save:
      mode = EditorMode::SelectSaveSlot;
      set_irq_ptr(irq_alt_buffer);
      break;
    case MenuOption::New:
      pal_fade_to(4, 0);
      if (is_playing) {
        stop_music();
      }
      ppu_off();
      maestro.clear();
      maestro.update_streams();
      reset_editor();
      ppu_on_all();
      pal_fade_to(0, 4);
      break;
    case MenuOption::Slower:
      stop_music();
      maestro.slower();
      break;
    case MenuOption::Faster:
      stop_music();
      maestro.faster();
      break;
    }
  }
}

void MusicEditor::slot_handler(u8 pressed) {
  if (pressed & (PAD_B)) {
    mode = EditorMode::Menu;
    set_irq_ptr(irq_buffer);
  } else if (pressed & (PAD_RIGHT | PAD_DOWN | PAD_SELECT)) {
    current_slot = (current_slot + 1) & 0x07;
  } else if (pressed & (PAD_LEFT | PAD_UP)) {
    current_slot = (current_slot + 7) & 0x07;
  } else if (pressed & (PAD_A | PAD_START)) {
    if (mode == EditorMode::SelectSaveSlot) {
      maestro.save(current_slot);
    } else { // SelectLoadSlot
      pal_fade_to(4, 0);
      if (is_playing) {
        stop_music();
      }
      ppu_off();
      maestro.load(current_slot);
      maestro.update_streams();
      reset_editor();
      ppu_on_all();
      pal_fade_to(0, 4);
    }
  }
}

void MusicEditor::loop() {
  play_note();

  u8 old_held = 0;
  u8 held_counter = 0;
  u8 held_meta_counter = 0;

  while (current_game_state == GameState::MusicEditor) {
    ppu_wait_nmi();

    pad_poll(0);

    u8 held = pad_state(0);

    u8 pressed = get_pad_new(0);

    if (held != old_held) {
      old_held = held;
      held_counter = 0;
      held_meta_counter = 0;
    } else {
      held_counter++;
      if (held_counter >= 16) {
        held_counter = held_meta_counter;
        if (held_meta_counter < 16) {
          held_meta_counter++;
        }
        pressed |= held;
      }
    }

    switch (mode) {
    case EditorMode::Edit:
      editor_handler(pressed);
      break;
    case EditorMode::Menu:
      menu_handler(pressed);
      break;
    case EditorMode::SelectLoadSlot:
    case EditorMode::SelectSaveSlot:
      slot_handler(pressed);
      break;
    }

    s16 cursor_x = 0x10 * Camera::music_margin + current_row * 0x10;
    u8 cursor_y = note_height[(u8)note[(u8)current_channel]];
    switch (current_channel) {
    case GGSound::Channel::Square_1:
    case GGSound::Channel::Square_2:
      cursor_x += 0x4;
      cursor_y += 0x4;
      break;
    case GGSound::Channel::Triangle:
      cursor_x += 0xc;
      cursor_y += 0x4;
      break;
    case GGSound::Channel::Noise:
      cursor_x += 0x4;
      cursor_y += 0xc;
      break;
    case GGSound::Channel::DPCM:
      cursor_x += 0xc;
      cursor_y += 0xc;
      break;
    }

    Camera::update(cursor_x, true);

    extern u8 VRAM_INDEX;
    if (VRAM_INDEX < 16) {
      load_strip(Camera::min_horizontal_strip(), false);
      load_strip(Camera::max_horizontal_strip(), false);
    }

    s16 playing_x = 0x10 * Camera::music_margin + playing_row * 0x10;

    render_sprites(cursor_x, cursor_y, playing_x);

    if (note_alt_sprite_timer > 0) {
      note_alt_sprite_timer--;
    }

    if (is_playing) {
      playing_step_counter++;
      if (playing_step_counter >= maestro.speed) {
        playing_step_counter = 0;
        playing_row++;
        if (playing_row == Maestro::MAX_ROWS) {
          playing_row = 0;
        }
      }
    }
  }
}

void MusicEditor::play_note() {
  if (is_playing) {
    return;
  }
  maestro.dynamic_sfx(
      current_channel, note[(u8)current_channel],
      instruments[(u8)current_channel][instrument_index[(u8)current_channel]]);
  note_alt_sprite_timer = 16;
}

void MusicEditor::render_sprites(s16 cursor_x, u8 cursor_y, s16 playing_x) {
  switch (mode) {
  case EditorMode::Edit: {
    void *metasprite;
    switch (current_channel) {
    case GGSound::Channel::Square_1:
      metasprite = note_alt_sprite_timer > 0
                       ? (void *)metasprite_square1_cursor_alt
                       : (void *)metasprite_square1_cursor;
      break;
    case GGSound::Channel::Square_2:
      metasprite = note_alt_sprite_timer > 0
                       ? (void *)metasprite_square2_cursor_alt
                       : (void *)metasprite_square2_cursor;
      break;
    case GGSound::Channel::Triangle:
      metasprite = note_alt_sprite_timer > 0
                       ? (void *)metasprite_triangle_cursor_alt
                       : (void *)metasprite_triangle_cursor;
      break;
    case GGSound::Channel::Noise:
      metasprite = note_alt_sprite_timer > 0
                       ? (void *)metasprite_noise_cursor_alt
                       : (void *)metasprite_noise_cursor;
      break;
    case GGSound::Channel::DPCM:
      metasprite = note_alt_sprite_timer > 0
                       ? (void *)metasprite_dpcm_cursor_alt
                       : (void *)metasprite_dpcm_cursor;
      break;
    }
    banked_oam_meta_spr_horizontal(cursor_x - Camera::x, cursor_y, metasprite);
    banked_oam_meta_spr_horizontal(speed_x_coords[12 - maestro.speed], 0xdf,
                                   metasprite_speed_cursor);
    banked_oam_meta_spr_horizontal(
        cursor_x - Camera::x + 24, cursor_y,
        note_metasprites[octaveless_note[(u8)note[(u8)current_channel]]]);
    banked_oam_meta_spr_horizontal(
        cursor_x - Camera::x + 24, cursor_y + 8,
        octave_metasprites[note_octave[(u8)note[(u8)current_channel]]]);

  } break;
  case EditorMode::Menu:
    banked_oam_meta_spr_horizontal(menu_option_x_coords[(u8)menu_option], 0xd7,
                                   metasprite_menu_cursor);
    banked_oam_meta_spr_horizontal(speed_x_coords[12 - maestro.speed], 0xdf,
                                   metasprite_speed_cursor);

    break;
  case EditorMode::SelectLoadSlot:
  case EditorMode::SelectSaveSlot:
    banked_oam_meta_spr_horizontal(0x10 * current_slot + 0x20, 0xd7,
                                   metasprite_menu_cursor);
    break;
  }
  banked_oam_meta_spr_horizontal(playing_x - Camera::x, 0xc9,
                                 metasprite_track_lizard);
  oam_hide_rest();
}

const unsigned char empty_strip[1 * 24] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const unsigned char left_border_strip[1 * 24] = {
    0x25, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x26};

const unsigned char left_notes_strip[1 * 24] = {
    0x23, 0x33, 0x22, 0x32, 0x20, 0x30, 0x0f, 0x1f, 0x0d, 0x1d, 0x0c, 0x1c,
    0x0a, 0x1a, 0x08, 0x18, 0x07, 0x17, 0x05, 0x15, 0x03, 0x13, 0x01, 0x11};

const unsigned char right_notes_strip[1 * 24] = {
    0x24, 0x34, 0x04, 0x14, 0x21, 0x31, 0x04, 0x14, 0x0e, 0x1e, 0x04, 0x14,
    0x0b, 0x1b, 0x09, 0x19, 0x04, 0x14, 0x06, 0x16, 0x04, 0x14, 0x02, 0x12};

const unsigned char right_border_strip[1 * 24] = {
    0x35, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x36};

void MusicEditor::load_strip(s16 strip, bool force) {
  if (!force && strip >= min_loaded_strip && strip <= max_loaded_strip)
    return;
  if (strip < 0 || strip > Camera::last_strip())
    return;

  u8 first_column[24], second_column[24];
  u8 row_index, tile_index;

  switch (Camera::strip_type(strip)) {

  case Camera::StripType::LeftMargin1:
    memcpy(first_column, empty_strip, 24);
    memcpy(second_column, left_border_strip, 24);
    break;
  case Camera::StripType::LeftMargin2:
  case Camera::StripType::RightMargin1:
    memcpy(first_column, left_notes_strip, 24);
    memcpy(second_column, right_notes_strip, 24);
    break;
  case Camera::StripType::RightMargin2:
    memcpy(first_column, right_border_strip, 24);
    memcpy(second_column, empty_strip, 24);
    break;
  case Camera::StripType::MusicRow:
    for (u8 i = 0; i < 24; i += 2) {
      first_column[i] = 0x75;
      first_column[i + 1] = 0x77;
      second_column[i] = 0x76;
      second_column[i + 1] = 0x78;
    }
    goto place_notes;
    break;
  case Camera::StripType::MusicFourthRow:
    for (u8 i = 0; i < 24; i += 2) {
      first_column[i] = 0x71;
      first_column[i + 1] = 0x73;
      second_column[i] = 0x72;
      second_column[i + 1] = 0x74;
    }
  place_notes:
    row_index = (u8)(strip - Camera::music_margin);
#define HAS_NOTE(channel)                                                      \
  (maestro.rows[row_index].channel.note != SongOpCode::None)
#define TILE_INDEX(channel)                                                    \
  (note_height[(u8)maestro.rows[row_index].channel.note] >> 3) - 2

    if (HAS_NOTE(square1)) {
      tile_index = TILE_INDEX(square1);
      first_column[tile_index] = 0x61;
    }

    if (HAS_NOTE(square2)) {
      tile_index = TILE_INDEX(square2);
      if (first_column[tile_index] == 0x61) {
        first_column[tile_index] = 0x63;
      } else {
        first_column[tile_index] = 0x62;
      }
    }

    if (HAS_NOTE(triangle)) {
      tile_index = TILE_INDEX(triangle);
      second_column[tile_index] = 0x64;
    }

    if (HAS_NOTE(noise)) {
      tile_index = TILE_INDEX(noise);
      first_column[tile_index + 1] = 0x65;
    }

    if (HAS_NOTE(dpcm)) {
      tile_index = TILE_INDEX(dpcm);
      second_column[tile_index + 1] = 0x66;
    }

    break;
  }

  s16 position;
  if ((strip & 0x10) == 0) {
    position = NTADR_A((strip & 0x0f) * 2, 2);
  } else {
    position = NTADR_D((strip & 0x0f) * 2, 2);
  }
  multi_vram_buffer_vert(first_column, 24, position);
  multi_vram_buffer_vert(second_column, 24, position + 1);

  if (strip < min_loaded_strip) {
    min_loaded_strip = strip;
    if (max_loaded_strip > min_loaded_strip + 31) {
      max_loaded_strip = min_loaded_strip + 31;
    }
  }

  if (strip > max_loaded_strip) {
    max_loaded_strip = strip;
    if (min_loaded_strip < max_loaded_strip - 31) {
      min_loaded_strip = max_loaded_strip - 31;
    }
  }
}
