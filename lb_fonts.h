#ifndef LB_FONTS_H
#define LB_FONTS_H

//U_INT_8_T lb_ft_bit_in_rawfont(unsigned char *font_buffer, int character, int i, int j, int cols, int rows);

void lb_ft_set_active_console(CONSOLE_T *C);
S_INT_16_T lb_ft_console_get_n(CONSOLE_T *C);


void lb_ft_draw_console(PICTURE_T *Pic, FONT_T *font, CONSOLE_T *C, COPYMODE_T copymode);
void lb_ft_print_console(CONSOLE_T *C);
void lb_ft_console_gotox(CONSOLE_T *C, S_INT_16_T x);
void lb_ft_console_gotoy(CONSOLE_T *C, S_INT_16_T y);
void lb_ft_console_gotoxy(CONSOLE_T *C, S_INT_16_T x, S_INT_16_T y);
void lb_ft_printc(CONSOLE_T *C, char c);
void lb_ft_printf(CONSOLE_T *C, const char *format, ...);
void lb_ft_resize_console(PICTURE_T *Pic, FONT_T *font, CONSOLE_T *C);


S_INT_8_T lb_ft_fsearch_str(FILE *file, const char *text);
S_INT_8_T lb_ft_fget_next_number(FILE *file, S_INT_32_T *n);

void lb_ft_load_GLCDfont(const char *filename, FONT_T *font);
void lb_ft_release_GLCDfont(FONT_T *font);

void lb_ft_draw_char(PICTURE_T *Pic, FONT_T *font, S_INT_16_T xc, S_INT_16_T yc, char c, COPYMODE_T copymode);

void lb_ft_draw_text(PICTURE_T *Pic, FONT_T *font, S_INT_16_T xc, S_INT_16_T yc, const char *text, COPYMODE_T copymode);
void lb_ft_draw_text_centered(PICTURE_T *Pic, FONT_T *font, S_INT_16_T x1, S_INT_16_T y1, S_INT_16_T x2, S_INT_16_T y2,
			      const char *text, COPYMODE_T copymode);

S_INT_8_T lb_ft_assert_dimensions_console(CONSOLE_T *C);
void  lb_ft_create_console(CONSOLE_T *C);
void  lb_ft_release_console(CONSOLE_T *C);


#endif /* LB_FONTS_H */
