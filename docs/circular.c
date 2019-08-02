#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h> /* For exit() */
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <math.h>
#include <sys/ioctl.h>


#define SIZE 10
#define TRUE 1
#define FALSE 0

char buffer[SIZE];
int a, b;
int flag_buffer_full=FALSE;



unsigned char lb_co_getch( ) 
{ 
  struct termios oldt,newt;
  int ch;
  tcgetattr( STDIN_FILENO, &oldt );
  newt = oldt;
  newt.c_lflag &= ~( ICANON | ECHO );
  tcsetattr( STDIN_FILENO, TCSANOW, &newt );
  ch = getchar();
  tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
  return ch;
}


void push(char c)
{
  buffer[b]=c;
  b++;
  if (b==SIZE)
    b=0;
  if (b==a)
    {
      a++; /* to keep at last the older data */
      if (a==SIZE)
	a=0;
      flag_buffer_full=TRUE;
    }
}

char pop(void)
{
  char temp;
  if (a!=b)
    {
      temp=buffer[a];
      a++;
      if (a==SIZE)
	a=0;
    }
  return temp;
}

int chars_in_buffer(void)
{
  if (a>b)
    return b+SIZE-a;
  else
    return b-a;
}

void printbuffer()
{
  int pos, i;

  i=0;
  pos=a;
  while (pos!=b)
    {
      printf("buffer[ %i] = %c\r\n",i,buffer[pos]);
      i++;
      pos++;
      if (pos==SIZE)
	pos=0;
    }
}

int main(void)
{
  char c;
  a=0;
  b=0;

  while ((c=lb_co_getch()) != 0x1B)
    {
      if (c== ' ')
	printf("Popping %c\r\n", pop());
      else
	push(c);
      printf("-----------------------------------------\r\n");
      printf("a=%d, b=%d, full=%d,  n=%d\r\n",a,b,flag_buffer_full,chars_in_buffer());
      printf("-----------------------------------------\r\n");
      printbuffer();
    }

}

