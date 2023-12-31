#pragma once

// plays song from the song bank
#include "ggsound.hpp"

void banked_play_song(Song song);

// plays sfx from the sfx bank
void banked_play_sfx(SFX sfx, GGSound::SFXPriority priority);

// draws metasprite from the metasprite bank w/ vertical scroll culling
extern "C" void banked_oam_meta_spr(char x, int y, const void *data);

// draws metasprite from the metasprite bank w/ horizontal scroll culling
extern "C" void banked_oam_meta_spr_horizontal(int x, char y, const void *data);

// [assets bank] loads title assets
void load_title_assets();

// [assets bank] loads music editor assets
void load_music_editor_assets();
