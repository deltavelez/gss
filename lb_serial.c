#include <stdio.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <sys/signal.h>
#include <sys/time.h>
#include "lb_types.h" 
#include "lb_serial.h"


void lb_se_signal_handler_IO (int status);  /* definition of signal handler */

void lb_se_init(COMM_PORT_T *port)
{
  /*******************************************************************************/   
  /* Open the device(com port) to be non-blocking (read will return immediately) */
  (*port).fd = open((*port).device,  O_RDWR  | O_NOCTTY );

  if ((*port).fd < 0)
    {
      perror("Error initializing serial port\r\n");
      exit(-1);
    }
  /*******************************************************************************/   

  /* Install the serial handler before making the device asynchronous */
  saio.sa_handler = lb_se_signal_handler_IO;
  sigemptyset(&saio.sa_mask);   /* saio.sa_mask = 0; */
  saio.sa_flags = 0;
  saio.sa_restorer = NULL;
  sigaction(SIGIO,&saio,NULL);

  /* allow the process to receive SIGIO */
  fcntl((*port).fd, F_SETOWN, getpid());
  
  /* Make the file descriptor asynchronous (the manual page says only
     O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
  fcntl((*port).fd, F_SETFL, FASYNC);
  
  tcgetattr((*port).fd,&oldtio); /* save current port settings */
  
  /*******************************************************************************/   
  fprintf(stderr,"-----------------------------------------------\r\n");
    
  /* set new port settings for canonical input processing */ 
  /* Baud rate */
  cfsetispeed(&newtio,(*port).baud_rate);
  cfsetospeed(&newtio,(*port).baud_rate);
  
  /* Parity */
  switch ((*port).parity)
    {
    case PA_none:
    default:
      newtio.c_cflag &= ~PARENB;
      fprintf(stderr,"Parity: None\r\n");
      break;
    case PA_even:
      newtio.c_cflag |= PARENB;
      newtio.c_cflag &= ~ PARODD;
      fprintf(stderr,"Parity: Even\r\n");
      break;
    case PA_odd:
      newtio.c_cflag |= PARENB;
      newtio.c_cflag |= PARODD;
      fprintf(stderr,"Parity: Odd\r\n");
      break;
    }
  
  switch ((*port).stop_bits)
    {
    case SB_1:
      newtio.c_cflag &= ~CSTOPB;
      fprintf(stderr,"Stop bits: 1\r\n");
      break;
    case SB_2:
      newtio.c_cflag |= CSTOPB;
      fprintf(stderr,"Stop bits: 2\r\n");
      break;
    }  /* end of switch stop bits */
  
  switch ((*port).flow_control)
    {
    case FC_none:
    default:
      newtio.c_cflag &= ~CRTSCTS;
      newtio.c_iflag &= ~(IXON | IXOFF | IXANY);
      fprintf(stderr,"Flow control: None\r\n");
      break;
    case FC_hw:
      newtio.c_cflag |= CRTSCTS;
      newtio.c_iflag &= ~(IXON | IXOFF | IXANY);
      fprintf(stderr,"Flow control: Hardware\r\n");
      break;
    case FC_sw:
      newtio.c_cflag &= CRTSCTS;
      newtio.c_iflag |= (IXON | IXOFF | IXANY);
      fprintf(stderr,"Flow control: Software\r\n");
      break;
    }  /* end of switch stop bits */
  
  /* Data bits */
  newtio.c_cflag &= ~CSIZE;
  
  newtio.c_cflag |= (*port).data_bits;
  
  switch ((*port).data_bits)
    {
    case CS8:
      fprintf(stderr,"Data Bits: CS8\r\n");
      break;
    case CS7:
      fprintf(stderr,"Data Bits: CS7\r\n");
      break;
    case CS6:
      fprintf(stderr,"Data Bits: CS6\r\n");
      break;
    case CS5:
      fprintf(stderr,"Data Bits: CS5\r\n");
      break;
    }
  fprintf(stderr,"-----------------------------------------------\r\n");
  newtio.c_lflag &= ~(ICANON | ECHO | ECHOE |ISIG);       /* ICANON; */
  newtio.c_oflag &= ~OPOST;
  newtio.c_cc[VMIN]=0;
  newtio.c_cc[VTIME]=0;
  
  newtio.c_cflag |= (CLOCAL | CREAD);
  
  tcflush((*port).fd, TCIFLUSH);
  /* oxo I changed this */
  tcsetattr((*port).fd,TCSANOW,&newtio);
  
  (*port).a=0;
  (*port).n=0;
} /* SERCOM_Initialize */


void lb_se_close(COMM_PORT_T *port)
{
  /* restore old port settings */
  /* tcsetattr(fd,TCSANOW,&oldtio); */
  tcflush((*port).fd, TCIFLUSH);
  tcsetattr((*port).fd,TCSAFLUSH,&oldtio);
  
  close((*port).fd);        /* close the com port */
} /* SERCOM_Close */

void lb_se_flush(COMM_PORT_T *port)
{
  tcflush((*port).fd, TCIFLUSH);
  tcflush((*port).fd, TCOFLUSH);
}

int lb_se_process_rx(COMM_PORT_T *port) 
{
  int res;
  res = read((*port).fd,buf,254);
  if (res>0)
    {
      for (i=0; i<res; i++)  /* for all chars in string */
	{
	  /* fputc (buf[i], output);
	     fflush(output);
	     fprintf(stderr,"Hex: 0x%x\r\n",buf[i]); */
	  if (((*port).a+(*port).n) < CIRC_BUFFER_SIZE)
	    {
	      (*port).data_buffer[(*port).a+(*port).n]=buf[i];
	      (*port).n++;
	    }
	  else
	    {
	      (*port).data_buffer[(*port).a+(*port).n-CIRC_BUFFER_SIZE]=buf[i];
	      if ((*port).n < CIRC_BUFFER_SIZE)
		(*port).n++;
	      else
		if ((*port).a < CIRC_BUFFER_SIZE - 1)
		  (*port).a++;
		else
		  (*port).a=0;
	    }
	}		     
      buf[res]=0;
    }  /* end if res>0 */
  return res;
}


void lb_se_tx_byte(COMM_PORT_T *port, char c)
{
  write((*port).fd,&c,1);
}


void lb_se_tx_str(COMM_PORT_T *port, const char *mystring)
{
  write((*port).fd,mystring,strlen(mystring));          /* Write 1 byte to the port */          
  tcdrain((*port).fd);
}

void lb_se_print_buffer(COMM_PORT_T *port)
{
  int k;
  fprintf(stderr,"-----------------------------------------------\r\n");
  fprintf(stderr,"a=%d  n=%d\r\n",(*port).a,(*port).n);
  fprintf(stderr,"Complete vector:\r\n");
  for (k=0;k<CIRC_BUFFER_SIZE;k++)
    fprintf(stderr,"%c",(*port).data_buffer[k]);
  fprintf(stderr,"\r\n");
  fprintf(stderr,"Most recent data:\r\n");
  fprintf(stderr,"-----------------------------------------------\r\n");

  /* Now we print the data in the right order */
  for (k=0;k<(*port).n;k++)
    if (((*port).a+k)<CIRC_BUFFER_SIZE)
      fprintf(stderr,"%c",(*port).data_buffer[(*port).a+k]);
    else
      fprintf(stderr,"%c",(*port).data_buffer[(*port).a+k-CIRC_BUFFER_SIZE]);
  fprintf(stderr,"\r\n");
} /* SERCOM_PrintBuffer */

void lb_se_clear_buffer(COMM_PORT_T *port)
{
  int k;
  for (k=0;k<CIRC_BUFFER_SIZE;k++)
    (*port).data_buffer[k]=0;
  (*port).a=0;
  (*port).n=0;
}

void lb_se_copy_buffer(COMM_PORT_T *port, char *str)
{
  int k;
  for (k=0;k<(*port).n;k++)
    if (((*port).a+k)<CIRC_BUFFER_SIZE)
      str[k]=(*port).data_buffer[(*port).a+k];
    else
      str[k]=(*port).data_buffer[(*port).a+k-CIRC_BUFFER_SIZE];
  str[k]='\0';
}


#define _POSIX_SOURCE 1         /* POSIX compliant source */
#define CIRC_BUFFER_SIZE 40



/***************************************************************************
 * signal handler. sets wait_flag to FALSE, to indicate above loop that     *
 * characters have been received.                                           *
 ***************************************************************************/

int wait_flag=TRUE;                   /* TRUE while no signal received */

void lb_se_signal_handler_IO (int status)
{
  wait_flag = FALSE;
}

