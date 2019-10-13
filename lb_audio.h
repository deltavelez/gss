#ifndef LB_AUDIO_H
#define LB_AUDIO_H

#include <SDL2/SDL.h>
#include "lb_types.h"

typedef struct wavfile_header {
	char	riff_tag[4];
	int	riff_length;
	char	wave_tag[4];
	char	fmt_tag[4];
	int	fmt_length;
	short	audio_format;
	short	num_channels;
	int	sample_rate;
	int	byte_rate;
	short	block_align;
	short	bits_per_sample;
	char	data_tag[4];
	int	data_length;
} WAVE_HEADER_T;

#define AUDIO_RATE 22050
#define AUDIO_SAMPLES 2048
#define AUDIO_BUFFER_SIZE 4096
#define AUDIO_BYTES_PER_SAMPLE 1
#define AUDIO_N_CHANNELS 1

Uint32 lb_au_audio_len;
Uint8 *lb_au_audio_pos;
SDL_AudioSpec want, have;
SDL_AudioDeviceID dev;

U_INT8_T _lb_au_sin_table8[AUDIO_SAMPLES];
S_INT16_T _lb_au_sin_table16[AUDIO_SAMPLES];
FLOAT_T _lb_au_ph0, _lb_au_ph1, _lb_au_f0, _lb_au_f1;


void lb_au_callback_copy(void *userdata, Uint8 *stream, int len); 
void lb_au_callback_DTMF(void *userdata, Uint8 *stream, int len); 
void lb_au_freq_DTMF(char digit, FLOAT_T *f1, FLOAT_T *f2);
S_INT8_T lb_au_SDL_audio_init_DTMF(void);
void lb_au_SDL_audio_start(void);
void lb_au_SDL_audio_stop(void);
void lb_au_SDL_audio_close_DTMF(void);
void lb_au_wave_load_to_vector_r(const char *filename, VECTOR_R_T *V);
void lb_au_wave_write_from_vector_r(const char *filename, VECTOR_R_T *V, U_INT16_T bit_rate, U_INT8_T bits_per_sample);
void lb_au_wave_write_or_append_from_vector_r(const char *filename, VECTOR_R_T *V, U_INT16_T bit_rate, U_INT8_T bits_per_sample);
void lb_au_wave_parse_header(WAVE_HEADER_T h);
U_INT8_T lb_au_sin_from_table_8_bit(U_INT16_T i);
S_INT16_T lb_au_sin_from_table_16_bit(U_INT16_T  i);

#endif /* LB_AUDIO_H */
