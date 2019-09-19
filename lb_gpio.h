/* GPIO library based on the work of Pieter-Jan Van de Maele */ 

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> 

#define BCM2708_PERI_BASE2      0x3F000000
#define BCM2708_PERI_BASE      0x7E000000
#define GPIO_BASE               (BCM2708_PERI_BASE2 + 0x200000)	// GPIO controller 
 
#define BLOCK_SIZE 		(4*1024)
 
// IO Acces

typedef struct
{
    unsigned long addr_p;
    int mem_fd;
    void *map;
    volatile unsigned int *addr;
} LB_GP_BCM2835_PERIPHERAL_T;
 
int lb_gp_map_peripheral(LB_GP_BCM2835_PERIPHERAL_T *p);

extern LB_GP_BCM2835_PERIPHERAL_T lb_gp_gpio;  // They have to be found somewhere, but can't be in the header

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x)
#define INP_GPIO(g)   *(lb_gp_gpio.addr + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g)   *(lb_gp_gpio.addr + ((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(lb_gp_gpio.addr + (((g)/10))) |= (((a)<=3?(a) + 4:(a)==4?3:2)<<(((g)%10)*3))
 
#define GPIO_SET  *(lb_gp_gpio.addr + 7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR  *(lb_gp_gpio.addr + 10) // clears bits which are 1 ignores bits which are 0
 
#define GPIO_READ(g)  *(lb_gp_gpio.addr + 13)
//&= (1<<(g))

#define GPIO_PULL *(lb_gp_gpio.addr + 37)
#define GPIO_PULLCLK0 *(lb_gp_gpio.addr + 38)
