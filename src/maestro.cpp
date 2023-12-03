#include "maestro.hpp"

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

#define ADDR_TO_BYTES(addr)                                                    \
  static_cast<u8>(((u16)(addr))), static_cast<u8>(((u16)(addr)) >> 8)

const u8 square1_stream[] = {(u8)SongOpCode::CAL, ADDR_TO_BYTES(square1_frame),
                             (u8)SongOpCode::GOT,
                             ADDR_TO_BYTES(square1_stream)};
const u8 square2_stream[] = {(u8)SongOpCode::CAL, ADDR_TO_BYTES(square2_frame),
                             (u8)SongOpCode::GOT,
                             ADDR_TO_BYTES(square2_stream)};
const u8 triangle_stream[] = {
    (u8)SongOpCode::CAL, ADDR_TO_BYTES(triangle_frame), (u8)SongOpCode::GOT,
    ADDR_TO_BYTES(triangle_stream)};
const u8 noise_stream[] = {(u8)SongOpCode::CAL, ADDR_TO_BYTES(noise_frame),
                           (u8)SongOpCode::GOT, ADDR_TO_BYTES(noise_stream)};
const u8 dpcm_stream[] = {(u8)SongOpCode::CAL, ADDR_TO_BYTES(dpcm_frame),
                          (u8)SongOpCode::GOT, ADDR_TO_BYTES(dpcm_stream)};

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
};