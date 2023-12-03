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
}

void Maestro::update_streams() {
  u8 square1_cursor = 0;
  u8 square2_cursor = 0;
  u8 triangle_cursor = 0;
  u8 noise_cursor = 0;
  u8 dpcm_cursor = 0;

  for (auto row : rows) {
  }

  const u8 demo[] = {(u8)SongOpCode::STI, 22,
                     (u8)SongOpCode::SL1, (u8)SongOpCode::A0,
                     (u8)SongOpCode::STI, 1,
                     (u8)SongOpCode::SL3, (u8)SongOpCode::FS4,
                     (u8)SongOpCode::SL2, (u8)SongOpCode::C3,
                     (u8)SongOpCode::E3,  (u8)SongOpCode::SL4,
                     (u8)SongOpCode::C3,  (u8)SongOpCode::C3,
                     (u8)SongOpCode::C3,  (u8)SongOpCode::DS3,
                     (u8)SongOpCode::F3,  (u8)SongOpCode::C3,
                     (u8)SongOpCode::C3,  (u8)SongOpCode::G3,
                     (u8)SongOpCode::E3,  (u8)SongOpCode::SL2,
                     (u8)SongOpCode::E3,  (u8)SongOpCode::G3,
                     (u8)SongOpCode::SL4, (u8)SongOpCode::G3,
                     (u8)SongOpCode::C3,  (u8)SongOpCode::SL6,
                     (u8)SongOpCode::C3,  (u8)SongOpCode::SL2,
                     (u8)SongOpCode::E3,  (u8)SongOpCode::RET};

  memcpy((void *)&square1_frame[0], (void *)&demo[0], sizeof(demo));
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