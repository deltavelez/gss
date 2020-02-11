MINIMALISTIC COMPUTING PROJECT
------------------------------
Author:          Diego Velez
Thesis advisor:  Jorge Londoño
------------------------------

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
First, its is generally suggested to update the system:
sudo apt-get update

In your Linux system, install the SDL2 libraries:

sudo apt-get install libsdl2-dev

If you wish to export any generated graphics to JPG files, install the JPG library:

sudo apt-get install libjpeg-dev

The recommended development system is emacs:

sudo apt-get install emacs.

That's it!   The *.c libraries themselves do not require an installation.  They just need to be in your path.

The default "main" program is called "demos.c".  A good way to learn to use the libaries is just to explore
the demos contained there and modify them.  Each one can be enabled through preprocessor directives:

//#define DEMO_LINE
#ifdef DEMO_LINE
{ demo code here }
#endif

COPYRIGHT (2020)
----------------
This software is property of both Universidad Pontificia Bolivariana and Diego Velez.

