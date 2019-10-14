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
#define GPIO_HIGH   1
#define GPIO_LOW    0

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
  unsigned char MOSI;
  unsigned char CLK;
  unsigned char MISO;
  unsigned char CPOL;
  unsigned char CPHA;
  unsigned int  delay_clk;
  unsigned int  delay_byte;
} SPI_PORT_T;

void       lb_gp_gpio_close();
int        lb_gp_gpio_open();
UINT8_T  lb_gp_gpio_rd(UINT8_T pin_number);
UINT32_T lb_gp_gpio_rd_all(void);
void       lb_gp_gpio_setup_pin(UINT8_T pin_number, UINT8_T mode);
UINT8_T  lb_gp_gpio_SPI_rw(SPI_PORT_T *port, UINT8_T byte_out);
UINT16_T lb_gp_gpio_SPI_rw_nbits(SPI_PORT_T *port, UINT16_T int16_out, UINT8_T n_bits);
void       lb_gp_gpio_SPI_rw_buffer(SPI_PORT_T *port, UINT8_T *buffer_out, UINT8_T *buffer_in, UINT8_T n_bytes);
void       lb_gp_gpio_wr(UINT8_T pin_number, UINT8_T value);
void       lb_gp_print_u32_as_binary(UINT32_T value, UINT8_T n_bits);


#endif /* LB_GPIO_H */
