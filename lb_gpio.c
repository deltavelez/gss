#include <stdlib.h>
#include "lb_types.h"
#include "lb_gpio.h"
#include "lb_time.h"


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
 
void lb_gp_close_gpio()
{
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


U_INT_8_T lb_gp_gpio_rd(U_INT_8_T pin_number)
{
  return ((*(lb_gp_gpio + 13)) >> pin_number) & 0x01;
}

void lb_gp_gpio_wr(U_INT_8_T pin_number, U_INT_8_T value)
{
  if (value==0)
    {
      *(lb_gp_gpio + 10) =1<<pin_number;  /* clears */
      return;
    }
  if (value==1)
    {
      *(lb_gp_gpio + 7) = 1<<pin_number;  /* sets */
      return;
    }
  {
    printf("Error: lb_gp_gpio_wr() --> Writting a value other than 0 or 1.  This may be unintended.\r\n");
    exit(EXIT_FAILURE);
  }
}

U_INT_8_T lb_gp_SPI_rw(SPI_PORT_T port, U_INT_8_T byte_out)
{
  U_INT_8_T i, buffer;
  buffer=0x00;
  lb_gp_gpio_wr(port.CLK,port.CPOL);
  lb_ti_delay(port.delay_clk);
 
  for(i=0;i<8;i++)
    {
      if (port.CPHA) /* Phase == 1*/
	{
	  lb_ti_delay(port.delay_clk);
	  lb_gp_gpio_wr(port.CLK,!port.CPOL);
	  lb_gp_gpio_wr(port.MOSI,(byte_out >> (7-i)) & 0x01);
	  lb_ti_delay(port.delay_clk);
	  buffer=buffer<<1;
	  buffer |= lb_gp_gpio_rd(port.MISO);
	  lb_gp_gpio_wr(port.CLK,port.CPOL);
        }
      else /* Phase ==0 */
	{
	  lb_gp_gpio_wr(port.MOSI, (byte_out >> (7-i)) & 0x01);
	  lb_ti_delay(port.delay_clk);
	  lb_gp_gpio_wr(port.CLK,!port.CPOL);
	  buffer=buffer<<1;
	  buffer |= lb_gp_gpio_rd(port.MISO);
	  lb_ti_delay(port.delay_clk);
	  lb_gp_gpio_wr(port.CLK, port.CPOL);
	}
    }
  return buffer;
}

void lb_gp_SPI_rw_buffer(SPI_PORT_T port, U_INT_8_T *buffer_out, U_INT_8_T *buffer_in, U_INT_8_T n_bytes)
{
  U_INT_8_T j;
  for (j=0; j<n_bytes; j++)
    buffer_in[j]=lb_gp_SPI_rw(port,buffer_out[j]);
}
