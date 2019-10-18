#include <jpeglib.h>
#include <SDL2/SDL.h>
#include "lb_types.h"

#ifndef LB_GRAPHICS_H
#define LB_GRAPHICS_H


void lb_gr_SDL_init(const char *title, Uint32 flags, SINT16_T width, SINT16_T height,  UINT8_T r, UINT8_T g, UINT8_T b);

void lb_gr_SDL_close();
void lb_gr_refresh();

void lb_gr_fb_line_h(SCREEN_T *screen, SINT16_T y0, SINT16_T x0, SINT16_T x1, UINT8_T r, UINT8_T g, UINT8_T b);
void lb_gr_fb_line_h_copymode(SCREEN_T *screen, SINT16_T y0, SINT16_T x0, SINT16_T x1, UINT8_T r, UINT8_T g, UINT8_T b, UINT8_T a, COPYMODE_T copymode);
void lb_gr_fb_line_v(SCREEN_T *screen, SINT16_T x0, SINT16_T y0, SINT16_T y1, UINT8_T r, UINT8_T g, UINT8_T b);
void lb_gr_fb_line_v_copymode(SCREEN_T *screen, SINT16_T x0, SINT16_T y0, SINT16_T y1, UINT8_T r, UINT8_T g, UINT8_T b, UINT8_T a, COPYMODE_T copymode);

void  lb_gr_fb_rectangle(SCREEN_T *screen, SINT16_T x0, SINT16_T y0, SINT16_T x1, SINT16_T y1, UINT8_T r, UINT8_T g, UINT8_T b);
void  lb_gr_fb_rectangle_copymode(SCREEN_T *screen, SINT16_T x0, SINT16_T y0, SINT16_T x1, SINT16_T y1, UINT8_T r, UINT8_T g, UINT8_T b, UINT8_T a, COPYMODE_T copymode);
void _lb_gr_fb_setpixel_ARGB(SCREEN_T *screen, SINT16_T x, SINT16_T y, UINT8_T r, UINT8_T g, UINT8_T b, UINT8_T a);
void  lb_gr_fb_setpixel_ARGB(SCREEN_T *screen, SINT16_T x, SINT16_T y, UINT8_T r, UINT8_T g, UINT8_T b, UINT8_T a);
void _lb_gr_fb_setpixel_XRGB(SCREEN_T *screen, SINT16_T x, SINT16_T y, UINT8_T r, UINT8_T g, UINT8_T b);
void  lb_gr_fb_setpixel_XRGB(SCREEN_T *screen, SINT16_T x, SINT16_T y, UINT8_T r, UINT8_T g, UINT8_T b);
void lb_gr_fb_setpixel_ARGB_copymode(SCREEN_T *screen, SINT16_T x, SINT16_T y, UINT8_T src_r, UINT8_T src_g, UINT8_T src_b, UINT8_T src_a, COPYMODE_T copymode);


PIXEL_T    lb_gr_12RGB(UINT16_T number);
SINT8_T  lb_gr_assert_dimensions_line2d_i(LINE_2D_SINT16_T *L);
SINT8_T  lb_gr_assert_dimensions_line2d_f(LINE_2D_REAL_T *L);
SINT8_T  lb_gr_assert_dimensions_line3d_f(LINE_3D_REAL_T *L);
SINT8_T  lb_gr_assert_dimensions_picture(PICTURE_T *Pic);
void       lb_gr_bitmap_rotate(PICTURE_T *pic_src, PICTURE_T *pic_dst, REAL_T angle, PIXEL_T default_color);
void       lb_gr_bitmap_rotate_sampling(PICTURE_T *pic_src, PICTURE_T *pic_dst, REAL_T angle, UINT8_T n_samples,
					PIXEL_T default_color);

void       lb_gr_BMPfile_getsize(const char *filename, SINT16_T *width, SINT16_T *height);
void       lb_gr_BMPfile_load_to_pic(const char *filename, PICTURE_T *Pic, UINT8_T alpha);
void       lb_gr_BMPfile_load_to_matrix(const char *filename, MATRIX_R_T *R, MATRIX_R_T *G, MATRIX_R_T *B);

SINT8_T  lb_gr_BMPfile_load_to_matrix_gs(const char *filename, MATRIX_R_T *P);
void       lb_gr_BMPfile_save(const char *filename, PICTURE_T *Pic);
SINT8_T  lb_gr_JPGfile_save(const char *filename, PICTURE_T *Pic, UINT8_T quality);

void       lb_gr_clear_picture(PICTURE_T *Pic, PIXEL_T default_color);
SINT16_T lb_gr_check_left_i(POINT_2D_SINT16_T P0, POINT_2D_SINT16_T P1, POINT_2D_SINT16_T P);
SINT16_T lb_gr_check_left_f(POINT_2D_REAL_T P0, POINT_2D_REAL_T P1, POINT_2D_REAL_T P);

void       lb_gr_create_line2d_i(LINE_2D_SINT16_T *L);
void       lb_gr_create_line2d_r(LINE_2D_REAL_T *L);
//void       lb_gr_create_line3d_r(LINE_3D_REAL_T *L);
void       lb_gr_create_picture(PICTURE_T *Pic, PIXEL_T default_color );
void       lb_gr_create_zbuffer(PICTURE_T *Pic, MATRIX_R_T *Z);
void       lb_gr_reset_zbuffer(MATRIX_R_T *Z);
void       lb_gr_plot_zbuffer_line_1(PICTURE_T *Pic, VIEWPORT_3D_T vp3d, REAL_T Rot[3][3], MATRIX_R_T *Z,
				     POINT_3D_REAL_T P0, POINT_3D_REAL_T P1, PIXEL_T color, COPYMODE_T copymode);
void       lb_gr_plot_zbuffer_dot(PICTURE_T *Pic, VIEWPORT_3D_T vp3d, REAL_T Rot[3][3], MATRIX_R_T *Z,
				  POINT_3D_REAL_T P, PIXEL_T color, COPYMODE_T copymode);


void       lb_gr_plot_zbuffer_triangle(PICTURE_T *Pic, VIEWPORT_3D_T vp3d, REAL_T Rot[3][3], MATRIX_R_T *Z,
				       POINT_3D_REAL_T PA, POINT_3D_REAL_T PB, POINT_3D_REAL_T PC,
				       PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_plot_zbuffer_pixel(PICTURE_T *Pic,  MATRIX_R_T *Z, REAL_T xp, REAL_T yp,
				    REAL_T PA_x, REAL_T PA_y, REAL_T PB_x, REAL_T PB_y, REAL_T PC_x, REAL_T PC_y,
				    REAL_T PA_hz, REAL_T PB_hz, REAL_T PC_hz,
				    PIXEL_T color, COPYMODE_T copymode);


void       lb_gr_draw_arrow(PICTURE_T *Pic, REAL_T x0, REAL_T y0, REAL_T x1, REAL_T y1, REAL_T w, REAL_T arrow_size, PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode);

void       lb_gr_draw_circle(PICTURE_T *Pic, SINT16_T xc, SINT16_T yc, SINT16_T radius, PIXEL_T color, COPYMODE_T copymode); 
void       lb_gr_draw_circle_antialiasing(PICTURE_T *Pic, REAL_T xc, REAL_T yc, REAL_T radius, REAL_T w, PIXEL_T color,
					  COPYMODE_T copymode); 
void       lb_gr_draw_circle_antialiasing2(PICTURE_T *Pic, SINT16_T xc, SINT16_T yc, SINT16_T radius, PIXEL_T color,
					   COPYMODE_T copymode); 
void       lb_gr_draw_circle_antialiasing3(PICTURE_T *Pic, SINT16_T xc, SINT16_T yc, SINT16_T radius, PIXEL_T color,
					   COPYMODE_T copymode); 
void       lb_gr_draw_circle_antialiasing_simple(PICTURE_T *Pic, REAL_T xc, REAL_T yc, REAL_T radius, REAL_T w, PIXEL_T color,
						 COPYMODE_T copymode); 
void       lb_gr_draw_circle_arc(PICTURE_T *Pic, SINT16_T xc, SINT16_T yc, SINT16_T radius, REAL_T a1, REAL_T a2,
				 PIXEL_T color, COPYMODE_T mode); 
void       lb_gr_draw_circle_filled_antialiasing(PICTURE_T *Pic, SINT16_T xc, SINT16_T yc, SINT16_T radius, PIXEL_T color,
						 COPYMODE_T copymode);
void       lb_gr_draw_circle_filled_antialiasing_r(PICTURE_T *Pic, REAL_T xc, REAL_T yc, REAL_T radius, PIXEL_T color, COPYMODE_T copymode); 

void       lb_gr_draw_circle_filled(PICTURE_T *Pic, SINT16_T xc, SINT16_T yc, SINT16_T radius, PIXEL_T color,
					 COPYMODE_T mode);
void       lb_gr_draw_circle_filled_slow(PICTURE_T *Pic, SINT16_T xc, SINT16_T yc, SINT16_T radius, PIXEL_T color,
					 COPYMODE_T copymode);

void       lb_gr_draw_ellipse(PICTURE_T *Pic, SINT32_T xc, SINT32_T yc, SINT32_T a, SINT32_T b, PIXEL_T color, COPYMODE_T copymode); 

void       lb_gr_draw_ellipse_rotated(PICTURE_T *Pic, SINT16_T xc, SINT16_T yc, SINT16_T a, SINT16_T b, REAL_T angle, PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_ellipse_rotated_antialiasing(PICTURE_T *Pic, REAL_T xc, REAL_T yc, REAL_T a, REAL_T b, REAL_T angle, REAL_T w, SINT16_T n_q1, PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode);

void       lb_gr_draw_ellipse_antialiasing2(PICTURE_T *Pic, SINT16_T xc, SINT16_T yc, SINT16_T a, SINT16_T b, PIXEL_T color, COPYMODE_T copymode); 

void       lb_gr_draw_ellipse_antialiasing3(PICTURE_T *Pic, SINT16_T xc, SINT16_T yc, SINT16_T a, SINT16_T b, PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_histogram(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, VECTOR_R_T *bins, 
				PIXEL_T color_border, PIXEL_T color_background, PIXEL_T color_bar_border, PIXEL_T color_bar);


void       lb_gr_implicit_2d(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, MATRIX_R_T *M, REAL_T w, PIXEL_T pixel,
			     COPYMODE_T copymode, LINEMODE_T linemode);


void       lg_gr_draw_axis_2d(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, FONT_T *font,
			      PIXEL_T color_axis, REAL_T w_axis, REAL_T arrow_size,
			      PIXEL_T color_grid_x, REAL_T delta_grid_x, 
			      PIXEL_T color_grid_y, REAL_T delta_grid_y, REAL_T w_grid,
			      UINT16_T options, COPYMODE_T copymode, LINEMODE_T linemode);

void       lg_gr_draw_axis_2d_polar(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, FONT_T *font,
				    REAL_T r0, REAL_T r1, REAL_T delta_r, PIXEL_T color_r, 
				    REAL_T t0, REAL_T t1, REAL_T delta_t, PIXEL_T color_t,
				    UINT16_T options, COPYMODE_T copymode);

void       lg_gr_draw_axis_3d(PICTURE_T *Pic, VIEWPORT_3D_T vp3d, REAL_T Rot[3][3], FONT_T *font,
			      REAL_T axis_size,
			      REAL_T w_axis,
			      PIXEL_T color_axis,
			      REAL_T xr_min, REAL_T xr_max, REAL_T delta_grid_x,
			      REAL_T yr_min, REAL_T yr_max, REAL_T delta_grid_y,
	      	      REAL_T zr_min, REAL_T zr_max, REAL_T delta_grid_z,
			      REAL_T w_grid,
			      PIXEL_T color_grid,
			      UINT16_T options,
			      const char *label_o,
			      const char *label_x,
			      const char *label_y,
			      const char *label_z,			      
			      COPYMODE_T copymode,
			      LINEMODE_T linemode);

void       lb_gr_draw_line(PICTURE_T *Pic, REAL_T x0, REAL_T y0, REAL_T x1, REAL_T y1, REAL_T w, PIXEL_T color,
			   COPYMODE_T copymode, LINEMODE_T linemode);
void       lb_gr_draw_line1(PICTURE_T *Pic, SINT16_T x0, SINT16_T y0, SINT16_T x1, SINT16_T y1, PIXEL_T color,
			    COPYMODE_T copymode);

void       lb_gr_draw_line1_r(PICTURE_T *Pic, REAL_T x0, REAL_T y0, REAL_T x1, REAL_T y1, PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_line2(PICTURE_T *Pic, SINT16_T _x0, SINT16_T _y0, SINT16_T _x1, SINT16_T _y1, PIXEL_T color,
			    COPYMODE_T copymode);
void       lb_gr_draw_line3(PICTURE_T *Pic, SINT16_T _x0, SINT16_T _y0, SINT16_T _x1, SINT16_T _y1, PIXEL_T color,
			    COPYMODE_T copymode);
void       lb_gr_draw_line_antialiasing(PICTURE_T *Pic, REAL_T _xr0, REAL_T _yr0, REAL_T _xr1, REAL_T _yr1, REAL_T w,
					PIXEL_T color, COPYMODE_T copymode);
void       lb_gr_draw_line_antialiasing2(PICTURE_T *Pic, SINT16_T _x0, SINT16_T _y0, SINT16_T _x1, SINT16_T _y1,
					 PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_line_antialiasing2_r(PICTURE_T *Pic, REAL_T _x0, REAL_T _y0, REAL_T _x1, REAL_T _y1,
					 PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_line_antialiasing3(PICTURE_T *Pic, SINT16_T _x0, SINT16_T _y0, SINT16_T _x1, SINT16_T _y1,
					 PIXEL_T color, COPYMODE_T copymode);
void       lb_gr_draw_line_antialiasing3_f(PICTURE_T *Pic, REAL_T _x0, REAL_T _y0, REAL_T _x1, REAL_T _y1,
					 PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_pixel(PICTURE_T *Pic, SINT16_T x, SINT16_T y, PIXEL_T pixel, COPYMODE_T copymode);
void       lb_gr_draw_polygon_i(PICTURE_T *Pic, LINE_2D_SINT16_T *L, REAL_T w, PIXEL_T color, COPYMODE_T copymode,
				  LINEMODE_T linemode);
void       lb_gr_draw_polygon_r(PICTURE_T *Pic, LINE_2D_REAL_T *L, REAL_T w, PIXEL_T color, COPYMODE_T copymode,
				LINEMODE_T linemode);
void       lb_gr_draw_polygon_fill_i(PICTURE_T *Pic, LINE_2D_SINT16_T *L, PIXEL_T color, COPYMODE_T copymode);
void       lb_gr_draw_polygon_antialiasing(PICTURE_T *Pic, LINE_2D_REAL_T *L, REAL_T w, PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_rectangle(PICTURE_T *Pic, SINT16_T x0, SINT16_T y0, SINT16_T x1, SINT16_T y1,  PIXEL_T color,
				COPYMODE_T copymode);

void       lb_gr_draw_rectangle_bar(PICTURE_T *Pic, SINT16_T x0, SINT16_T y0, SINT16_T x1, SINT16_T y1, SINT16_T w,
				    PIXEL_T color_line, PIXEL_T color_inside,  COPYMODE_T copymode);

void       lb_gr_draw_rectangle_line(PICTURE_T *Pic, SINT16_T x0, SINT16_T y0, SINT16_T x1, SINT16_T y1, SINT16_T w,
				     PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_draw_rectangle_solid(PICTURE_T *Pic, SINT16_T x0, SINT16_T y0, SINT16_T x1, SINT16_T y1, PIXEL_T color);

void        lb_gr_draw_triangle_fill_i(PICTURE_T *Pic, POINT_2D_SINT16_T P0, POINT_2D_SINT16_T P1, POINT_2D_SINT16_T P2,
				PIXEL_T color, COPYMODE_T copymode);

SINT8_T lb_gr_is_in_polygon_i(LINE_2D_SINT16_T *L, POINT_2D_SINT16_T P);
SINT8_T lb_gr_is_in_polygon_f(LINE_2D_REAL_T *L, POINT_2D_REAL_T P);
SINT8_T  lb_gr_matrix_gs_to_pic(MATRIX_R_T *A, PICTURE_T *Pic);

void       lb_gr_plot_continuous_fn_2d(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, FUNCTION_X fx_t, FUNCTION_X fy_t,
				       REAL_T t0, REAL_T t1, REAL_T delta,
				       SINT16_T max_exp, PIXEL_T color, COPYMODE_T copymode);
void       lb_gr_plot_continuous_fn_2d_antialiasing(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, FUNCTION_X fx_t, FUNCTION_X fy_t,
					      REAL_T t0, REAL_T t1, REAL_T delta,
						    SINT16_T max_exp, REAL_T w, PIXEL_T color, COPYMODE_T copymode);

void       lb_gr_plot2d(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, REAL_T xr, REAL_T yr, REAL_T w,
			PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode);
void       lb_gr_plot2d_line(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, LINE_2D_REAL_T *L, REAL_T w,
			     PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode);
void       lb_gr_plot2d_curve_neighbor(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, VECTOR_R_T *Lx, VECTOR_R_T *Ly, REAL_T w,
				     PIXEL_T color, COPYMODE_T copymode);
void       lb_gr_plot2d_curve_neighbor_slow(PICTURE_T *Pic, VIEWPORT_2D_T vp2d, VECTOR_R_T *Lx, VECTOR_R_T *Ly, REAL_T w,
					  PIXEL_T color, COPYMODE_T copymode);
void       lb_gr_plot3d(PICTURE_T *Pic, VIEWPORT_3D_T vp3d,  REAL_T Rot[3][3], POINT_3D_REAL_T Pr, REAL_T w,
			PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode);

void       lb_gr_plot3d_surface(PICTURE_T *Pic, VIEWPORT_3D_T vp3d,  REAL_T Rot[3][3], MATRIX_POINT_3D_REAL_T *S,
				REAL_T w, PIXEL_T color, COPYMODE_T copymode, LINEMODE_T linemode);

void       lb_gr_project_2d(VIEWPORT_2D_T vp2d, REAL_T xr, REAL_T yr, REAL_T *xp, REAL_T *yp);
void       lb_gr_project_2d_inv(VIEWPORT_2D_T vp2d, REAL_T xp, REAL_T yp, REAL_T *xr, REAL_T *yr);
void       lb_gr_project_2d_log(VIEWPORT_2D_T vp2d, REAL_T xr, REAL_T yr, REAL_T *xp, REAL_T *yp);
void       lb_gr_project_2d_x(VIEWPORT_2D_T vp2d, REAL_T xr, REAL_T *xp);
void       lb_gr_project_2d_x_log(VIEWPORT_2D_T vp2d, REAL_T xr, REAL_T *xp);
void       lb_gr_project_2d_y(VIEWPORT_2D_T vp2d, REAL_T yr, REAL_T *yp);
void       lb_gr_project_2d_y_log(VIEWPORT_2D_T vp2d, REAL_T yr, REAL_T *yp);

void       lb_gr_project_2d_vector_c_to_line_int(VIEWPORT_2D_T vp2d, VECTOR_C_T *V, LINE_2D_SINT16_T *L);
void       lb_gr_project_2d_vector_r_to_line_int(VIEWPORT_2D_T vp2d, VECTOR_R_T *X, VECTOR_R_T *Y, LINE_2D_SINT16_T *L);

void       lb_gr_project_3d(VIEWPORT_3D_T vp3d, REAL_T Rot[3][3], POINT_3D_REAL_T P, REAL_T *xr, REAL_T *yr, REAL_T *hz);


void       lb_gr_release_line2d_i(LINE_2D_SINT16_T *L);
void       lb_gr_release_line2d_f(LINE_2D_REAL_T *L);
void       lb_gr_release_line3d_f(LINE_3D_REAL_T *L);
void       lb_gr_release_picture(volatile PICTURE_T *Pic);
void lb_gr_render_picture(PICTURE_T *Pic, SINT16_T x, SINT16_T y, COPYMODE_T copymode, RENDERMODE_T rendermode);
PIXEL_T    lb_gr_value_to_color(SINT8_T value);



//void lb_gr_release_line3d(LINE_3D_REAL_T *L);
//void lb_gr_plot_line3d(VIEWPORT_3D_T vp3d, MATRIX_R_T *Rot, LINE_3D_REAL_T *L);
//void lb_gr_project_3d_to_2d_analytical(VIEWPORT_3D_T vp3d, MATRIX_R_T *Rot, VECTOR_R_T *Pa, UINT16_T *xp, UINT16_T *yp, UINT8_T *flag);
//void lb_gr_project_3d_to_2d_analytical_two_vectors(VIEWPORT_3D_T vp3d, VECTOR_R_T *V1, VECTOR_R_T *V2, VECTOR_R_T *Pa, UINT16_T *xp, UINT16_T *yp, UINT8_T *flag);

// void lb_gr_vp_putpixel(VIEWPORT_2D_T vp2d, REAL_T x, REAL_T y, int color);

#endif /* LB_GRAPHICS_H */
