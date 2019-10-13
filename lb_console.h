#include <termios.h>

#ifndef LB_CONSOLE_H
#define LB_CONSOLE_H

#define TEXT_MODE_BOLD 1 
#define TEXT_MODE_FAINT  2 
#define TEXT_MODE_UNDERLINE  4
#define TEXT_MODE_NEGATIVE 7
#define TEXT_MODE_CONCEAL 8
#define TEXT_MODE_CROSSEDOUT 9
#define TEXT_MODE_NOBOLD 22
#define TEXT_MODE_NOUNDERLINE 24
#define TEXT_MODE_POSITIVE 27
#define TEXT_MODE_REVEAL 28
#define TEXT_MODE_NOCROSSEDOUT 29
#define TEXT_COLOR_BLACK 30
#define TEXT_COLOR_RED 31
#define TEXT_COLOR_GREEN 32
#define TEXT_COLOR_YELLOW 33
#define TEXT_COLOR_BLUE 34
#define TEXT_COLOR_MAGENTA 35
#define TEXT_COLOR_CYAN 36
#define TEXT_COLOR_WHITE 37
#define TEXT_COLOR_DEFAULT 39
#define TEXT_COLOR_BACKGROUND 10

/* The following values are common but are not supported by all consoles */
//#define TEXT_MODE_ITALIC 3 
//#define TEXT_MODE_BLINK 5
//#define TEXT_MODE_BLINK_FAST 6
//#define TEXT_MODE_FONT0 10  /* 10: Primary font, 11-19 n-font */
//#define TEXT_MODE_FRAKTUR 20
//#define TEXT_MODE_BOLDOFFORUNDERLINE 21
//#define TEXT_MODE_NOITALICNOFRAKTUR 23
//#define TEXT_MODE_BLINKOFF 25

void lb_co_cls(void);
void lb_co_cls_from_cursor(void);

void lb_co_color(S_INT16_T color);
void lb_co_color_bg(PIXEL_T color);
void lb_co_color_fg(PIXEL_T color);
void lb_co_color_bg_216(S_INT16_T r, S_INT16_T g, S_INT16_T b);
void lb_co_color_fg_216(S_INT16_T r, S_INT16_T g, S_INT16_T b);
void lb_co_color_bg_RGB(PIXEL_T color);
void lb_co_color_fg_RGB(PIXEL_T color);

void lb_co_cursor_clearfrom(void);
void lb_co_cursor_clearline(void);
void lb_co_cursor_clearto(void);
void lb_co_cursor_hide(void);
void lb_co_cursor_shift(S_INT16_T x, S_INT16_T y);
void lb_co_cursor_show(void);
 
unsigned char lb_co_getch(void);
unsigned char lb_co_getch_pc(void);
void lb_co_gotox(S_INT16_T x);
void lb_co_gotoxy(S_INT16_T x,S_INT16_T y);
S_INT8_T lb_co_kbhit();
void lb_co_printf_block(char *str, S_INT16_T a, S_INT16_T b);
void lb_co_printf_firstn(char *str, S_INT16_T n);
void lb_co_printf_lastn(char *str, S_INT16_T n);
void lb_co_reset(void);
void lb_co_reset_attributes(void);
void lb_co_reset_colors(void);
void lb_co_set_echo(S_INT8_T state); 
void lb_co_str_to_upper(char *str);


#endif /* LB_CONSOLE_H */
