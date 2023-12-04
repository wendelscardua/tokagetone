
#include "music-editor.hpp"
#include "banked-asset-helpers.hpp"
#include "common.hpp"
#include "ggsound.hpp"
#include "maestro.hpp"
#include "soundtrack.hpp"
#include <nesdoug.h>
#include <neslib.h>

MusicEditor::MusicEditor(Maestro &maestro) : maestro(maestro) {
  load_music_editor_assets();

  pal_bright(0);

  oam_clear();

  scroll(0, 0);

  ppu_on_all();

  pal_fade_to(0, 4);
}

MusicEditor::~MusicEditor() {
  pal_fade_to(4, 0);
  ppu_off();
}

void MusicEditor::loop() {
  while (current_game_state == GameState::MusicEditor) {
    ppu_wait_nmi();

    pad_poll(0);

    u8 pressed = get_pad_new(0);
    if (pressed & (PAD_A | PAD_START)) {
      maestro.update_streams();
      maestro.dynamic_sfx(GGSound::Channel::DPCM, SongOpCode::C3,
                          Instrument::AEIOU);
    }
    if (pressed & (PAD_B)) {
      current_game_state = GameState::TitleScreen;
    }
  }
}

void MusicEditor::render_sprites() {}