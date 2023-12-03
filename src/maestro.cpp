#include "maestro.hpp"
#include "bank-helper.hpp"
#include "banked-asset-helpers.hpp"
#include "ggsound.hpp"
#include "soundtrack-ptr.hpp"
#include <string.h>

__attribute__((section(
    ".prg_ram.noinit"))) volatile u8 square1_frame[Maestro::MAX_INSTRUCTIONS];
__attribute__((section(
    ".prg_ram.noinit"))) volatile u8 square2_frame[Maestro::MAX_INSTRUCTIONS];
__attribute__((section(
    ".prg_ram.noinit"))) volatile u8 triangle_frame[Maestro::MAX_INSTRUCTIONS];
__attribute__((section(
    ".prg_ram.noinit"))) volatile u8 noise_frame[Maestro::MAX_INSTRUCTIONS];
__attribute__((section(
    ".prg_ram.noinit"))) volatile u8 dpcm_frame[Maestro::MAX_INSTRUCTIONS];

extern const GGSound::Track *synthetic_song_list[];

Maestro::Maestro() {
  for (auto &row : rows) {
    row.square1 = row.square2 = row.triangle = row.noise = row.dpcm =
        Entry{SongOpCode::None, Instrument::Silence};
  }

  rows[0].square1 = {
      SongOpCode::C3,
      Instrument::Powerchord,
  };
  rows[4].square1 = {
      SongOpCode::E3,
      Instrument::Powerchord,
  };
  rows[8].square1 = {
      SongOpCode::G3,
      Instrument::Powerchord,
  };
  rows[10].square1 = {
      SongOpCode::C3,
      Instrument::Powerchord,
  };
};

void Maestro::update_streams() {
  u8 cursor = 0;

  Entry current_entry{SongOpCode::A0, Instrument::Silence};
  u16 length = 0;
  Instrument current_instrument = Instrument::None;

  // square1
  for (auto row : rows) {
    Entry entry = row.square1;
    if (entry.note == SongOpCode::None) {
      length++;
    } else {
      if (length > 0) {
        if (current_entry.instrument != current_instrument) {
          current_instrument = current_entry.instrument;
          square1_frame[cursor++] = (u8)SongOpCode::STI;
          square1_frame[cursor++] = (u8)current_instrument;
        }
        if (length < 0x10) {
          square1_frame[cursor++] = (u8)SongOpCode::SL1 + (u8)length - 1;
        } else {
          square1_frame[cursor++] = (u8)SongOpCode::SLL;
          square1_frame[cursor++] = length & 0xff;
          if (length > 0xff) {
            square1_frame[cursor++] = (u8)SongOpCode::SLH;
            square1_frame[cursor++] = length >> 8;
          }
        }
        square1_frame[cursor++] = (u8)current_entry.note;
        length = 1;
      }
      current_entry = entry;
    }
  }
  if (length > 0) {
    if (current_entry.instrument != current_instrument) {
      current_instrument = current_entry.instrument;
      square1_frame[cursor++] = (u8)SongOpCode::STI;
      square1_frame[cursor++] = (u8)current_instrument;
    }
    if (length < 0x10) {
      square1_frame[cursor++] = (u8)SongOpCode::SL1 + (u8)length - 1;
    } else {
      square1_frame[cursor++] = (u8)SongOpCode::SLL;
      square1_frame[cursor++] = length & 0xff;
      if (length > 0xff) {
        square1_frame[cursor++] = (u8)SongOpCode::SLH;
        square1_frame[cursor++] = length >> 8;
      }
    }
    square1_frame[cursor++] = (u8)current_entry.note;
    length = 0;
  }
  square1_frame[cursor++] = (u8)SongOpCode::RET;

  const u8 demo[] = {(u8)SongOpCode::STI, 22,
                     (u8)SongOpCode::SLL, 32,
                     (u8)SongOpCode::A0,  (u8)SongOpCode::RET};

  // memcpy((void *)&square1_frame[0], (void *)&demo[0], sizeof(demo));
  memcpy((void *)&square2_frame[0], (void *)&demo[0], sizeof(demo));
  memcpy((void *)&triangle_frame[0], (void *)&demo[0], sizeof(demo));
  memcpy((void *)&noise_frame[0], (void *)&demo[0], sizeof(demo));
  memcpy((void *)&dpcm_frame[0], (void *)&demo[0], sizeof(demo));

  {
    ScopedBank scopedBank(GET_BANK(instrument_list));
    GGSound::init(GGSound::Region::NTSC, synthetic_song_list, sfx_list,
                  instrument_list, dpcm_list, GET_BANK(instrument_list));
  }
  banked_play_song(Song::Lalala);
};