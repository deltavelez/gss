#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include "lb_algebra.h"
#include "lb_types.h"
#include "lb_integer.h"

void lb_sieve_erathostenes(VECTOR_SINT16_T *P, SINT16_T n)
{
  UINT16_T i,j;
  VECTOR_SINT8_T flags;

  if (n<2)
    flags.items=0;
  else
    flags.items=n-1;

  lb_al_create_vector_si8(&flags);

  for (i=2;i<=n;i++)
    flags.array[i-2]=TRUE;
      
  for (i=2;i<ceil(sqrt(n));i++)
    {
      if (flags.array[i-2]) 
	{
	  j=i*i;
	  while (j<=n)
	    {
	      flags.array[j-2]=FALSE;
	      j+=i;
	    }
	}
    }
 
  for (i=0, j=0; i<=(n-2); i++)
    {
      if (flags.array[i])
	{
	  j++;
	  printf("i=%i\r\n",i+2);
	}
    }
  (*P).items=j;
  lb_al_create_vector_si16(P);

  for (i=0, j=0; i<=(n-2); i++)
    {
      if (flags.array[i])
	{
	  (*P).array[j]=i+2;
	  j++;
	}
    }

  lb_al_release_vector_si8(&flags);
}

/* Classic algorithm by Erathostenes which uses an array with only odd numbers */
void lb_sieve_erathostenes_2(VECTOR_SINT16_T *P, SINT16_T n)
{
  UINT16_T i,j;
  VECTOR_SINT8_T flags;

  flags.items=ceil((REAL_T)(n-2)/2)+1;
  lb_al_create_vector_si8(&flags);

  for (i=0;i<flags.items;i++)
    flags.array[i]=TRUE;
      
  for (i=1;i<=ceil(sqrt(n)/2);i++)
    {
      if (flags.array[i]) 
	{
	  j=(2*i+1)*(2*i+1);
	  while (j<=n)
	    {
	      if ((j%2)==1)
		{
		  flags.array[(j-1)/2]=FALSE;
		  //printf("i=%d j=%d, index=%d\r\n",i,j,(j-1)/2);
		}
	      j+=2*i+1;
	    }
	}
    }
  j=0;
  for (i=0; i<flags.items; i++)
    {
      if (flags.array[i])
	j++;
    }

  if (n>=2)
    (*P).items=j;
  else
    (*P).items=0;
    
  lb_al_create_vector_si16(P);
  

  (*P).array[0]=2;
  for (i=1, j=1; i<=n/2; i++)
    {
      if (flags.array[i])
	{
	  (*P).array[j]=2*i+1;
	  j++;
	}
    }
  lb_al_release_vector_si8(&flags);
}

SINT8_T lb_in_pos_in_sorted_vector(VECTOR_SINT16_T *P, SINT16_T n)
{
  UINT16_T a, b, c;
  if ((*P).items==0)
    return -1; /* not found */
  a=0;
  b=(*P).items-1;
  while ( ((b-a)>1) && (n!=(*P).array[a]) && (n!=(*P).array[b]) )
    {
      c=(a+b)/2;
      if (n<(*P).array[c])
	b=c;
      else
	a=c;
      printf("a=%d, b=%d\r\n",a,b);
    }
  if (n==(*P).array[a])
    return a;
  if (n==(*P).array[b])
    return b;
  return -1;
}

//if (ceil(sqrt(n))<(*P).array[(*P).items-1])
//  {
//   a=0;
//   b=(*P).items-1;
//   while 
// }



UINT16_T lb_in_invert_bits(UINT16_T x, UINT16_T n_bits)
{
  UINT8_T bit;
  UINT16_T inverted=0;
  for (bit=0;bit<=n_bits;bit++)
    inverted|= ((x>>bit) & 0x01)<<((n_bits-1-bit));
  inverted = inverted & ((0x01<<n_bits) -1);
  return inverted;
}

SINT8_T lb_in_is_even(SINT16_T x)
{
  if (x%2==0)
    return TRUE;
  return FALSE;
}

SINT8_T lb_in_is_odd(SINT16_T x)
{
  if (x%2==0)
    return FALSE;
  return TRUE;
}

SINT8_T  lb_in_is_perfectsquare(SINT32_T n)
{
  SINT32_T x;
  x=lb_in_isqrt(n);
  if ( (x*x) == n )
    return TRUE;
  return FALSE;
}


SINT8_T lb_in_is_power_of_two(UINT32_T x)
{
  UINT8_T bit_size=0;
  while (((x % 2) == 0) && (x > 1)) /* While x is even and > 1 */
    {
      x /= 2;
      bit_size++;
    }
  if (x == 1) return bit_size;
  return 0;
}

SINT8_T lb_in_is_prime(SINT32_T n)
{
  SINT32_T i, w;
  if (n<2)
    return FALSE;

  if ( (n==2) || (n==3) )
    return TRUE;

  if ( (n%2) == 0 ) 
    return FALSE;
  if ((n%3) == 0 )
    return FALSE;

  i=5;
  w=2;

  while (i*i<=n)
    {
      if ( (n%i) == 0 )
	return FALSE;

      i+=w;
      w=6-w;
    }
  return TRUE;
}

SINT32_T  lb_in_isqrt(SINT32_T n)
{
  SINT32_T start, end, mid, ans;
  if (n<0)
    {
      printf("Error: lb_in_isqrt() --> negative argument [%li]\r\n",n);
      exit(1);
    }
  if (n == 0 || n == 1) 
    return n;
 
  /* Do Binary Search for floor(sqrt(x)) */
  start = 1;
  end = n/2+1;   
  while (start <= end) 
    {        
      mid = (start + end) / 2;
 
      if (mid*mid == n) /* perfect square */
	return mid;
 
      /* Since we need floor, we update answer when mid*mid is 
         smaller than x, and move closer to sqrt(x) */
      if (mid*mid < n) 
        {
	  start = mid + 1;
	  ans = mid;
        } 
      else // If mid*mid is greater than x
	end = mid - 1;        
    }
    return ans;
}


SINT16_T lb_in_littleS16_to_bigS16(SINT16_T x)
{
  return ((x & 0x00FF)<<8) | ((x & 0xFF00)>>8);
}

SINT16_T lb_in_max(SINT16_T x, SINT16_T y)
{
  if (x>=y) return x;
  return y;
}

SINT16_T lb_in_max_n(SINT16_T n, ...)
{
  /* oxo: this function requires further work.  Warnings are generated: va_args(argsSINT16_T) */
  va_list args;
  int i, max, temp;

  va_start(args,n);

  if (n<=1)
    {
      printf("Error: lb_in_max_n() --> Invalid number of arguments, n=%d\r\n",n);
      exit(1);
    }
  max=va_arg(args, int);
  
  for (i=1;i<n;i++)
    {
      temp=va_arg(args, int);
      if (temp>max)
	max=temp;
    }
  va_end(args);
  return max;
}

  
SINT16_T lb_in_min(SINT16_T x, SINT16_T y)
{
  if (x<=y) return x;
  return y;
}

SINT16_T lb_in_pow(SINT16_T k, SINT16_T n)
{
  SINT16_T temp, i;
  if (n==0) 
    return 1;
  temp=k;
  for (i=1;i<n;i++)
    temp*=k;
  return temp;
}

SINT16_T lb_in_round_div_up(SINT32_T a, SINT32_T b)
{
  if(b==0) return 0;
  return (a + (b-1))/b;
}

SINT16_T lb_in_simpson2d_coef(SINT16_T i, SINT16_T j, SINT16_T m, SINT16_T n)
{
  SINT16_T i_exp, j_exp;
  if ((i==0) || (i==m-1)) 
    i_exp=0;
  else
    i_exp=i%2+1;
  if ((j==0) ||(j==n-1)) 
    j_exp=0;
  else
    j_exp=j%2+1;
  return lb_in_pow(2,i_exp+j_exp);
}

