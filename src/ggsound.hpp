#pragma once

#include "common.hpp"
#include "soundtrack.hpp"

#define DEFAULT_TEMPO (256 * 15)
#define STREAM_ACTIVE_SET 0b00000001
#define STREAM_PAUSE_SET 0b00000100
#define STREAM_PAUSE_CLEAR 0b11111011

#define MAX_STREAMS (6)
#define MAX_MUSIC_STREAMS (4)

#define FEATURE_DPCM
#define FEATURE_ARPEGGIOS

namespace GGSound {
  struct Track {
    u16 ntsc_tempo;
    u16 pal_tempo;
    void *square_1_stream;
    void *square_2_stream;
    void *triangle_stream;
    void *noise_stream;
    void *dpcm_stream;
  };

  enum class Region : u8 {
    NTSC = 0,
    PAL = 1,
    Dandy = 2,
  };

  enum class Channel : u8 {
    Square_1,
    Square_2,
    Triangle,
    Noise,
#ifdef FEATURE_DPCM
    DPCM,
#endif
  };

  enum class Stream : u8 {
    Square_1,
    Square_2,
    Triangle,
    Noise,
#ifdef FEATURE_DPCM
    DPCM,
#endif
    SFX1,
    SFX2
  };

  // doubles as a "stream" index
  enum class SFXPriority : u8 {
    One = (u8)Stream::SFX1,
    Two = (u8)Stream::SFX2,
  };

#ifdef FEATURE_DPCM
  enum class DPCM_STATE : u8 {
    NOP = 0,
    UPLOAD = 1,
    UPLOAD_THEN_WAIT = 2,
    WAIT = 3,
  };
#endif
  // Initialize sound engine
  void init(Region region, Track *song_list[], Track *sfx_list[],
            void *instruments[],
#ifdef FEATURE_DPCM
            void *dpcm_pointers[],
#endif
            u8 bank);

  // Kill all active streams and halt sound
  void stop();

  // Plays a song
  void play_song(Song song);

  // Plays a sound effect with a given priority
  void play_sfx(SFX sfx, SFXPriority priority);

  // Pauses a song
  void pause();

  // Resumes a song
  void resume();
} // namespace GGSound
