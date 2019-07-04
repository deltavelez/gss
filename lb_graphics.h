#include <jpeglib.h>
#include <SDL2/SDL.h>
#include "lb_types.h"

#ifndef LB_GRAPHICS_H
#define LB_GRAPHICS_H


void lb_gr_SDL_init(const char *title, Uint32 flags, S_INT_16_T width, S_INT_16_T height,  U_INT_8_T r, U_INT_8_T g, U_INT_8_T b);
void lb_gr_SDL_close();
void lb_gr_refresh();

void lb_gr_fb_line_h(SCREEN_T *screen, S_INT_16_T y0, S_INT_16_T x0, S_INT_16_T x1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b);
void lb_gr_fb_line_h_copymode(SCREEN_T *screen, S_INT_16_T y0, S_INT_16_T x0, S_INT_16_T x1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T a, COPYMODE_T copymode);
void lb_gr_fb_line_v(SCREEN_T *screen, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T y1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b);
void lb_gr_fb_line_v_copymode(SCREEN_T *screen, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T y1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T a, COPYMODE_T copymode);

void  lb_gr_fb_rectangle(SCREEN_T *screen, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b);
void  lb_gr_fb_rectangle_copymode(SCREEN_T *screen, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T a, COPYMODE_T copymode);
void _lb_gr_fb_setpixel_ARGB(SCREEN_T *screen, S_INT_16_T x, S_INT_16_T y, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T a);
void  lb_gr_fb_setpixel_ARGB(SCREEN_T *screen, S_INT_16_T x, S_INT_16_T y, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b, U_INT_8_T a);
void _lb_gr_fb_setpixel_XRGB(SCREEN_T *screen, S_INT_16_T x, S_INT_16_T y, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b);
void  lb_gr_fb_setpixel_XRGB(SCREEN_T *screen, S_INT_16_T x, S_INT_16_T y, U_INT_8_T r, U_INT_8_T g, U_INT_8_T b);
void lb_gr_fb_setpixel_ARGB_copymode(SCREEN_T *screen, S_INT_16_T x, S_INT_16_T y, U_INT_8_T src_r, U_INT_8_T src_g, U_INT_8_T src_b, U_INT_8_T src_a, COPYMODE_T copymode);




PIXEL_T    lb_gr_12RGB(U_INT_16_T number);
S_INT_8_T  lb_gr_assert_dimensions_line2d_i(LINE_2D_INT_T *L);
S_INT_8_T  lb_gr_assert_dimensions_line2d_f(LINE_2D_FLOAT_T *L);
S_INT_8_T  lb_gr_assert_dimensions_line3d_f(LINE_3D_FLOAT_T *L);
S_INT_8_T  lb_gr_assert_dimensions_picture(PICTURE_T *Pic);
void       lb_gr_bitmap_rotate(PICTURE_T *pic_src, PICTURE_T *pic_dst, FLOAT_T angle, PIXEL_T default_color);
void       lb_gr_bitmap_rotate_sampling(PICTURE_T *pic_src, PICTURE_T *pic_dst, FLOAT_T angle, U_INT_8_T n_samples,
					PIXEL_T default_color);

void       lb_gr_BMPfile_getsize(const char *filename, S_INT_16_T *width, S_INT_16_T *height);
void       lb_gr_BMPfile_load_to_pic(const char *filename, PICTURE_T *Pic, U_INT_8_T alpha);
void       lb_gr_BMPfile_load_to_matrix(const char *filename, MATRIX_R_T *R, MATRIX_R_T *G, MATRIX_R_T *B);

S_INT_8_T  lb_gr_BMPfile_load_to_matrix_gs(const char *filename, MATRIX_R_T *P);
void       lb_gr_BMPfile_save(const char *filename, PICTURE_T *Pic);
S_INT_8_T  lb_gr_JPGfile_save(const char *filename, PICTURE_T *Pic, U_INT_8_T quality);

void       lb_gr_clear_picture(PICTURE_T *Pic, PIXEL_T default_color);
S_INT_16_T lb_gr_check_left_i(POINT_2D_INT_T P0, POINT_2D_INT_T P1, POINT_2D_INT_T P);
S_INT_16_T lb_gr_check_left_f(POINT_2D_FLOAT_T P0, POINT_2D_FLOAT_T P1, POINT_2D_FLOAT_T P);

void       lb_gr_create_line2d_i(LINE_2D_INT_T *L);
void       lb_gr_create_line2d_f(LINE_2D_FLOAT_T *L);
void       lb_gr_create_line3d_f(LINE_3D_FLOAT_T *L);
void       lb_gr_create_picture(PICTURE_T *Pic, PIXEL_T default_color );
void       lb_gr_create_zbuffer(PICTURE_T *Pic, MATRIX_R_T *Z);
void       lb_gr_reset_zbuffer(MATRIX_R_T *Z);
void       lb_gr_plot_zbuffer_line_1(PICTURE_T *Pic, VIEWPORT_3D_T vp3d, FLOAT_T Rot[3][3], MATRIX_R_T *Z,
				     POINT_3D_FLOAT_T P0, POINT_3D_FLOAT_T P1, PIXEL_T color, COPYMODE_T copymode);
void       lb_gr_plot_zbuffer_dot(PICTURE_T *Pic, VIEWPORT_3D_T vp3d, FLOAT_T Rot[3][3], MATRIX_R_T *Z,
				  POINT_3D_FLOAT_T P, PIXEL_T color, COPYMODE_T copymode);


void       lb_gr_plot_zbuffer_triangle(PICTURE_T *Pic, VIEWPORT_3D_T vp3d, FLOAT_T Rot[3][3], MATRIX_R_T *Z,
				       POINT_3D_FLOAT_T PA, POINT_3D_FLOAT_T PB, POINT_3D_FLOAT_T PC,
				       PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_plot_zbuffer_pixel(PICTURE_T *Pic,  MATRIX_R_T *Z, FLOAT_T xp, FLOAT_T yp,
				    FLOAT_T PA_x, FLOAT_T PA_y, FLOAT_T PB_x, FLOAT_T PB_y, FLOAT_T PC_x, FLOAT_T PC_y,
				    FLOAT_T PA_hz, FLOAT_T PB_hz, FLOAT_T PC_hz,
				    PIXEL_T color, COPYMODE_T copymode);


void       lb_gr_delay(S_INT_32_T delay);

void       lb_gr_draw_circle(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T radius, PIXEL_T color, COPYMODE_T copymode); 
void       lb_gr_draw_circle_antialiasing(PICTURE_T *Pic, FLOAT_T xc, FLOAT_T yc, FLOAT_T radius, FLOAT_T w, PIXEL_T color,
					  COPYMODE_T copymode); 
void       lb_gr_draw_circle_antialiasing2(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T radius, PIXEL_T color,
					   COPYMODE_T copymode); 
void       lb_gr_draw_circle_antialiasing3(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T radius, PIXEL_T color,
					   COPYMODE_T copymode); 
void       lb_gr_draw_circle_antialiasing_simple(PICTURE_T *Pic, FLOAT_T xc, FLOAT_T yc, FLOAT_T radius, FLOAT_T w, PIXEL_T color,
						 COPYMODE_T copymode); 
void       lb_gr_draw_circle_arc(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T radius, FLOAT_T a1, FLOAT_T a2,
				 PIXEL_T color, COPYMODE_T mode); 
void       lb_gr_draw_circle_filled_antialiasing(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T radius, PIXEL_T color,
						 COPYMODE_T copymode);
void       lb_gr_draw_circle_filled_antialiasing_f(PICTURE_T *Pic, FLOAT_T xc, FLOAT_T yc, FLOAT_T radius, PIXEL_T color, COPYMODE_T copymode); 

void       lb_gr_draw_circle_filled_fast(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T radius, PIXEL_T color,
					 COPYMODE_T mode);
void       lb_gr_draw_circle_filled_slow(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T radius, PIXEL_T color,
					 COPYMODE_T copymode);

void       lb_gr_draw_ellipse(PICTURE_T *Pic, S_INT_32_T xc, S_INT_32_T yc, S_INT_32_T a, S_INT_32_T b, PIXEL_T color, COPYMODE_T copymode); 

void       lb_gr_draw_ellipse_rotated(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T a, S_INT_16_T b, FLOAT_T angle, PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_ellipse_rotated_antialiasing(PICTURE_T *Pic, FLOAT_T xc, FLOAT_T yc, FLOAT_T a, FLOAT_T b, FLOAT_T angle, FLOAT_T w, S_INT_16_T n_q1, PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode);

void       lb_gr_draw_ellipse_antialiasing2(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T a, S_INT_16_T b, PIXEL_T color, COPYMODE_T copymode); 

void       lb_gr_draw_ellipse_antialiasing3(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, S_INT_16_T a, S_INT_16_T b, PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_histogram(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, VECTOR_R_T *bins, 
				PIXEL_T color_border, PIXEL_T color_background, PIXEL_T color_bar_border, PIXEL_T color_bar);


void       lb_gr_implicit_2d(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, MATRIX_R_T *M, FLOAT_T w, PIXEL_T pixel,
			     COPYMODE_T copymode, LINEMODE_T linemode);

void       lg_gr_draw_axis_2d(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, FONT_T *font, FLOAT_T w_axis, PIXEL_T color_axis, FLOAT_T w_grid,
			      PIXEL_T color_grid_x, FLOAT_T delta_grid_x, PIXEL_T color_grid_y, FLOAT_T delta_grid_y,
			      U_INT_16_T options, COPYMODE_T copymode, LINEMODE_T linemode);

void       lg_gr_draw_axis_2d_polar(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, FONT_T *font,
				    FLOAT_T r0, FLOAT_T r1, FLOAT_T delta_r, PIXEL_T color_r, 
				    FLOAT_T t0, FLOAT_T t1, FLOAT_T delta_t, PIXEL_T color_t,
				    U_INT_16_T options, COPYMODE_T copymode);

void       lg_gr_draw_axis_3d(PICTURE_T *Pic, VIEWPORT_3D_T vp3d, FLOAT_T Rot[3][3], FONT_T *font,
			      FLOAT_T axis_size,
			      FLOAT_T w_axis,
			      PIXEL_T color_axis,
			      FLOAT_T xr_min, FLOAT_T xr_max, FLOAT_T delta_grid_x,
			      FLOAT_T yr_min, FLOAT_T yr_max, FLOAT_T delta_grid_y,
			      FLOAT_T zr_min, FLOAT_T zr_max, FLOAT_T delta_grid_z,
			      FLOAT_T w_grid,
			      PIXEL_T color_grid,
			      U_INT_16_T options,
			      const char *label_o,
			      const char *label_x,
			      const char *label_y,
			      const char *label_z,			      
			      COPYMODE_T copymode,
			      LINEMODE_T linemode);

void       lb_gr_draw_line(PICTURE_T *Pic, FLOAT_T x0, FLOAT_T y0, FLOAT_T x1, FLOAT_T y1, FLOAT_T w, PIXEL_T color,
			   COPYMODE_T copymode, LINEMODE_T linemode);
void       lb_gr_draw_line1(PICTURE_T *Pic, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1, PIXEL_T color,
			    COPYMODE_T copymode);

void       lb_gr_draw_line1_f(PICTURE_T *Pic, FLOAT_T x0, FLOAT_T y0, FLOAT_T x1, FLOAT_T y1, PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_line2(PICTURE_T *Pic, S_INT_16_T _x0, S_INT_16_T _y0, S_INT_16_T _x1, S_INT_16_T _y1, PIXEL_T color,
			    COPYMODE_T copymode);
void       lb_gr_draw_line3(PICTURE_T *Pic, S_INT_16_T _x0, S_INT_16_T _y0, S_INT_16_T _x1, S_INT_16_T _y1, PIXEL_T color,
			    COPYMODE_T copymode);
void       lb_gr_draw_line_antialiasing(PICTURE_T *Pic, FLOAT_T _xr0, FLOAT_T _yr0, FLOAT_T _xr1, FLOAT_T _yr1, FLOAT_T w,
					PIXEL_T color, COPYMODE_T copymode);
void       lb_gr_draw_line_antialiasing2(PICTURE_T *Pic, S_INT_16_T _x0, S_INT_16_T _y0, S_INT_16_T _x1, S_INT_16_T _y1,
					 PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_line_antialiasing2_f(PICTURE_T *Pic, FLOAT_T _x0, FLOAT_T _y0, FLOAT_T _x1, FLOAT_T _y1,
					 PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_line_antialiasing3(PICTURE_T *Pic, S_INT_16_T _x0, S_INT_16_T _y0, S_INT_16_T _x1, S_INT_16_T _y1,
					 PIXEL_T color, COPYMODE_T copymode);
void       lb_gr_draw_line_antialiasing3_f(PICTURE_T *Pic, FLOAT_T _x0, FLOAT_T _y0, FLOAT_T _x1, FLOAT_T _y1,
					 PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_pixel(PICTURE_T *Pic, S_INT_16_T x, S_INT_16_T y, PIXEL_T pixel, COPYMODE_T copymode);
void       lb_gr_draw_polygon_i(PICTURE_T *Pic, LINE_2D_INT_T *L, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode,
				  LINEMODE_T linemode);
void       lb_gr_draw_polygon_f(PICTURE_T *Pic, LINE_2D_FLOAT_T *L, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode,
				LINEMODE_T linemode);
void       lb_gr_draw_polygon_fill_i(PICTURE_T *Pic, LINE_2D_INT_T *L, PIXEL_T color, COPYMODE_T copymode);
void       lb_gr_draw_polygon_antialiasing(PICTURE_T *Pic, LINE_2D_FLOAT_T *L, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_rectangle(PICTURE_T *Pic, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1,  PIXEL_T color,
				COPYMODE_T copymode);

void       lb_gr_draw_rectangle_bar(PICTURE_T *Pic, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1, S_INT_16_T w,
				    PIXEL_T color_line, PIXEL_T color_background,  COPYMODE_T copymode);

void       lb_gr_draw_rectangle_line(PICTURE_T *Pic, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1, S_INT_16_T w,
				     PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_rectangle_solid(PICTURE_T *Pic, S_INT_16_T x0, S_INT_16_T y0, S_INT_16_T x1, S_INT_16_T y1, PIXEL_T color);

void        lb_gr_draw_triangle_fill_i(PICTURE_T *Pic, POINT_2D_INT_T P0, POINT_2D_INT_T P1, POINT_2D_INT_T P2,
				PIXEL_T color, COPYMODE_T copymode);

S_INT_16_T lb_gr_is_in_polygon_i(LINE_2D_INT_T *L, POINT_2D_INT_T P);
S_INT_16_T lb_gr_is_in_polygon_f(LINE_2D_FLOAT_T *L, POINT_2D_FLOAT_T P);
S_INT_8_T  lb_gr_matrix_gs_to_pic(MATRIX_R_T *A, PICTURE_T *Pic);

void       lb_gr_plot_continuous_fn_2d(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, FUNCTION_X fx_t, FUNCTION_X fy_t,
				       FLOAT_T t0, FLOAT_T t1, FLOAT_T delta,
				       S_INT_16_T max_exp, PIXEL_T color, COPYMODE_T copymode);
void       lb_gr_plot_continuous_fn_2d_antialiasing(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, FUNCTION_X fx_t, FUNCTION_X fy_t,
					      FLOAT_T t0, FLOAT_T t1, FLOAT_T delta,
						    S_INT_16_T max_exp, FLOAT_T w, PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_plot2d(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, FLOAT_T xr, FLOAT_T yr, FLOAT_T w,
			PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode);
void       lb_gr_plot2d_line(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, LINE_2D_FLOAT_T *L, FLOAT_T w,
			     PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode);
void       lb_gr_plot2d_line_reverse(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, VECTOR_R_T *Lx, VECTOR_R_T *Ly, FLOAT_T w,
				     PIXEL_T color, COPYMODE_T copymode);
void       lb_gr_plot2d_line_reverse_slow(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, VECTOR_R_T *Lx, VECTOR_R_T *Ly, FLOAT_T w,
					  PIXEL_T color, COPYMODE_T copymode);
void       lb_gr_plot3d(PICTURE_T *Pic, VIEWPORT_3D_T vp3d,  FLOAT_T Rot[3][3], POINT_3D_FLOAT_T Pr, FLOAT_T w,
			PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode);

void       lb_gr_plot3d_surface(PICTURE_T *Pic, VIEWPORT_3D_T vp3d,  FLOAT_T Rot[3][3], MATRIX_POINT_3D_T *S,
				FLOAT_T w, PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode);

void       lb_gr_project_2d(VIEWPORT_2D_T vp2d, FLOAT_T xr, FLOAT_T yr, FLOAT_T *xp, FLOAT_T *yp);
void       lb_gr_project_2d_inv(VIEWPORT_2D_T vp2d, FLOAT_T xp, FLOAT_T yp, FLOAT_T *xr, FLOAT_T *yr);
void       lb_gr_project_2d_log(VIEWPORT_2D_T vp2d, FLOAT_T xr, FLOAT_T yr, FLOAT_T *xp, FLOAT_T *yp);
void       lb_gr_project_2d_x(VIEWPORT_2D_T vp2d, FLOAT_T xr, FLOAT_T *xp);
void       lb_gr_project_2d_x_log(VIEWPORT_2D_T vp2d, FLOAT_T xr, FLOAT_T *xp);
void       lb_gr_project_2d_y(VIEWPORT_2D_T vp2d, FLOAT_T yr, FLOAT_T *yp);
void       lb_gr_project_2d_y_log(VIEWPORT_2D_T vp2d, FLOAT_T yr, FLOAT_T *yp);

void       lb_gr_project_2d_vector_c_to_line_int(VIEWPORT_2D_T vp2d, VECTOR_C_T *V, LINE_2D_INT_T *L);
void       lb_gr_project_2d_vector_r_to_line_int(VIEWPORT_2D_T vp2d, VECTOR_R_T *X, VECTOR_R_T *Y, LINE_2D_INT_T *L);

void       lb_gr_project_3d(VIEWPORT_3D_T vp3d, FLOAT_T Rot[3][3], POINT_3D_FLOAT_T P, FLOAT_T *xr, FLOAT_T *yr, FLOAT_T *hz);


void       lb_gr_release_line2d_i(LINE_2D_INT_T *L);
void       lb_gr_release_line2d_f(LINE_2D_FLOAT_T *L);
void       lb_gr_release_line3d_f(LINE_3D_FLOAT_T *L);
void       lb_gr_release_picture(volatile PICTURE_T *Pic);
void lb_gr_render_picture(PICTURE_T *Pic, S_INT_16_T xc, S_INT_16_T yc, COPYMODE_T copymode);
PIXEL_T    lb_gr_value_to_color(S_INT_8_T value);



//void lb_gr_release_line3d(LINE_3D_FLOAT_T *L);
//void lb_gr_plot_line3d(VIEWPORT_3D_T vp3d, MATRIX_R_T *Rot, LINE_3D_FLOAT_T *L);
//void lb_gr_project_3d_to_2d_analytical(VIEWPORT_3D_T vp3d, MATRIX_R_T *Rot, VECTOR_R_T *Pa, U_INT_16_T *xp, U_INT_16_T *yp, U_INT_8_T *flag);
//void lb_gr_project_3d_to_2d_analytical_two_vectors(VIEWPORT_3D_T vp3d, VECTOR_R_T *V1, VECTOR_R_T *V2, VECTOR_R_T *Pa, U_INT_16_T *xp, U_INT_16_T *yp, U_INT_8_T *flag);

// void lb_gr_vp_putpixel(VIEWPORT_2D_T vp2d, FLOAT_T x, FLOAT_T y, int color);

#endif /* LB_GRAPHICS_H */
