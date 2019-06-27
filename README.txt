MINIMALISTIC COMPUTING PROJECT
------------------------------
Author:          Diego Velez
Thesis advisor:  Jorge Londoño
------------------------------
NOTES:
  As of June 20th, 2019, the project isn't yet functional and requires significant work. The files must be 
re-organized and cleaned up.
 
It can, however, be compiled and it may (possibly) show a colored screen related to the current work being 
done on the project.

HOW TO USE?
-----------
This project is focused to Linux and Raspberry Pi systems. Using it under Windows and Mac OSX should be 
possible with minor changes, but this is left to those who may be interested.  

The project has very few dependencies, namely SDL2 (Simple DirectMedia Layer),
which is a game-oriented library with no graphic primitives (other than a Pixels, Lines and Rectangles, which
are not used anyway).  What interests us is that it allows us to write programs for the X-window system with 
support for devices such as the keyboard, mouse, audio and joysticks.
Think of SDL as a "driver", solely used to present the final graphic result or to interact with the machine.

INSTALLATION
------------ 
In your Linux system, install the SDL2 libraries:

sudo apt-get install libsdl2-dev

If you wish to export any generated graphics to JPG files, install the JPG library:

sudo apt-get install libjpeg-dev

The recommended development system is emacs:

sudo apt-get install emacs.

That's it!   The *.c libraries themselves do not require an installation.  They just need to be in your path.

The default "main" program is called "graf.c".  A good way to learn to use the libaries is just to explore
the demos contained there and modify them.  Each one is "enabled" (so to say) by preprocessor directives:

//#define DEMO_LINE
#ifdef DEMO_LINE
{ demo code here }
#endif

COPYRIGHT
---------
This software is, until being released, property of both Universidad Pontificia Bolivariana and Diego Velez.

