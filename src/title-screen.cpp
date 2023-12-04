#include "title-screen.hpp"
#include "banked-asset-helpers.hpp"
#include "common.hpp"
#include <cstdio>
#include <nesdoug.h>
#include <neslib.h>

__attribute__((noinline)) TitleScreen::TitleScreen() {
  putchar('T');
  putchar('\n');
  pal_bright(0);

  ppu_off();

  load_title_assets();

  oam_clear();

  scroll(0, 0);

  ppu_on_all();

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
      current_game_state = GameState::MusicEditor;
    }
  }
}
