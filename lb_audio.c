#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lb_audio.h"

void lb_au_callback_copy(void *userdata, Uint8 *stream, int len) 
{
  /* Only play if we have data left */
  if ( lb_au_audio_len == 0 )
    return;
  
  /* Mix as much data as possible */
  len = ( len > lb_au_audio_len ? lb_au_audio_len : len );
  SDL_memcpy (stream, lb_au_audio_pos, len); 	
  /* This is worth investigating 
     SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME); */
  lb_au_audio_pos += len;
  lb_au_audio_len -= len;
}


void lb_au_callback_DTMF(void *userdata, Uint8 *stream, int len) 
{
  U_INT_16_T i;
  S_INT_16_T sample_left, sample_right;
  //printf("len=%d\r\n",len);
  //printf("tetha=%f\r\n",tetha);
    
  if (len==0)
    return;

  i=0;
  while(i<len)
    {
      if (AUDIO_BYTES_PER_SAMPLE==1)
	{
	  sample_left=lb_au_sin_from_table_8_bit(_lb_au_ph0)/2+lb_au_sin_from_table_8_bit(_lb_au_ph1)/2;
	  sample_right = rand() % 32;
	}
      else
	{
	  sample_left=lb_au_sin_from_table_16_bit(_lb_au_ph0)/4+lb_au_sin_from_table_8_bit(_lb_au_ph1)/4;
	  sample_right = 0*(rand() % 12);
	}
      if (AUDIO_N_CHANNELS==1)
	{
	  SDL_memcpy(stream+i, &sample_left, AUDIO_BYTES_PER_SAMPLE);
	  i+=AUDIO_BYTES_PER_SAMPLE;
	}
      else
	if (AUDIO_N_CHANNELS==2)
	  {
	    SDL_memcpy(stream+i, &sample_left, AUDIO_BYTES_PER_SAMPLE);
	    i+=AUDIO_BYTES_PER_SAMPLE;
	    SDL_memcpy(stream+i, &sample_right, AUDIO_BYTES_PER_SAMPLE);
	    i+=AUDIO_BYTES_PER_SAMPLE;
	  }
      _lb_au_ph0+=4.0*AUDIO_SAMPLES*_lb_au_f0/AUDIO_RATE;
      _lb_au_ph1+=4.0*AUDIO_SAMPLES*_lb_au_f1/AUDIO_RATE;
      if (_lb_au_ph0>=4.0*AUDIO_SAMPLES)
	_lb_au_ph0-=4.0*AUDIO_SAMPLES;
      if (_lb_au_ph1>=4.0*AUDIO_SAMPLES)
	_lb_au_ph1-=4.0*AUDIO_SAMPLES;
    }
}

void lb_au_freq_DTMF(char digit, FLOAT_T *f1, FLOAT_T *f2)
{
  if ((digit=='1') || (digit=='2') || (digit=='3') || (digit=='a'))
    *f1=697.0;
  if ((digit=='4') || (digit=='5') || (digit=='6') || (digit=='b'))
    *f1=770.0;
  if ((digit=='7') || (digit=='8') || (digit=='9') || (digit=='c'))
    *f1=852.0;
  if ((digit=='*') || (digit=='0') || (digit=='#') || (digit=='d'))
    *f1=941.0;

  if ((digit=='1') || (digit=='4') || (digit=='7') || (digit=='*'))
    *f2=1209.0;
  if ((digit=='2') || (digit=='5') || (digit=='8') || (digit=='0'))
    *f2=1336.0;
  if ((digit=='3') || (digit=='6') || (digit=='9') || (digit=='#'))
    *f2=1477.0;
  if ((digit=='a') || (digit=='b') || (digit=='c') || (digit=='d'))
    *f2=1633.0;

  if (digit=='x')
    {
      *f1=0*700.0; /* left */ 
      *f2=1*1500.0; /* right */
    }
  return;
}  

S_INT_8_T lb_au_SDL_audio_init_DTMF(void)
{
  U_INT_16_T i;

  /* We first check the SDL audio has been initialized */
  if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
      printf("Error: lb_au_init_SDL_audio() --> SDL audio hasn't been initialized\r\n");
      exit(1);
    }

  /* We load the sinewave lookup tables */
  for (i=0;i<AUDIO_SAMPLES;i++)
    {
      _lb_au_sin_table8[i]=(U_INT_8_T)(127.0*sin((FLOAT_T)(i+0.5)*0.5*M_PI/AUDIO_SAMPLES));
      _lb_au_sin_table16[i]=(S_INT_16_T)(0.5+32766.0*sin((FLOAT_T)(i+0.5)*0.5*M_PI/AUDIO_SAMPLES));
      //  printf("sin[%i]=%d\r\n",i,_lb_au_sin_table16[i]);
    }
  
  SDL_memset(&want, 0, sizeof(want)); 
  {
    want.freq = AUDIO_RATE;
    if (AUDIO_BYTES_PER_SAMPLE==1)
      want.format = AUDIO_U8;
    else
      if (AUDIO_BYTES_PER_SAMPLE==2)
	want.format = AUDIO_S16MSB;
      else
	{
	  printf("Error: lb_au_init_SDL_audio() --> invalid setting: AUDIO_BYTES_PER_SAMPLE [%d]\r\n",AUDIO_BYTES_PER_SAMPLE);
	  exit(1);
	}
    want.channels = AUDIO_N_CHANNELS;
    want.samples = AUDIO_BUFFER_SIZE;
    want.callback = lb_au_callback_DTMF;
  }
  
  dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
  if (dev == 0) 
    {
      printf("Error: lb_au_init_SDL_audio() --> Failed to open audio: %s\r\n", SDL_GetError());
      exit(1);
    } 
  else 
    {
      if (have.format != want.format) 
	{ 
	  printf("Error: lb_au_init_SDL_audio() --> We didn't get the desired audio format\r\n");
	  switch (want.format)
	    {
	    case AUDIO_U8: 
	      printf("Unsigned 8-bit samples\r\n");
	      break;
	    case AUDIO_S8:
	      printf("Signed 8-bit samples\r\n");
	      break;
	    case AUDIO_U16LSB: 
	      printf("Unsigned 16-bit little-endian samples\r\n");
	      break;
	    case AUDIO_S16LSB:
	      printf("Signed 16-bit little-endian samples\r\n");
	      break;
	    case AUDIO_U16MSB:
	      printf("Unsigned 16-bit big-endian samples\r\n");
	      break;
	    case AUDIO_S16MSB:
	      printf("Signed 16-bit big-endian samples\r\n");
	      break;
	    }
	  exit(1);
	}
    }
    printf("have.format=%d\r\n",have.format);
	
  _lb_au_ph0=0;
  _lb_au_ph1=0;
  printf("sampling_rate=%d\r\n",have.freq);
  printf("stream_buffer_size=%d\r\n",have.samples);
  return 0; /* No error occurred */
}

void lb_au_SDL_audio_start(void)
{
  SDL_PauseAudioDevice(dev, 0);
}

void lb_au_SDL_audio_stop(void)
{
  SDL_PauseAudioDevice(dev, 1);
}

void lb_au_SDL_audio_close_DTMF(void)
{
  SDL_CloseAudioDevice(dev);
}

void lb_au_wave_load_to_vector_r(const char *filename, VECTOR_R_T *V)
{
  WAVE_HEADER_T h;
  FILE *file;
  U_INT_16_T i, i_max;
  short sample16;

  file=fopen(filename,"rb");
  fread(&h,sizeof(h),1,file);

  i_max=8*h.data_length/(h.bits_per_sample*h.num_channels);
  if ((*V).items<i_max)
    i_max=(*V).items;
  for(i=0;i<i_max;i++)
    {
      fread(&sample16,sizeof(sample16),1,file);
      (*V).array[i]=(FLOAT_T)sample16/32767.0;
      //printf("sample[%d]=%d\r\n",i,sample16);
  
    }
  printf("i_max=%d\r\n",i_max);
  fclose(file);
}

void lb_au_wave_write_from_vector_r(const char *filename, VECTOR_R_T *V, U_INT_16_T bit_rate, U_INT_8_T bits_per_sample)
{
  WAVE_HEADER_T h;
  FILE *file;
  U_INT_16_T i;
  //short sample16;
  
  if (!((bit_rate==11025) || (bit_rate==22050) || (bit_rate==44100)))
    {
      printf("Error: lb_au_wave_write_from_vector_r --> unsupported bit rate =%d\r\n",bit_rate);
      exit(1);
    }

  if (!((bits_per_sample==8) || (bits_per_sample==16)))
    {
      printf("Error: lb_au_wave_write_from_vector_r --> unsupported bits per sample =%d\r\n",bits_per_sample);
      exit(1);
    }

  strncpy(h.riff_tag,"RIFF",4);
  h.riff_length = sizeof(WAVE_HEADER_T)+(*V).items*bits_per_sample/8-8;
  strncpy(h.wave_tag,"WAVE",4);
  strncpy(h.fmt_tag,"fmt ",4);
  h.fmt_length = 16;           /* 16 for PCM.  This is the size of the rest of the Subchunk which follows this number. */
  h.audio_format = 1;          /* 1 for PCM */
  h.num_channels = 1;
  h.sample_rate = bit_rate;
  h.byte_rate = bit_rate*(bits_per_sample/8);
  h.block_align = bits_per_sample/8;
  h.bits_per_sample = bits_per_sample;
  strncpy(h.data_tag,"data",4);
  h.data_length =(*V).items*bits_per_sample/8;

  file = fopen(filename,"w+");
  if(!file) 
    {
      printf("Error: lb_au_wave_write_from_vector_r --> could not open file\r\n");
      exit(1);
    }
  fwrite(&h,sizeof(h),1,file);
  lb_au_wave_parse_header(h);

  unsigned char my_c;
  int my_int;

  if (bits_per_sample==8)
    for(i=0;i<(*V).items;i++)
      {
	my_c=127+128.0*(*V).array[i];
	//my_c=255.0*(*V).array[i];
	fwrite(&my_c,1,1,file);
      }
  else
    for(i=0;i<(*V).items;i++)
      {
	my_int=32767.0*(*V).array[i];
	fwrite(&my_int,2,1,file);
      }


  //int file_length = ftell(file);
  
  //int data_length = file_length - sizeof(struct wavfile_header);
  //fseek(file,sizeof(struct wavfile_header) - sizeof(int),SEEK_SET);
  //fwrite(&data_length,sizeof(data_length),1,file);
  
  //int riff_length = file_length - 8;
  //fseek(file,4,SEEK_SET);
  //fwrite(&riff_length,sizeof(riff_length),1,file);
  
  fclose(file);
}

void lb_au_wave_write_or_append_from_vector_r(const char *filename, VECTOR_R_T *V, U_INT_16_T bit_rate, U_INT_8_T bits_per_sample)
{
  WAVE_HEADER_T h;
  FILE *file;
  U_INT_16_T i;
  //short sample16;
  
  if (!((bit_rate==8000) || (bit_rate==11025) || (bit_rate==22050) || (bit_rate==44100)))
    {
      printf("Error: lb_au_wave_write_or_append_from_vector_r --> unsupported bit rate =%d\r\n",bit_rate);
      exit(1);
    }

  if (!((bits_per_sample==8) || (bits_per_sample==16)))
    {
      printf("Error: lb_au_wave_write_or_append_from_vector_r --> unsupported bits per sample =%d\r\n",bits_per_sample);
      exit(1);
    }

  file = fopen(filename,"a+"); /* Open for reading and appending */
  if(!file) 
    {
      printf("Error: lb_au_wave_write_or_append_from_vector_r --> could not open file\r\n");
      exit(1);
    }

  /* We make sure we just opened a WAV file */
  fseek(file, 0, SEEK_END);
  if (ftell(file)>0)
    {
      fseek(file, 0, SEEK_SET);
      fread(&h,sizeof(h),1,file);
      lb_au_wave_parse_header(h);

      if((h.riff_tag[0]!='R') || (h.riff_tag[1]!='I') || (h.riff_tag[2]!='F') || (h.riff_tag[3]!='F'))
	{
	  printf("Error: lb_au_wave_write_or_append_from_vector_r --> not a WAV file\r\n");
	  exit(1);
	}
      /* We also make sure both the sampling rate as well as the bit parameters are the same */
      if (h.sample_rate != bit_rate)
	{
	  printf("Error: lb_au_wave_write_or_append_from_vector_r --> bit rate of existing file [%d] != %d\r\n",
		 h.sample_rate, bit_rate);
	  exit(1);
	}
      if (h.bits_per_sample != bits_per_sample)
	{
	  printf("Error: lb_au_wave_write_or_append_from_vector_r --> bits per sample of existing file [%d] != %d\r\n",
		 h.bits_per_sample, bits_per_sample);
	  exit(1);
	}
    }
  else  /* New file */
    {      
      strncpy(h.riff_tag,"RIFF",4);
      h.riff_length = 0;                                                     
      strncpy(h.wave_tag,"WAVE",4);
      strncpy(h.fmt_tag,"fmt ",4);
      h.fmt_length = 16;           /* 16 for PCM.  This is the size of the rest of the Subchunk which follows this number. */
      h.audio_format = 1;          /* 1 for PCM */
      h.num_channels = 1;
      h.sample_rate = bit_rate;
      h.byte_rate = bit_rate*(bits_per_sample/8);
      h.block_align = bits_per_sample/8;
      h.bits_per_sample = bits_per_sample;
      strncpy(h.data_tag,"data",4);
      h.data_length = 0;

      fwrite(&h,sizeof(h),1,file);
    }

  /* We append the data */
  unsigned char my_c;
  int my_int;
  
  if (bits_per_sample==8)
    for(i=0;i<(*V).items;i++)
      {
	//	my_c=127+128.0*(*V).array[i];
	my_c=127+127.0*(*V).array[i];
	//my_c=255.0*(*V).array[i];
	fwrite(&my_c,1,1,file);
      }
  else
    for(i=0;i<(*V).items;i++)
      {
	//	my_int=32767.0*(*V).array[i];
	my_int=8000.0*(*V).array[i];
	fwrite(&my_int,2,1,file);
      }

  fseek(file,0,SEEK_END);
  
  int file_length = ftell(file);
  printf("file_length = %d\r\n",file_length);
  
  int data_length = file_length - sizeof(WAVE_HEADER_T);
  fseek(file,sizeof(WAVE_HEADER_T) - sizeof(int),SEEK_SET);
  fwrite(&data_length,sizeof(data_length),1,file);
  printf("data_length = %d\r\n",data_length);
 
  int riff_length = file_length - 8;
  fseek(file,4,SEEK_SET);
  fwrite(&riff_length,sizeof(riff_length),1,file);
  printf("riff_length = %d\r\n",riff_length);

  fclose(file);
}

void lb_au_wave_parse_header(WAVE_HEADER_T h)
{
  printf("-------------------\r\n");
  printf("riff_tag[4]=%s\r\n",     h.riff_tag);
  printf("riff_length=%d\r\n",     h.riff_length);
  printf("wave_tag[4]=%s\r\n",     h.wave_tag);
  printf("fmt_tag[4]=%s\r\n",      h.fmt_tag);
  printf("fmt_length=%d\r\n",      h.fmt_length);
  printf("audio_format=%d\r\n",    h.audio_format);
  printf("num_channels=%d\r\n",    h.num_channels);
  printf("sample_rate=%d\r\n",     h.sample_rate);
  printf("byte_rate=%d\r\n",       h.byte_rate);
  printf("block_align=%d\r\n",     h.block_align);
  printf("bits_per_sample=%d\r\n", h.bits_per_sample);
  printf("data_tag[4]=%s\r\n",     h.data_tag);
  printf("data_length=%d\r\n",     h.data_length);
}

U_INT_8_T lb_au_sin_from_table_8_bit(U_INT_16_T i)
{
  if (i>=4*AUDIO_SAMPLES)
    {
      printf("Error: lb_au_sin_from_table_8_bit(): argument [%d] exceeds 4*AUDIO_SAMPLES [4* %d]\r\n",i,AUDIO_SAMPLES);
      exit(1);
    }
  switch (i/AUDIO_SAMPLES)
    {
    case 0:
      return 127+_lb_au_sin_table8[i % AUDIO_SAMPLES];
      break;
    case 1:
      return 127+_lb_au_sin_table8[AUDIO_SAMPLES - (i % AUDIO_SAMPLES) -1];
      break;
    case 2:
      return 127-_lb_au_sin_table8[i % AUDIO_SAMPLES];
      break;
    case 3:
      return 127-_lb_au_sin_table8[AUDIO_SAMPLES - (i % AUDIO_SAMPLES)-1];
      break;
    }
  return 0;
}

S_INT_16_T lb_au_sin_from_table_16_bit(U_INT_16_T  i)
{
  if (i>=4*AUDIO_SAMPLES)
    {
      printf("Error: lb_au_sin_from_table_16_bit(): argument [%d] exceeds 4*AUDIO_SAMPLES [4* %d]\r\n",i,AUDIO_SAMPLES);
      exit(1);
    }
  switch (i/AUDIO_SAMPLES)
    {
    case 0:
      return _lb_au_sin_table16[i % AUDIO_SAMPLES];
      break;
    case 1:
      return _lb_au_sin_table16[AUDIO_SAMPLES - (i % AUDIO_SAMPLES) -1];
      break;
    case 2:
      return -_lb_au_sin_table16[i % AUDIO_SAMPLES];
      break;
    case 3:
      return -_lb_au_sin_table16[AUDIO_SAMPLES - (i % AUDIO_SAMPLES)-1];
      break;
    default:
      printf("Error: sin_from_table_16_bit(): out of range\r\n");
      exit(1);
      break;
    }
  return 0;
}
