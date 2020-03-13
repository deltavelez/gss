#include <time.h>
#include "lb_time.h"

UINT32_T lb_ti_last_clock;
UINT64_T lb_ti_clock_offset=0;

UINT64_T lb_ti_clock64(void)
{
  UINT32_T time_stamp;
  time_stamp = clock();
  if (time_stamp<lb_ti_last_clock)
    lb_ti_clock_offset+=0xFFFFFFFF;
  
  lb_ti_last_clock=time_stamp;
      
  return (UINT64_T)time_stamp+lb_ti_clock_offset;
}

/* Functions to be used in systems where sizeof(clock()) == 8 */ 
#ifdef CLOCK_FUNCTIONS_64BIT
void lb_ti_delay_ms(UINT32_T delay_ms)
{
  clock_t time_a, time_b, time;
  time_a=clock();
  time_b=time_a+1000*delay_ms;
  do
    {
      time=clock();
    } while (time<time_b);
}

 
void lb_ti_delay_us(UINT32_T delay_us)
{
  clock_t time_a, time_b, time;
  time_a=clock();
  time_b=time_a+(clock_t)delay_us;
  do
    {
      time=clock();
    } while (time<time_b);
}
#endif

void lb_ti_delay_wall_ms(UINT32_T delay_ms)
{
  struct timespec ts_time;
  UINT64_T end_u64, time_u64;

  clock_gettime(CLOCK_MONOTONIC, &ts_time);
  end_u64   = ((UINT64_T)ts_time.tv_sec)*1000000000 + ((UINT64_T)ts_time.tv_nsec) + ((UINT64_T)delay_ms)*1000000;
  do
    {
      clock_gettime(CLOCK_MONOTONIC, &ts_time);
      time_u64   = ts_time.tv_sec*1000000000 + ts_time.tv_nsec;
    } while (time_u64<end_u64);
}


/* Functions to be used in systems where sizeof(clock()) = 4 such as the Raspberry Pi */
/* These functions internally use 64-bit counters */

//#define CLOCK_FUNCTIONS_32_BIT_A
#ifdef CLOCK_FUNCTIONS_32_BIT_A

void lb_ti_delay_us(UINT32_T delay_us)
{
  UINT64_T time_a, time_b, time;
  time_a=clock();
  time_b=time_a+delay_us;
  do
    {
      time=clock();
      if (time<time_a)
	time+=0xFFFFFFFF;
    } while (time<time_b);
}

void lb_ti_delay_ms(UINT32_T delay_ms)
{
  UINT64_T time_a, time_b, time;
  time_a=clock();
  time_b=time_a+1000*delay_ms;
  do
    {
      time=clock();
      if (time<time_a)
	time+=0xFFFFFFFF;
    } while (time<time_b);
}
#endif

/* Functions to be used in systems where sizeof(clock()) = 4 such as the Raspberry Pi */
/* These functions still use 32-bit variables with a trick */

#define CLOCK_FUNCTIONS_32_BIT_B
#ifdef CLOCK_FUNCTIONS_32_BIT_B

void lb_ti_delay_us(UINT32_T delay_us)
{
  UINT32_T time_a, time_b, time;
  time_a=clock() & 0x0FFFFFFF;
  time_b=time_a+delay_us;
  do
    {
      time=clock() & 0x0FFFFFFF;
      if (time<time_a)
	time+=0x0FFFFFFF;
    } while (time<time_b);
}

void lb_ti_delay_ms(UINT32_T delay_ms)
{
  UINT32_T time_a, time_b, time;
  time_a=clock() & 0x0FFFFFFF;
  time_b=time_a+1000*delay_ms;
  do
    {
      time=clock() & 0x0FFFFFFF;
      if (time<time_a)
	time+=0x0FFFFFFF;
    } while (time<time_b);
}
#endif

REAL_T lb_ti_time_wall(void)
{
  struct timespec ts_time;
  UINT64_T end_u64, time_u64;

  //  clock_gettime(CLOCK_MONOTONIC, &ts_time);
  clock_gettime(CLOCK_REALTIME, &ts_time);
  return  ((REAL_T)ts_time.tv_sec) + ((REAL_T)ts_time.tv_nsec*0.000000001);
}
