#include <stdlib.h>
#include "lb_types.h"
#include "lb_gpio.h"
#include "lb_time.h"

volatile unsigned int *lb_gp_gpio;
int mem_fd;
void *reg_map;

void lb_gp_gpio_close()
{
  munmap(reg_map, BLOCK_SIZE);
  close(mem_fd);
}

int lb_gp_gpio_open()
{
  if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
    {
      printf("Error: lb_gp_init_gpio() --> Failed to open /dev/mem, try checking permissions.\n");
      return EXIT_FAILURE;
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
 

U_INT_8_T lb_gp_gpio_rd(U_INT_8_T pin_number)
{
  return ((*(lb_gp_gpio + 13)) >> pin_number) & 0x01;
}

U_INT_32_T lb_gp_gpio_rd_all(void)
{
  return *(lb_gp_gpio + 13);
}

void lb_gp_gpio_setup_pin(U_INT_8_T pin_number, U_INT_8_T mode)
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

U_INT_8_T lb_gp_gpio_SPI_rw(SPI_PORT_T *port, U_INT_8_T byte_out)
{
  U_INT_8_T i, buffer;
  buffer=0x00;
  lb_gp_gpio_wr((*port).CLK,(*port).CPOL);
  lb_ti_delay_us((*port).delay_clk);
 
  for(i=0;i<8;i++)
    {
      if ((*port).CPHA) /* Phase == 1*/
	{
	  lb_gp_gpio_wr((*port).CLK,!(*port).CPOL);
	  lb_gp_gpio_wr((*port).MOSI,(byte_out >> (7-i)) & 0x01);
	  lb_ti_delay_us((*port).delay_clk);
	  buffer=buffer<<1;
	  buffer |= lb_gp_gpio_rd((*port).MISO);
	  lb_gp_gpio_wr((*port).CLK,(*port).CPOL);
	  lb_ti_delay_us((*port).delay_clk);
	
        }
      else /* Phase ==0 */
	{
	  lb_gp_gpio_wr((*port).MOSI, (byte_out >> (7-i)) & 0x01);
	  lb_ti_delay_us((*port).delay_clk);
	  lb_gp_gpio_wr((*port).CLK,!(*port).CPOL);
	  buffer=buffer<<1;
	  buffer |= lb_gp_gpio_rd((*port).MISO);
	  lb_ti_delay_us((*port).delay_clk);
	  lb_gp_gpio_wr((*port).CLK, (*port).CPOL);
	}
    }
  return buffer;
}

void lb_gp_gpio_SPI_rw_buffer(SPI_PORT_T *port, U_INT_8_T *buffer_out, U_INT_8_T *buffer_in, U_INT_8_T n_bytes)
{
  U_INT_8_T j;
  for (j=0; j<n_bytes; j++)
    {
      buffer_in[j]=lb_gp_gpio_SPI_rw(port,buffer_out[j]);
      lb_ti_delay_us((*port).delay_byte);
    }
}

void lb_gp_gpio_wr(U_INT_8_T pin_number, U_INT_8_T value)
{
 
  if (value & 0x01)
      *(lb_gp_gpio + 7) = 1<<pin_number;  /* sets */
  else
      *(lb_gp_gpio + 10) =1<<pin_number;  /* clears */
}


void lb_gp_print_u32_as_binary(U_INT_32_T value, U_INT_8_T n_bits)
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

