#ifndef LB_GPIO_H
#define LB_GPIO_H


#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 
#include "lb_types.h"

/* Notes:
   from newest --> oldest
   BCM2837 : Used in the Raspberry Pi model 3.  Peripherals are based at physical address 0x3F000000
   BCM2836 : Used in the Raspberry Pi model 2.  Peripherals are based at physical address 0x20000000. (This hasn't been tested).
   BCM2835 : Broadcom chip used in the Raspberry Pi models A, B, B+, Compute module, and Pi Zero
*/

#define PHYSICAL_ADDRESS     0x3F000000
#define GPIO_OFFSET          0x00200000
#define BLOCK_SIZE 		(4*1024)
#define GPIO_INPUT  1
#define GPIO_OUTPUT 0



// IO Acces

 
extern volatile unsigned int *lb_gp_gpio;  // They have to be found somewhere, but can't be in the header

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x)
#define GPIO_SET_INP(g)   *(lb_gp_gpio + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define GPIO_SET_OUT(g)   *(lb_gp_gpio + ((g)/10)) |=  (1<<(((g)%10)*3))
#define GPIO_SET_ALT(g,a) *(lb_gp_gpio + (((g)/10))) |= (((a)<=3?(a) + 4:(a)==4?3:2)<<(((g)%10)*3))
 
#define GPIO_SET  *(lb_gp_gpio + 7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR  *(lb_gp_gpio + 10) // clears bits which are 1 ignores bits which are 0
 
#define GPIO_READ(g)  *(lb_gp_gpio + 13)
//&= (1<<(g))

#define GPIO_PULL *(lb_gp_gpio + 37)
#define GPIO_PULLCLK0 *(lb_gp_gpio + 38)

/* This structure defines a general SPI port, supporting all 4 modes and a programmable delay parameters between clocks and between bytes */
typedef struct
{
  unsigned char MISO;
  unsigned char MOSI;
  unsigned char CLK;
  unsigned char CPOL;
  unsigned char CPHA;
  unsigned int  delay_clk;
  unsigned int  delay_byte;
} SPI_PORT_T;

int lb_gp_init_gpio();
void lb_gp_print_as_binary(U_INT_32_T value, U_INT_8_T n_bits);
void lb_gp_setup_gpio_pin(U_INT_8_T pin_number, U_INT_8_T mode);
U_INT_8_T lb_gp_gpio_rd(U_INT_8_T pin_number);
void lb_gp_gpio_wr(U_INT_8_T pin_number, U_INT_8_T value);
U_INT_8_T lb_gp_SPI_rw(SPI_PORT_T port, U_INT_8_T byte_out);
void lb_gp_SPI_rw_buffer(SPI_PORT_T port, U_INT_8_T *buffer_out, U_INT_8_T *buffer_in, U_INT_8_T n_bytes);

#endif /* LB_GPIO_H */
