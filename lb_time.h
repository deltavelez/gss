/* Time related functions */

#ifndef LB_TIME_H
#define LB_TIME_H

#include "lb_types.h"

UINT64_T lb_ti_clock64(void);
void lb_ti_delay_ms(UINT32_T delay);
void lb_ti_delay_us(UINT32_T delay);

#endif
