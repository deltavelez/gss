/*
A simple graphics library for CSE 20211 by Douglas Thain
For course assignments, you should not change this file.
For complete documentation, see:
http://www.nd.edu/~dthain/courses/cse20211/fall2011/gfx
Version 3, 11/07/2012 - Now much faster at changing colors rapidly.
Version 2, 9/23/2011 - Fixes a bug that could result in jerky animation.
*/


#ifndef LB_X11_H
#define LB_X11_H

extern PICTURE_T Pic_shadow;

/* Open a new graphics window. */
void lb_x11_open(S_INT_16_T width, S_INT_16_T height, S_INT_16_T scale_x, S_INT_16_T scale_y, U_INT_8_T renderoptions, const char *title); 

void lb_x11_close(void);

/* Draw a point at (x,y) */
void lb_x11_point(S_INT_16_T x, S_INT_16_T y);

/* Draw a line from (x1,y1) to (x2,y2) */
void lb_x11_line(S_INT_16_T x1, S_INT_16_T y1, S_INT_16_T x2, S_INT_16_T y2 );

void lb_x11_rectangle(S_INT_16_T x, S_INT_16_T y, S_INT_16_T w, S_INT_16_T h);

/* Change the current drawing color. */
void lb_x11_set_color(U_INT_8_T r, U_INT_8_T g, U_INT_8_T b);

/* Clear the graphics window to the background color. */
void lb_x11_clear();

/* Change the current background color. */
void lb_x11_set_color_background(U_INT_8_T r, U_INT_8_T g, U_INT_8_T b);

/* Wait for the user to press a key or mouse button. */
U_INT_16_T lb_x11_wait();

/* Return the X and Y coordinates of the last event. */
S_INT_16_T lb_x11_xpos();
S_INT_16_T lb_x11_ypos();

/* Check to see if an event is waiting. */
int lb_x11_event_waiting();

/* Flush all previous output to the window. */
void lb_x11_flush();

void lb_x11_sync();

#endif

