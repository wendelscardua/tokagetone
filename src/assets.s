    .section .prg_rom_0,"aR",@progbits

    .global bg_tiles, spr_tiles
    bg_tiles: .incbin "BG.chr.donut"
    spr_tiles: .incbin "SPR.chr.donut"

    .global bg_palette, spr_palette
    bg_palette: .incbin "BG.pal"
    spr_palette: .incbin "SPR.pal"

    .global title_nametable
    title_nametable: .incbin "title.nam.zx02"