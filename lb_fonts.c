#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h> /* for ussing mmap */
#include <fcntl.h>    /* for using O_RDONLY */ 
#include <unistd.h>   /* for using close() */
#include <stdarg.h>
#include "lb_types.h"
#include "lb_graphics.h"
#include "lb_fonts.h"
#include "lb_integer.h"

SINT8_T lb_ft_fsearch_str(FILE *file, const char *text)
{
  char c;
  int k, length;
  k=0;

  length=strlen(text);
  while (((c = fgetc(file)) != EOF) && (k!=length-1))
    {
      //printf("%c",c);
      fflush(stdout);
      if (text[k]==c)
	k++;
    }
  //printf("\n\rlast character=%c\r\n",c);
  if(k==length-1)
    return 1;
  return 0;
}

SINT8_T lb_ft_fget_next_number(FILE *file, SINT32_T *n)
{
  SINT8_T flag_start, flag_end, flag_error;
  SINT16_T k;

  flag_start=FALSE;
  flag_end=FALSE;
  flag_error=FALSE;
  k=0;
  char c, str[20];
    
  str[0]='\0';
  while ((!flag_end) && (!flag_error))
    {
      c=fgetc(file);
      if (c==EOF)
	flag_end=TRUE;
      else
	if (isdigit(c))
	  {
	    flag_start=TRUE;
	    str[k]=c;
	    str[k+1]='\0';
	    k++;
	    if(k>=20)
	      flag_error=TRUE;
	  }
	else
	  if (flag_start)
	    flag_end=TRUE;
    }
  if (flag_end)
    {
      *n=atoi(str);
      return 1;
    }
  *n=0;
  return 0;
}

void lb_ft_load_GLCDfont(const char *filename, FONT_T *font)
{
  FILE *file;
  SINT32_T n_bytes, i, k_file, k_buff, number;
  UINT8_T  bit_counter, my_byte;
  
  file=fopen(filename,"r");
  if (!file)
    {
      printf("Error: lb_ft_load_font() --> could not load file");
      exit(1);
    }

  lb_ft_fsearch_str(file, "WIDTH=\"");
  lb_ft_fget_next_number(file, (SINT32_T *)&(*font).cols);
  //printf("width=%d\r\n",(*font).cols);
  lb_ft_fsearch_str(file, "HEIGHT=\"");
  lb_ft_fget_next_number(file, (SINT32_T *)&(*font).rows);
  //printf("height=%d\r\n",(*font).rows);
  lb_ft_fsearch_str(file, "FROM=\"");
  lb_ft_fget_next_number(file, (SINT32_T *)&(*font).range_a);
  //printf("range_a=%d\r\n",(*font).range_a);
  lb_ft_fget_next_number(file, (SINT32_T *)&(*font).range_b);
  //printf("range_b=%d\r\n",(*font).range_b);
  

  
  /* rounding up:  roundup(a/b) = (a+(b-1))/b; */
  n_bytes=lb_in_round_div_up((*font).cols*(*font).rows*((*font).range_b-(*font).range_a),8*sizeof(unsigned char));
  //  n_bytes=((*font).cols*(*font).rows*((*font).range_b-(*font).range_a)+(8*sizeof(unsigned char)-1))/(8*sizeof(unsigned char));

  
  (*font).data=(unsigned char *)malloc(n_bytes);
   	     
  k_buff=0;
  bit_counter=0;
  my_byte=0;
  for(k_file=0;k_file<((*font).range_b-(*font).range_a);k_file++)
    {
      lb_ft_fsearch_str(file, "PIXELS=\"");
      for(i=0;i<(*font).cols*(*font).rows;i++)
	{
	  lb_ft_fget_next_number(file, &number);
	  
	  if(number==0)
	    my_byte|=(0x01<<bit_counter);
	  bit_counter++;
	  if(bit_counter==8)
	    {
	      bit_counter=0;
	      (*font).data[k_buff]=my_byte;
	      my_byte=0x00;
	      k_buff++;
	    }
	}
    }

  /* Load default font rendering attributes */
  (*font).scale_x=1;
  (*font).scale_y=1;
  (*font).gap_x=1;
  (*font).gap_y=1;
  (*font).max_x=40;
  (*font).angle=0;
  (*font).flag_fg=TRUE;
  (*font).flag_bg=FALSE;
 
  fclose(file);
}

void lb_ft_release_GLCDfont(FONT_T *font)
{
  free((*font).data);
}

void lb_ft_draw_char(PICTURE_T *Pic, FONT_T *font, SINT16_T xc, SINT16_T yc, char c, COPYMODE_T copymode)
{
  UINT16_T index, offset, i, j, k_scale_x, k_scale_y, dx, dy;

  if ((c=='\0') && (*font).flag_bg)
    {
      for (j=0;j<(*font).cols+(*font).gap_x;j++) 
	for (i=0;i<(*font).rows+(*font).gap_y;i++)
	  for(k_scale_x=0;k_scale_x<(*font).scale_x;k_scale_x++)
	    for(k_scale_y=0;k_scale_y<(*font).scale_y;k_scale_y++)
	      {
		dx = j*(*font).scale_x+k_scale_x;
		dy = i*(*font).scale_y+k_scale_y;
		switch((*font).angle)
		  {
		  case 0:
		    lb_gr_draw_pixel(Pic, xc + dx, yc + dy, (*font).color_bg, copymode);
		    break;
		  case 90:
		    lb_gr_draw_pixel(Pic, xc + dy, yc - dx, (*font).color_bg, copymode);
		    break;
		  case 180:
		    lb_gr_draw_pixel(Pic, xc - dx, yc - dy, (*font).color_bg, copymode);
		    break;
		  case 270:
		    lb_gr_draw_pixel(Pic, xc - dy, yc + dx, (*font).color_bg, copymode);
		    break;
		  default:
		    // printf("Error: lb_ft_draw_text() --> invalid angle = %d\r\n",(*font).angle);
		    exit(1);
		    break;
		  }
	      }
      return;
    }

  if ( (c<(*font).range_a) || (c>(*font).range_b) )
    return;
      
  index=(c-(*font).range_a)*(*font).cols*(*font).rows/8;
  offset=(c-(*font).range_a)*(*font).cols*(*font).rows - 8*index;

  
  for (j=0;j<(*font).cols;j++) 
    for (i=0;i<(*font).rows;i++)
      {
	if( ((*font).data[index]>>offset) & 0x01 )
	  {
	    if ((*font).flag_fg)
	      for(k_scale_x=0;k_scale_x<(*font).scale_x;k_scale_x++)
		for(k_scale_y=0;k_scale_y<(*font).scale_y;k_scale_y++)
		  {
		    dx = j*(*font).scale_x+k_scale_x;
		    dy = i*(*font).scale_y+k_scale_y;
		
		    switch((*font).angle)
		      {
		      case 0:
			lb_gr_draw_pixel(Pic, xc + dx, yc + dy, (*font).color_fg, copymode);
			break;
		      case 90:
			lb_gr_draw_pixel(Pic, xc + dy, yc - dx, (*font).color_fg, copymode);
			break;
		      case 180:
			lb_gr_draw_pixel(Pic, xc - dx, yc - dy, (*font).color_fg, copymode);
			break;
		      case 270:
			lb_gr_draw_pixel(Pic, xc - dy, yc + dx, (*font).color_fg, copymode);
			break;
		      default:
			//printf("Error: lb_ft_draw_char() --> invalid angle = %d\r\n",(*font).angle);
			exit(1);
			break;
		      }
		  }
	  }
	else
	  {
	    if ((*font).flag_bg)
	      for(k_scale_x=0;k_scale_x<(*font).scale_x;k_scale_x++)
		for(k_scale_y=0;k_scale_y<(*font).scale_y;k_scale_y++)
		  {
		    dx = j*(*font).scale_x+k_scale_x;
		    dy = i*(*font).scale_y+k_scale_y;
		
		    switch((*font).angle)
		      {
		      case 0:
			lb_gr_draw_pixel(Pic, xc + dx, yc + dy, (*font).color_bg, copymode);
			break;
		      case 90:
			lb_gr_draw_pixel(Pic, xc + dy, yc - dx, (*font).color_bg, copymode);
			break;
		      case 180:
			lb_gr_draw_pixel(Pic, xc - dx, yc - dy, (*font).color_bg, copymode);
			break;
		      case 270:
			lb_gr_draw_pixel(Pic, xc - dy, yc + dx, (*font).color_bg, copymode);
			break;
		      default:
			//printf("Error: lb_ft_draw_char() --> invalid angle = %d\r\n",(*font).angle);
			exit(1);
			break;
		      }
		  }
	  }
	    
	offset++;
	if (offset==8)
	  {
	    index++;
	    offset=0;
	  }
      }

  /* Fill in the gaps */
  if ((*font).flag_bg)
    {
      for (j=(*font).cols;j<(*font).cols+(*font).gap_x;j++)
	for (i=0;i<(*font).rows+(*font).gap_y;i++) 
	  for(k_scale_x=0;k_scale_x<(*font).scale_x;k_scale_x++)
	    for(k_scale_y=0;k_scale_y<(*font).scale_y;k_scale_y++)
	      {
		dx = j*(*font).scale_x+k_scale_x;
		dy = i*(*font).scale_y+k_scale_y;
		
		switch((*font).angle)
		  {
		  case 0:
		    lb_gr_draw_pixel(Pic, xc + dx, yc + dy, (*font).color_bg, copymode);
		    break;
		  case 90:
		    lb_gr_draw_pixel(Pic, xc + dy, yc - dx, (*font).color_bg, copymode);
		    break;
		  case 180:
		    lb_gr_draw_pixel(Pic, xc - dx, yc - dy, (*font).color_bg, copymode);
		    break;
		  case 270:
		    lb_gr_draw_pixel(Pic, xc - dy, yc + dx, (*font).color_bg, copymode);
		    break;
		  default:
		    //printf("Error: lb_ft_draw_char() --> invalid angle = %d\r\n",(*font).angle);
		    exit(1);
		    break;
		  }
	      }
      
      for (j=0;j<(*font).cols;j++)
	for (i=(*font).rows;i<(*font).rows+(*font).gap_y;i++) 
	  for(k_scale_x=0;k_scale_x<(*font).scale_x;k_scale_x++)
	    for(k_scale_y=0;k_scale_y<(*font).scale_y;k_scale_y++)
	      {
		dx = j*(*font).scale_x+k_scale_x;
		dy = i*(*font).scale_y+k_scale_y;
		
		switch((*font).angle)
		  {
		  case 0:
		    lb_gr_draw_pixel(Pic, xc + dx, yc + dy, (*font).color_bg, copymode);
		    break;
		  case 90:
		    lb_gr_draw_pixel(Pic, xc + dy, yc - dx, (*font).color_bg, copymode);
		    break;
		  case 180:
		    lb_gr_draw_pixel(Pic, xc - dx, yc - dy, (*font).color_bg, copymode);
		    break;
		  case 270:
		    lb_gr_draw_pixel(Pic, xc - dy, yc + dx, (*font).color_bg, copymode);
		    break;
		  default:
		    //printf("Error: lb_ft_draw_char() --> invalid angle = %d\r\n",(*font).angle);
		    exit(1);
		    break;
		  }
	      }
    }
}


void lb_ft_console_gotox(CONSOLE_T *C, SINT16_T x)
{
  if ((x>0) && (x<(*C).MAX_ROWS))
    (*C).w_col=x;
}


void lb_ft_console_gotoy(CONSOLE_T *C, SINT16_T y)
{
  // if ((y>0) && (y<(*C).MAX_ROWS_V))
  //
  //    (*C).n_back=0;
   
}

SINT16_T lb_ft_console_get_n(CONSOLE_T *C)
{
  if ((*C).b_row >= (*C).a_row)
    return (*C).b_row - (*C).a_row;
  else
    return (*C).MAX_ROWS + (*C).b_row - (*C).a_row;
}


void lb_ft_console_gotoxy(CONSOLE_T *C, SINT16_T x, SINT16_T y);

void lb_ft_printc(CONSOLE_T *C, char c)
{
  SINT16_T j, n;
  switch (c)
    {
    case PCKEY_UP:
      (*C).n_back++;

      n=lb_in_min((*C).MAX_ROWS-1, lb_ft_console_get_n(C));
      (*C).n_back=lb_in_min(n, (*C).n_back);
      
      if ((*C).n_back-(*C).n_back_adj>(*C).MAX_ROWS_V-1)
	(*C).n_back_adj++;

      if ((*C).n_back_adj>(*C).MAX_ROWS-1)
	(*C).n_back_adj=(*C).MAX_ROWS-1;

      break;
    case PCKEY_DOWN:
      printf("Max_rows=%d, Max_rows_v=%d n_back=%d, n_back_adj=%d\r\n",(*C).MAX_ROWS,(*C).MAX_ROWS_V,(*C).n_back,(*C).n_back_adj);

      if ((*C).n_back_adj==(*C).n_back)
	(*C).n_back_adj--;
      
      if ((*C).n_back_adj<0)
	(*C).n_back_adj=0;

      (*C).n_back--;

      if ((*C).n_back<0)
	(*C).n_back=0;
      break;
    case PCKEY_LEFT:
      (*C).s_col--;
      if ((*C).s_col<0)
	{
	  if (lb_ft_console_get_n(C)>(*C).n_back)
	    {
	      (*C).s_col=(*C).MAX_COLS-1;
	      (*C).n_back++;

	      if ((*C).n_back>(*C).MAX_ROWS-1)
		(*C).n_back=(*C).MAX_ROWS-1;

	      if ((*C).n_back-(*C).n_back_adj>(*C).MAX_ROWS_V-1)
		(*C).n_back_adj++;
	    }
	  else
	    (*C).s_col=0;
	}
      break;
    case PCKEY_RIGHT:
      if ((*C).n_back==0)
	{
	  if ((*C).s_col<(*C).w_col)
	    (*C).s_col++;
	}
      else
	{
	  (*C).s_col++;
	  if ((*C).s_col>=(*C).MAX_COLS)
	    {
	      (*C).s_col=0;

	      if ((*C).n_back_adj==(*C).n_back)
		(*C).n_back_adj--;
      
	      if ((*C).n_back_adj<0)
		(*C).n_back_adj=0;

	      (*C).n_back--;

	      if ((*C).n_back<0)
		(*C).n_back=0;
	    }
	}
      break;
    case PCKEY_INSERT:
      break;
    case PCKEY_BS:
      (*C).w_col--;
      if ((*C).w_col<0)
	{
	  if (lb_ft_console_get_n(C)>=1)
	    {
	      (*C).w_col=(*C).MAX_COLS-1;
	      (*C).b_row--;
	    }
	  else
	    (*C).w_col=0;
	}
     
      (*C).screen[(*C).b_row][(*C).w_col].c='\0';
      (*C).screen[(*C).b_row][(*C).w_col].color_fg=(*C).color_fg;
      (*C).screen[(*C).b_row][(*C).w_col].color_bg=(*C).color_bg;
      (*C).n_back=0;
      (*C).s_col=(*C).w_col;
      (*C).s_row=(*C).b_row;

      break;
    case PCKEY_ENTER:
      (*C).w_col=0;
      if ( (*C).b_row < (*C).MAX_ROWS-1)
	(*C).b_row++;
      else
	(*C).b_row=0;
      
      if ((*C).b_row==(*C).a_row)
	{
	  (*C).a_row++;
	  if ((*C).a_row >= (*C).MAX_ROWS)
	    (*C).a_row=0;
	}
      for (j=0;j<(*C).MAX_COLS;j++)
	(*C).screen[(*C).b_row][j].c='\0';

      (*C).n_back=0;
      (*C).n_back_adj=(*C).n_back;
      (*C).s_col=(*C).w_col;
      (*C).s_row=(*C).b_row;
      break;
    case '\r':
      (*C).w_col=0;
      (*C).s_col=(*C).w_col;
      break;
    default:
      (*C).screen[(*C).b_row][(*C).w_col].c=c;
      (*C).screen[(*C).b_row][(*C).w_col].color_fg=(*C).color_fg;
      (*C).screen[(*C).b_row][(*C).w_col].color_bg=(*C).color_bg;
      (*C).w_col++;
      if ((*C).w_col>=(*C).MAX_COLS)
	{
	  (*C).w_col=0;
	  if ( (*C).b_row < (*C).MAX_ROWS-1)
	    (*C).b_row++;
	  else
	    (*C).b_row=0;

	  if ((*C).b_row==(*C).a_row)
	    {
	      (*C).a_row++;
	      if ((*C).a_row >= (*C).MAX_ROWS)
		(*C).a_row=0;
	    }
	}
      (*C).n_back=0;
      (*C).n_back_adj=(*C).n_back;
      (*C).s_col=(*C).w_col;
      break;
    }
}


#ifdef NADA
void lb_ft_printc(CONSOLE_T *C, char c)
{
  SINT16_T j;
  switch (c)
    {
    case PCKEY_UP:
      if ((*C).n_row>(*C).n_back)
	{
	  (*C).n_back++;
	  if ((*C).n_back>(*C).MAX_ROWS-1)
	    (*C).n_back=(*C).MAX_ROWS-1;
	}
      if ((*C).n_back>(*C).MAX_ROWS_V-1)
	(*C).n_back_adj=(*C).n_back-(*C).MAX_ROWS_V+1;
      break;
    case PCKEY_DOWN:
      (*C).n_back--;
      if ((*C).n_back<0)
	(*C).n_back=0;
      if ((*C).n_row-(*C).n_back>(*C).MAX_ROWS_V-1)
	(*C).n_back_adj=(*C).n_back;
      break;
    case PCKEY_LEFT:
      (*C).s_col--;
      if ((*C).s_col<0)
	{
	  if ((*C).n_row>(*C).n_back)
	    {
	      (*C).s_col=(*C).MAX_COLS-1;
	      (*C).n_back++;
	      if ((*C).n_back>(*C).MAX_ROWS-1)
		(*C).n_back=(*C).MAX_ROWS-1;
	      if ((*C).n_back>(*C).MAX_ROWS_V-1)
		(*C).n_back_adj=(*C).n_back-(*C).MAX_ROWS_V+1;
	    }
	  else
	    (*C).s_col=0;
	}
      break;
    case PCKEY_RIGHT:
      if ((*C).n_back==0)
	{
	  if ((*C).s_col<(*C).w_col)
	    (*C).s_col++;
	}
      else
	{
	  (*C).s_col++;
	  if ((*C).s_col>=(*C).MAX_COLS)
	    {
	      (*C).s_col=0;
	      (*C).n_back--;
	      if ((*C).n_back<0)
		(*C).n_back=0;
	      if ((*C).n_row-(*C).n_back>(*C).MAX_ROWS_V-1)
		(*C).n_back_adj=(*C).n_back;
	    }
	}
      break;
    case PCKEY_INSERT:
      break;
    case PCKEY_BS:
      (*C).w_col--;
      (*C).screen[(*C).w_row][(*C).w_col].c='\0';
      if ((*C).w_col<0)
	{
	  if ((*C).n_row>0)
	    {
	      (*C).w_col=(*C).MAX_COLS-1;
	      (*C).n_row--;
	      (*C).w_row--;
	      if ((*C).w_row<0)
		(*C).w_row=(*C).MAX_ROWS-1;
	    }
	  else
	    (*C).w_col=0;
	}
      (*C).n_back=0;
      (*C).n_back_adj=(*C).n_back;
      (*C).s_col=(*C).w_col;
      break;
    case PCKEY_ENTER:
      (*C).w_col=0;
      (*C).w_row++;
      if ((*C).w_row>=(*C).MAX_ROWS)
	(*C).w_row=0;
	
      for (j=0;j<(*C).MAX_COLS;j++)
	(*C).screen[(*C).w_row][j].c='\0';
      (*C).n_row++;
      if ((*C).n_row>=(*C).MAX_ROWS)
	(*C).n_row=(*C).MAX_ROWS-1;
      (*C).n_back=0;
      (*C).s_col=(*C).w_col;
      break;
    case '\r':
      (*C).w_col=0;
      (*C).s_col=(*C).w_col;
      break;
    default:
      (*C).screen[(*C).w_row][(*C).w_col].c=c;
      (*C).screen[(*C).w_row][(*C).w_col].color_fg=(*C).color_fg;
      (*C).screen[(*C).w_row][(*C).w_col].color_bg=(*C).color_bg;
      (*C).w_col++;
      if ((*C).w_col>=(*C).MAX_COLS)
	{
	  (*C).w_col=0;
	  (*C).w_row++;
	  if ((*C).w_row>=(*C).MAX_ROWS)
	    (*C).w_row=0;
	  for (j=0;j<(*C).MAX_COLS;j++)
	    (*C).screen[(*C).w_row][j].c='\0';
	  (*C).n_row++;
	  if ((*C).n_row>=(*C).MAX_ROWS)
	    (*C).n_row=(*C).MAX_ROWS-1;
	}
      (*C).n_back=0;
      (*C).n_back_adj=(*C).n_back;
      (*C).s_col=(*C).w_col;
      break;
    }
}
#endif

void lb_ft_printf(CONSOLE_T *C, const char *format, ...)
{
  va_list args;
  SINT16_T size, i;
  char *buffer;

  va_start(args,format);

  /* If the framebuffer hasn't been opened, do a regular printf to tty */
  if ( C==NULL )
    {
      vprintf(format, args);
      return;
    }
  
  //if (C==NULL)
  // {
  //  printf("Error: lb_ft_printf() --> Console hasn't been initialized\r\n");
  //  
      exit(EXIT_FAILURE);
   // }

  /* We want this function to support an arbitrary size. */
  size=vsnprintf(NULL, 0, format, args);
  buffer=malloc(size+1);
  if (buffer==NULL)
    {
      printf("Error: lb_ft_printf() --> malloc failed");
      exit(EXIT_FAILURE);
    }
  /* This seems to be a bug */
  va_end(args);
  va_start(args,format);
  vsprintf(buffer, format, args);
  i=0;
  
  while (buffer[i]!='\0')
    {
      lb_ft_printc(C, buffer[i]);
      i++;
    }
  
  va_end(args);
  free(buffer);
}

void lb_ft_resize_console(PICTURE_T *Pic, FONT_T *font, CONSOLE_T *C)
{
  SINT16_T width, height;
  if (font==NULL)
    {
      printf("Error: lb_ft_resize_console() --> Font hasn been initialized\r\n");
      exit(EXIT_FAILURE);
    }
  if (Pic==NULL)
    {
      width=ty_screen.w;
      height=ty_screen.h;
    }
  else
    {
      width=(*Pic).w;
      height=(*Pic).h;
    }
  
  (*C).MAX_COLS=width /((*font).scale_x*((*font).cols+(*font).gap_x));
  (*C).MAX_ROWS_V=height/((*font).scale_y*((*font).rows+(*font).gap_y));
  (*C).MAX_ROWS=4*(*C).MAX_ROWS_V; /* Default: 4 pages */ 
  //printf("lb_ft_resize_console() --> text console has: cols=%d  rows=%d\r\n",(*C).MAX_COLS,(*C).MAX_ROWS);  
}


void lb_ft_draw_console(PICTURE_T *Pic, FONT_T *font, CONSOLE_T *C, COPYMODE_T copymode)
{
  SINT16_T i, j, n_vis_rows, temp_cursor, temp_row;
  PIXEL_T temp_color_fg, temp_color_bg;
  //  for (i=0;i<(*C).MAX_ROWS_V;i++)
  // for (j=0;j<(*C).MAX_COLS;j++)
  //  lb_ft_draw_char(Pic, font,
  //		      j*(*font).scale_x*((*font).cols+(*font).gap_x),
  //		      i*(*font).scale_y*((*font).rows+(*font).gap_y), (*C).screen[i][j], copymode);

  n_vis_rows=lb_in_min(lb_ft_console_get_n(C),(*C).MAX_ROWS_V-1);


  for (i=0;i<=n_vis_rows;i++)
    {
      for (j=0;j<(*C).MAX_COLS;j++)
	{
	  temp_cursor=((*C).b_row-(*C).n_back+(*C).MAX_ROWS) % (*C).MAX_ROWS;
	  temp_row=(i+(*C).b_row-(*C).n_back_adj-n_vis_rows+(*C).MAX_ROWS) % (*C).MAX_ROWS;

	  temp_color_fg=(*font).color_fg;
	  temp_color_bg=(*font).color_bg;

	  /* The cursor inverts the colors */
	  if ((temp_cursor==temp_row) && (j==(*C).s_col))
	    {
	      (*font).color_fg=(*C).screen[temp_row][j].color_bg;
	      (*font).color_bg=(*C).screen[temp_row][j].color_fg;
	    }
	  else
	    {
	      (*font).color_fg=(*C).screen[temp_row][j].color_fg;
	      (*font).color_bg=(*C).screen[temp_row][j].color_bg;
	    }
	    
	  lb_ft_draw_char(Pic, font,
			  j*(*font).scale_x*((*font).cols+(*font).gap_x),
			  i*(*font).scale_y*((*font).rows+(*font).gap_y),
			  (*C).screen[temp_row][j].c, copymode);
	
	  (*font).color_fg=temp_color_fg;
	  (*font).color_bg=temp_color_bg;
	    	  
	  //if ((temp_cursor==temp_row) && (j==s_col))
	  //lb_co_color_fg(lb_gr_12RGB(COLOR_WHITE));
	}
    }
}

void lb_ft_print_console(CONSOLE_T *C)
{
  //  SINT16_T i, j;
  //for (i=0;i<(*C).max_rows;i++)
  // {
  //  for (j=0;j<(*C).max_cols;j++)
  //	printf("[%c]",(*C).screen[i][j]);
  //  printf("\n");
  //}
}


void lb_ft_draw_text(PICTURE_T *Pic, FONT_T *font, SINT16_T xc, SINT16_T yc, const char *text,  COPYMODE_T copymode)
{
  UINT16_T pos_x, pos_y, k_text;
  k_text=0;
  pos_x=0;
  pos_y=0;
  while (text[k_text]!='\0')
    {
      /* Special cases first: non-printable characters */
      if(text[k_text]=='\r')
	pos_x=0;
      else
	if(text[k_text]=='\n')
	  pos_y++;
	else
	  {
	    switch((*font).angle)
	      {
	      case 0:
		lb_ft_draw_char(Pic, font,
				xc + pos_x*(*font).scale_x*((*font).cols+(*font).gap_x),
				yc + pos_y*(*font).scale_y*((*font).rows+(*font).gap_y), text[k_text], copymode);
		break;
	      case 90:
		lb_ft_draw_char(Pic, font,
				xc + pos_y*(*font).scale_y*((*font).rows+(*font).gap_y),
				yc - pos_x*(*font).scale_x*((*font).cols+(*font).gap_x), text[k_text], copymode);
		break;
	      case 180:
		lb_ft_draw_char(Pic, font,
				xc - pos_x*(*font).scale_x*((*font).cols+(*font).gap_x),
				yc - pos_y*(*font).scale_y*((*font).rows+(*font).gap_y), text[k_text], copymode);
		break;
	      case 270:
		lb_ft_draw_char(Pic, font,
				xc - pos_y*(*font).scale_y*((*font).rows+(*font).gap_y),
				yc + pos_x*(*font).scale_x*((*font).cols+(*font).gap_x), text[k_text], copymode);
		break;
	      default:
		printf("Error: lb_ft_draw_text() --> invalid angle = %d\r\n",(*font).angle);
		exit(1);
		break;
	      }
	    pos_x++;
	    if(pos_x==(*font).max_x)
	      {
		pos_x=0;
		pos_y++;
	      }
	  }
      k_text++;
    }
}

void lb_ft_draw_text_centered(PICTURE_T *Pic, FONT_T *font, SINT16_T x1, SINT16_T y1, SINT16_T x2, SINT16_T y2,
			      const char *text, COPYMODE_T copymode)
{
  SINT16_T xc, yc, x_low, y_low, x_high, y_high, pos_x, pos_y, pos_x_max, k_text;
  if(x2>=x1)
    {
      x_low=x1;
      x_high=x2;
    }
  else
    {
      x_low=x2;
      x_high=x1;
    }
  if(y2>=y1)
    {
      y_low=y1;
      y_high=y2;
    }
  else
    {
      y_low=y2;
      y_high=y1;
    }
  
  k_text=0;
  pos_x=0;
  pos_y=0;
  pos_x_max=0;
  while (text[k_text]!='\0')
    {
      /* Special cases first: non-printable characters */
      if(text[k_text]=='\r')
	pos_x=0;
      else
	if(text[k_text]=='\n')
	  pos_y++;
	else
	  {
	    pos_x++;
	    if(pos_x==(*font).max_x)
	      {
		pos_x=0;
		pos_y++;
	      }
	    if(pos_x>pos_x_max)
	      pos_x_max=pos_x;
	  }
      k_text++;
    }
  switch((*font).angle)
    {
    case 0:
      xc = x_low + 0.5*((x_high-x_low)-pos_x_max*(*font).scale_x*((*font).cols+(*font).gap_x));
      yc = y_low + 0.5*((y_high-y_low)-(pos_y+1)*(*font).scale_y*((*font).rows+(*font).gap_y));
      break;
    case 90:
      xc = x_low + 0.5*((x_high-x_low)-(pos_y+1)*(*font).scale_y*((*font).rows+(*font).gap_y));
      yc = y_low + 0.5*((y_high-y_low)+pos_x_max*(*font).scale_x*((*font).cols+(*font).gap_x));
      break;
    case 180:
      xc = x_low + 0.5*((x_high-x_low)+pos_x_max*(*font).scale_x*((*font).cols+(*font).gap_x));
      yc = y_low + 0.5*((y_high-y_low)+(pos_y+1)*(*font).scale_y*((*font).rows+(*font).gap_y));
      break;
    case 270:
      xc = x_low + 0.5*((x_high-x_low)+(pos_y+1)*(*font).scale_y*((*font).rows+(*font).gap_y));
      yc = y_low + 0.5*((y_high-y_low)-pos_x_max*(*font).scale_x*((*font).cols+(*font).gap_x));
      break;
    default:
      printf("Error: lb_ft_draw_text() --> invalid angle = %d\r\n",(*font).angle);
      exit(1);
      break;
    }
  lb_ft_draw_text(Pic, font, xc, yc, text, copymode);   
}


SINT8_T lb_ft_assert_dimensions_console(CONSOLE_T *C)
{
  if ( ((*C).MAX_ROWS <= 0) || ((*C).MAX_ROWS > 1024) ||
       ((*C).MAX_COLS <= 0) || ((*C).MAX_COLS > 4096) )
    return 0;
  return 1;
}

void  lb_ft_create_console(CONSOLE_T *C)
{
  UINT16_T i, j;
  
  if (!lb_ft_assert_dimensions_console(C))
    {
      printf("Error: lb_ft_create_console() --> invalid dimension. MAX_ROWS=%d, MAX_COLS=%d\r\n",
	     (*C).MAX_ROWS,(*C).MAX_COLS);
      exit(EXIT_FAILURE);
    }
  
  /* 2D circular buffer memory allocation */
  (*C).screen=malloc((*C).MAX_ROWS*sizeof(CONSOLE_CHAR_T*));
  if ((*C).screen==NULL)
    {
      //printf("Error: lb_ft_create_console() --> Out of memory: 02\r\n");
      //exit(1);
    }
  for (i=0;i<(*C).MAX_ROWS;i++)
    { 
      (*C).screen[i]=malloc((*C).MAX_COLS*sizeof(CONSOLE_CHAR_T));
      if ((*C).screen[i]== NULL)
	{
	  printf("Error: lb_ft_create_console() --> Out of memory: 03\r\n");
	  exit(1);
	}
    }
  for (i=0;i<(*C).MAX_ROWS;i++)
    for (j=0;j<(*C).MAX_COLS;j++)
      {
	(*C).screen[i][j].c='\0';
	(*C).screen[i][j].color_fg=(*C).color_fg;
	(*C).screen[i][j].color_bg=(*C).color_bg;
      }
  
  /* 2D circular buffer */
  
  (*C).a_row=0;  
  (*C).b_row=0;  
  (*C).w_col=0;        /* Index of the column of the next cell to be written */
  (*C).s_row=0;        /* Index of the row of the next cell to be "seen" (cursor) */
  (*C).s_col=0;        /* Index of the column of the next cell to be "seen" (cursor) */
  (*C).n_back=0;       /* Number of rows "backwards" counted from the next cell to be written */
  (*C).n_back_adj=0;   /* Adjusted number of rows "backwards", to add hysteresis */
}

void  lb_ft_release_console(CONSOLE_T *C)
{
  UINT16_T i;
  for (i=0;i<(*C).MAX_ROWS;i++)
    free((*C).screen[i]);
  free((*C).screen);
}
