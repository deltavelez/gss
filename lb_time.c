#include <time.h>
#include "lb_time.h"

void lb_ti_delay(S_INT_32_T delay)
{
  clock_t stamp, delta;
  stamp=clock();
  delta=(CLOCKS_PER_SEC/1000)*delay;
  while ( clock()<=(stamp+delta) )
    {
      ;
    }
}
