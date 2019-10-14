#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h> /* For exit() */
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <math.h>
#include <sys/ioctl.h>
#include "lb_types.h"
#include "lb_console.h"
#include "lb_fonts.h"

void lb_co_cls(void)
{
  printf("\x1b[%dJ",2);
}

void lb_co_cls_from_cursor(void)
{
  printf("\x1b[%dJ",0);
}

void lb_co_color(SINT16_T color)
{
  printf("\x1b[%dm",color);
}

/* Default fg/bg color function uses a 216-color space since it is compatible with most terminals */
void lb_co_color_bg(PIXEL_T color)
{
  printf("\x1b[48;5;%dm",16+36*(color.r*5/MAX_R)+6*(color.g*5/MAX_G)+color.b*5/MAX_B);
}

/* Default fg/bg color function uses a 216-color space since it is compatible with most terminals */
void lb_co_color_fg(PIXEL_T color)
{
  printf("\x1b[38;5;%dm",16+36*(color.r*5/MAX_R)+6*(color.g*5/MAX_G)+color.b*5/MAX_B);
}

void lb_co_color_bg_216(SINT16_T r, SINT16_T g, SINT16_T b)
{
  if ((r>5) || (g>5) || (b>5))
    {
      printf("Error: lb_co_textcolorRGB --> value higher than 5\r\n");
      exit(1);
    }  
  printf("\x1b[48;5;%dm",16+36*r+6*g+b);
}

void lb_co_color_fg_216(SINT16_T r, SINT16_T g, SINT16_T b)
{
  if ((r>5) || (g>5) || (b>5))
    {
      printf("Error: lb_co_textcolorRGB --> value higher than 5\r\n");
      exit(1);
    }  
  printf("\x1b[38;5;%dm",16+36*r+6*g+b);
}

/* Works only on consoles supporting ISO-8613-3 24-bit foreground and background color setting 
   such as: Xterm, KDE's Konsole and all libvte based terminals (including GNOME Terminal) */
void lb_co_color_bg_RGB(PIXEL_T color)
{
  printf("\x1b[48;2;%d;%d;%dm",
	 (int)round(color.r*FACTOR_N_TO_8_R),
	 (int)round(color.g*FACTOR_N_TO_8_G),
	 (int)round(color.b*FACTOR_N_TO_8_B) );
}

void lb_co_color_fg_RGB(PIXEL_T color)
{
  printf("\x1b[38;2;%d;%d;%dm",
	 (int)round(color.r*FACTOR_N_TO_8_R),
	 (int)round(color.g*FACTOR_N_TO_8_G),
	 (int)round(color.b*FACTOR_N_TO_8_B) );
}

void lb_co_cursor_clearfrom(void)
{
  printf("\x1b[0K");
}

void lb_co_cursor_clearline(void)
{
  printf("\x1b[2K");
}

void lb_co_cursor_clearto(void)
{
  printf("\x1b[1K");
}

void lb_co_cursor_hide(void)
{
    printf("\x1b[?25l");
}

void lb_co_cursor_shift(SINT16_T x, SINT16_T y)
{
  if (y<0) printf("\x1b[%dA",-y);
  else
    if (y>0) printf("\x1b[%dB",y);
  if (x<0) printf("\x1b[%dD",-x);
  else
     if (x>0) printf("\x1b[%dC",x);
}

void lb_co_cursor_show(void)
{
  printf("\x1b[?25h");
}

unsigned char lb_co_getch( ) 
{ 
  struct termios oldt,newt;
  int ch;
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
  return ch;
}

void lb_co_set_echo(SINT8_T state) 
{ 
  struct termios tsettings;

  tcgetattr( STDIN_FILENO, &tsettings );

  if (state)
    tsettings.c_lflag |= ECHO;
  else
    tsettings.c_lflag &= ~ECHO;

  tcsetattr( STDIN_FILENO, TCSANOW, &tsettings );
}

unsigned char lb_co_getch_pc(void)
{
  int n_chars;
  char bu[4];

  n_chars=lb_co_kbhit();

  if (n_chars==0) return ASCII_NUL;

  bu[0]=lb_co_getch();
  if ((bu[0]>=0x20) && (bu[0]!=0x7F)) return bu[0];
  
  /* Printable characters return a single value */
  if (bu[0]==0x7F)	return PCKEY_BS;
  if (bu[0]==0x09)	return PCKEY_TAB;
  if (bu[0]==0x0A)	return PCKEY_ENTER;
  if (bu[0]==0x1B)
    {
      if (n_chars==1) return PCKEY_ESC;
      if (n_chars>=2)
	bu[1]=lb_co_getch();

      if (n_chars==2)
	{
	  printf("An error occurred (2) [0x%02x-0x%02x]\r\n",bu[0],bu[1]);
	  return ASCII_NUL;
	}

      if (bu[1]==0x5B)
	{
	  if(n_chars>=3)
	    {
	      bu[2]=lb_co_getch();
	      if ( bu[2]==0x41 ) return PCKEY_UP;
	      if ( bu[2]==0x42 ) return PCKEY_DOWN;
	      if ( bu[2]==0x43 ) return PCKEY_RIGHT;
	      if ( bu[2]==0x44 ) return PCKEY_LEFT;
	      if ( bu[2]==0x50 ) return PCKEY_PAUSE;
	      if(n_chars>=4)
		{
		  bu[3]=lb_co_getch();
		  if ( (bu[2]==0x31) && (bu[3]==0x7E) ) return PCKEY_HOME;
		  if ( (bu[2]==0x32) && (bu[3]==0x7E) ) return PCKEY_INSERT;
		  if ( (bu[2]==0x33) && (bu[3]==0x7E) ) return PCKEY_DEL;
		  if ( (bu[2]==0x34) && (bu[3]==0x7E) ) return PCKEY_END;
		  if ( (bu[2]==0x35) && (bu[3]==0x7E) ) return PCKEY_PAGE_UP;
		  if ( (bu[2]==0x36) && (bu[3]==0x7E) ) return PCKEY_PAGE_DOWN;

		  if ( (bu[2]==0x5B) && (bu[3]==0x41) ) return PCKEY_F1;
		  if ( (bu[2]==0x5B) && (bu[3]==0x42) ) return PCKEY_F2;
		  if ( (bu[2]==0x5B) && (bu[3]==0x43) ) return PCKEY_F3;
		  if ( (bu[2]==0x5B) && (bu[3]==0x44) ) return PCKEY_F4;
		  if ( (bu[2]==0x5B) && (bu[3]==0x45) ) return PCKEY_F5;

		  if(n_chars>=5)
		    {
		      bu[4]=lb_co_getch();
		      
		      if ( (bu[2]==0x31) && (bu[3]==0x37) && (bu[4]==0x7E) ) return PCKEY_F6;
		      if ( (bu[2]==0x31) && (bu[3]==0x38) && (bu[4]==0x7E) ) return PCKEY_F7;
		      if ( (bu[2]==0x31) && (bu[3]==0x39) && (bu[4]==0x7E) ) return PCKEY_F8;
		      if ( (bu[2]==0x32) && (bu[3]==0x30) && (bu[4]==0x7E) ) return PCKEY_F9;
		      if ( (bu[2]==0x32) && (bu[3]==0x31) && (bu[4]==0x7E) ) return PCKEY_F10;
		      if ( (bu[2]==0x32) && (bu[3]==0x33) && (bu[4]==0x7E) ) return PCKEY_F11;
		      if ( (bu[2]==0x32) && (bu[3]==0x34) && (bu[4]==0x7E) ) return PCKEY_F12;
		    }
		  lb_ft_printf(ty_C,"An error occurred (4) [0x%02x-0x%02x-0x%02x-0x%02x]\r\n",
			       bu[0],bu[1],bu[2],bu[3]);
		}
	      else
		lb_ft_printf(ty_C,"An error occurred (3) [0x%02x-0x%02x-0x%02x]\r\n",bu[0],bu[1],bu[2]);
	    }
	  else
	    printf("An error occurred (2) [0x%02x-0x%02x]\r\n",bu[0],bu[1]);
	}
    }
  return ASCII_NUL;
}

void lb_co_gotox(SINT16_T x)
{
  printf("\x1b[%dG",x);
}

void lb_co_gotoxy(SINT16_T x, SINT16_T y)
{
  printf("\x1b[%d;%df",y,x);
  //  printf("\x1b[%d;%dH",x,y);  /* Equivalent */
}

SINT8_T lb_co_kbhit()
{
  // Use termios to turn off line buffering
  struct termios term;
  tcgetattr(STDIN_FILENO, &term);
  term.c_lflag &= ~ICANON;
  tcsetattr(STDIN_FILENO, TCSANOW, &term);
  setbuf(stdin, NULL);


  int bytesWaiting;
  ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
  return bytesWaiting;
}


/* Prints part of a string */
void lb_co_printf_block(char *str, SINT16_T a, SINT16_T b)
{
  SINT16_T i;
  if ( (a<=b) && (b<strlen(str)) )
    {
      i=a;
      while ( i<=b )
	{
	  printf("%c",str[i]);
	  i++;
	}
    }
}

/* Prints the first n-characters of a string */
void lb_co_printf_firstn(char *str, SINT16_T n)
{
  SINT16_T i=0;
  while ( (i<n) && (str[i]!='\0') )
    {
      printf("%c",str[i]);
      i++;
    }
}

void lb_co_printf_lastn(char *str, SINT16_T n)
{
  SINT16_T i;
  if (n<strlen(str))
    {
      i=strlen(str)-n;
      while ( str[i]!='\0' )
	{
	  printf("%c",str[i]);
	  i++;
	}
    }
}

void lb_co_reset(void)
{
    printf("\x1b""c");
}

void lb_co_reset_attributes(void)
{
    printf("\x1b[0m");
}

void lb_co_reset_colors(void)
{
    printf("\x1b[39;49m");
}


void lb_co_str_to_upper(char *str)
{
  SINT16_T i;
  i=0;
  while (str[i]!='\0')
    {
      str[i]=toupper(str[i]);
      i++;
    }
}
