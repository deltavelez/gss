#ifndef LB_FB_H
#define LB_FB_H

S_INT_8_T lb_fb_use_shadow(void);
void lb_fb_clear(char *fb, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T t);
void lb_fb_setmode_graphics(void);
void lb_fb_setmode_text(void);
void lb_fb_exit(int code);
void lb_fb_exit_msg(const char *format, ...);
void lb_fb_flush(void);
void lb_fb_close(void);
void lb_fb_get_pixel(char *fb, S_INT_16_T x, S_INT_16_T y, U_INT_8_T *r, U_INT_8_T *g, U_INT_8_T *b, U_INT_8_T *t);
void lb_fb_get_res(const char *path_fb, S_INT_16_T *width, S_INT_16_T *height);

void lb_fb_line_h(char *fb, S_INT_16_T y0, S_INT_16_T x0, S_INT_16_T x1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T t);
void lb_fb_line_v(char *fb,  S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T y1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T t);
void lb_fb_open(const char *path_fb, const char *path_tty, S_INT_16_T scale_x, S_INT_16_T scale_y, U_INT_8_T renderoptions);
void lb_fb_parse_finfo(void);
void lb_fb_parse_vinfo(void);
void lb_fb_draw_pixel(char *fb, S_INT_16_T x, S_INT_16_T y, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T t);
void lb_fb_draw_triangle_fill_i(char *fb, POINT_2D_INT_T P0, POINT_2D_INT_T P1, POINT_2D_INT_T P2,
				U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T t);
void lb_fb_rectangle(char *fb,  S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T t);

#endif
