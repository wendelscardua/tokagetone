                                ; config.s
        .global __chr_rom_size
        .global __prg_ram_size
        .global __four_screen
        .global __mirroring

                                ; Kilobytes
        __prg_rom_size = 64
        __chr_rom_size = 8
        __prg_ram_size = 8

                                ; Flags
        __four_screen = 0
        __mirroring = 1 ; horizontal mirroring
