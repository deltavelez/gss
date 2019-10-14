#include <time.h>
#include "lb_time.h"

void lb_ti_delay_ms(UINT32_T delay_ms)
{
  clock_t stamp, delta;

  delta=(UINT32_T)(CLOCKS_PER_SEC/1000)*delay_ms;
  stamp=clock();
  while ( (clock()-stamp)<delta )
    {
      ;
    }
}


void lb_ti_delay_us(UINT32_T delay_us)
{
  clock_t stamp, delta;
  delta=(UINT32_T)(CLOCKS_PER_SEC/1000000)*delay_us;
  stamp=clock();
  while ( (clock()-stamp)<delta )
    {
      ;
    }
}
