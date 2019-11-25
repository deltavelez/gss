#include <stdlib.h>  
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>  // for atof() error
#include <float.h>
#include <math.h>
#include "lb_real.h"
#include "lb_complex.h"
#include "lb_types.h"
#include "lb_console.h" 
#include "lb_integer.h"
#include "lb_numer.h"
#include "lb_parser.h"
#include "lb_statistics.h"

OPERATOR_T ops[MAX_OPERATORS] = { { op_positive,              "+",  a_right, 5, op_unary  },
				  { op_negative,              "-",  a_right, 5, op_unary  }, 
				  { op_factorial,             "!",  a_right, 6, op_unary  },
				  { op_power,                 "^",  a_right, 5, op_binary },
				  { op_division,              "/",  a_left,  4, op_binary },
				  { op_product,               "*",  a_left,  4, op_binary },
				  { op_substraction,          "-",  a_left,  3, op_binary },
				  { op_addition,              "+",  a_left,  3, op_binary },
				  { op_equal_than,            "=",  a_left,  2, op_binary },
				  { op_larger_than,           ">",  a_left,  2, op_binary },
				  { op_smaller_than,          "<",  a_left,  2, op_binary },
				  { op_different_than,        "!=", a_left,  2, op_binary },
				  { op_larger_or_equal_than,  ">=", a_left,  2, op_binary },
				  { op_smaller_or_equal_than, "<=", a_left,  2, op_binary } };

FUNCTION_T fncs[MAX_FUNCTIONS] = { { fn_sin,    "SIN",    1, 0b00000000 },
				   { fn_cos,    "COS",    1, 0b00000000 },
				   { fn_tan,    "TAN",    1, 0b00000000 },
				   { fn_asin,   "ASIN",   1, 0b00000000 },
				   { fn_acos,   "ACOS",   1, 0b00000000 },
				   { fn_atan,   "ATAN",   1, 0b00000000 },
				   { fn_sinh,   "SINH",   1, 0b00000000 },
				   { fn_cosh,   "COSH",   1, 0b00000000 },
				   { fn_tanh,   "TANH",   1, 0b00000000 },
				   { fn_asinh,  "ASINH",  1, 0b00000000 },
				   { fn_acosh,  "ACOSH",  1, 0b00000000 },
				   { fn_atanh,  "ATANH",  1, 0b00000000 },
				   { fn_exp,    "EXP",    1, 0b00000000 },
				   { fn_ln,     "LN",     1, 0b00000000 },
				   { fn_log,    "LOG",    1, 0b00000000 },
				   { fn_log2,   "LOG2",   1, 0b00000000 },
				   { fn_logb,   "LOGB",   2, 0b00000000 },
				   { fn_max2,   "MAX2",   2, 0b00000000 },
				   { fn_min2,   "MIN2",   2, 0b00000000 },
				   { fn_if,     "IF",     3, 0b00000000 },
				   { fn_sqr,    "SQR",    1, 0b00000000 },
				   { fn_sqrt,   "SQRT",   1, 0b00000000 },
				   { fn_abs,    "ABS",    1, 0b00000000 },			   
				   /* From statistics.c */
				   { fn_erf,    "ERF",    1, 0b00000000 },
				   { fn_erfc,   "ERFC",   1, 0b00000000 }, 
				   { fn_rand,   "RAND",   0, 0b00000000 },
				   { fn_marsa,  "MARSA",  1, 0b00000000 },
				   { fn_dstd,   "DSTD",   1, 0b00000000 },
				   /* Complex functions */
				   { fn_real,   "REAL",   1, 0b00000000 },
				   { fn_imag,   "IMAG",   1, 0b00000000 },
				   { fn_polar,  "POLAR",  2, 0b00000000 },
				   { fn_rect,   "RECT",   2, 0b00000000 },
				   /* Functions with a variable number of arguments */
				   { fn_avg,    "AVG",   -1, 0b00000000 },
				   { fn_idef,   "IDEF",   5, 0b00001000 },
				   { fn_idef2,  "IDEF2",  8, 0b01100000 },
				   { fn_sigma,  "SIGMA",  5, 0b00001000 },
				   { fn_prod,   "PROD",   5, 0b00001000 },
				   { fn_diff,   "DIFF",   4, 0b00000010 },
				   { fn_diff2,  "DIFF2",  4, 0b00000010 }
};

CONSTANT_T cons[MAX_CONSTANTS] = { { "PI", M_PI },
				   { "E",  M_E  } };


/* Notes
 * A domain error occurs if an input argument is outside the domain over which the mathematical function is defined.
 * A pole error (also known as a singularity or infinitary) occurs if the mathematical function has an exact infinite result as the*  finite input argument(s) are approached in the limit.
 A range error occurs if the mathematical result of the function cannot be represented in an object of the specified type, due to extreme magnitude. */

ERRORS_T errors[MAX_ERRORS] = { { e_none,                   "No errors"                                         },
                             	{ e_div_zero,               "Division by zero"                                  }, 
				{ e_too_big,                "Range error: number is too big"                    },
				{ e_complex,                "Result is a non-real number"                       },
				{ e_undefined,              "Function is undefined at point"                    },
				{ e_arg_smaller_one,        "Invalid argument: smaller than 1.0"                },
				{ e_arg_larger_one,         "Invalid argument: larger than 1.0"                 },
				{ e_arg_negative,           "Invalid argument: non-positive argument found"     },
				{ e_arg_non_int,            "Invalid argument: non-integer argument found"      },
				{ e_arg_non_even,           "Invalid argument: non-even argument found"         },
				{ e_arg_non_odd,            "Invalid argument: non-odd argument found"         },
				{ e_arg_less_two,           "Invalid argument: lower-than_two argument found"   }, 
				{ e_arg_too_big,            "Invalid argument: too big"                         },
				{ e_arg_too_small,          "Invalid argument: too small"                       },
				{ e_arg_inf_loop,           "Invalid argument: infinite loop"                   },
				{ e_arg_non_real,           "Invalid argument: complex number"                  },
				{ e_arg_not_var,            "Invalid argument: must be a single variable"       },

};

void lb_pa_pop(FN_RECORD_T *fnrec, STACK_ELEMENT_T *item, SINT16_T *s, ARGS_T *args)
{
  SINT16_T n, total_subitems, sum_last=0;


  (*fnrec).out[(*fnrec).oq]=*item;
  for(n=0;n<MAX_FIXED_ARGUMENTS;n++)
    (*fnrec).out[(*fnrec).oq].var_arg[n]=0;

  if((*item).type==st_function)   
    {  
#ifdef DEBUG_POP
      printf("Info: function %s is defined with %d argument(s)\r\n",fncs[(*item).index].name,fncs[(*item).index].n_arguments);
      printf("Info: depth (z) = %d\r\n",(*args).z);
      printf("Info: arguments found: %d\r\n",(*args).arguments[(*args).z]);
      for(n=0;n<=(*args).arguments[(*args).z];n++)
	printf("Info: arg[%d] has %d subitems\r\n",n,(*args).subitems[n][(*args).z]);
#endif
      /* We assertain the number of arguments for the function */
      if(fncs[(*item).index].n_arguments==-1)
	{
	  total_subitems=0;
	  for(n=0;n<=(*args).arguments[(*args).z];n++)
	    {
	      if ((*args).subitems[n][(*args).z]==0)
		{
		  printf("Error: lb_pop_to_out: function %s has a null argument No. [%d]\r\n",
			 fncs[(*item).index].name,n);
		  exit(1);
		}
	      total_subitems+=(*args).subitems[n][(*args).z];
	    }
	  (*fnrec).out[(*fnrec).oq].var_arg[0]=(*args).arguments[(*args).z]+1;
	  (*args).z--;
	  (*args).subitems[(*args).arguments[(*args).z]][(*args).z]+=total_subitems+1;
	  (*fnrec).arg_count-=(*fnrec).out[(*fnrec).oq].var_arg[0]-1;	    
	} 
      else
	{
	  if ( !((fncs[(*item).index].n_arguments==0) && ((*args).subitems[0][(*args).z]==0)) && 
	       (fncs[(*item).index].n_arguments!=((*args).arguments[(*args).z]+1))   )
	    {
	      printf("Error: lb_pop_to_out: function %s requires %d arguments. (*args).arguments[(*args).z]=%d\r\n",
		     fncs[(*item).index].name,
		     fncs[(*item).index].n_arguments,
		     (*args).arguments[(*args).z]);
	      exit(1);
	    }

	  sum_last=0; /* Adds up the subitems for the last parameters */
	  for (n=fncs[(*item).index].n_arguments-1;n>=0;n--)
	    {
	      if ((fncs[(*item).index].n_arguments!=0) && ((*args).subitems[n][(*args).z]==0))
		{
		  printf("Error: lb_pop_to_out: function %s has a null argument No. [%d]\r\n",
			 fncs[(*item).index].name,n);
		  exit(1);
		}
	      (*fnrec).out[(*fnrec).oq].var_arg[n]=(*args).subitems[n][(*args).z];
	     
	      sum_last+=(*args).subitems[n][(*args).z];
	
	      /* We check if a parameter is a "variable", e.g., the 'x' when integrating or differentiating.
                 Also, we must make sure there isn't any other stuff.  
		 For example, diff(x^10,x+3,10.0,0.01) is invalid: {x+3} */

	   
	      if( (fncs[(*fnrec).out[(*fnrec).oq].index].var_mask>>n) & 0x01)
		{
		  if ((*fnrec).out[(*fnrec).oq].var_arg[n]>1) 
		    {
		      printf("Error: lb_pop_to_out: function %s requires a single parameter in argument No. [%d]\r\n",
			     fncs[(*item).index].name,n);
		      exit(1);
		    }
		  else
		    {
		      //lb_pa_print_output(fnrec, TEXT_COLOR_BACKGROUND+TEXT_COLOR_YELLOW);
		      //printf("DEBUG: oq=%d, sum=%d, oq-sum=%d\r\n",(*fnrec).oq,sum_last,(*fnrec).oq-sum);
		      if ((*fnrec).out[(*fnrec).oq-sum_last].type!=st_variable)
			{
			  printf("Error: lb_pop_to_out: function %s requires a variable in argument No. [%d]\r\n",
				 fncs[(*item).index].name,n);
			  exit(1);
			}
		    }
		}
	    }
	  (*args).z--;
	  (*args).subitems[ (*args).arguments[(*args).z   ]] [(*args).z  ]    +=
          (*args).subitems[ (*args).arguments[(*args).z+1 ]] [(*args).z+1] + 1;
	     
	  //printf("(*args).arguments[%d][%d] = %d\r\n",(*args).arguments[(*args).z],(*args).z,(*args).subitems[(*args).arguments[(*args).z]][(*args).z]);
	  (*fnrec).arg_count-=fncs[(*item).index].n_arguments-1;	    
	}
    }
   lb_pa_inc_out("pop_to_out()-->inc oq",&(*fnrec).oq);
  if ((*s) <= 0)
    {
      printf("Error: lb_pa_pop() st<=0: %d\r\n",*s);
      exit(1);
    }
  (*s)--;
}

void lb_pa_inc_out(char *debug_msg,  SINT16_T *var)
{
  if ((*var+1)>= MAX_STACK_SIZE)
    {
      printf("Error: lb_pa_inc_out() called by %s: oq [%d]>=MAX_STACK_SIZE [%d]\r\n",debug_msg,*var,MAX_STACK_SIZE-1);
      exit(1);
    }
  (*var)++;
}

SINT16_T lb_pa_startpos(FN_RECORD_T *fnrec, SINT16_T i, SINT16_T argument)
{
  SINT16_T n, sum_args;

  if ((*fnrec).out[i].type!=st_function)
    {
      printf("Error: lb_pa_startpos:  parameter is not a function, index=%d\r\n",i);
      exit(1);
    }
  sum_args=0;
  //printf("STARTPOS i=%d, argument=%d, ",i,argument);
  for (n=0;n<(fncs[(*fnrec).out[i].index].n_arguments-argument);n++)
    {
      //printf("arg[%d]=%d ",fncs[(*fnrec).out[i].index].n_arguments-n-1,
      //     (*fnrec).out[i].var_arg[fncs[(*fnrec).out[i].index].n_arguments-n-1]);
      sum_args+=(*fnrec).out[i].var_arg[fncs[(*fnrec).out[i].index].n_arguments-n-1];
    }
  //printf(" pos= %d\r\n",i-sum_args);
  return i-sum_args;
}

REAL_T lb_pa_eval_real(FN_RECORD_T *fnrec, REAL_T *values, MATHERROR_T *error)
{
  REAL_T F[MAX_STACK_SIZE];
  SINT16_T i, k, n, temp_pos;
  REAL_T temp_float;

  /* Special constants */
  REAL_T max_float;
  //REAL_T min_float;

  FN_RECORD_T fnrec1;                  

  if (sizeof(REAL_T)==4)
    {
      max_float=FLT_MAX;
      //  min_float=FLT_MIN;
    } else
    if (sizeof(REAL_T)==8)
      {
	max_float=DBL_MAX;
	//  min_float=DBL_MIN;
      } else
      {
  	printf("Error: lb_pa_eval(): invalid floating data type: [size = %zd]\r\n",sizeof(REAL_T));
  	exit(1);
      }
    //printf("Max_float = %.12e\r\n",max_float); 
  //printf("Min_float = %.12e\r\n",min_float);
  //printf("1/Min_float = %.12e\r\n",1/min_float);

  k=0;
  i=0;
  errno=0;

  while(i<(*fnrec).oq)    
    {
      switch ((*fnrec).out[i].type)
	{
	case st_constant: 
	  if (fabs((*fnrec).out[i].float_value)<max_float)
	    F[k]=(*fnrec).out[i].float_value;
	  else
	    {
	      F[k]=0;
	      *error=e_too_big;
	    }
	  k++;
	  break;
	case st_variable:
	  if (fabs(values[(*fnrec).out[i].index])<max_float)
	    F[k]=values[(*fnrec).out[i].index];
	  else
	    {
	      F[k]=0;
	      *error=e_too_big;
	    }
	  k++;
	  break;
	case st_function:
	  switch ((*fnrec).out[i].index)
	    {
	    case fn_sin:
	      F[k-1]=sin(F[k-1]);
	      break;
	    case fn_cos:
	      F[k-1]=cos(F[k-1]);
	      break;
	    case fn_tan:
	      F[k-1]=lb_re_tan(F[k-1],error);
	      break;
	    case fn_asin:
	      F[k-1]=lb_re_asin(F[k-1],error);
	      break;
	    case fn_acos:
	      F[k-1]=lb_re_acos(F[k-1],error);
	      break;
	    case fn_atan:
	      F[k-1]=atan(F[k-1]);
	      break;
	    case fn_sinh:
	      F[k-1]=lb_re_sinh(F[k-1],error);
	      break;
	    case fn_cosh:
	      F[k-1]=lb_re_cosh(F[k-1],error);
	      break;
	    case fn_tanh:
	      F[k-1]=tanh(F[k-1]);
	      break;
	    case fn_asinh:
	      F[k-1]=asinh(F[k-1]);
	      break;
	    case fn_acosh:
	      F[k-1]=lb_re_acosh(F[k-1],error);
	      break;
	    case fn_atanh:
	      F[k-1]=lb_re_atanh(F[k-1],error);
	      break;
	    case fn_exp:
	      F[k-1]=lb_re_exp(F[k-1],error);
	      break;
	    case fn_ln:
	      F[k-1]=lb_re_ln(F[k-1],error);
	      break;
	    case fn_log:
	      F[k-1]=lb_re_log(F[k-1],error);
	      break;
	    case fn_log2:
	      F[k-1]=lb_re_log2(F[k-1],error);
	      break;
	    case fn_max2:
	      F[k-2]=lb_re_max(F[k-2],F[k-1]);
	      k--;
	      break;
	    case fn_min2:
	      F[k-2]=lb_re_min(F[k-2],F[k-1]);
	      k--;
	      break;
	    case fn_if:
	      if (!lb_re_equal(F[k-3],0))
		  F[k-3]=F[k-2];
	      else
		F[k-3]=F[k-1];
	      k-=2;
	      break;
	    case fn_sqr:
	      F[k-1]=lb_re_sqr(F[k-1],error);
	      break;
	    case fn_sqrt:
	      F[k-1]=lb_re_sqrt(F[k-1],error);
	      break; // fn_erf, fn_ erfc, fn_rand, fn_marsa
	    case fn_abs:
	      F[k-1]=fabs(F[k-1]);
	      break;
	    case fn_erf:
	      F[k-1]=lb_st_erf(F[k-1]);
	      break;
	    case fn_erfc:
	      F[k-1]=lb_st_erfc(F[k-1]);
	      break;
	    case fn_rand:
	      F[k]=lb_st_frand(); 
	      k++;
	      break;
	    case fn_marsa:
	      F[k-1]=lb_st_marsaglia_polar(F[k-1]);
	      break;
	    case fn_dstd:
	      F[k-1]=lb_st_gauss_area(F[k-1]);
	      break;
	    case fn_avg: 
	      temp_float=0.0;
	      //printf("var_arg=%d\r\n",(*fnrec).out[i].var_arg);
	      for (n=(*fnrec).out[i].var_arg[0];n>0;n--)
		temp_float+=F[k-n];
	      F[k-(*fnrec).out[i].var_arg[0]]=temp_float/(*fnrec).out[i].var_arg[0];
	      k+=1-(*fnrec).out[i].var_arg[0];
	      break;
	    case fn_idef:
	      fnrec1.oq=0;
	      
	      temp_pos=lb_pa_startpos(fnrec,i,2);
	      for(n=0;n<(*fnrec).out[i].var_arg[2];n++)
	      {
		fnrec1.out[fnrec1.oq]=(*fnrec).out[temp_pos+n];
		lb_pa_inc_out("lb_pa_eval()-->fn_idef",&(fnrec1.oq));
	      }
	      //lb_pa_print_output(&fnrec1, TEXT_COLOR_BACKGROUND+TEXT_COLOR_YELLOW);
	      
	      if(!lb_re_is_int(F[k-1]))
		*error=e_arg_non_int;
   	
	      if ((*error)!=e_none)
		return 0.0;
		  
	      temp_float=lb_nu_simpson(F[k-5],F[k-4],&fnrec1,(*fnrec).out[lb_pa_startpos(fnrec,i,3)].index,F[k-1],values,error);
	    	     	      
	      F[k- fncs[(*fnrec).out[i].index].n_arguments]=temp_float;
	      k+=1-fncs[(*fnrec).out[i].index].n_arguments;
	      break;
	    case fn_idef2:
	      fnrec1.oq=0;
	     
	      /* This section copies the function parameter within IDEF2 */
	      temp_pos=lb_pa_startpos(fnrec,i,4);
	      for(n=0;n<(*fnrec).out[i].var_arg[4];n++)
	      {
		fnrec1.out[fnrec1.oq]=(*fnrec).out[temp_pos+n];
		lb_pa_inc_out("lb_pa_eval()-->fn_idef2",&(fnrec1.oq));
	      }
	      lb_pa_print_output(&fnrec1, TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLUE);
	      
	      if(!lb_re_is_int(F[k-1]))
		*error=e_arg_non_int;
		 
	      if(*error!=e_none)
		temp_float =0.0;
	      else
		temp_float=lb_nu_simpson2d(F[k-8], F[k-7], F[k-6], F[k-5], &fnrec1, 
					   (*fnrec).out[lb_pa_startpos(fnrec,i,5)].index, 
					   (*fnrec).out[lb_pa_startpos(fnrec,i,6)].index,
					   F[k-1], F[k-1], values, error);
	    
	      F[k- fncs[(*fnrec).out[i].index].n_arguments]=temp_float;
	      k+=1-fncs[(*fnrec).out[i].index].n_arguments; 
	      break;
	    case fn_sigma:
	      fnrec1.oq=0;   
	      
	      temp_pos=lb_pa_startpos(fnrec,i,2);
	      for(n=0;n<(*fnrec).out[i].var_arg[2];n++)
		{
		  fnrec1.out[fnrec1.oq]=(*fnrec).out[temp_pos+n];
		  lb_pa_inc_out("lb_pa_eval()-->fn_sigma",&(fnrec1.oq));
		}
	      	  
	      temp_float=lb_nu_sigma(F[k-5],F[k-4],&fnrec1,(*fnrec).out[lb_pa_startpos(fnrec,i,3)].index,F[k-1],values,error);
	    	     	      
	      F[k- fncs[(*fnrec).out[i].index].n_arguments]=temp_float;
	      k+=1-fncs[(*fnrec).out[i].index].n_arguments;
	      break;
	    case fn_prod:
	      fnrec1.oq=0;
	      
	      temp_pos=lb_pa_startpos(fnrec,i,2);
	      for(n=0;n<(*fnrec).out[i].var_arg[2];n++)
		{
		  fnrec1.out[fnrec1.oq]=(*fnrec).out[temp_pos+n];
		  lb_pa_inc_out("lb_pa_eval()-->fn_prod",&(fnrec1.oq));
		}
	      
	      lb_pa_print_output(&fnrec1, TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLUE);
	      
	      temp_float=lb_nu_prod(F[k-5],F[k-4],&fnrec1,(*fnrec).out[lb_pa_startpos(fnrec,i,3)].index,F[k-1],values,error);
	      printf("temp_float = %f\r\n",temp_float);
	      F[k- fncs[(*fnrec).out[i].index].n_arguments]=temp_float;
	      k+=1-fncs[(*fnrec).out[i].index].n_arguments;
	      break;
	    case fn_diff:
	      fnrec1.oq=0;
	      temp_pos=lb_pa_startpos(fnrec,i,0);
	      for(n=0;n<(*fnrec).out[i].var_arg[0];n++)
		{
		  fnrec1.out[fnrec1.oq]=(*fnrec).out[temp_pos+n];
		  lb_pa_inc_out("lb_pa_eval()-->fn_diff",&(fnrec1.oq));
		}
	      lb_pa_print_output(&fnrec1,TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLUE);
  	      
	      temp_float=lb_nu_diff(&fnrec1, (*fnrec).out[lb_pa_startpos(fnrec,i,1)].index, values, F[k-2], F[k-1], error);
	      if ((*error)!=e_none)
		return 0;
	      F[k- fncs[(*fnrec).out[i].index].n_arguments]=temp_float;
	      k+=1-fncs[(*fnrec).out[i].index].n_arguments;
	      break;
	    case fn_diff2:
	      fnrec1.oq=0;
	      temp_pos=lb_pa_startpos(fnrec,i,0);
	      for(n=0;n<(*fnrec).out[i].var_arg[0];n++)
		{
		  fnrec1.out[fnrec1.oq]=(*fnrec).out[temp_pos+n];
		  lb_pa_inc_out("lb_pa_eval()-->fn_diff2",&(fnrec1.oq));
		}
	      lb_pa_print_output(&fnrec1,TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLUE);
  	      
	      temp_float=lb_nu_diff2(&fnrec1, (*fnrec).out[lb_pa_startpos(fnrec,i,1)].index, values, F[k-2], F[k-1], error);
	      if ((*error)!=e_none)
		return 0;
	      F[k- fncs[(*fnrec).out[i].index].n_arguments]=temp_float;
	      k+=1-fncs[(*fnrec).out[i].index].n_arguments;
	      break;
	    }
	  break;
    case st_separator:
      printf("Error: lb_pa_eva22l(): separator in output queue, i=%d\r\n",i);
      break;
    case st_operator:
	  switch ((*fnrec).out[i].index)
	    {
	    case op_positive:
	      break;
	    case op_negative:
	      //printf("&&&& F[%d]=%f\r\n",k-1,F[k-1]);
	      if (F[k-1]!=0)
		F[k-1]=-F[k-1];
	      break;
	    case op_factorial:
	      F[k-1]=lb_re_factorial(F[k-1],error);
	      break;
	    case op_power:
	      F[k-2]=lb_re_pow(F[k-2],F[k-1], error);
	      k--;
	      break;
	    case op_division:
	      F[k-2]=lb_re_divide(F[k-2],F[k-1], error);
	      k--;
	      break;
	    case op_product:
	      F[k-2]*=F[k-1];
	      k--;
	      break;
	    case op_substraction:
	      F[k-2]-=F[k-1];
	      k--;
	      break;
	    case op_addition:
	      F[k-2]+=F[k-1];
	      k--;
	      break;
	    case op_equal_than:
	      F[k-2]=lb_re_equal(F[k-2],F[k-1]);
	      k--;
	      break;
	    case op_larger_than:
	      if (F[k-2]>F[k-1])
		F[k-2]=1.0;
	      else
		F[k-2]=0.0;
	      k--;
	      break;
	    case op_smaller_than:
	      if (F[k-2]<F[k-1])
		F[k-2]=1.0;
	      else
		F[k-2]=0.0;
	      k--;
	      break;
	    case op_different_than:
	      F[k-2]=!lb_re_equal(F[k-2],F[k-1]);
	      k--;
	      break;
	    case op_larger_or_equal_than:
	      F[k-2]=lb_re_larger_or_equal(F[k-2],F[k-1]);
	      k--;
	      break;
	    case op_smaller_or_equal_than:
	      F[k-2]=lb_re_smaller_or_equal(F[k-2],F[k-1]);
	       k--;
	      break;
	    }
	   break;
	case st_lpar:
	  printf("Error: lb_pa_eval(): left parenthesis in stack \r\n");
	  break;
	case st_rpar:
	  printf("Error: lb_pa_eval(): right parenthesis in stack \r\n");
	  break;
	}
      // printf("Completed i= %d  ",i);
      // lb_pa_print_item((*fnrec).out[i], values);

      if ((*error)!=e_none)
	return 0.0;
      i++;
    }
  if (k!=1)
    {
      printf("Error: lb_pa_eval(): output queue has more than one element: %d\r\n",k);
      for (i=0;i<k;i++)
      	printf("***F[%i]=%f\r\n",i,F[i]);
      return 0;
    }
  else
    {
      //printf("*** Ending Eval.  Result = %f \r\n", F[0]);
      return F[0];
    }
}

COMPLEX_T lb_pa_eval_complex(FN_RECORD_T *fnrec, COMPLEX_T *values, MATHERROR_T *error)
{
  COMPLEX_T F[MAX_STACK_SIZE];
  SINT16_T i, k, n, temp_pos;
  COMPLEX_T temp_z;
 
  /* Special constants */
  REAL_T max_float;
  //REAL_T min_float;

  FN_RECORD_T fnrec1;                  

  if(*error!=e_none)
    return ZERO_C;

  if (sizeof(REAL_T)==4)
    {
      max_float=FLT_MAX;
      //min_float=FLT_MIN;
    } else
    if (sizeof(REAL_T)==8)
      {
	max_float=DBL_MAX;
	//min_float=DBL_MIN;
      } else
      {
	printf("Error: lb_pa_eval(): invalid floating data type: [size = %zd]\r\n",sizeof(REAL_T));
	exit(1);
      }
  
  //printf("Max_float = %.12e\r\n",max_float); 
  //printf("Min_float = %.12e\r\n",min_float);
  //printf("1/Min_float = %.12e\r\n",1/min_float);

  k=0;
  i=0;
  errno=0;
  
  while(i<(*fnrec).oq)    
    {
      switch ((*fnrec).out[i].type)
	{
	case st_constant: 
	  if (fabs((*fnrec).out[i].float_value)<max_float)
	    {
	      F[k].r=(*fnrec).out[i].float_value;
	      F[k].i=0;
	    }
	  else
	    {
	      F[k]=ZERO_C;
	      *error=e_too_big;
	    }
	  k++;
	  break;
	case st_variable:
	  if ( (fabs(values[(*fnrec).out[i].index].r)<max_float) && (fabs(values[(*fnrec).out[i].index].i)<max_float) )
	    F[k]=values[(*fnrec).out[i].index];
	  else
	    {
	      F[k]=ZERO_C;
	      *error=e_too_big;
	    }
	  k++;
	  break;
	case st_function:
	  switch ((*fnrec).out[i].index)
	    {
	    case fn_sin:
	      F[k-1]=lb_cp_sin(F[k-1],error);
	      break;
	    case fn_cos:
	      F[k-1]=lb_cp_cos(F[k-1],error);
	      break;
	    case fn_tan:
	      F[k-1]=lb_cp_tan(F[k-1],error);
	      break;
	    case fn_asin:
	      F[k-1]=lb_cp_asin(F[k-1],error);
	      break;
	    case fn_acos:
	      F[k-1]=lb_cp_acos(F[k-1],error);
	      break;
	    case fn_atan:
	      F[k-1]=lb_cp_atan(F[k-1],error);;
	      break;
	    case fn_sinh:
	      F[k-1]=lb_cp_sinh(F[k-1],error);;
	      break;
	    case fn_cosh:
	      F[k-1]=lb_cp_cosh(F[k-1],error);;
	      break;
	    case fn_tanh:
	      F[k-1]=lb_cp_tanh(F[k-1],error);;
	      break;
	    case fn_asinh:
	      F[k-1]=lb_cp_asinh(F[k-1],error);;
	      break;
	    case fn_acosh:
	      F[k-1]=lb_cp_acosh(F[k-1],error);;
	      break;
	    case fn_atanh:
	      F[k-1]=lb_cp_atanh(F[k-1],error);;
	      break;
	    case fn_exp:
	      F[k-1]=lb_cp_exp(F[k-1],error);
	      break;
	    case fn_ln:
	      F[k-1]=lb_cp_ln(F[k-1],error);
	      break;
	    case fn_log:
	      F[k-1]=lb_cp_log(F[k-1],error);
	      break;
	    case fn_log2:
	      F[k-1]=lb_cp_log2(F[k-1],error);
	      break;
	    case fn_logb:
	      F[k-2]=lb_cp_logb(F[k-2],F[k-1],error);
	      k--;
	      break;
	    case fn_max2:
	      if (lb_cp_abs(F[k-2]) < lb_cp_abs(F[k-1]))
		F[k-2]=F[k-1];
	      k--;
	      break;
	    case fn_min2:
	      if (lb_cp_abs(F[k-2]) > lb_cp_abs(F[k-1]))
		F[k-2]=F[k-1];
	      k--;
	      break;
	    case fn_if:
	      if (lb_re_equal(F[k-3].i,0))
		if (!lb_re_equal(F[k-3].r,0))
		  F[k-3]=F[k-2];
		else
		  F[k-3]=F[k-1];
	      else
		{
		  F[k-3].r=0;
		  F[k-3].i=0;
		  *error=e_arg_non_real;
		}
	      k-=2;
	      break;
	    case fn_sqr:
	      F[k-1]=lb_cp_sqr(F[k-1],error);
	      break;
	    case fn_sqrt:
	      F[k-1]=lb_cp_sqrt(F[k-1],error);
	      break; // fn_erf, fn_ erfc, fn_rand, fn_marsa
	    case fn_abs:
	      F[k-1].r=lb_cp_abs(F[k-1]);
	      F[k-1].i=0.0;
	      break;
	    case fn_erf:
	      if (lb_re_equal(F[k-1].i,0))
		F[k-1].r=lb_st_erf(F[k-1].r);
	      else
		{
		  F[k-1].r=0.0;
		  *error=e_arg_non_real;
		}     
	      F[k-1].i=0;
	      break;
	    case fn_erfc:
	      if (lb_re_equal(F[k-1].i,0))
		F[k-1].r=lb_st_erfc(F[k-1].r);
	      else
		{
		  F[k-1].r=0.0;
		  *error=e_arg_non_real;
		}     
	      F[k-1].i=0;
	      break;
	    case fn_rand:
	      F[k].r=lb_st_frand(); 
	      F[k].i=lb_st_frand(); 
	      k++;
	      break;
	    case fn_marsa:
	      if (lb_re_equal(F[k-1].i,0))
		{
		  lb_st_marsaglia_polar2(F[k-1].r, &temp_z.r, &temp_z.i);
		  F[k-1]=temp_z;
		}
	      else
		{
		  F[k-1]=ZERO_C;
		  *error=e_arg_non_real;
		}
	      break;
	    case fn_dstd:
	      if (lb_re_equal(F[k-1].i,0))
		F[k-1].r=lb_st_gauss_area(F[k-1].r);
	      else
		{
		  F[k-1].r=0.0;
		  *error=e_arg_non_real;
		}     
	      F[k-1].i=0;
	      break;
	    case fn_real:
	      F[k-1].r=lb_cp_re(F[k-1]);
	      F[k-1].i=0.0;
	      break;
	    case fn_imag:
	      F[k-1].r=lb_cp_im(F[k-1]);
	      F[k-1].i=0.0;
	      break;
	    case fn_polar:
	      if (lb_re_equal(F[k-2].i,0) && lb_re_equal(F[k-1].i,0)) 
		F[k-2]=lb_cp_polar_to_rect(F[k-2].r,F[k-1].r);
	      else
		{
		  *error=e_arg_non_real;
		  F[k-2]=ZERO_C;
		}
	      k--;
	      break;
	    case fn_rect:
	      if (lb_re_equal(F[k-2].i,0) && lb_re_equal(F[k-1].i,0)) 
		F[k-2]=lb_cp_rect_to_polar(F[k-2].r,F[k-1].r);
	      else
		{
		  *error=e_arg_non_real;
		  F[k-2]=ZERO_C;
		}
	      k--;
	      break;
	    case fn_avg: 
	      temp_z=ZERO_C;
	      //printf("var_arg=%d\r\n",(*fnrec).out[i].var_arg);
	      for (n=(*fnrec).out[i].var_arg[0];n>0;n--)
		temp_z=lb_cp_add(temp_z,F[k-n]);
	      F[k-(*fnrec).out[i].var_arg[0]]=lb_cp_multiply_real(temp_z,1.0/(*fnrec).out[i].var_arg[0]);
	      k+=1-(*fnrec).out[i].var_arg[0];
	      break;
	    case fn_idef:
	      fnrec1.oq=0;
	      
	      temp_pos=lb_pa_startpos(fnrec,i,2);
	      for(n=0;n<(*fnrec).out[i].var_arg[2];n++)
	      {
		fnrec1.out[fnrec1.oq]=(*fnrec).out[temp_pos+n];
		lb_pa_inc_out("lb_pa_eval()-->fn_idef",&(fnrec1.oq));
	      }
	      
	      lb_pa_print_output(&fnrec1, TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLUE);

	      if(!lb_re_equal(F[k-1].i,0))
		*error=e_arg_non_real;
	      
	      if(!lb_re_is_int(F[k-1].r))
		*error=e_arg_non_int;
   	
	      if ((*error)!=e_none)
		return ZERO_C;
		  
	      temp_z=lb_nu_simpson_c(F[k-5],F[k-4],&fnrec1,(*fnrec).out[lb_pa_startpos(fnrec,i,3)].index,F[k-1].r,values,error);
	    	     	      
	      F[k- fncs[(*fnrec).out[i].index].n_arguments]=temp_z;
	      k+=1-fncs[(*fnrec).out[i].index].n_arguments;
	      break;
	    case fn_idef2:
	      fnrec1.oq=0;
	      /* This section copies the function parameter within IDEF2 */
	      temp_pos=lb_pa_startpos(fnrec,i,4);
	      for(n=0;n<(*fnrec).out[i].var_arg[4];n++)
	      {
		fnrec1.out[fnrec1.oq]=(*fnrec).out[temp_pos+n];
		lb_pa_inc_out("lb_pa_eval()-->fn_idef2",&(fnrec1.oq));
	      }
	      lb_pa_print_output(&fnrec1, TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLUE);
	      
	      if(!lb_re_is_int(F[k-1].r))
		*error=e_arg_non_int;
	      
	      if(*error!=e_none)
		temp_z=ZERO_C;
	      else
		temp_z=lb_nu_simpson2d_c(F[k-8], F[k-7], F[k-6], F[k-5], &fnrec1, 
					 (*fnrec).out[lb_pa_startpos(fnrec,i,5)].index, 
					 (*fnrec).out[lb_pa_startpos(fnrec,i,6)].index,
					 F[k-1].r, F[k-1].r, values, error);
	    
	      F[k- fncs[(*fnrec).out[i].index].n_arguments]=temp_z;
	      k+=1-fncs[(*fnrec).out[i].index].n_arguments; 
	      break;
	    case fn_sigma:
	      fnrec1.oq=0;
	      
	      temp_pos=lb_pa_startpos(fnrec,i,2);
	      for(n=0;n<(*fnrec).out[i].var_arg[2];n++)
		{
		  fnrec1.out[fnrec1.oq]=(*fnrec).out[temp_pos+n];
		  lb_pa_inc_out("lb_pa_eval()-->fn_sigma",&(fnrec1.oq));
		}
	      
	      lb_pa_print_output(&fnrec1, TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLUE);
	      
	      if (!(lb_re_equal(F[k-5].i,0.0) && lb_re_equal(F[k-4].i,0.0) && lb_re_equal(F[k-1].i,0.0)))
		*error=e_arg_non_real;
	       
	      if(*error!=e_none)
		return ZERO_C;
	      
	      temp_z=lb_nu_sigma_c(F[k-5].r,F[k-4].r,&fnrec1,(*fnrec).out[lb_pa_startpos(fnrec,i,3)].index,F[k-1].r,values,error);
	      
	      F[k- fncs[(*fnrec).out[i].index].n_arguments]=temp_z;
	      k+=1-fncs[(*fnrec).out[i].index].n_arguments;
	      break;
	    case fn_prod:
	      fnrec1.oq=0;
	      temp_pos=lb_pa_startpos(fnrec,i,2);
	      for(n=0;n<(*fnrec).out[i].var_arg[2];n++)
		{
		  fnrec1.out[fnrec1.oq]=(*fnrec).out[temp_pos+n];
		  lb_pa_inc_out("lb_pa_eval()-->fn_prod",&(fnrec1.oq));
		}
	      
	      lb_pa_print_output(&fnrec1, TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLUE);
	      
	      if (!(lb_re_equal(F[k-5].i,0.0) && lb_re_equal(F[k-4].i,0.0) && lb_re_equal(F[k-1].i,0.0)))
		*error=e_arg_non_real;
	      
	      if(*error!=e_none)
		return ZERO_C;
	      
	      temp_z=lb_nu_prod_c(F[k-5].r,F[k-4].r,&fnrec1,(*fnrec).out[lb_pa_startpos(fnrec,i,3)].index,F[k-1].r,values,error);
	      
	      F[k- fncs[(*fnrec).out[i].index].n_arguments]=temp_z;
	      k+=1-fncs[(*fnrec).out[i].index].n_arguments;
	      break;
	    case fn_diff:
	      fnrec1.oq=0;
	      temp_pos=lb_pa_startpos(fnrec,i,0);
	      for(n=0;n<(*fnrec).out[i].var_arg[0];n++)
		{
		  fnrec1.out[fnrec1.oq]=(*fnrec).out[temp_pos+n];
		  lb_pa_inc_out("lb_pa_eval()-->fn_diff",&(fnrec1.oq));
		}
	      lb_pa_print_output(&fnrec1,TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLUE);
  	      
	      if(!lb_re_equal(F[k-1].i,0))
		{
		  *error=e_arg_non_real;
		  return ZERO_C;
	     	}

	      temp_z=lb_nu_diff_c(&fnrec1, (*fnrec).out[lb_pa_startpos(fnrec,i,1)].index, values, F[k-2], F[k-1].r, error);
	      if ((*error)!=e_none)
		return ZERO_C;
	      F[k- fncs[(*fnrec).out[i].index].n_arguments]=temp_z;
	      k+=1-fncs[(*fnrec).out[i].index].n_arguments;
	      break; 
	    case fn_diff2:
	      fnrec1.oq=0;
	      temp_pos=lb_pa_startpos(fnrec,i,0);
	      for(n=0;n<(*fnrec).out[i].var_arg[0];n++)
		{
		  fnrec1.out[fnrec1.oq]=(*fnrec).out[temp_pos+n];
		  lb_pa_inc_out("lb_pa_eval()-->fn_diff2",&(fnrec1.oq));
		}
	      lb_pa_print_output(&fnrec1,TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLUE);
  	      
	      if(!lb_re_equal(F[k-1].i,0))
		{
		  *error=e_arg_non_real;
		  return ZERO_C;
	     	}

	      temp_z=lb_nu_diff2_c(&fnrec1, (*fnrec).out[lb_pa_startpos(fnrec,i,1)].index, values, F[k-2], F[k-1].r, error);
	      if ((*error)!=e_none)
		return ZERO_C;
	      F[k- fncs[(*fnrec).out[i].index].n_arguments]=temp_z;
	      k+=1-fncs[(*fnrec).out[i].index].n_arguments;
	      break; 
	    } 
	  break;
	case st_separator:
	  printf("Error: lb_pa_eva22l(): separator in output queue, i=%d\r\n",i);
	  break;
	case st_operator:
	  switch ((*fnrec).out[i].index)
	    {
	    case op_positive:
	      break;
	    case op_negative:
	      if (F[k-1].r!=0)
		F[k-1].r=-F[k-1].r;
	      if (F[k-1].i!=0)
		F[k-1].i=-F[k-1].i;
	      break;
	    case op_factorial:
	      if (lb_re_equal(F[k-1].i,0.0))
		F[k-1].r=lb_re_factorial(F[k-1].r,error);
	      else
		{
		  F[k-1].r=0.0;
		  *error=e_arg_non_real;
		}
	      F[k-1].i=0.0;
 	      break;
	    case op_power:
	      F[k-2]=lb_cp_power(F[k-2],F[k-1], error);
	      k--;
	      break;
	    case op_division:
	      F[k-2]=lb_cp_divide(F[k-2],F[k-1], error);
	      k--;
	      break;
	    case op_product:
	      F[k-2]=lb_cp_multiply(F[k-2],F[k-1]);
	      k--;
	      break;
	    case op_substraction:
	      F[k-2].r-=F[k-1].r;
	      F[k-2].i-=F[k-1].i;
	      k--;
	      break;
	    case op_addition:
	      F[k-2].r+=F[k-1].r;
	      F[k-2].i+=F[k-1].i;
	      k--;
	      break;
	    case op_equal_than:
	      if (lb_cp_equal(F[k-2],F[k-1]))
		F[k-2].r=1.0;
	      else
		F[k-2].r=0.0;
	      F[k-2].i=0.0;
	      k--;
	      break;
	    case op_larger_than:
	      if (lb_cp_abs(F[k-2])>lb_cp_abs(F[k-1]))
		F[k-2].r=1.0;
	      else
		F[k-2].r=0.0;
	      F[k-2].i=0.0;
	      k--;
	      break;
	    case op_smaller_than:
	      if (lb_cp_abs(F[k-2])<lb_cp_abs(F[k-1]))
		F[k-2].r=1.0;
	      else
		F[k-2].r=0.0;
	      F[k-2].i=0.0;
	      k--;
	      break;
	    case op_different_than:
	      if (lb_cp_equal(F[k-2],F[k-1]))
		F[k-2].r=0.0;
	      else
		F[k-2].r=1.0;
	      F[k-2].i=0.0;
	      k--;
	      break;
	    case op_larger_or_equal_than:
	      if ((lb_cp_abs(F[k-2])>lb_cp_abs(F[k-1])) || lb_cp_equal(F[k-2],F[k-1]))
		F[k-2].r=1.0;
	      else
		F[k-2].r=0.0;
	      F[k-2].i=0.0;
	      k--;
	      break;
	    case op_smaller_or_equal_than:
	      if ((lb_cp_abs(F[k-2])<lb_cp_abs(F[k-1])) || lb_cp_equal(F[k-2],F[k-1]))
		F[k-2].r=1.0;
	      else
		F[k-2].r=0.0;
	      F[k-2].i=0.0;
	      k--;
	      break;
	    }
	  break;
	case st_lpar:
	  printf("Error: lb_pa_eval(): left parenthesis in stack \r\n");
	  break;
	case st_rpar:
	  printf("Error: lb_pa_eval(): right parenthesis in stack \r\n");
	  break;
	}
      // printf("Completed i= %d  ",i);
      // lb_pa_print_item((*fnrec).out[i], values);
      if(*error!=e_none)
	return ZERO_C;
      i++;
    }
  if (k!=1)
    {
      printf("Error: lb_pa_eval(): output queue has more than one element: %d\r\n",k);
      for (i=0;i<k;i++)
      	printf("***F[%i]=[%0.4f , %0.4f]\r\n",i,F[i].r,F[i].i);
      return ZERO_C;
    }
  else
    {
      //printf("*** Ending Eval.  Result = %f \r\n", F[0]);
      return F[0];
    }
}

void lb_pa_shunting_yard(FN_RECORD_T *fnrec)
{
  STACK_ELEMENT_T stack[MAX_STACK_SIZE];
  SINT16_T st, i, oq_size;
  SINT8_T f_cond1, f_cond2, f_cond3;
  ARGS_T args;
  
  if ( (*fnrec).oq<=0 )
    {
      printf("Error: lb_pa_shunting_yard(): size(oq) = %d\r\n",(*fnrec).oq);
    }
  
  oq_size=(*fnrec).oq;
  (*fnrec).oq=0;
  args.z=0;
  args.arguments[args.z]=0;
  args.subitems[args.arguments[args.z]][args.z]=0;
  (*fnrec).arg_count=0;
  
  st=0;
  for (i=0;i<oq_size;i++)
    {
      //printf("i=%d\r\n",i);
      //lb_pa_print_item((*fnrec).out[i]);
      switch ((*fnrec).out[i].type)
	{
	case st_constant:
	  if (args.z>0)
	    args.subitems[args.arguments[args.z]][args.z]++;
	
	  /* Rule No. 01: 
	     If the token is a number, then add it to the output queue. */
	  if (i!=(*fnrec).oq) 
	    (*fnrec).out[(*fnrec).oq] =(*fnrec).out[i];
	  lb_pa_inc_out("shunting_yard() -> st_constant",&(*fnrec).oq);
	  (*fnrec).arg_count++;
	  break;
	case st_function:
	  args.z++;
	  args.arguments[args.z]=0;
	  args.subitems[args.arguments[args.z]][args.z]=0;

	  /* If the token is a function token, then push it onto the stack. */ 
	  stack[st]=(*fnrec).out[i];
	  lb_pa_inc_out("shunting_yard() -> st_function",&st);
	  break;
	case st_variable:
	  if (args.z>0)
	    args.subitems[args.arguments[args.z]][args.z]++;

	  /* If the token is a variable token, then push it onto the output queue. */ 
	  if (i!=(*fnrec).oq) 
	    (*fnrec).out[(*fnrec).oq]=(*fnrec).out[i];
	  lb_pa_inc_out("shunting_yard() -> st_variable",&(*fnrec).oq);
	  (*fnrec).arg_count++;
	  break;
	case st_separator:
	  /* Rule No. 03:
	     Until the token at the top of the stack is a left parenthesis, pop operators off the stack onto the output queue. 
	     If no left parentheses are encountered, either the separator was misplaced or parentheses were mismatched. */

	  while ( stack[st-1].type!= st_lpar )
	    {
	      if (st>0)
		{
		  lb_pa_pop(fnrec,&stack[st-1],&st,&args);
		}
	      else
		{
		  printf("Error: lb_pa_shunting_yard(): misplaced separator or mismatched parenthesis at %i \r\n",i);
		  exit(1);
		}
	    }
	  //printf("args.arguments[args.z] =%d\r\n",args.arguments[args.z]);

	  args.arguments[args.z]++;
	  if(args.arguments[args.z]==MAX_ARGUMENTS)
	    {
	      printf("Error: lb_pa_shunting_yard(): too many arguments [%d] > max [%d]\r\n",
		     args.arguments[args.z]+1,MAX_ARGUMENTS);
	      exit(1);
	    }
	  args.subitems[args.arguments[args.z]][args.z]=0;
	  break;
	case st_operator:
	  if (args.z>0)
	    args.subitems[args.arguments[args.z]][args.z]++;

	  //lb_pa_print_output(fnrec);
	  /* Rule 04: 
	     If the token is an operator, o1, then:
	     - while there is an operator token, o2, at the top of the operator stack, and either
	     *** o1 is left-associative and its precedence is less than or equal to that of o2, or
	     *** o1 is right associative, and has precedence less than that of o2,
	     then pop o2 off the operator stack, onto the output queue;
	     - push o1 onto the operator stack.  */
	  do
	    {
	      f_cond1=(st>0) && (stack[st-1].type==st_operator);  
	      if (f_cond1)
		{
		  f_cond2=(ops[(*fnrec).out[i].index].asso==a_left)  && 
		    (ops[(*fnrec).out[i].index].pre<=ops[stack[st-1].index].pre);
		  f_cond3=(ops[(*fnrec).out[i].index].asso==a_right) && 
		    (ops[(*fnrec).out[i].index].pre< ops[stack[st-1].index].pre);
		  if (f_cond2 || f_cond3)
		    lb_pa_pop(fnrec,&stack[st-1],&st,&args);
		}
	    } while (f_cond1 && (f_cond2 || f_cond3));
	  stack[st]=(*fnrec).out[i];
	  lb_pa_inc_out("shunting_yard() -> st_operator",&st);
	  if (ops[(*fnrec).out[i].index].op_type==op_binary)
	    (*fnrec).arg_count--;
	  break;
	case st_lpar:
	  /* Rule 05: 
	     If the token is a left parenthesis (i.e. "("), then push it onto the stack. */
	  stack[st]=(*fnrec).out[i];
	  lb_pa_inc_out("shunting_yard -> st_lpar",&st);
	  break;
	case st_rpar:
	  //lb_pa_print_stack(stack, st);
	  /* Rule 06:
	     If the token is a right parenthesis (i.e. ")"):
	     - Until the token at the top of the stack is a left parenthesis, pop operators off the stack onto the output queue.
	     - Pop the left parenthesis from the stack, but not onto the output queue.
	     - If the token at the top of the stack is a function token, pop it onto the output queue.
	     - If the stack runs out without finding a left parenthesis, then there are mismatched parentheses. */
	  while (stack[st-1].type!=st_lpar)
	    {
	      //printf("popping: st (before popping)=%d  ",st);
	      //lb_pa_print_item(stack[st-1]);
	      //printf("r\n");
	      lb_pa_pop(fnrec,&stack[st-1],&st,&args);
	      //printf("st (after popping)=%d ",st);
	      if (st<=0)
		{
		  printf("Error in Rule 06: missing left parenthesis\r\n");
		  exit(1);
		}
	    }
	  st--; /* this pops the parenthesis */
	  if (st>=1)
	    {
	      if ( (stack[st-1].type==st_function) || (stack[st-1].type==st_variable) )
		lb_pa_pop(fnrec,&stack[st-1],&st,&args);
	    }
	  else
	    if (st<0)
	      {
		printf("Error: lb_pa_shunting_yard() --> st_rpar st<0: %d\r\n",st);
		exit(1);
	      }
	  break;
	}
    } /* for */
  /* Final Rule: 
     When there are no more tokens to read: 
     - While there are still operator tokens in the stack:
     - If the operator token on the top of the stack is a parenthesis, then there are mismatched parentheses.
     - Pop the operator onto the output queue. */
  while (st>0)
    {
      if ( (stack[st-1].type==st_lpar) || (stack[st-1].type==st_rpar))
	{
	  printf("Error: mismatched parenthesis\r\n");
	  exit(1);
	}
      else
	lb_pa_pop(fnrec,&stack[st-1],&st,&args);
    }
  if ((*fnrec).arg_count!=1)
    {
      printf("Error: lb_pa_shuning_yard(): ");
      if ((*fnrec).arg_count<1) printf("missing %d argument(s)\r\n",1-(*fnrec).arg_count);
      else printf("extra %d argument(s) found\r\n",(*fnrec).arg_count);
      exit(1);
    }
}

void lb_pa_parse(char *fnstr, FN_RECORD_T *fnrec, char *vars, SINT16_T *n_vars)
{
  char vars_list[MAX_NUMBER_VARIABLES][MAX_VARIABLE_SIZE+1];
  char token[MAX_FORMULA_SIZE+1];
  SINT16_T i, k, length_fn, length_token, length_number;
  SINT8_T decimal_flag, match_flag=FALSE, match_flag2=FALSE;
  SINT8_T f_is_first, f_prev_is_lpar, f_prev_is_separator, f_prev_is_plus_or_minus, temp_operator;
  REAL_T temp_float;
 
  lb_pa_parse_vars(vars, vars_list, n_vars);

  length_fn=strlen(fnstr);
  i=0;
  (*fnrec).oq=0;

  while(i<length_fn)
    {
      /* ignore spaces */ 
      while (fnstr[i] == ' ')
	i++;
      if (fnstr[i] == '\0')
	{
	  printf("EOLN\r\n");
	  return;
	}
      
      /* process constants */
      strcpy(token,"");
      if (strchr("0123456789.", fnstr[i]))
	{
	  length_token=0;
	  token[length_token]=fnstr[i];
	  length_token++;
	  i++;
	  decimal_flag = FALSE;
	  while (isdigit(fnstr[i]) || ((fnstr[i] == '.') && (!decimal_flag)))
	    {
	      if (fnstr[i] == '.')
		decimal_flag = TRUE;
	      token[length_token]=fnstr[i];
	      length_token++;
	      i++;
	    }
	  if ((length_token == 1) && (token[0] == '.'))
	    {
	      printf("Error: lb_pa_parse(): invalid number\r\n");
	      exit(1);
	    }
	  if (toupper(fnstr[i]) == 'E')
	    {
	      token[length_token]=toupper(fnstr[i]);
	      length_token++;
	      i++;
	      if(strchr("+-", fnstr[i]) != NULL)
		{
		  token[length_token]=fnstr[i];
		  length_token++;
		  i++;
		}
	      length_number=0;
	      while (isdigit(fnstr[i]) && (length_number < 3))
		{
		  token[length_token]=fnstr[i];
		  length_token++;
		  length_number++;
		  i++;
		}
	    }
	  token[length_token]='\0';
	  temp_float=atof(token);
	  if (errno == ERANGE)
	    {
	      printf("Error: lb_pa_parse(): conversion error\r\n");
	      exit(1);
	    }
	  else
	    {
	      /* Rule No. 01: 
		 If the token is a number, then add it to the output queue. */
	      
	      (*fnrec).out[(*fnrec).oq].type= st_constant;
	      (*fnrec).out[(*fnrec).oq].float_value=temp_float;
	      lb_pa_inc_out("process_constant",&(*fnrec).oq);
	    }
	} /* end of "if_is_number()" [process constants] */
      else
	if (isalpha(fnstr[i]))
	  {
	    strcpy(token,"");
	    length_token=0;
	    token[length_token]=toupper(fnstr[i]);
	    length_token++;
	    i++;
	    while (isalpha(fnstr[i]) || isdigit(fnstr[i]) || (fnstr[i] == '_') )
	      {
		token[length_token]=toupper(fnstr[i]);
		length_token++;
		i++;
	      }
	    token[length_token]='\0';
	    // working here
	    k=0;
	    match_flag=FALSE;
	    
	    while ((k<MAX_CONSTANTS) && !match_flag)
	      {
		if(!strcmp(token,cons[k].name))
		  {
		    (*fnrec).out[(*fnrec).oq].type= st_constant;
		    (*fnrec).out[(*fnrec).oq].float_value=cons[k].value;
		    lb_pa_inc_out("lb_pa_parse()-->literal constant",&(*fnrec).oq);
		    match_flag=TRUE;
		  }
		k++;
	      }
	    k=0;
	    while ((k<(*n_vars)) && !match_flag)
	      {
		if(!strcmp(token,vars_list[k]))
		  {
		    (*fnrec).out[(*fnrec).oq].type= st_variable;
		    (*fnrec).out[(*fnrec).oq].index=k;
		    lb_pa_inc_out("lb_pa_parse()-->variable",&(*fnrec).oq);
		    match_flag=TRUE;
		  }
		k++;
	      }
	    if (!match_flag)
	      {
		k=0;
		while((k<MAX_FUNCTIONS) && !match_flag)
		  {
		    if(!strcmp(token,fncs[k].name))
		      {
			if (fnstr[i]!='(')
			  {
			    printf("Error: lb_pa_parse(): function must be followed by parenthesis\r\n");
			    exit(1);
			  }
			(*fnrec).out[(*fnrec).oq].type= st_function;
			(*fnrec).out[(*fnrec).oq].index=k;
		
			lb_pa_inc_out("lb_pa_parse()-->function",&(*fnrec).oq);
			match_flag=TRUE;
		      }
		    k++;
		  }
		if (!match_flag)
		  {
		    printf("Error: lb_pa_parse(): unknown variable or function: %s\r\n", token);
		    exit(1);
		  }
	      }
	  } /* end of "if_is_alpha()" [process variable or function] */
	else 
	  {	 
	    switch(fnstr[i])
	      {
	      case ',':
		(*fnrec).out[(*fnrec).oq].type=st_separator;
		lb_pa_inc_out("lb_pa_parse()-->separator",&(*fnrec).oq);
		break;
	      case '(':
		(*fnrec).out[(*fnrec).oq].type=st_lpar;
		lb_pa_inc_out("lb_pa_parse()-->lpar",&(*fnrec).oq);
		break;
	      case ')': 
		(*fnrec).out[(*fnrec).oq].type=st_rpar;
		lb_pa_inc_out("lb_pa_parse()-->rpar",&(*fnrec).oq);
		break;
	      default:
		match_flag=FALSE;
		k=0;
		while ((k<MAX_OPERATORS_DUAL) && !match_flag)
		  {
		    f_is_first     = (*fnrec).oq==0;
		    if(!f_is_first)
		      {
			f_prev_is_plus_or_minus = ((*fnrec).out[(*fnrec).oq-1].type==st_operator) && 
			  ( (ops[(*fnrec).out[(*fnrec).oq-1].index].symbol[0]=='+') || 
			    (ops[(*fnrec).out[(*fnrec).oq-1].index].symbol[0]=='-') );
			f_prev_is_lpar      = (*fnrec).out[(*fnrec).oq-1].type==st_lpar;
			f_prev_is_separator = (*fnrec).out[(*fnrec).oq-1].type==st_separator;
		      }
		    //printf("f_is_first = %d\r\n",f_is_first);
		    //printf("f_prev_is_op = %d\r\n",f_prev_is_op);
		    //printf("f_prev_is_lpar = %d\r\n",f_prev_is_lpar);
		    		    	
		    if ((fnstr[i]==ops[k].symbol[0])  && 
			(f_is_first  || ((!f_is_first) && (f_prev_is_plus_or_minus || f_prev_is_lpar || f_prev_is_separator))))
		      {
			match_flag=TRUE;
			(*fnrec).out[(*fnrec).oq].type=st_operator;
			(*fnrec).out[(*fnrec).oq].index=k;
			lb_pa_inc_out("lb_pa_parse()-->operator dual",&(*fnrec).oq);
		      }
		    k++;
		  }
		while ((k<MAX_OPERATORS) && !match_flag)
		  {
		    if (fnstr[i]==ops[k].symbol[0])
		      {
			temp_operator=k;
			match_flag=TRUE;
			(*fnrec).out[(*fnrec).oq].type=st_operator;
			match_flag2=FALSE;
			while ((k<MAX_OPERATORS) && !match_flag2)
			  {
			    if ((fnstr[i]==ops[k].symbol[0]) && (fnstr[i+1]==ops[k].symbol[1]))
			      {
				printf("\r\n");
				printf("fnstr[i] = %c\r\n",fnstr[i]);
				printf("ops[k].symbol = %s\r\n",ops[k].symbol);
				printf("ops[k].symbol[0] = %c\r\n",ops[k].symbol[0]);
				printf("k = %d\r\n",k);
				temp_operator=k;
				i++;
			      }
			    k++;
			  }
			(*fnrec).out[(*fnrec).oq].index=temp_operator;
			lb_pa_inc_out("lb_pa_parse()-->operator",&(*fnrec).oq);
		      }
		    k++;
		  }
		if (!match_flag)
		  {
		    printf("Error: lb_pa_parse(): Invalid character=%c \r\n",fnstr[i]);
		    exit(1);
		  }
		break;
	      }
	    i++;
	  }
    }
}

void lb_pa_parse_vars(char *vars, char vars_list[MAX_NUMBER_VARIABLES][MAX_VARIABLE_SIZE+1], SINT16_T *n_vars)
/* This function parses the variables in a null-terminated strings
   Rules:  1. Variables must start with a letter 
           2. Variables can be followed by letters or numbers
           3. Other characters are considered invalid 
           4. Variables are not case sensitive */
{
  SINT16_T i, j, k, length_string;
  char token[MAX_VARIABLE_SIZE+1];

  /* Parsing the variables */
  *n_vars=0;
  
  strcpy(token,"");
  i=0;   j=0;
  length_string=strlen(vars);
  while(i<length_string)
    {
      while(vars[i]==' ')
	i++;
      if(isalpha(vars[i]))
	{
	  token[j]=toupper(vars[i]);
	  j++;
	  i++;
	  while( isalpha(vars[i]) || isdigit(vars[i]) || (vars[i]=='_') )
	    {
	      token[j]=toupper(vars[i]);
	      j++;
	      i++;
	      if (j>MAX_VARIABLE_SIZE)
		{
		  printf("Error: lb_pa_parse_vars(): variable is too long: %d\r\n",j);
		  return;
		}
	    }
	  while(vars[i]==' ')
	    i++;
	  if ( (vars[i]==',') || (vars[i]=='\0') )
	    {
	      token[j]='\0';

	      if ((*n_vars+1)>MAX_NUMBER_VARIABLES)
		{
		  printf("Error: lb_pa_parse_vars(): too many_variables: %d\r\n",*n_vars);
		  exit(1);
		}
	      for (k=0;k<MAX_CONSTANTS;k++)
		{
		  if(!strcmp(token,cons[k].name))
		    {
		      printf("Error: lb_pa_parse_vars(): variable named as built-in constant: %s\r\n",cons[k].name);
		      exit(1);
		    }
		}
	      for (k=0;k<*n_vars;k++)
		if(!strcmp(token,vars_list[k]))
		  {
		    printf("Error: lb_pa_parse_vars(): duplicate variable: %s\r\n",vars_list[k]);
		    exit(1);
		  }
      	      for (k=0;k<MAX_FUNCTIONS;k++)
		if(!strcmp(token,fncs[k].name))
		  {
		    printf("Error: lb_pa_parse_vars(): variable named as built-in function: %s\r\n",token);
		    exit(1);
		  }
	      strcpy(vars_list[*n_vars],token);
	      strcpy(token,"");
	      j=0;
	      (*n_vars)++;
	      i++;
	      while(vars[i]==' ')
		i++;
	    }
	  else
	    {
	      printf("Error: lb_pa_parse_vars(): invalid character in variable: i=%d, char[%d]\r\n",i,vars[i]);
	      exit(1);
	    } 
	}
      else
	{
	  printf("Error: lb_pa_parse_vars(): variables must start with a letter: %c\r\n",vars[i]);
	  exit(1);
	}
    }
#ifdef DEBUG_PARSE_VARS
  printf("n variables=%d\n",*n_vars);
  for (i=0;i<*n_vars;i++)
    printf(" var[%d] {%d}= %s\r\n",i,strlen(vars_list[i]),vars_list[i]);
#endif
}

void lb_pa_print_item(STACK_ELEMENT_T item)
{
  SINT16_T n;
  switch(item.type)
    {
    case st_constant: 
      printf("CONSTANT=%f\r\n",item.float_value);
      break;
    case st_variable: 
      printf("VARIABLE[%i]\r\n",item.index);
      break;
    case st_function: 
      printf("FUNCTION[%d]=%s", item.index,fncs[item.index].name);
      for (n=0;n<MAX_FIXED_ARGUMENTS;n++)
	printf(", a%d=%d",n,item.var_arg[n]);
      printf("\r\n");
      break;
    case st_separator: 
      printf("SEPARATOR\r\n");
      break;
    case st_operator: 
      printf("OPERATOR[%i]=[%s]",item.index, ops[item.index].symbol);
      if (ops[item.index].op_type==op_unary)
	printf(" [U]\r\n");
      else
	printf(" [B]\r\n");
      break;
    case st_lpar: 
      printf("LPAR\r\n");
      break;
    case st_rpar: 
      printf("RPAR\r\n");
      break;
    default:
      printf("Invalid type\r\n");
      break;
    }
}

void lb_pa_print_output(FN_RECORD_T *fnrec, SINT16_T color)
{
  lb_co_color(color);
  lb_co_cls_from_cursor();
  SINT16_T i;
  if ((*fnrec).oq==0)
    printf("Output queue is empty\r\n");
  for (i=0;i<(*fnrec).oq;i++)
    {
      printf("item %d of %d = ",i,(*fnrec).oq-1);
      lb_pa_print_item((*fnrec).out[i]);
    }
  lb_co_reset_colors();
  lb_co_cls_from_cursor();
 }

void lb_pa_print_stack(STACK_ELEMENT_T *stack, SINT16_T st)
{
  SINT16_T i;
  if (st==0)
    printf("Stack is empty\r\n");
  for (i=0;i<st;i++)
    {
      printf("item %d of %d = ",i,st-1);
      lb_pa_print_item(stack[i]);
    }
}

REAL_T lb_pa_formula(char *fnstr, char *vars, REAL_T f1, REAL_T f2, REAL_T f3, MATHERROR_T *error)
{
  FN_RECORD_T fnrec;
  REAL_T vector[3];
  SINT16_T n_vars;
 
  vector[0]=f1;
  vector[1]=f2;
  vector[2]=f3;

  lb_pa_parse(fnstr, &fnrec, vars, &n_vars);
  lb_pa_print_output(&fnrec,TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLUE);
  if (n_vars>3)
    {
      printf("Error: lb_pa_formula(): too many variables: %d\r\n",n_vars);
      exit(1);
    }
  printf("Parsed formula:\r\n");
  //lb_pa_print_output(&fnrec,TEXT_COLOR_BACKGROUND+TEXT_COLOR_GREEN);

  //printf("Before executing shunting yard:\r\n");
  lb_pa_shunting_yard(&fnrec);
  //printf("After executing shunting yard:\r\n");
  lb_pa_print_output(&fnrec,TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLUE);

  //printf("After reducing:\r\n");
  //lb_pa_print_output(&fnrec,TEXT_COLOR_BACKGROUND+TEXT_COLOR_CYAN);
  
  return lb_pa_eval_real(&fnrec, vector, error);
}

COMPLEX_T lb_pa_formula_complex(char *fnstr, char *vars, COMPLEX_T f1, COMPLEX_T f2, COMPLEX_T f3, MATHERROR_T *error)
{
  FN_RECORD_T fnrec;
  COMPLEX_T vector[3];
  SINT16_T n_vars;
 
  vector[0]=f1;
  vector[1]=f2;
  vector[2]=f3;

  lb_pa_parse(fnstr, &fnrec, vars, &n_vars);
  //lb_pa_print_output(&fnrec,TEXT_COLOR_BLUE);
  lb_pa_print_output(&fnrec,TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLUE);
  if (n_vars>3)
    {
      printf("Error: lb_pa_formula(): too many variables: %d\r\n",n_vars);
      exit(1);
    }

  printf("Before executing shunting yard:\r\n");
  lb_pa_shunting_yard(&fnrec);
  printf("After executing shunting yard:\r\n");
  lb_pa_print_output(&fnrec,TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLACK);

  //printf("After reducing:\r\n");
  //lb_pa_print_output(&fnrec,TEXT_COLOR_BACKGROUND+TEXT_COLOR_CYAN);
  
  return lb_pa_eval_complex(&fnrec, vector, error);
}



