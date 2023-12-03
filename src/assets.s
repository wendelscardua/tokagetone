    .section .prg_rom_0,"aR",@progbits

    .global bg_palette, spr_palette
    bg_palette: .incbin "BG.pal"
    spr_palette: .incbin "SPR.pal"

    .global title_nametable
    title_nametable: .incbin "title.nam.zx02"

    .section .chr_rom,"aR",@progbits
    .incbin "BG.chr"
    .incbin "SPR.chr"