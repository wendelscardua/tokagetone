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

        .global square1_frame, square2_frame, triangle_frame, noise_frame, dpcm_frame

        synth_square1_stream:
        .byte CAL
        .word square1_frame
        .byte GOT
        .word synth_square1_stream

        synth_square2_stream:
        .byte CAL
        .word square2_frame
        .byte GOT
        .word synth_square2_stream

        synth_triangle_stream:
        .byte CAL
        .word triangle_frame
        .byte GOT
        .word synth_triangle_stream

        synth_noise_stream:
        .byte CAL
        .word noise_frame
        .byte GOT
        .word synth_noise_stream

        synth_dpcm_stream:
        .byte CAL
        .word dpcm_frame
        .byte GOT
        .word synth_dpcm_stream

        