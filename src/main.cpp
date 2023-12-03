#include "bank-helper.hpp"
#include "banked-asset-helpers.hpp"
#include "ggsound.hpp"
#include "maestro.hpp"
#include <mapper.h>
#include <nesdoug.h>
#include <neslib.h>
#include <peekpoke.h>

static void main_init() {
  set_prg_8000(0);
  set_wram_mode(WRAM_ON);

  disable_irq();

  ppu_off();

  // set 8x8 sprite mode
  oam_size(0);

  // Use lower half of PPU memory for background tiles
  bank_bg(0);
  // ... and upper half for sprites
  bank_spr(1);

  set_vram_buffer();

  load_title_assets();
}

int main() {
  main_init();

  ppu_on_all();

  Maestro maestro;
  maestro.update_streams();

  while (true) {
    ppu_wait_nmi();
    pad_poll(0);
    auto pad = get_pad_new(0);
    if (pad & PAD_A) {
      maestro.dynamic_sfx(GGSound::Channel::Square_1, SongOpCode::C3,
                          Instrument::StarPlink);
    }
    if (pad & PAD_B) {
      maestro.dynamic_sfx(GGSound::Channel::Square_1, SongOpCode::E4,
                          Instrument::StarPlink);
    }
    if (pad & PAD_UP) {
      maestro.dynamic_sfx(GGSound::Channel::DPCM, SongOpCode::C3,
                          Instrument::AEIOU);
    }
    if (pad & PAD_DOWN) {
      maestro.dynamic_sfx(GGSound::Channel::DPCM, SongOpCode::DS3,
                          Instrument::AEIOU);
    }
  }
}
