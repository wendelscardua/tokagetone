#include "maestro.hpp"
#include "bank-helper.hpp"
#include "banked-asset-helpers.hpp"
#include "common.hpp"
#include "ggsound.hpp"
#include "soundtrack-ptr.hpp"
#include "soundtrack.hpp"
#include <string.h>

__attribute__((section(".prg_ram.noinit"))) volatile u8
    frame[Maestro::MAX_CHANNELS][Maestro::MAX_INSTRUCTIONS];

__attribute__((section(".prg_ram.noinit"))) volatile u8
    sfx_frame[Maestro::MAX_CHANNELS][Maestro::MAX_SFX_INSTRUCTIONS];

extern const GGSound::Track *synthetic_song_list[];
extern const GGSound::Track *synthetic_sfx_list[];

extern const u8 sfx_frame_template[];

Entry Row::channel_entry(GGSound::Channel channel) {
  switch (channel) {
  case GGSound::Channel::Square_1:
    return square1;
    break;
  case GGSound::Channel::Square_2:
    return square2;
    break;
  case GGSound::Channel::Triangle:
    return triangle;
    break;
  case GGSound::Channel::Noise:
    return noise;
    break;
  case GGSound::Channel::DPCM:
    return dpcm;
    break;
  }
}

Maestro::Maestro() {
  for (auto &row : rows) {
    row.square1 = row.square2 = row.triangle = row.noise = row.dpcm =
        Entry{SongOpCode::None, Instrument::Silence};
  }
  {
    ScopedBank scopedBank(GET_BANK(instrument_list));
    for (u8 i = 0; i < MAX_CHANNELS; i++) {
      memcpy((void *)&sfx_frame[i][0], (void *)&sfx_frame_template[0],
             MAX_SFX_INSTRUCTIONS);
    }
    GGSound::init(GGSound::Region::NTSC, synthetic_song_list,
                  synthetic_sfx_list, instrument_list, dpcm_list,
                  GET_BANK(instrument_list));
  }
};

void Maestro::update_streams() {
  for (GGSound::Channel channel : (GGSound::Channel[]){
           GGSound::Channel::Square_1, GGSound::Channel::Square_2,
           GGSound::Channel::Triangle, GGSound::Channel::Noise,
           GGSound::Channel::DPCM}) {
    u8 cursor = 0;

    Entry current_entry{SongOpCode::A0, Instrument::Silence};
    u16 length = 0;
    Instrument current_instrument = Instrument::None;
    u16 current_length = 0;

    for (auto row : rows) {
      Entry entry = row.channel_entry(channel);
      if (entry.note == SongOpCode::None) {
        length++;
      } else {
        if (length > 0) {
          if (current_entry.instrument != current_instrument) {
            current_instrument = current_entry.instrument;
            frame[(u8)channel][cursor++] = (u8)SongOpCode::STI;
            frame[(u8)channel][cursor++] = (u8)current_instrument;
          }
          if (current_length != length) {
            current_length = length;
            if (length < 0x10) {
              frame[(u8)channel][cursor++] =
                  (u8)SongOpCode::SL1 + (u8)length - 1;
            } else {
              frame[(u8)channel][cursor++] = (u8)SongOpCode::SLL;
              frame[(u8)channel][cursor++] = length & 0xff;
              if (length > 0xff) {
                frame[(u8)channel][cursor++] = (u8)SongOpCode::SLH;
                frame[(u8)channel][cursor++] = length >> 8;
              }
            }
          }
          frame[(u8)channel][cursor++] = (u8)current_entry.note;
        }
        length = 1;
        current_entry = entry;
      }
    }
    if (length > 0) {
      if (current_entry.instrument != current_instrument) {
        current_instrument = current_entry.instrument;
        frame[(u8)channel][cursor++] = (u8)SongOpCode::STI;
        frame[(u8)channel][cursor++] = (u8)current_instrument;
      }
      if (current_length != length) {
        current_length = length;
        if (length < 0x10) {
          frame[(u8)channel][cursor++] = (u8)SongOpCode::SL1 + (u8)length - 1;
        } else {
          frame[(u8)channel][cursor++] = (u8)SongOpCode::SLL;
          frame[(u8)channel][cursor++] = length & 0xff;
          if (length > 0xff) {
            frame[(u8)channel][cursor++] = (u8)SongOpCode::SLH;
            frame[(u8)channel][cursor++] = length >> 8;
          }
        }
      }
      frame[(u8)channel][cursor++] = (u8)current_entry.note;
      length = 1;
    }
    frame[(u8)channel][cursor++] = (u8)SongOpCode::RET;
  }
};

void Maestro::dynamic_sfx(GGSound::Channel channel, SongOpCode note,
                          Instrument instrument) {
  sfx_frame[(u8)channel][3] = (u8)note;
  sfx_frame[(u8)channel][2] = (u8)instrument;
  banked_play_sfx((SFX)channel, GGSound::SFXPriority::Two);
}