        .segment "_pdpcm"
        .include "soundtrack_dpcm.asm"

        .segment "_pprg__rom__2"
        .include "ggsound.inc"

        .global song_list
        .global sfx_list
        .global instrument_list
        .global dpcm_list
        
        .include "soundtrack.asm"

        .global synthetic_song_list
        synthetic_song_list:
        .word synthetic_track

        synthetic_track:
        .byte 0, 6
        .byte 0, 5
        .word synth_square1_stream
        .word synth_square2_stream
        .word synth_triangle_stream
        .word synth_noise_stream
        .word synth_dpcm_stream

        .global frame

        max_instructions = 200

        synth_square1_stream:
        .byte CAL
        .word frame
        .byte GOT
        .word synth_square1_stream

        synth_square2_stream:
        .byte CAL
        .word frame + max_instructions
        .byte GOT
        .word synth_square2_stream

        synth_triangle_stream:
        .byte CAL
        .word frame + 2 * max_instructions
        .byte GOT
        .word synth_triangle_stream

        synth_noise_stream:
        .byte CAL
        .word frame + 3 * max_instructions
        .byte GOT
        .word synth_noise_stream

        synth_dpcm_stream:
        .byte CAL
        .word frame + 4 * max_instructions
        .byte GOT
        .word synth_dpcm_stream

        