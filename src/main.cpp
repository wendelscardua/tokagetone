#include "bank-helper.hpp"
#include "banked-asset-helpers.hpp"
#include "soundtrack-ptr.hpp"
#include <mapper.h>
#include <nesdoug.h>
#include <neslib.h>

static void main_init() {
  set_prg_bank(0);

  ppu_off();

  // set 8x8 sprite mode
  oam_size(0);

  // Use lower half of PPU memory for background tiles
  bank_bg(0);
  // ... and upper half for sprites
  bank_spr(1);

  set_vram_buffer();

  {
    ScopedBank scopedBank(GET_BANK(song_list));
    GGSound::init(GGSound::Region::NTSC, song_list, sfx_list, instrument_list,
                  dpcm_list, GET_BANK(song_list));
  }
  load_title_assets();
  banked_play_song(Song::Lalala);
}

int main() {
  main_init();

  ppu_on_all();

  banked_play_song(Song::Lalala);

  while (true) {
    ppu_wait_nmi();
    pad_poll(0);
    auto pad = get_pad_new(0);
    if (pad & PAD_A) {
      banked_play_sfx(SFX::A, GGSound::SFXPriority::Two);
    }
    if (pad & PAD_B) {
      banked_play_sfx(SFX::E, GGSound::SFXPriority::Two);
    }
    if (pad & PAD_UP) {
      banked_play_sfx(SFX::I, GGSound::SFXPriority::Two);
    }
    if (pad & PAD_DOWN) {
      banked_play_sfx(SFX::O, GGSound::SFXPriority::Two);
    }
    if (pad & PAD_RIGHT) {
      banked_play_sfx(SFX::U, GGSound::SFXPriority::Two);
    }
  }
}
