demos: demos.c lb_algebra.c lb_algebra.h lb_audio.h lb_audio.c lb_complex.c lb_complex.h lb_console.c lb_console.h lb_fonts.c lb_fonts.h lb_fourier.c lb_fourier.h lb_geometry.c lb_geometry.h lb_graphics.c lb_graphics.h lb_gpio.h lb_gpio.c lb_hp6600.c lb_hp6600.h lb_integer.c lb_integer.h lb_numer.c lb_numer.h lb_parser.c lb_parser.h lb_real.c lb_real.h lb_serial.c lb_serial.h lb_statistics.c lb_statistics.h lb_time.c lb_time.h lb_types.h 

	gcc -o demos demos.c lb_audio.c lb_algebra.c lb_complex.c lb_console.c lb_fonts.c lb_fourier.c lb_geometry.c lb_gpio.c lb_graphics.c lb_hp6600.c lb_hp6600.h lb_integer.c lb_numer.c lb_parser.c lb_real.c lb_serial.c lb_statistics.c lb_time.c -lm -D_REENTRANT -ljpeg -lpthread -Wall -I/usr/include -lSDL2 -O0


clean:
	rm *.*~*
	rm *~

# add -lSDL2main if there are compilation/linking errors.  It was removed to try out if it was necessary.
