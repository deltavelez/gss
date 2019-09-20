 
#include "lb_gpio.h"
#include <stdlib.h>

/* GPIO library based on the work of Pieter-Jan Van de Maele */ 
volatile unsigned int *lb_gp_gpio;
int mem_fd;
void *reg_map;

int lb_gp_init_gpio()
{
  if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
    {
      printf("Failed to open /dev/mem, try checking permissions.\n");
      return -1;
   }
 
  reg_map = mmap(
		 NULL,
		 BLOCK_SIZE,
		 PROT_READ|PROT_WRITE,
		 MAP_SHARED,
		 mem_fd,      // File descriptor to physical memory virtual file '/dev/mem'
		 PHYSICAL_ADDRESS+GPIO_OFFSET       // Address in physical map that we want this memory block to expose
   );
 
   if (reg_map == MAP_FAILED)
     {
        perror("mmap");
	close(mem_fd);
        return -1;
     }
 
   lb_gp_gpio = (volatile unsigned int *)reg_map;
 
   return 0;
}
 
void lb_gp_close_gpio() {
 
    munmap(reg_map, BLOCK_SIZE);
    close(mem_fd);
}

void lb_gp_print_as_binary(U_INT_32_T value, U_INT_8_T n_bits)
{
  U_INT_8_T i;
  U_INT_8_T bit_value;
  if (n_bits<1)
    {
      printf("Error: lb_gp_print_as_binary() --> n_bits <1\r\n");
      exit(EXIT_FAILURE);
    }
  for (i=0;i<n_bits;i++)
    {
      bit_value=(value>>(n_bits-1-i)) & 0x01;
      if ( (i!=0) && ((i % 4)==0) )
	printf(".");
      if (bit_value)
	printf("1");
      else
	printf("0");
    }
}

void lb_gp_setup_gpio_pin(U_INT_8_T pin_number, U_INT_8_T mode)
{
  if (mode == GPIO_OUTPUT)
    {
      GPIO_SET_OUT(pin_number);
      return;
    }
  if (mode == GPIO_INPUT)
    {
      GPIO_SET_INP(pin_number);
      return;
    }

  printf("Error: lb_gp_setup_gpio_pin() --> invalid mode\r\n");
  exit(EXIT_FAILURE);
}
