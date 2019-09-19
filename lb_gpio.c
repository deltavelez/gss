 
#include "lb_gpio.h"

/* GPIO library based on the work of Pieter-Jan Van de Maele */ 
LB_GP_BCM2835_PERIPHERAL_T lb_gp_gpio;

// Exposes the physical address defined in the passed structure using mmap on /dev/mem
int lb_gp_map_peripheral(LB_GP_BCM2835_PERIPHERAL_T *p)
{
  lb_gp_gpio.addr_p=GPIO_BASE;
   // Open /dev/mem
   if ((p->mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("Failed to open /dev/mem, try checking permissions.\n");
      return -1;
   }
 
   p->map = mmap(
      NULL,
      BLOCK_SIZE,
      PROT_READ|PROT_WRITE,
      MAP_SHARED,
      p->mem_fd,      // File descriptor to physical memory virtual file '/dev/mem'
      p->addr_p       // Address in physical map that we want this memory block to expose
   );
 
   if (p->map == MAP_FAILED) {
        perror("mmap");
        return -1;
   }
 
   p->addr = (volatile unsigned int *)p->map;
 
   return 0;
}
 
void lb_gp_unmap_peripheral(LB_GP_BCM2835_PERIPHERAL_T *p) {
 
    munmap(p->map, BLOCK_SIZE);
    close(p->mem_fd);
}
