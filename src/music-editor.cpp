#include "music-editor.hpp"
#include "banked-asset-helpers.hpp"
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
      instrument_index{0, 0, 0, 0, 0}, x_scroll(0) {
  load_music_editor_assets();

  pal_bright(0);

  oam_clear();

  scroll(0, 0);

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

void MusicEditor::loop() {
  play_note();
  while (current_game_state == GameState::MusicEditor) {
    ppu_wait_nmi();

    pad_poll(0);

    u8 pressed = get_pad_new(0);
    if (pressed & (PAD_A | PAD_START)) {
      maestro.dynamic_sfx(GGSound::Channel::DPCM, SongOpCode::C3,
                          Instrument::AEIOU);
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

    render_sprites();
  }
}

void MusicEditor::play_note() {
  maestro.dynamic_sfx(
      current_channel, note[(u8)current_channel],
      instruments[(u8)current_channel][instrument_index[(u8)current_channel]]);
}

void MusicEditor::render_sprites() {
  s16 x_position = 0x28 + current_row * 0x10;
  u8 y_position = note_height[(u8)note[(u8)current_channel]];
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
  banked_oam_meta_spr_horizontal(x_position - x_scroll, y_position, metasprite);
  oam_hide_rest();
}