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

/* Open a new graphics window. */
void lb_x11_open( int width, int height, const char *title );

/* Draw a point at (x,y) */
void lb_x11_point( int x, int y );

/* Draw a line from (x1,y1) to (x2,y2) */
void lb_x11_line( int x1, int y1, int x2, int y2 );

/* Change the current drawing color. */
void lb_x11_color( int red, int green, int blue );

/* Clear the graphics window to the background color. */
void lb_x11_clear();

/* Change the current background color. */
void lb_x11_clearcolor( int red, int green, int blue );

/* Wait for the user to press a key or mouse button. */
char lb_x11_wait();

/* Return the X and Y coordinates of the last event. */
int lb_x11_xpos();
int lb_x11_ypos();

/* Return the X and Y dimensions of the window. */
int lb_x11_xsize();
int lb_x11_ysize();

/* Check to see if an event is waiting. */
int lb_x11_event_waiting();

/* Flush all previous output to the window. */
void lb_x11_flush();

#endif

