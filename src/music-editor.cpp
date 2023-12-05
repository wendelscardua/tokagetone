#include "music-editor.hpp"
#include "banked-asset-helpers.hpp"
#include "camera.hpp"
#include "common.hpp"
#include "ggsound.hpp"
#include "maestro.hpp"
#include "metasprites.hpp"
#include <nesdoug.h>
#include <neslib.h>

MusicEditor::MusicEditor(Maestro &maestro)
    : maestro(maestro), current_row(0),
      current_channel(GGSound::Channel::Square_1),
      note{SongOpCode::C3, SongOpCode::C3, SongOpCode::C4, (SongOpCode)0x00,
           SongOpCode::C3},
      instrument_index{0, 0, 0, 0, 0} {
  load_music_editor_assets();

  pal_bright(0);

  oam_clear();

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
    load_strip(strip);
    flush_vram_update2();
  }

  ppu_on_all();

  pal_fade_to(0, 4);
}

MusicEditor::~MusicEditor() {
  pal_fade_to(4, 0);
  ppu_off();
}

// TODO: revise this
const Instrument square_instruments[] = {
    Instrument::Plink, Instrument::StarPlink, Instrument::Voice1};
const Instrument triangle_instruments[] = {Instrument::TriangleSnare,
                                           Instrument::BassKick};
const Instrument noise_instruments[] = {
    Instrument::KickHatsCombo, Instrument::SnareHatsCombo, Instrument::HiHats};
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
    0xc8, // C0
    0xb8, // CS0
    0xa8, // D0
    0x98, // DS0
    0x88, // E0
    0x78, // F0
    0x68, // FS0
    0x58, // G0
    0x48, // GS0
    0x38, // A0
    0x28, // AS0
    0x18, // B0
    0xc8, // C1
    0xb8, // CS1
    0xa8, // D1
    0x98, // DS1
    0x88, // E1
    0x78, // F1
    0x68, // FS1
    0x58, // G1
    0x48, // GS1
    0x38, // A1
    0x28, // AS1
    0x18, // B1
    0xc8, // C2
    0xb8, // CS2
    0xa8, // D2
    0x98, // DS2
    0x88, // E2
    0x78, // F2
    0x68, // FS2
    0x58, // G2
    0x48, // GS2
    0x38, // A2
    0x28, // AS2
    0x18, // B2
    0xc8, // C3
    0xb8, // CS3
    0xa8, // D3
    0x98, // DS3
    0x88, // E3
    0x78, // F3
    0x68, // FS3
    0x58, // G3
    0x48, // GS3
    0x38, // A3
    0x28, // AS3
    0x18, // B3
    0xc8, // C4
    0xb8, // CS4
    0xa8, // D4
    0x98, // DS4
    0x88, // E4
    0x78, // F4
    0x68, // FS4
    0x58, // G4
    0x48, // GS4
    0x38, // A4
    0x28, // AS4
    0x18, // B4
    0xc8, // C5
    0xb8, // CS5
    0xa8, // D5
    0x98, // DS5
    0x88, // E5
    0x78, // F5
    0x68, // FS5
    0x58, // G5
    0x48, // GS5
    0x38, // A5
    0x28, // AS5
    0x18, // B5
    0xc8, // C6
    0xb8, // CS6
    0xa8, // D6
    0x98, // DS6
    0x88, // E6
    0x78, // F6
    0x68, // FS6
    0x58, // G6
    0x48, // GS6
    0x38, // A6
    0x28, // AS6
    0x18, // B6
    0xc8, // C7
    0xb8, // CS7
    0xa8, // D7
    0x98, // DS7
    0x88, // E7
    0x78, // F7
    0x68, // FS7
    0x58, // G7
    0x48, // GS7
    0x38, // A7
    0x28, // AS7
    0x18, // B7
};

void MusicEditor::loop() {
  play_note();
  while (current_game_state == GameState::MusicEditor) {
    ppu_wait_nmi();

    pad_poll(0);

    u8 pressed = get_pad_new(0);
    if (pressed & (PAD_START)) {
      maestro.update_streams();
      banked_play_song(Song::Synthetic);
    }
    if (pressed & (PAD_A)) {
      maestro.rows[current_row].channel_entry(current_channel) = {
          note[(u8)current_channel],
          instruments[(u8)current_channel]
                     [instrument_index[(u8)current_channel]],
      };
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

    s16 cursor_x = 0x28 + current_row * 0x10;
    u8 cursor_y = note_height[(u8)note[(u8)current_channel]];

    Camera::update(cursor_x, true);
    load_strip(Camera::min_horizontal_strip());
    load_strip(Camera::max_horizontal_strip());

    render_sprites(cursor_x, cursor_y);
  }
}

void MusicEditor::play_note() {
  maestro.dynamic_sfx(
      current_channel, note[(u8)current_channel],
      instruments[(u8)current_channel][instrument_index[(u8)current_channel]]);
}

void MusicEditor::render_sprites(s16 cursor_x, u8 cursor_y) {
  void *metasprite;
  switch (current_channel) {
  case GGSound::Channel::Square_1:
    metasprite = (void *)metasprite_square1_cursor;
    break;
  case GGSound::Channel::Square_2:
    metasprite = (void *)metasprite_square2_cursor;
    break;
  case GGSound::Channel::Triangle:
    metasprite = (void *)metasprite_triangle_cursor;
    break;
  case GGSound::Channel::Noise:
    metasprite = (void *)metasprite_noise_cursor;
    break;
  case GGSound::Channel::DPCM:
    metasprite = (void *)metasprite_dpcm_cursor;
    break;
  }
  banked_oam_meta_spr_horizontal(cursor_x - Camera::x, cursor_y, metasprite);
  oam_hide_rest();
}

void MusicEditor::load_strip(s16 strip) {
  if (strip >= min_loaded_strip && strip <= max_loaded_strip)
    return;
  if (strip < 0 || strip >= Camera::last_strip())
    return;

  u8 first_column[24], second_column[24];

  switch (Camera::strip_type(strip)) {

  case Camera::StripType::LeftMargin1:
    for (u8 i = 0; i < 24; i += 2) {
      first_column[i] = 0x11;
      first_column[i + 1] = 0x11;
      second_column[i] = 0x11;
      second_column[i + 1] = 0x11;
    }
    break;
  case Camera::StripType::LeftMargin2:
    for (u8 i = 0; i < 24; i += 2) {
      first_column[i] = 0x12;
      first_column[i + 1] = 0x12;
      second_column[i] = 0x12;
      second_column[i + 1] = 0x12;
    }
    break;
  case Camera::StripType::MusicRow:
    for (u8 i = 0; i < 24; i += 2) {
      first_column[i] = 0x75;
      first_column[i + 1] = 0x77;
      second_column[i] = 0x76;
      second_column[i + 1] = 0x78;
    }
    break;
  case Camera::StripType::MusicFourthRow:
    for (u8 i = 0; i < 24; i += 2) {
      first_column[i] = 0x71;
      first_column[i + 1] = 0x73;
      second_column[i] = 0x72;
      second_column[i + 1] = 0x74;
    }
    break;
  case Camera::StripType::RightMargin1:
    for (u8 i = 0; i < 24; i += 2) {
      first_column[i] = 0x13;
      first_column[i + 1] = 0x13;
      second_column[i] = 0x13;
      second_column[i + 1] = 0x13;
    }
    break;
  case Camera::StripType::RightMargin2:
    for (u8 i = 0; i < 24; i += 2) {
      first_column[i] = 0x14;
      first_column[i + 1] = 0x14;
      second_column[i] = 0x14;
      second_column[i + 1] = 0x14;
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
