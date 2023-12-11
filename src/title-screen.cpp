#include "title-screen.hpp"
#include "bank-helper.hpp"
#include "banked-asset-helpers.hpp"
#include "common.hpp"
#include "ggsound.hpp"
#include "maestro.hpp"
#include "soundtrack.hpp"
#include <nesdoug.h>
#include <neslib.h>

__attribute__((noinline)) TitleScreen::TitleScreen(Maestro &maestro) {
  pal_bright(0);

  ppu_off();

  load_title_assets();

  oam_clear();

  scroll(0, 0);

  ppu_on_all();

  maestro.load_title();

  banked_play_song(Song::Lalala);

  pal_fade_to(0, 4);
}

__attribute__((noinline)) TitleScreen::~TitleScreen() {
  pal_fade_to(4, 0);
  ppu_off();
}

__attribute__((noinline)) void TitleScreen::loop() {
  while (current_game_state == GameState::TitleScreen) {
    ppu_wait_nmi();

    pad_poll(0);

    rand16();

    u8 pressed = get_pad_new(0);
    if (pressed & (PAD_A | PAD_START)) {
      {
        ScopedBank scoped_bank(GET_BANK(instrument_list));
        GGSound::stop();
      }
      current_game_state = GameState::MusicEditor;
    }
  }
}
