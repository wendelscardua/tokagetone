#include "ggsound.hpp"
#include <peekpoke.h>

#pragma clang section text = ".prg_rom_2.ggsound.text"
#pragma clang section rodata = ".prg_rom_2.ggsound.rodata"

extern "C" u8 sound_param_byte_0;
extern "C" u8 sound_param_byte_1;
extern "C" void *sound_param_word_0;
extern "C" void *sound_param_word_1;
extern "C" void *sound_param_word_2;
extern "C" void *sound_param_word_3;
extern "C" u8 sound_bank;

namespace GGSound {
  inline namespace Wrapper {
    extern "C" void sound_initialize();
    extern "C" void sound_stop();
    extern "C" void play_song();
    extern "C" void play_sfx();
    extern "C" void pause_song();
    extern "C" void resume_song();
  } // namespace Wrapper

  __attribute__((noinline)) void init(Region arg_region,
                                      const Track *arg_song_list[],
                                      const Track *arg_sfx_list[],
                                      const void *arg_instruments[],
#ifdef FEATURE_DPCM
                                      const void *dpcm_pointers[],
#endif
                                      u8 arg_bank) {
    sound_param_byte_0 = (u8)arg_region;
    sound_param_word_0 = (void *)arg_song_list;
    sound_param_word_1 = (void *)arg_sfx_list;
    sound_param_word_2 = (void *)arg_instruments;
#ifdef FEATURE_DPCM
    sound_param_word_3 = (void *)dpcm_pointers;
#endif
    sound_bank = arg_bank;
    sound_initialize();
  }

  void stop() { sound_stop(); }
  void pause() { pause_song(); }
  void resume() { resume_song(); }

  void play_song(Song song) {
    sound_param_byte_0 = (u8)song;
    Wrapper::play_song();
  }

  void play_sfx(SFX sfx, SFXPriority priority) {
    sound_param_byte_0 = (u8)sfx;
    sound_param_byte_1 = (u8)priority;
    Wrapper::play_sfx();
  }
} // namespace GGSound
