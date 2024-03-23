                                ; config.s
        .global __chr_rom_size
        .global __prg_nvram_size
        .global __four_screen
        .global __mirroring
        .global __battery

                                ; Kilobytes
        __prg_rom_size = 64
        __chr_rom_size = 8
        __prg_nvram_size = 8
        __battery = 1

                                ; Flags
        __four_screen = 0
        __mirroring = 1 ; horizontal mirroring
