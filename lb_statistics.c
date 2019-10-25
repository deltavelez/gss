#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "lb_algebra.h"
#include "lb_types.h"
#include "lb_statistics.h"

#define REL_ERROR 1e-12

REAL_T lb_st_erf(REAL_T x)
//erf(x) = 2/sqrt(pi)*integral(exp(-t^2),t,0,x)
//         = 2/sqrt(pi)*[x - x^3/3 + x^5/5*2! - x^7/7*3! + ...]
//         = 1-erfc(x)
{
  REAL_T two_sqrtpi=2/sqrt(M_PI);
  //static const double two_sqrtpi=  1.128379167095512574;        // 2/sqrt(pi)
  if (fabs(x) > 2.2) {
    return 1.0 - erfc(x);        //use continued fraction when fabs(x) > 2.2
  }
  
  REAL_T sum=x, term=x, xsqr=x*x;
  int j=1;
  do {
    term*= xsqr/j;
    sum-= term/(2*j+1);
    ++j;
    term*= xsqr/j;
    sum+= term/(2*j+1);
    ++j;
  } while (fabs(term)/sum > REL_ERROR);
  return two_sqrtpi*sum;
}

REAL_T lb_st_erfc(REAL_T x)
/* 
   erfc(x) = 2/sqrt(pi)*integral(exp(-t^2),t,x,inf)
           = exp(-x^2)/sqrt(pi) * [1/x+ (1/2)/x+ (2/2)/x+ (3/2)/x+ (4/2)/x+ [...]
           = 1-erf(x)
	   expression inside [] is a continued fraction so '+' means add to denominator only */
{
  REAL_T one_sqrtpi = 1/sqrt(M_PI); 
  //  static const double one_sqrtpi =  0.564189583547756287;        // 1/sqrt(pi)
  if (fabs(x) < 2.2) {
    return 1.0 - erf(x);        //use series when fabs(x) < 2.2
  }
  if (signbit(x)) {               //continued fraction only valid for x>0
    return 2.0 - erfc(-x);
  }
  REAL_T a=1, b=x;                //last two convergent numerators
  REAL_T c=x, d=x*x+0.5;          //last two convergent denominators
  REAL_T q1,q2;                   //last two convergents (a/c and b/d)
  REAL_T n= 1.0, t;
  do {
    t= a*n+b*x;
    a= b;
    b= t;
    t= c*n+d*x;
    c= d;
    d= t;
    n+= 0.5;
    q1= q2;
    q2= b/d;
  } while (fabs(q1-q2)/q2 > REL_ERROR);
  return one_sqrtpi*exp(-x*x)*q2;
}

REAL_T lb_st_frand(void)
{
  return (REAL_T)rand()/RAND_MAX;
} 

REAL_T lb_st_marsaglia_polar(REAL_T variance)
{
  REAL_T r1, r2;
  lb_st_marsaglia_polar2(variance, &r1, &r2);
  return r1;
}

void lb_st_marsaglia_polar2(REAL_T variance, REAL_T *r1, REAL_T *r2)
{
  REAL_T x, y, s;
  do {
    x= 2.0*rand()/(REAL_T)RAND_MAX -1.0;
    y= 2.0*rand()/(REAL_T)RAND_MAX -1.0;
    s = x*x + y*y;
  } while ((s>=1) || (s==0));
  *r1=variance*x*sqrt(-2.0*log(s)/s);
  *r2=variance*y*sqrt(-2.0*log(s)/s);
}

/* This function calculates the area under the bell, following the widely-spread-over-the-entire-Internet 
   recipe outlined by Abramowitz and Stegun in their "Handbook of Mathematical Functions, in section 7.1.26. */
  
REAL_T lb_st_gauss_area(REAL_T x)
{
  // constants
  REAL_T a1 =  0.254829592;
  REAL_T a2 = -0.284496736;
  REAL_T a3 =  1.421413741;
  REAL_T a4 = -1.453152027;
  REAL_T a5 =  1.061405429;
  REAL_T p  =  0.3275911;
  
  // Save the sign of x
  SINT8_T  sign = 1;
  if (x < 0)
    sign = -1;
  x = fabs(x)/sqrt(2.0);
  
  REAL_T t = 1.0/(1.0 + p*x);
  REAL_T y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);
  
  return 0.5*(1.0 + sign*y);
}

REAL_T lb_st_average(VECTOR_R_T *Data)
{
 UINT16_T i;
 REAL_T sum;
 if (!lb_al_assert_dimensions_vector_r(Data))
   {
     printf("Error: lb_st_average() --> invalid dimension --> Invalid dimension [V]\r\n");
     exit(1);
   }
 sum=0;
 for (i=0;i<(*Data).items;i++)
   sum+=(*Data).array[i];

 return sum/(*Data).items;
}

REAL_T lb_st_stddev2(VECTOR_R_T *Data, REAL_T mu)
{
 UINT16_T i;
 REAL_T sum, temp;
 if (!lb_al_assert_dimensions_vector_r(Data))
   {
     printf("Error: lb_st_stddev() --> invalid dimension --> Invalid dimension [V]\r\n");
     exit(1);
   }
 sum=0;
 for (i=0;i<(*Data).items;i++)
   {
     temp=(*Data).array[i]-mu;
     sum+=temp*temp;
   }
 return sum/(*Data).items;
}



void lb_st_histogram(VECTOR_R_T *Data, VECTOR_R_T *Bins, REAL_T a, REAL_T b, REAL_T *mu, REAL_T *sigma2 )
{
  REAL_T sum, temp;
  UINT16_T i;
  SINT16_T index;
  
  if (!lb_al_assert_dimensions_vector_r(Data))
    {
      printf("Error: lb_st_histogram() --> invalid dimension --> Invalid dimension [V]\r\n");
      exit(1);
    }

  if (!lb_al_assert_dimensions_vector_r(Bins))
    {
      printf("Error: lb_st_histogram() --> invalid dimension --> Invalid dimension [V]\r\n");
      exit(1);
    }

  if (lb_re_equal(a,b))
    {
      printf("Error: lb_st_histogram() --> invalid range: a=b\r\n");
      exit(1);
    }

  if (a>=b)
    {
      printf("Error: lb_st_histogram() --> invalid range: a>b\r\n");
      exit(1);
    }

  *mu=lb_st_average(Data);

  sum=0.0;
  for(i=0;i<(*Bins).items;i++)
    (*Bins).array[i]=0.0;

  for(i=0;i<(*Data).items;i++)
   {
     temp=(*Data).array[i]-*mu;
     sum+=temp*temp;

     if(lb_re_equal((*Data).array[i],b))
       (*Bins).array[(*Bins).items-1]++;
     else
       {
	 index=(*Bins).items*((*Data).array[i]-a)/(b-a);
	 printf("index = %d\r\n",index);
	 if( (index>=0) && (index<(*Bins).items) )
	   (*Bins).array[index]++;
       }
   }
  *sigma2=sum/(*Data).items;
}
