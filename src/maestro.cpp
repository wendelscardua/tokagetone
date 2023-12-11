#include "maestro.hpp"
#include "bank-helper.hpp"
#include "banked-asset-helpers.hpp"
#include "common.hpp"
#include "ggsound.hpp"
#include "soundtrack-ptr.hpp"
#include "soundtrack.hpp"
#include <string.h>

extern const u8 synth_square1_stream[];
extern const u8 synth_square2_stream[];
extern const u8 synth_triangle_stream[];
extern const u8 synth_noise_stream[];
extern const u8 synth_dpcm_stream[];

static GGSound::Track synthetic_track;

__attribute__((
    section(".prg_ram.noinit"))) volatile unsigned long save_signature;

__attribute__((section(".prg_ram.noinit"))) volatile u8
    frame[Maestro::MAX_CHANNELS][Maestro::MAX_INSTRUCTIONS];

__attribute__((section(".prg_ram.noinit"))) volatile u8
    sfx_frame[Maestro::MAX_CHANNELS][Maestro::MAX_SFX_INSTRUCTIONS];

__attribute__((section(".prg_ram.noinit")))
Row save_files[Maestro::MAX_SLOTS][Maestro::MAX_ROWS];
__attribute__((section(".prg_ram.noinit"))) u8 save_speeds[Maestro::MAX_SLOTS];

extern const GGSound::Track *synthetic_sfx_list[];

extern const u8 sfx_frame_template[];

const GGSound::Track *synthetic_song_list[] = {
    &synthetic_track,
};

Entry &Row::channel_entry(GGSound::Channel channel) {
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
  if (save_signature != SAVE_SIGNATURE) {
    for (u8 slot = 0; slot < MAX_SLOTS; ++slot) {
      for (auto &row : save_files[slot]) {
        row.square1 = row.square2 = row.triangle = row.noise = row.dpcm =
            Entry{SongOpCode::None, Instrument::Silence};
      }
      save_speeds[slot] = 6;
    }
    save_signature = SAVE_SIGNATURE;
  }
  clear();
  update_streams();
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

void Maestro::clear() {
  speed = 6;
  {
    ScopedBank scopedBank(GET_BANK(synth_square1_stream));
    synthetic_track = {(u16)(speed * 256),
                       (u16)(speed * 256 * 5 / 6),
                       (void *)synth_square1_stream,
                       (void *)synth_square2_stream,
                       (void *)synth_triangle_stream,
                       (void *)synth_noise_stream,
                       (void *)synth_dpcm_stream};
  }
  for (auto &row : rows) {
    row.square1 = row.square2 = row.triangle = row.noise = row.dpcm =
        Entry{SongOpCode::None, Instrument::Silence};
  }
}

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

void Maestro::slower() {
  if (speed < 12) {
    speed++;

    synthetic_track.ntsc_tempo = (u16)(speed * 256);
    synthetic_track.pal_tempo = (u16)(speed * 256 * 5 / 6);
    {
      ScopedBank scopedBank(GET_BANK(instrument_list));
      GGSound::init(GGSound::Region::NTSC, synthetic_song_list,
                    synthetic_sfx_list, instrument_list, dpcm_list,
                    GET_BANK(instrument_list));
    }
  }
}

void Maestro::faster() {
  if (speed > 1) {
    speed--;

    synthetic_track.ntsc_tempo = (u16)(speed * 256);
    synthetic_track.pal_tempo = (u16)(speed * 256 * 5 / 6);
    {
      ScopedBank scopedBank(GET_BANK(instrument_list));
      GGSound::init(GGSound::Region::NTSC, synthetic_song_list,
                    synthetic_sfx_list, instrument_list, dpcm_list,
                    GET_BANK(instrument_list));
    }
  }
}

void Maestro::save(u8 slot) {
  for (u8 i = 0; i < MAX_ROWS; i++) {
    save_files[slot][i] = rows[i];
  }
  save_speeds[slot] = speed;
}

void Maestro::load(u8 slot) {
  for (u8 i = 0; i < MAX_ROWS; i++) {
    rows[i] = save_files[slot][i];
  }
  speed = save_speeds[slot];
}

void Maestro::load_title() {
  ScopedBank scoped_bank(GET_BANK(instrument_list));
  extern const Row lalala_saved[Maestro::MAX_ROWS];
  memcpy(rows, lalala_saved, Maestro::MAX_ROWS * sizeof(Row));
  update_streams();
};