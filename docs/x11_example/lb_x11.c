/*
  A simple graphics library for CSE 20211 by Douglas Thain
  For complete documentation, see:
  http://www.nd.edu/~dthain/courses/cse20211/fall2011/gfx
  Version 3, 11/07/2012 - Now much faster at changing colors rapidly.
  Version 2, 9/23/2011 - Fixes a bug that could result in jerky animation.
*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "lb_graphics.h"
#include "lb_x11.h"

/*
  gfx_open creates several X11 objects, and stores them in globals
  for use by the other functions in the library.
*/

static Display   *gfx_display=0;
static Window     gfx_window;
static GC         gfx_gc;
static Colormap   gfx_colormap;
static int        gfx_fast_color_mode = 0;

/* These values are saved by gfx_wait then retrieved later by gfx_xpos and gfx_ypos. */

static int saved_xpos = 0;
static int saved_ypos = 0;

/* Open a new graphics window. */

void lb_x11_open(S_INT_16_T width, S_INT_16_T height, S_INT_16_T scale_x, S_INT_16_T scale_y,
		 U_INT_8_T renderoptions, const char *title ) 
{
  if ( (width<=0) || (height<=0) || (scale_x<=0) || (scale_y<=0) )
    {
      printf("Error: b_x11_open() --> invalid parameters: w=%d, h=%d, scale_x=%d, scale_y=%d\r\n",
	     width, height, scale_x, scale_y);
      exit(1);
    }

  ty_width         = width;
  ty_height        = height;
  ty_scale_x       = scale_x;
  ty_scale_y       = scale_y;
  ty_renderoptions = renderoptions;
  
  Pic_shadow.w=width;
  Pic_shadow.h=height;
  lb_gr_create_picture(&Pic_shadow,lb_gr_12RGB(COLOR_BLACK));
    
  gfx_display = XOpenDisplay(0);
  if(!gfx_display)
    {
      fprintf(stderr,"gfx_open: unable to open the graphics window.\n");
      exit(1);
    }

  Visual *visual = DefaultVisual(gfx_display,0);
  if(visual && visual->class==TrueColor)
      gfx_fast_color_mode = 1;
  else
      gfx_fast_color_mode = 0;
  
  int blackColor = BlackPixel(gfx_display, DefaultScreen(gfx_display));
  int whiteColor = WhitePixel(gfx_display, DefaultScreen(gfx_display));

  gfx_window = XCreateSimpleWindow(gfx_display, DefaultRootWindow(gfx_display), 0, 0,
				   width*scale_x, height*scale_y, 0, blackColor, blackColor);

  XSetWindowAttributes attr;
  attr.backing_store = Always;

  XChangeWindowAttributes(gfx_display,gfx_window,CWBackingStore,&attr);
  XStoreName(gfx_display,gfx_window,title);
  XSelectInput(gfx_display, gfx_window, StructureNotifyMask|KeyPressMask|ButtonPressMask);
  XMapWindow(gfx_display,gfx_window);

  gfx_gc = XCreateGC(gfx_display, gfx_window, 0, 0);
  gfx_colormap = DefaultColormap(gfx_display,0);

  XSetForeground(gfx_display, gfx_gc, whiteColor);

  // Wait for the MapNotify event

  for(;;)
    {
      XEvent e;
      XNextEvent(gfx_display, &e);
      if (e.type == MapNotify)
	break;
    }
}

void lb_x11_close(void)
{
  lb_gr_release_picture(&Pic_shadow);
}


/* Draw a single point at (x,y) */

void lb_x11_point(S_INT_16_T x, S_INT_16_T y)
{
  XDrawPoint(gfx_display,gfx_window,gfx_gc,x,y);
}


/* Draw a line from (x1,y1) to (x2,y2) */

void lb_x11_line(S_INT_16_T x1, S_INT_16_T y1, S_INT_16_T x2, S_INT_16_T y2)
{
  XDrawLine(gfx_display,gfx_window,gfx_gc,x1,y1,x2,y2);
}

void lb_x11_rectangle(S_INT_16_T x, S_INT_16_T y, S_INT_16_T w, S_INT_16_T h)
{
  XFillRectangle(gfx_display,gfx_window,gfx_gc,x,y,w,h);
}

/* Change the current drawing color. */

void lb_x11_set_color(U_INT_8_T r, U_INT_8_T g, U_INT_8_T b )
{
  XColor color;

  /* If this is a truecolor display, we can just pick the color directly. */
  if(gfx_fast_color_mode)
    color.pixel = (r<<16)| (g<<8) | b;
  else
    {
      /* Otherwise, we have to allocate it from the colormap of the display. */
      color.pixel = 0;
      color.red = r<<8;
      color.green = g<<8;
      color.blue = b<<8;
      XAllocColor(gfx_display,gfx_colormap,&color);
    }
  XSetForeground(gfx_display, gfx_gc, color.pixel);
}

/* Clear the graphics window to the background color. */

void lb_x11_clear()
{
  XClearWindow(gfx_display,gfx_window);
}

/* Change the current background color. */

void lb_x11_set_color_background(U_INT_8_T r, U_INT_8_T g, U_INT_8_T b )
{
  XColor color;
  color.pixel = 0;
  color.red = r<<8;
  color.green = g<<8;
  color.blue = b<<8;
  XAllocColor(gfx_display,gfx_colormap,&color);

  XSetWindowAttributes attr;
  attr.background_pixel = color.pixel;
  XChangeWindowAttributes(gfx_display,gfx_window,CWBackPixel,&attr);
}

int lb_x11_event_waiting()
{
  XEvent event;

  lb_x11_flush();

  while (1)
    {
      if(XCheckMaskEvent(gfx_display,-1,&event))
	{
	  if(event.type==KeyPress)
	    {
	      XPutBackEvent(gfx_display,&event);
	      return 1;
	    }
	  else
	    if (event.type==ButtonPress)
	      {
		XPutBackEvent(gfx_display,&event);
		return 1;
	      }
	    else
	      return 0;
	}
      else
	return 0;
    }
}

#ifdef NADA
XK_Escape
    XK_F1
    XK_F2
    XK_F3
    XK_F4
    XK_F5
    XK_F6
    XK_F7
    XK_F8
    XK_F9
    XK_F10
    XK_F11
    XK_F12
    // printscreen
    XK_Scroll_Lock
    XK_Pause
    XK_Insert
    XK_Home
    XK_Page_Up
    XK_Delete
    XK_End
    XK_Page_Down
    XK_Up
    XK_Left
    XK_Right
    XK_Down
    XK_Shift_L
    XK_Shift_R
    XK_Control_L
    XK_Control_R
    XK_Alt_L
    XK_Menu */
   
/* Keypad */
    XK_Num_Lock
    XK_KP_Divide
    XK_KP_Multiply
    XK_KP_Subtract
 #endif   



/* Wait for the user to press a key or mouse button. */

/* Some reference codes
  0xffbe
XK_F2  0xffbf
XK_F3  0xffc0
XK_F4  0xffc1
XK_F5  0xffc2
XK_F6  0xffc3
XK_F7  0xffc4
XK_F8  0xffc5
XK_F9  0xffc6
XK_F10 0xffc7
XK_F11 0xffc8
XK_F12 0xffc9
*/

U_INT_16_T lb_x11_wait()
{
  XEvent event;
  int c, c2;

  lb_x11_flush();

  while(1)
    {
      XNextEvent(gfx_display,&event);
      //http://www.ccp4.ac.uk/dist/checkout/tcltk++_8.4.20.3/sources/tk8.4.20/win/tkWinKey.c
      if(event.type==KeyPress)
	{
	  saved_xpos = event.xkey.x;
	  saved_ypos = event.xkey.y;
	  
	  /* Keycode is sort of an internal value, it must be translated to a Keysim */
	  /* The old function has been deprecated */
	  //printf("keysim=0x%x = %dd\r\n", XKeycodeToKeysym(gfx_display, event.xkey.keycode, 0));

	
	  // printf("keycode=0x%x\r\n", event.xkey.keycode);
	  printf("keysim=0x%x \r\n", (unsigned int)XkbKeycodeToKeysym(gfx_display, event.xkey.keycode,0,
	     event.xkey.state & (ShiftMask | LockMask ) ? 1 : 0));
	   printf("Shift=%x\r\n",event.xkey.state & ShiftMask);
	   printf("Lock=%x\r\n", event.xkey.state & LockMask);
	   printf("Mod1=%x\r\n", event.xkey.state & Mod1Mask);
	   printf("Mod2=%x\r\n", event.xkey.state & Mod2Mask);
	   printf("Mod3=%x\r\n", event.xkey.state & Mod3Mask);
	   printf("Mod4=%x\r\n", event.xkey.state & Mod4Mask);
	   printf("Mod5=%x\r\n", event.xkey.state & Mod5Mask);
	
	  
	  c = XLookupKeysym(&event.xkey,0);
  	  printf("c=0x%x\r\n", c);

	  /* Alphanumeric */
	  if ( (XK_a<=c) && (c<=XK_z) )
	    {
	      if ((ShiftMask | LockMask) & event.xkey.state)
		return c-32;
	      else
		return c;
	    }
	  else
	    {
	      /* Keys which are independent of the condition of any "lock" keys */
	      switch (c)
		{
		case XK_Escape:
		  return PCKEY_ESC;
		  break;
		case XK_F1:
		  return PCKEY_F1;
		  break;
		case XK_F2:
		  return PCKEY_F2;
		  break;
		case XK_F3:
		  return PCKEY_F3;
		  break;
		case XK_F4:
		  return PCKEY_F4;
		  break;
		case XK_F5:
		  return PCKEY_F5;
		  break;
		case XK_F6:
		  return PCKEY_F6;
		  break;
		case XK_F7:
		  return PCKEY_F7;
		  break;
		case XK_F8:
		  return PCKEY_F8;
		  break;
		case XK_F9:
		  return PCKEY_F9;
		  break;
		case XK_F10:
		  return PCKEY_F10;
		  break;
		case XK_F11:
		  return PCKEY_F11;
		  break;
		case XK_F12:
		  return PCKEY_F12;
		  break;
		case XK_Scroll_Lock:
		  return PCKEY_SCROLL_LOCK;
		  break;
		case XK_Break:
		  return PCKEY_PAUSE;
		case XK_Tab:
		  return PCKEY_TAB;
		  break;
		case XK_Shift_L:
		case XK_Shift_R:
		  return PCKEY_SHIFT;
		  break;
		case XK_Control_L:
		case XK_Control_R:
		  return PCKEY_CONTROL;
		  break;
		case XK_Alt_L: /* Check in non-virtual-machine  */
		case XK_Alt_R:    
		case XK_ISO_Level3_Shift:
		  return PCKEY_ALT;
		  break;
		case  XK_Menu:
		  return 0;
		    break;
		case XK_Insert:
		  return PCKEY_INSERT;
		  break;
		case XK_Home:
		  return PCKEY_HOME;
		  break;
		case XK_Page_Up:
		  return PCKEY_PAGE_UP;
		  break;
		case XK_Page_Down:
		  return PCKEY_PAGE_DOWN;
		  break;
		case XK_Up:
		  return PCKEY_UP;
		  break;
		case XK_Down:
		  return PCKEY_DOWN;
		  break;
		case XK_Left:
		  return PCKEY_LEFT;
		  break;
		case XK_Right:
		  return PCKEY_RIGHT;
		  break;
		case XK_Num_Lock:
		  return PCKEY_NUM_LOCK;
		  break;
		}
	      
	      /* Keypad */
	      if ( (XK_KP_Space<=c) && (c<=XK_KP_9) )
		{
		  switch (c)
		    {
		    case XK_KP_Divide:
		      return ASCII_SLASH;
		      break;
		    case XK_KP_Multiply:
		      return ASCII_ASTERISK;
		      break;
		    case XK_KP_Subtract:
		      return ASCII_MINUS;
		      break;
		    case XK_KP_Add:
		      return ASCII_PLUS;
		      break;
		    case XK_KP_Enter:
		      return ASCII_LF;
		      break;
		    case XK_KP_Delete:
		      return ASCII_POINT;
		      break;
		    case XK_KP_Insert:
		      return ASCII_0;
		      break;
		    case XK_KP_End:
		      return ASCII_1;
		      break;
		    case XK_KP_Down:
		      return ASCII_2;
		      break;
		    case XK_KP_Page_Down:
		      return ASCII_3;
		      break;
		    case XK_KP_Left:
		      return ASCII_4;
		      break;
		    case XK_KP_Begin:
		      return ASCII_5;
		      break;
		    case XK_KP_Right:
		      return ASCII_6;
		      break;
		    case XK_KP_Home:
		      return ASCII_7;
		      break;
		    case XK_KP_Up:
		      return ASCII_8;
		      break;
		    case XK_KP_Page_Up:
		      return ASCII_9;
		      break;
		    }
		}
	      else
		{
		  c2 = XLookupKeysym(&event.xkey,ShiftMask & event.xkey.state);
		  /* Most codes for the Keysyms correspond to the ASCII ones, with a few exceptions: */
		  switch (c2)
		    {
		    case XK_dead_grave:
		      return '`';
		      break;
		    case XK_dead_tilde:
		      return '~';
		      break;
		    case XK_dead_circumflex:
		      return '^';
		      break;
		      //case XK_dead_acute:
		      //return '\'';
		      //break;
		      //case XK_dead_diaeresis:
		      //return '\"';
		      //break;
		    default:
		      return c2;
		      break;
		    }
		}
	    }
	}
      else
	if(event.type==ButtonPress)
	  {
	    saved_xpos = event.xkey.x;
	    saved_ypos = event.xkey.y;
	    return event.xbutton.button;
	  }
    }
}

/* Return the X and Y coordinates of the last event. */

S_INT_16_T lb_x11_xpos()
{
  return saved_xpos;
}

S_INT_16_T lb_x11_ypos()
{
  return saved_ypos;
}

/* Flush all previous output to the window. */

void lb_x11_flush()
{
  XFlush(gfx_display);
}

void lb_x11_sync()
{
  XSync(gfx_display,TRUE);
}



