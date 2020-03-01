#ifndef LB_TYPES_H
#define LB_TYPES_H

/******************************************************/
/*Formatting constants */
/******************************************************/

#define FLOAT_FORMAT "%0.8f"
#define FLOAT_FORMAT_MATRIX "%9.5f"
//#define FLOAT_FORMAT "%04.8lf"

/******************************************************/
/* Boolean data types */
/******************************************************/
#define TRUE 1
#define FALSE 0

/******************************************************/
/* Basic data types */
/******************************************************/

#define REAL_T double
#define SINT8_T char
#define UINT8_T unsigned char
#define SINT16_T short
#define UINT16_T unsigned short
#define SINT32_T long
#define UINT32_T unsigned int
#define SINT64_T long long
#define UINT64_T unsigned long long

/******************************************************/
/* Math data types */
/******************************************************/

typedef enum { e_none=0, e_div_zero, e_too_big, e_complex, e_undefined, e_arg_smaller_one, e_arg_larger_one, e_arg_negative, e_arg_non_int, e_arg_non_even, e_arg_non_odd, e_arg_less_two, e_arg_too_big, e_arg_too_small, e_arg_inf_loop, e_arg_non_real, e_arg_not_var } MATHERROR_T;

/******************************************************/
/* Complex variable data types */
/******************************************************/

typedef struct 
{
  REAL_T r, i;
} COMPLEX_T;

#define ZERO_C (COMPLEX_T){0.0, 0.0}

/******************************************************/
/* Linear Algebra constants */
/******************************************************/
#define ARRAY_MAX_DIM      5
#define ARRAY_MAX_N        65535

#define MATRIX_MAX_ROWS  65535
#define MATRIX_MAX_COLS  65535
#define VECTOR_MAX_ITEMS 65535

/******************************************************/
/* Linear Algebra data types */
/******************************************************/

typedef struct
{
  SINT8_T *array;
  UINT16_T items;
} VECTOR_SINT8_T;

typedef struct
{
  SINT16_T *array;
  UINT16_T items;
} VECTOR_SINT16_T;

typedef struct
{
  REAL_T *array;
  UINT16_T items;
} VECTOR_R_T;

typedef struct
{
  COMPLEX_T *array;
  UINT16_T items;
} VECTOR_C_T;

typedef struct
{
  SINT8_T **array;
  UINT16_T rows;
  UINT16_T cols;
} MATRIX_SINT8_T;

typedef struct
{
  REAL_T **array;
  UINT16_T rows;
  UINT16_T cols;
} MATRIX_R_T;

typedef struct
{
  COMPLEX_T **array;
  UINT16_T rows;
  UINT16_T cols;
} MATRIX_C_T;

typedef struct
{
  SINT8_T n;
  UINT16_T dim[5];
  REAL_T *a1;
  REAL_T **a2;
  REAL_T ***a3;
  REAL_T ****a4;
  REAL_T *****a5;
} ARRAY_R_T;


typedef REAL_T(*FUNCTION_X_Y)(REAL_T, REAL_T, MATHERROR_T*);
typedef REAL_T(*FUNCTION_X)(REAL_T, MATHERROR_T*);

/******************************************************/
/* Computer Graphics constants */
/******************************************************/
#define PICTURE_MAX_HEIGHT 65535
#define PICTURE_MAX_WIDTH 65535

#define N_BITS_R 8            /* Number of bits for red channel for PICTURE_T */
#define N_BITS_G 8            /* Number of bits for green channel for PICTURE_T */
#define N_BITS_B 8            /* Number of bits for blue channel for PICTURE_T */
#define N_BITS_A 8            /* Number of bits for k-channel for PICTURE_T */


#if N_BITS_R == 1
#define MAX_R 1               /* Maximum value: 2^n_bits_r - 1*/
#define FACTOR_N_TO_8_R 255   /* 255/MAX_R */
#endif

#if N_BITS_R == 2
#define MAX_R 3             
#define FACTOR_N_TO_8_R (255/3) 
#endif

#if N_BITS_R == 3
#define MAX_R 7             
#define FACTOR_N_TO_8_R (255/7)
#endif

#if N_BITS_R == 4
#define MAX_R 15             
#define FACTOR_N_TO_8_R (255/15)
#endif

#if N_BITS_R == 5
#define MAX_R 31             
#define FACTOR_N_TO_8_R (255/31) 
#endif

#if N_BITS_R == 6
#define MAX_R 63             
#define FACTOR_N_TO_8_R (255/63) 
#endif

#if N_BITS_R == 7
#define MAX_R 127             
#define FACTOR_N_TO_8_R (255/127) 
#endif

#if N_BITS_R == 8
#define MAX_R 255             
#define FACTOR_N_TO_8_R 1 
#endif

/*************/

#if N_BITS_G == 1
#define MAX_G 1               /* Maximum value: 2^n_bits_g - 1*/
#define FACTOR_N_TO_8_G 255   /* 255/MAX_G */
#endif

#if N_BITS_G == 2
#define MAX_G 3             
#define FACTOR_N_TO_8_G (255/3)  
#endif

#if N_BITS_G == 3
#define MAX_G 7             
#define FACTOR_N_TO_8_G (255/7)
#endif

#if N_BITS_G == 4
#define MAX_G 15             
#define FACTOR_N_TO_8_G (255/15)
#endif

#if N_BITS_G == 5
#define MAX_G 31             
#define FACTOR_N_TO_8_G (255/31)
#endif

#if N_BITS_G == 6
#define MAX_G 63             
#define FACTOR_N_TO_8_G (255/63)
#endif

#if N_BITS_G == 7
#define MAX_G 127             
#define FACTOR_N_TO_8_G (255/127)
#endif

#if N_BITS_G == 8
#define MAX_G 255             
#define FACTOR_N_TO_8_G 1
#endif


/*************/

#if N_BITS_B == 1
#define MAX_B 1               /* Maximum value: 2^n_bits_b - 1*/
#define FACTOR_N_TO_8_B 255   /* 255/MAX_B */
#endif

#if N_BITS_B == 2
#define MAX_B 3             
#define FACTOR_N_TO_8_B (255/3)  
#endif

#if N_BITS_B == 3
#define MAX_B 7             
#define FACTOR_N_TO_8_B (255/7)
#endif

#if N_BITS_B == 4
#define MAX_B 15             
#define FACTOR_N_TO_8_B (255/15)
#endif

#if N_BITS_B == 5
#define MAX_B 31             
#define FACTOR_N_TO_8_B (255/31)
#endif

#if N_BITS_B == 6
#define MAX_B 63             
#define FACTOR_N_TO_8_B (255/63)
#endif

#if N_BITS_B == 7
#define MAX_B 127             
#define FACTOR_N_TO_8_B (255/127)
#endif

#if N_BITS_B == 8
#define MAX_B 255             
#define FACTOR_N_TO_8_B 1
#endif

/*************/

#if N_BITS_A == 1
#define MAX_A 1               /* Maximum value: 2^n_bits_k - 1*/
#define FACTOR_N_TO_8_A 255   /* 255/MAX_A */
#endif

#if N_BITS_A == 2
#define MAX_A 3             
#define FACTOR_N_TO_8_A (255/3)  
#endif

#if N_BITS_A == 3
#define MAX_A 7             
#define FACTOR_N_TO_8_A (255/7)
#endif

#if N_BITS_A == 4
#define MAX_A 15             
#define FACTOR_N_TO_8_A (255/15)
#endif

#if N_BITS_A == 5
#define MAX_A 31             
#define FACTOR_N_TO_8_A (255/31)
#endif

#if N_BITS_A == 6
#define MAX_A 63             
#define FACTOR_N_TO_8_A (255/63)
#endif

#if N_BITS_A == 7
#define MAX_A 127             
#define FACTOR_N_TO_8_A (255/127)
#endif
  
#if N_BITS_A == 8
#define MAX_A 255             
#define FACTOR_N_TO_8_A 1
#endif
  
/* Examples: 
   for 2 bits:  (2, 3,  85.00) 
   for 3 bits:  (3, 7,  36.42) 
   for 4 bits:  (4, 15, 17.00) 
   for 5 bits:  (5, 31, 8.22) 
   for 6 bits:  (6, 63, 4.04) 
   for 7 bits:  (7, 127, 2.00) 
   for 8 bits:  (8, 255, 1.00) 
*/


/* 32 most basic color definitions for the 12-bit color mode 
#define PIXEL_BLACK       (PIXEL_T) { 0x0, 0x0, 0x0, 0xF } */

/* Transparency Attributes */
#define COLOR_SOLID                 0xF000 
#define COLOR_SEMI_SOLID            0x7000 
#define COLOR_TRANSPARENT           0x0FFF 

/* Common colors */
#define COLOR_RED	            0x00F
#define COLOR_DARKRED	            0x008
#define COLOR_ORANGE	            0x0AF
#define COLOR_DARKORANGE	    0x08F
#define COLOR_GOLD	            0x0DF

#define COLOR_LIME	            0x0F0
#define COLOR_GREEN	            0x080
#define COLOR_OLIVE	            0x088
#define COLOR_LIGHTGREEN	    0x8E8
#define COLOR_LIMEGREEN	            0x3C3
#define COLOR_DARKGREEN	            0x060
#define COLOR_SPRINGGREEN	    0x7F0

#define COLOR_BLUE	            0xF00
#define COLOR_LIGHTBLUE	            0xEDA
#define COLOR_SKYBLUE	            0xEC8
#define COLOR_DARKBLUE	            0x800

#define COLOR_YELLOW	            0x0FF
#define COLOR_LIGHTYELLOW	    0xDFF

#define COLOR_CYAN	            0xFF0
#define COLOR_TEAL	            0x880
#define COLOR_LIGHTCYAN	            0xFFD

#define COLOR_MAGENTA	            0xF0F
#define COLOR_PURPLE	            0x808

/* Other common colors */
#define COLOR_PINK	            0xCBF
#define COLOR_LIGHTPINK	            0xBBF

#define COLOR_BROWN	            0x22A
#define COLOR_KHAKI	            0x8EE
#define COLOR_BEIGE	            0xDEE
#define COLOR_VIOLET	            0xE8E
#define COLOR_BLUEVIOLET	    0xD38

/* Gray scale */
#define COLOR_BLACK	            0x000
#define COLOR_DIMGRAY	            0x666
#define COLOR_GRAY	            0x888
#define COLOR_DARKGRAY	            0xAAA
#define COLOR_SILVER	            0xBBB
#define COLOR_LIGHTGRAY	            0xCCC
#define COLOR_GAINSBORO	            0xDDD
#define COLOR_WHITESMOKE	    0xEEE
#define COLOR_WHITE	            0xFFF

/* Many other "designer's" colors */
#define COLOR_HOTPINK	            0xB6F
#define COLOR_DEEPPINK	            0x91F
#define COLOR_PALEVIOLETRED	    0x97D
#define COLOR_MEDIUMVIOLETRED	    0x81C
#define COLOR_LIGHTSALMON	    0x79F
#define COLOR_SALMON	            0x78F
#define COLOR_DARKSALMON	    0x79E
#define COLOR_LIGHTCORAL	    0x88E
#define COLOR_INDIANRED	            0x55C
#define COLOR_CRIMSON	            0x41D
#define COLOR_ORANGERED	            0x04F
#define COLOR_TOMATO	            0x46F
#define COLOR_CORAL	            0x57F
#define COLOR_LEMONCHIFFON	    0xCFF
#define COLOR_LIGHTGOLDENRODYELLOW  0xCFF
#define COLOR_MOCCASIN	            0xBDF
#define COLOR_PALEGOLDENROD	    0xAEE
#define COLOR_DARKKHAKI	            0x6BB
#define COLOR_CORNSILK	            0xDFF
#define COLOR_BLANCHEDALMOND	    0xCEF
#define COLOR_BISQUE	            0xCDF
#define COLOR_NAVAJOWHITE	    0xADF
#define COLOR_WHEAT	            0xBDE
#define COLOR_BURLYWOOD	            0x8BD
#define COLOR_TAN	            0x8BC
#define COLOR_ROSYBROWN	            0x88B
#define COLOR_SANDYBROWN	    0x6AE
#define COLOR_GOLDENROD	            0x2AD
#define COLOR_DARKGOLDENROD	    0x18B
#define COLOR_PERU	            0x48C
#define COLOR_CHOCOLATE	            0x26C
#define COLOR_SADDLEBROWN	    0x148
#define COLOR_SIENNA	            0x359
#define COLOR_DARKOLIVEGREEN	    0x365
#define COLOR_OLIVEDRAB	            0x286
#define COLOR_YELLOWGREEN	    0x3C9
#define COLOR_LAWNGREEN	            0x0F7
#define COLOR_CHARTREUSE	    0x0F7
#define COLOR_GREENYELLOW	    0x3FA
#define COLOR_MEDIUMSPRINGGREEN	    0x9F0
#define COLOR_PALEGREEN	            0x9F9
#define COLOR_DARKSEAGREEN	    0x8B8
#define COLOR_MEDIUMSEAGREEN	    0x7B4
#define COLOR_SEAGREEN	            0x583
#define COLOR_FORESTGREEN	    0x282
#define COLOR_MEDIUMAQUAMARINE	    0xAC6
#define COLOR_PALETURQUOISE	    0xEEA
#define COLOR_AQUAMARINE	    0xCF7
#define COLOR_TURQUOISE	            0xCD4
#define COLOR_MEDIUMTURQUOISE	    0xCC4
#define COLOR_DARKTURQUOISE	    0xCC0
#define COLOR_LIGHTSEAGREEN	    0xAA2
#define COLOR_CADETBLUE	            0x996
#define COLOR_LIGHTSTEELBLUE	    0xDCA
#define COLOR_LIGHTSKYBLUE	    0xFC8
#define COLOR_DEEPSKYBLUE	    0xFB0
#define COLOR_DODGERBLUE	    0xF82
#define COLOR_CORNFLOWERBLUE	    0xE96
#define COLOR_STEELBLUE	            0xB84
#define COLOR_ROYALBLUE	            0x810
#define COLOR_MEDIUMBLUE	    0xC00
#define COLOR_MIDNIGHTBLUE	    0x711
#define COLOR_LAVENDER	            0xFEE
#define COLOR_THISTLE	            0xDBD
#define COLOR_PLUM	            0xD9D
#define COLOR_ORCHID	            0xD7D
#define COLOR_MEDIUMORCHID	    0xC5B
#define COLOR_MEDIUMPURPLE	    0xD79
#define COLOR_DARKORCHID	    0xC39
#define COLOR_INDIGO	            0x804
#define COLOR_DARKSLATEBLUE	    0x844
#define COLOR_REBECCAPURPLE	    0x936
#define COLOR_SLATEBLUE	            0xC56
#define COLOR_MEDIUMSLATEBLUE	    0xE67
#define COLOR_HONEYDEW	            0xEFE
#define COLOR_AZURE	            0xFFE
#define COLOR_SEASHELL	            0xEEF
#define COLOR_FLORALWHITE	    0xEFF
#define COLOR_IVORY	            0xEFF
#define COLOR_ANTIQUEWHITE	    0xDEF
#define COLOR_MISTYROSE	            0xDDF
#define COLOR_LIGHTSLATEGRAY	    0x987
#define COLOR_SLATEGRAY	            0x887
#define COLOR_DARKSLATEGRAY	    0x553

/* Redundant colors */
#define COLOR_MAROON	            0x008
#define COLOR_NAVY	            0x800
#define COLOR_AQUA	            0xFF0
#define COLOR_FUCHSIA	            0xF0F
#define COLOR_DARKMAGENTA	    0x808
#define COLOR_DARKCYAN	            0x880
#define COLOR_DARKVIOLET	    0xC09
#define COLOR_GHOSTWHITE	    0xFFF
#define COLOR_BROWN	            0x22A
#define COLOR_POWDERBLUE	    0xEDA
#define COLOR_SNOW	            0xFFF
#define COLOR_OLDLACE	            0xEEF
#define COLOR_PEACHPUFF	            0xBDF
#define COLOR_LINEN	            0xEEF
#define COLOR_LAVENDERBLUSH	    0xEEF
#define COLOR_MINTCREAM	            0xFFE
#define COLOR_ALICEBLUE	            0xFFE
#define COLOR_PAPAYAWHIP	    0xDEF


/******************************************************/
/* Computer Graphics data types */
/******************************************************/
#define LINE_MAX_ITEMS 65535


typedef enum { COPYMODE_FRAMEBUFFER=0,
	       COPYMODE_COPY,
	       COPYMODE_AND,
	       COPYMODE_OR,
	       COPYMODE_XOR,
	       COPYMODE_NOT_SRC,
	       COPYMODE_NOT_DST,
	       COPYMODE_ADD,
	       COPYMODE_SRC_MINUS_DST,
	       COPYMODE_DST_MINUS_SRC,
	       COPYMODE_AVG,
	       COPYMODE_BLEND,
	       COPYMODE_MULTIPLY,
	       COPYMODE_SRC_DIV_DST,
	       COPYMODE_DST_DIV_SRC,
	       COPYMODE_PORTERDUFF_CLEAR_DST,
	       COPYMODE_PORTERDUFF_COPY_SRC,
	       COPYMODE_PORTERDUFF_LEAVES_DST,
	       COPYMODE_PORTERDUFF_SRC_OVER_DST,
	       COPYMODE_PORTERDUFF_DST_OVER_SRC,
	       COPYMODE_PORTERDUFF_SRC_IN_DST,
	       COPYMODE_PORTERDUFF_DST_IN_SRC,
	       COPYMODE_PORTERDUFF_SRC_OUT_DST,
	       COPYMODE_PORTERDUFF_DST_OUT_SRC,
	       COPYMODE_PORTERDUFF_SRC_ATOP_DST,
	       COPYMODE_PORTERDUFF_DST_ATOP_SRC,
	       COPYMODE_PORTERDUFF_XOR } COPYMODE_T; 


#define RENDERMODE_T UINT32_T

/* This section deals with the attributes passed through the "copymode" parameter. They're listed from lower to higher order. 
   For example: value = (value >> THAT_VALUE_SHIFT ) && THAT_VALUE_MASK */
#define RENDERMODE_PIXELBG_R_MASK    0b00000000000000000000000000001111
#define RENDERMODE_PIXELBG_G_MASK    0b00000000000000000000000011110000
#define RENDERMODE_PIXELBG_B_MASK    0b00000000000000000000111100000000
#define RENDERMODE_PIXELMODE_MASK    0b00000000000000001111000000000000
#define RENDERMODE_SCALE_X_MASK      0b00000000001111110000000000000000
#define RENDERMODE_SCALE_Y_MASK      0b00001111110000000000000000000000

#define RENDERMODE_PIXELMODE_0       0b00000000000000000000000000000000
#define RENDERMODE_PIXELMODE_1       0b00000000000000000001000000000000
#define RENDERMODE_PIXELMODE_2       0b00000000000000000010000000000000
#define RENDERMODE_PIXELMODE_3       0b00000000000000000011000000000000

#define RENDERMODE_PIXELMODE_SHIFT      12 
#define RENDERMODE_PIXELBG_R_SHIFT       0
#define RENDERMODE_PIXELBG_G_SHIFT       4
#define RENDERMODE_PIXELBG_B_SHIFT       8
#define RENDERMODE_SCALE_X_SHIFT         16
#define RENDERMODE_SCALE_Y_SHIFT         22

/* These macros help the user setting an arbitrary "pixel size" */
#define RENDERMODE_SCALE_X(X) (((X-1)<<RENDERMODE_SCALE_X_SHIFT) & RENDERMODE_SCALE_X_MASK)
#define RENDERMODE_SCALE_Y(X) (((X-1)<<RENDERMODE_SCALE_Y_SHIFT) & RENDERMODE_SCALE_Y_MASK)
#define RENDERMODE_BGCOLOR(X) (X & (RENDERMODE_PIXELBG_R_MASK | RENDERMODE_PIXELBG_G_MASK | RENDERMODE_PIXELBG_B_MASK) )


typedef enum
  { 
    LINEMODE_DOTS_SOLID, 
    LINEMODE_DOTS_FILTERED,
    LINEMODE_SOLID, /* Draws solid pixels */
    LINEMODE_FILTERED   /* Uses anti-aliasing */
  } LINEMODE_T;
   

typedef struct
{
  SINT16_T x;
  SINT16_T y;
  SINT8_T flag;
} POINT_2D_SINT16_T;

typedef struct
{
  REAL_T x;
  REAL_T y;
  SINT8_T flag;
} POINT_2D_REAL_T;

typedef struct
{
  REAL_T x;
  REAL_T y;
  REAL_T z;
  SINT8_T flag;
} POINT_3D_REAL_T;

typedef struct
{
  POINT_2D_SINT16_T *array;
  UINT16_T items;
} LINE_2D_SINT16_T;

typedef struct
{
  POINT_2D_REAL_T *array;
  UINT16_T items;
} LINE_2D_REAL_T;

typedef struct
{
  POINT_3D_REAL_T *array;
  UINT16_T items;
} LINE_3D_REAL_T;

typedef struct
{
  POINT_2D_REAL_T **array;
  UINT16_T rows;
  UINT16_T cols;
} MATRIX_POINT_2D_REAL_T;

typedef struct
{
  POINT_3D_REAL_T **array;
  UINT16_T rows;
  UINT16_T cols;
} MATRIX_POINT_3D_REAL_T;

typedef struct 
  {
    SINT16_T xp_min;
    SINT16_T yp_min;
    SINT16_T xp_max;
    SINT16_T yp_max;
    REAL_T xr_min;
    REAL_T xr_max;
    REAL_T yr_min;
    REAL_T yr_max;
  } VIEWPORT_2D_T;

typedef struct 
  {
    SINT16_T xp_min;
    SINT16_T yp_min;
    SINT16_T xp_max;
    SINT16_T yp_max;
    
    REAL_T scale;    /* Zoom */
    REAL_T cam_d;    /* Stereoscopic */
    REAL_T cam_h;    /* Depth */
    POINT_3D_REAL_T cam;   /* Camera's location */
} VIEWPORT_3D_T;

typedef struct 
  {
    UINT8_T r:N_BITS_R;
    UINT8_T g:N_BITS_G;
    UINT8_T b:N_BITS_B;
    UINT8_T a:N_BITS_A;
  } PIXEL_T;


typedef struct
{
  PIXEL_T **pic;
  UINT16_T w;
  UINT16_T h;
} PICTURE_T;

typedef struct
{
  char *dat;
  UINT16_T w;
  UINT16_T h;
} SCREEN_T;

/******************************************************/
/* Fonts data types */
/******************************************************/

typedef struct
{
  UINT8_T cols;
  UINT8_T rows;
  UINT8_T range_a;
  UINT8_T range_b;
  unsigned char* data;
  UINT8_T scale_x;
  UINT8_T scale_y;
  UINT8_T gap_x;
  UINT8_T gap_y;
  UINT8_T max_x;
  SINT16_T angle;
  UINT8_T flag_fg;
  UINT8_T flag_bg;
  PIXEL_T color_fg;
  PIXEL_T color_bg;
} FONT_T;

typedef struct
{
  char c;
  PIXEL_T color_fg;
  PIXEL_T color_bg;
} CONSOLE_CHAR_T;

typedef struct
{
  /* 2D circular buffer */
  CONSOLE_CHAR_T **screen; /* 2D circular buffer for visualization, scrollable */
  SINT16_T MAX_ROWS;     /* Maximum (total) number of rows stored in memory */  
  SINT16_T MAX_ROWS_V;   /* Visible number of rows*/
  SINT16_T MAX_COLS;
  /*  SINT16_T n_row;       Number of "rows" which have been received*/
  SINT16_T a_row;        /* Index of the oldest row that was written */
  SINT16_T b_row;        /* Index of the row of the next cell to be written */
  SINT16_T w_col;        /* Index of the column of the next cell to be written */
  SINT16_T s_row;        /* Index of the row of the next cell to be "seen" (cursor) */
  SINT16_T s_col;        /* Index of the column of the next cell to be "seen" (cursor) */
  SINT16_T n_back;       /* Number of rows "backwards" counted from the next cell to be written */
  SINT16_T n_back_adj;   /* Adjusted number of rows "backwards", to add hysteresis */
  PIXEL_T    color_fg;     /* Current foreground color */
  PIXEL_T    color_bg;     /* Current background color */
} CONSOLE_T;


#define AXIS_DRAW_X               0b0000000000000001
#define AXIS_DRAW_X_ARROWS        0b0000000000000010
#define AXIS_DRAW_X_GRID          0b0000000000000100
#define AXIS_DRAW_X_GRID_LOG      0b0000000000001000
#define AXIS_DRAW_X_LABEL         0b0000000000010000
#define AXIS_DRAW_Y               0b0000000000100000
#define AXIS_DRAW_Y_ARROWS        0b0000000001000000
#define AXIS_DRAW_Y_GRID          0b0000000010000000
#define AXIS_DRAW_Y_GRID_LOG      0b0000000100000000
#define AXIS_DRAW_Z_GRID          0b0000001000000000

#define AXIS_DRAW_COLORVALUES_X_1 0b0000010000000000 /* Degradé */
#define AXIS_DRAW_COLORVALUES_X_2 0b0000100000000000 /* Color code */
#define AXIS_DRAW_COLORVALUES_Y_1 0b0001000000000000 /* Degradé */
#define AXIS_DRAW_COLORVALUES_Y_2 0b0010000000000000 /* Color code */

#define AXIS_POLAR_R_GRID     0b00000001
#define AXIS_POLAR_T_GRID     0b00000010



/******************************************************/
/* Global Variables */
/******************************************************/

SCREEN_T ty_screen;
SINT8_T  ty_SDL_initialized;

#endif /* LB_TYPES_H */

/******************************************************/
/* ASCII table */
/******************************************************/

#define ASCII_NUL              0x00      /* NULL */ 
#define ASCII_SOH              0x01      /* Start of heading */ 
#define ASCII_STX              0x02      /* Start of text */ 
#define ASCII_ETX              0x03      /* End of text */
#define ASCII_EOT              0x04      /* End of transmission */
#define ASCII_ENQ              0x05      /* Enquiry */
#define ASCII_ACK              0x06      /* Acknowledge */ 
#define ASCII_BEL              0x07      /* Bell */
#define ASCII_BS               0x08      /* Backspace */
#define ASCII_TAB              0x09      /* Horizontal tab */
#define ASCII_LF               0x0A      /* New line feed, new line */
#define ASCII_VT               0x0B      /* Vertical tab */
#define ASCII_FF               0x0C      /* New page form feed, new page */
#define ASCII_CR               0x0D      /* Carriage return */
#define ASCII_SO               0x0E      /* Shift out */
#define ASCII_SI               0x0F      /* Shift in */

#define ASCII_DLE              0x10      /* Data link escape */ 
#define ASCII_DC1              0x11      /* Device control 1 */
#define ASCII_DC2              0x12      /* Device control 2 */
#define ASCII_DC3              0x13      /* Device control 3 */
#define ASCII_DC4              0x14      /* Device control 4 */
#define ASCII_NAK              0x15      /* Negative acknowdledge */
#define ASCII_SYN              0x16      /* Synchronous idle */
#define ASCII_ETB              0x17      /* Endof transmission block */
#define ASCII_CAN              0x18      /* Cancel */ 
#define ASCII_EM               0x19      /* End of Medium */
#define ASCII_SUB              0x1A      /* Substitute */
#define ASCII_ESC              0x1B      /* Escape */
#define ASCII_FS               0x1C      /* File separator */
#define ASCII_GS               0x1D      /* Group separator */
#define ASCII_RS               0x1E      /* Record separator */ 
#define ASCII_US               0x1F      /* Unit separator */

#define ASCII_SPACE            0x20      /*   */
#define ASCII_EXCLAMATION      0x21      /* ! */
#define ASCII_QUOTATION        0x22      /* " */
#define ASCII_NUMBER           0x23      /* # */
#define ASCII_DOLLAR           0x24      /* $ */
#define ASCII_PERCENT          0x25      /* % */
#define ASCII_AMPERSAND        0x26      /* & */
#define ASCII_APOSTOPHE        0x27      /* ' */
#define ASCII_PARENTHESES_L    0x28      /* ( */
#define ASCII_PARENTHESES_R    0x29      /* ) */
#define ASCII_ASTERISK         0x2A      /* * */
#define ASCII_PLUS             0x2B      /* + */
#define ASCII_COMMA            0x2C      /* , */
#define ASCII_MINUS            0x2D      /* - */
#define ASCII_POINT            0x2E      /* .  Formal name is full stop */
#define ASCII_SLASH            0x2F      /* / */

#define ASCII_0                0x30  
#define ASCII_1                0x31 
#define ASCII_2                0x32 
#define ASCII_3                0x33
#define ASCII_4                0x34
#define ASCII_5                0x35
#define ASCII_6                0x36
#define ASCII_7                0x37
#define ASCII_8                0x38
#define ASCII_9                0x39
#define ASCII_COLON            0x3A     /* : */
#define ASCII_SEMICOLON        0x3B     /* ; */
#define ASCII_LESS_THAN        0x3C     /* < */
#define ASCII_EQUALS           0x3D     /* = */
#define ASCII_GREATER_THAN     0x3E     /* > */
#define ASCII_QUESTION         0x3F     /* ? */

#define ASCII_AT               0x40     /* @ */ 
#define ASCII_A                0x41 
#define ASCII_B                0x42 
#define ASCII_C                0x43
#define ASCII_D                0x44
#define ASCII_E                0x45
#define ASCII_F                0x46
#define ASCII_G                0x47
#define ASCII_H                0x48
#define ASCII_I                0x49
#define ASCII_J                0x4A
#define ASCII_K                0x4B
#define ASCII_L                0x4C
#define ASCII_M                0x4D
#define ASCII_N                0x4E
#define ASCII_O                0x4F

#define ASCII_P                0x50
#define ASCII_Q                0x51 
#define ASCII_R                0x52 
#define ASCII_S                0x53
#define ASCII_T                0x54
#define ASCII_U                0x55
#define ASCII_V                0x56
#define ASCII_W                0x57
#define ASCII_X                0x58
#define ASCII_Y                0x59
#define ASCII_Z                0x5A
#define ASCII_SQUARE_BRACKET_L 0x5B
#define ASCII_BACKSLASH        0x5C
#define ASCII_SQUARE_BRACKET_R 0x5D
#define ASCII_CARET            0x5E
#define ASCII_UNDERSCORE       0x5F

#define ASCII_GRAVE_ACCENT     0x60     /* ` */ 
#define ASCII_a                0x61 
#define ASCII_b                0x62 
#define ASCII_c                0x63
#define ASCII_d                0x64
#define ASCII_e                0x65
#define ASCII_f                0x66
#define ASCII_g                0x67
#define ASCII_h                0x68
#define ASCII_i                0x69
#define ASCII_j                0x6A
#define ASCII_k                0x6B
#define ASCII_l                0x6C
#define ASCII_m                0x6D
#define ASCII_n                0x6E
#define ASCII_o                0x6F

#define ASCII_p                0x70
#define ASCII_q                0x71 
#define ASCII_r                0x72 
#define ASCII_s                0x73
#define ASCII_t                0x74
#define ASCII_u                0x75
#define ASCII_v                0x76
#define ASCII_w                0x77
#define ASCII_x                0x78
#define ASCII_y                0x79
#define ASCII_z                0x7A
#define ASCII_CURLY_BRACKET_L  0x7B
#define ASCII_VERTICAL_BAR     0x7C
#define ASCII_CURLY_BRACKET_R  0x7D
#define ASCII_TILDE            0x7E
#define ASCII_DEL              0x7F

/******************************************************/
/* PC keyboard */
/******************************************************/

#define PCKEY_UP           0x01 
#define PCKEY_DOWN         0x02
#define PCKEY_RIGHT        0x03
#define PCKEY_LEFT         0x04 
#define PCKEY_PAGE_UP      0x05 
#define PCKEY_PAGE_DOWN    0x06
#define PCKEY_INSERT       0x07
#define PCKEY_BS           ASCII_BS   /* 0x08 for compatibility with Backspace*/
#define PCKEY_TAB          ASCII_TAB  /* 0x09 for compatibility with Tab */
#define PCKEY_ENTER        ASCII_LF   /* 0x0A for compatibility with New Line (Enter) */
#define PCKEY_F1           0x0B
#define PCKEY_F2           0x0C
#define PCKEY_F3           0x0D
#define PCKEY_F4           0x0E 
#define PCKEY_F5           0x0F
#define PCKEY_F6           0x10
#define PCKEY_F7           0x11
#define PCKEY_F8           0x12
#define PCKEY_F9           0x13
#define PCKEY_F10          0x14
#define PCKEY_F11          0x15
#define PCKEY_F12          0x16
#define PCKEY_HOME         0x17
#define PCKEY_PAUSE        0x18
#define PCKEY_ALT          0x19      /* Warning: cannot be read through console */
#define PCKEY_SCROLL_LOCK  0x1A      /* Warning: cannot be read through console */
#define PCKEY_ESC          ASCII_ESC /* 0x1B Reserved for compatibility with Escape */
#define PCKEY_CAPS_LOCK    0x1C      /* Warning: cannot be read through console */
#define PCKEY_SHIFT        0x1D      /* Warning: cannot be read through console */
#define PCKEY_CONTROL      0x1E      /* Warning: cannot be read through console */
#define PCKEY_NUM_LOCK     0x1F      /* Warning: cannot be read through console */
#define PCKEY_DEL          ASCII_DEL /* 0x7F Reserved for compatibility with Delete */
#define PCKEY_END          0x80
