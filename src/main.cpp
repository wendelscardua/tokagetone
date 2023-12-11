#include "common.hpp"
#include "maestro.hpp"
#include "music-editor.hpp"
#include "title-screen.hpp"
#include <mapper.h>
#include <nesdoug.h>
#include <neslib.h>
#include <peekpoke.h>

GameState current_game_state;

static void main_init() {
  set_prg_8000(0);
  set_wram_mode(WRAM_ON);

  disable_irq();

  ppu_off();

  // set 8x8 sprite mode
  oam_size(0);

  bank_bg(0);
  bank_spr(1);
  set_chr_mode_0(0);
  set_chr_mode_1(2);
  set_chr_mode_2(4);
  set_chr_mode_3(5);
  set_chr_mode_4(6);
  set_chr_mode_5(7);

  set_vram_buffer();

  current_game_state = GameState::TitleScreen;

  asm volatile("cli");
}

int main() {
  main_init();

  ppu_on_all();

  Maestro maestro;

  while (true) {
    switch (current_game_state) {
    case GameState::TitleScreen: {
      TitleScreen title_screen{maestro};
      title_screen.loop();
    } break;
    case GameState::MusicEditor: {
      MusicEditor music_editor{maestro};
      music_editor.loop();
    } break;
    }
  }
}
