#include "banked-asset-helpers.hpp"
#include "assets.hpp"
#include "bank-helper.hpp"
#include "log.hpp"
#include "zx02.hpp"
#include <cstdio>
#include <mapper.h>
#include <neslib.h>

void banked_play_song(Song song) {
  ScopedBank scoopedBank(GET_BANK(song_list));
  GGSound::play_song(song);
}

void banked_play_sfx(SFX sfx, GGSound::SFXPriority priority) {
  ScopedBank scopedBank(GET_BANK(instrument_list));
  GGSound::play_sfx(sfx, priority);
}

__attribute__((noinline, section(".prg_rom_last"))) void load_title_palette() {
  ScopedBank scopedBank(PALETTES_BANK);
  pal_bg(bg_palette);
  pal_spr(spr_palette);
}

__attribute__((noinline, section(".prg_rom_0"))) void load_title_assets() {
  vram_adr(NAMETABLE_A);
  zx02_decompress_to_vram((void *)title_nametable, NAMETABLE_A);
  load_title_palette();
}

__attribute__((noinline, section(".prg_rom_0"))) void
load_music_editor_assets() {
  vram_adr(NAMETABLE_A);
  zx02_decompress_to_vram((void *)music_editor_nametable, NAMETABLE_A);
  vram_adr(NAMETABLE_B);
  zx02_decompress_to_vram((void *)music_editor_nametable, NAMETABLE_A);
  load_title_palette();
}
