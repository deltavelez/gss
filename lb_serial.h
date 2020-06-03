#ifndef LB_SERIAL_H
#define LB_SERIAL_H

#define _POSIX_SOURCE 1         /* POSIX compliant source */
#define CIRC_BUFFER_SIZE 40

#include <signal.h>
#include <termios.h>
#include "lb_types.h"

typedef struct
{
  speed_t  baud_rate; /* Baud Rate Type */
  tcflag_t data_bits;  
  char device[20];
  enum 
    {
      SB_1, SB_2     
    } stop_bits; /* Stop bits type */
  enum 
    {
      PA_none, PA_even, PA_odd     
    } parity; /* Parity type */
  enum 
    {
      FC_none, FC_hw, FC_sw     
    } flow_control; /* Parity type */
  int a; /* pointer to the oldest character in the circular buffer */
  int n; /* number of bytes available in the circular buffer */
  int fd; /* file descriptor */
  char data_buffer[40];  /* circular data buffer where data is stored for parsing */
} COMM_PORT_T;



int i;
char Key;
struct termios oldtio, newtio;       /* Place for old and new port settings for serial port       */
struct sigaction saio;               /* definition of signal action  */

char buf[255];                       /* buffer for where data is put */
//struct timeval t_initial;

/* Info on the structure of termios
struct termios 
{
  tcflag_t c_iflag;
  tcflag_t c_oflag;
  tcflag_t c_cflag;
  tcflag_t c_lflag;
  cc_t c_cc[NCCS];
  speed_t c_ispeed;
  speed_t c_ospeed;
  }; */



void lb_se_init(COMM_PORT_T *port);
void lb_se_close(COMM_PORT_T *port);
void lb_se_flush(COMM_PORT_T *port);
int  lb_se_process_rx(COMM_PORT_T *port);
void lb_se_tx_byte(COMM_PORT_T *port, char c);
void lb_se_tx_str(COMM_PORT_T *port, const char *mystring);
void lb_se_print_buffer(COMM_PORT_T *port);
void lb_se_clear_buffer(COMM_PORT_T *port);
void lb_se_copy_buffer(COMM_PORT_T *port, char *str);
float  lb_se_get_timestamp();
void lb_se_signal_handler_IO (int status);
REAL_T lb_se_query_instrument(COMM_PORT_T *port, char *command, UINT32_T delay_ms);

#endif /* LB_SERIAL_H */
