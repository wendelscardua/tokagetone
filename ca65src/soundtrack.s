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

        .global synthetic_sfx_list
        synthetic_sfx_list:
        .word synthetic_sfx_square1_track
        .word synthetic_sfx_square2_track
        .word synthetic_sfx_triangle_track
        .word synthetic_sfx_noise_track
        .word synthetic_sfx_dpcm_track

        synthetic_sfx_square1_track:
        .byte 0, 1
        .byte 0, 1
        .word synth_sfx_square1_stream
        .word 0
        .word 0
        .word 0
        .word 0

        synthetic_sfx_square2_track:
        .byte 0, 1
        .byte 0, 1
        .word 0
        .word synth_sfx_square2_stream
        .word 0
        .word 0
        .word 0

        synthetic_sfx_triangle_track:
        .byte 0, 1
        .byte 0, 1
        .word 0
        .word 0
        .word synth_sfx_triangle_stream
        .word 0
        .word 0

        synthetic_sfx_noise_track:
        .byte 0, 1
        .byte 0, 1
        .word 0
        .word 0
        .word 0
        .word synth_sfx_noise_stream
        .word 0

        synthetic_sfx_dpcm_track:
        .byte 0, 1
        .byte 0, 1
        .word 0
        .word 0
        .word 0
        .word 0
        .word synth_sfx_dpcm_stream

        .global sfx_frame

        max_sfx_instructions = 5

        synth_sfx_square1_stream:
        .byte CAL
        .word sfx_frame
        .byte TRM

        synth_sfx_square2_stream:
        .byte CAL
        .word sfx_frame + max_sfx_instructions
        .byte TRM

        synth_sfx_triangle_stream:
        .byte CAL
        .word sfx_frame + 2 * max_sfx_instructions
        .byte TRM

        synth_sfx_noise_stream:
        .byte CAL
        .word sfx_frame + 3 * max_sfx_instructions
        .byte TRM

        synth_sfx_dpcm_stream:
        .byte CAL
        .word sfx_frame + 4 * max_sfx_instructions
        .byte TRM

        .global sfx_frame_template
        sfx_frame_template:
        .byte SL8,STI,0,C3,RET
