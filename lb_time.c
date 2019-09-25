#include <time.h>
#include "lb_time.h"

void lb_ti_delay_ms(U_INT_32_T delay_ms)
{
  clock_t stamp, delta;
  stamp=clock();
  delta=(U_INT_32_T)CLOCKS_PER_SEC*delay_ms/1000;
  while ( clock()<=(stamp+delta) )
    {
      ;
    }
}


void lb_ti_delay_us(U_INT_32_T delay_us)
{
  clock_t stamp, delta;
  stamp=clock();
  delta=(U_INT_32_T)(CLOCKS_PER_SEC/1000000)*delay_us;
  while ( clock()<=(stamp+delta) )
    {
      ;
    }
}
