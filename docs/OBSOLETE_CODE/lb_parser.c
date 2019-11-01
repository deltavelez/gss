#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>  // for atof() error
#include <float.h>
#include <math.h>
#include "lb_real.h"
#include "lb_types.h"
#include "lb_console.h" 
#include "lb_integer.h"
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
				   { fn_exp,    "EXP",    1, 0b00000000 },
				   { fn_ln,     "LN",     1, 0b00000000 },
				   { fn_log,    "LOG",    1, 0b00000000 },
				   { fn_log2,   "LOG2",   1, 0b00000000 },
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
				   /* Functions with a variable number of arguments */
				   { fn_avg,    "AVG",   -1, 0b00000000 },
				   { fn_idef,   "IDEF",   5, 0b00000100 },
				   { fn_sigma,  "SIGMA",  5, 0b00000100 },
				   { fn_diff,   "DIFF",   4, 0b00000001 }
};

CONSTANT_T cons[MAX_CONSTANTS] = { { "PI", M_PI },
				   { "E",  M_E  } };

/* Notes
 * A domain error occurs if an input argument is outside the domain over which the mathematical function is defined.
 * A pole error (also known as a singularity or infinitary) occurs if the mathematical function has an exact infinite result as the*  finite input argument(s) are approached in the limit.
 A range error occurs if the mathematical result of the function cannot be represented in an object of the specified type, due to extreme magnitude. */

ERRORS_T errors[MAX_ERRORS] = { { e_none,                  "No errors"                                         },
                             	{ e_pole_div_zero,         "Pole error: Division by zero"                      }, 
				{ e_range_too_big,         "Range error: Number is too big"                    },
				{ e_domain_undefined,      "Funtion is undefined at point"                     },
				{ e_domain_arg_larger_one, "Domain error: Argument larger than 1.0"            },
				{ e_domain_complex,        "Domain error: rational power of a negative number" },
				{ e_arg_negative,          "Invalid argument: non-positive argument found"     },
				{ e_arg_non_int,           "Invalid argument: non-integer argument found"      },
				{ e_arg_non_even,          "Invalid argument: non-even argument found"         },
				{ e_arg_less_two,          "Invalid argument: lower-than_two argument found"   }, 
				{ e_arg_too_big,           "Invalid argument: too big"                         },
				{ e_arg_too_small,         "Invalid argument: too small"                       },
				{ e_arg_inf_loop,          "Invalid argument: infinite loop"                   }
};

void lb_pa_pop(FN_RECORD_T *fnrec, STACK_ELEMENT_T *item, S_INT_16_T *s, ARGS_T *args)
{
  S_INT_16_T n, total_subitems;

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
	} else
	{
	  if ( !((fncs[(*item).index].n_arguments==0) && ((*args).subitems[0][(*args).z]==0)) && 
	       (fncs[(*item).index].n_arguments!=((*args).arguments[(*args).z]+1)))
	    {
	      printf("Error: lb_pop_to_out: function %s requires %d arguments but %d were found\r\n",
		     fncs[(*item).index].name,
		     fncs[(*item).index].n_arguments,
		     (*args).arguments[(*args).z]+1);
	      exit(1);
	    }

	  S_INT_16_T tempo=0;

	  for(n=0;n<=(*args).arguments[(*args).z];n++)
	    {
	      if ((fncs[(*item).index].n_arguments!=0) && ((*args).subitems[n][(*args).z]==0))
		{
		  printf("Error: lb_pop_to_out: function %s has a null argument No. [%d]\r\n",
			 fncs[(*item).index].name,n);
		  exit(1);
		}
	      tempo+=(*args).subitems[n][(*args).z];
	    }
	  /* oxo */
	  for (n=0;n<fncs[(*item).index].n_arguments;n++)
	    {
	      (*fnrec).out[(*fnrec).oq].var_arg[n]=(*args).subitems[n][(*args).z];
	      // printf("(*fnrec).out[%d].var_arg[%d]=%d, special=%d\r\n",(*fnrec).oq,n,(*fnrec).out[(*fnrec).oq].var_arg[n],
	      // (fncs[(*fnrec).out[(*fnrec).oq].index].var_mask>>n) & 0x01);
	    }
	  (*args).z--;
	  if(fncs[(*fnrec).out[(*fnrec).oq].index].var_mask!=0)
	    {
	      (*args).subitems[(*args).arguments[(*args).z]][(*args).z]+=tempo+1;
	      //printf("tempo=%d\r\n",tempo);
	    }
	  else
	    (*args).subitems[(*args).arguments[(*args).z]][(*args).z]+=(*args).subitems[(*args).arguments[(*args).z+1]][(*args).z+1]+1;
	  //printf("(*args).arguments[%d][%d] = %d\r\n",
	  // (*args).arguments[(*args).z],  
	  //	 (*args).z,  
	  //	 (*args).subitems[(*args).arguments[(*args).z]][(*args).z]);
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

void lb_pa_inc_out(char *debug_msg,  S_INT_16_T *var)
{
  if ((*var+1)>= MAX_STACK_SIZE)
    {
      printf("Error: lb_pa_inc_out() called by %s: oq [%d]>=MAX_STACK_SIZE [%d]\r\n",debug_msg,*var,MAX_STACK_SIZE-1);
      exit(1);
    }
  (*var)++;
}

S_INT_8_T lb_pa_fn_can_be_reduced(FN_RECORD_T *fnrec, S_INT_16_T i, S_INT_16_T k)
{
  S_INT_8_T flag_all_constants;
  S_INT_16_T n, total_args;
  /* We assert the type of argument is a function */
  if ((*fnrec).out[i].type!=st_function)
    {
      printf("Error: lb_pa_fn_can_be_reduced:  parameter is not a function, index=%d\r\n",i);
      //lb_pa_print_output(fnrec);
      exit(1);
    }

  flag_all_constants=TRUE;
  n=0;
  //printf("(*fnrec).out[i].var_arg1 = %d\r\n",(*fnrec).out[i].var_arg1);
  if (fncs[(*fnrec).out[i].index].n_arguments==-1) /* if function has an undetermined number of arguments */
    while ((n<(*fnrec).out[i].var_arg[0]) && flag_all_constants)
      {
	if ((*fnrec).out[k-n-1].type!=st_constant) 
	  flag_all_constants=FALSE;
	n++;
      }
  else /* if function has a pre-determined number of arguments */
    {
      total_args=0;
      for(n=0;n<fncs[(*fnrec).out[i].index].n_arguments;n++)
	{
	  if ((fncs[(*fnrec).out[i].index].var_mask>>n) & 0x01)
	    total_args+=(*fnrec).out[i].var_arg[n];
	  else
	    total_args++;
	}
      n=0;
      while ((n<total_args) && flag_all_constants)
	{
	  if ((*fnrec).out[k-n-1].type!=st_constant) 
	    flag_all_constants=FALSE;
	  n++;
	}
    }
  return flag_all_constants;
}

S_INT_16_T lb_pa_startpos(FN_RECORD_T *fnrec, S_INT_16_T i, S_INT_16_T argument)
{
  S_INT_16_T n, sum_args;

  if ((*fnrec).out[i].type!=st_function)
    {
      printf("Error: lb_pa_startpos:  parameter is not a function, index=%d\r\n",i);
      exit(1);
    }

  sum_args=0;
  for (n=0;n<(fncs[(*fnrec).out[i].index].n_arguments-argument);n++)
    {
      //      printf("arg%d=%d\r\n",fncs[(*fnrec).out[i].index].n_arguments-n-1,
      //     (*fnrec).out[i].var_arg[fncs[(*fnrec).out[i].index].n_arguments-n-1]);
      sum_args+=(*fnrec).out[i].var_arg[fncs[(*fnrec).out[i].index].n_arguments-n-1];
    }
  return i-sum_args;
}

S_INT_8_T lb_pa_var_can_be_reduced(FN_RECORD_T *fnrec, S_INT_16_T i, S_INT_16_T k)
{
  S_INT_8_T flag;
  S_INT_16_T counter;
  /* We assert the type of argument is a function */
  if ((*fnrec).out[i].type!=st_variable)
    {
      printf("Error: lb_pa_var_can_be_reduced:  parameter is not a variable, index=%d\r\n",i);
      exit(1);
    }

  flag=TRUE;
  counter=0;
  if (fncs[(*fnrec).out[i].index].n_arguments!=-1) /* if function has a KNOWN number of arguments */
    while ((counter<fncs[(*fnrec).out[i].index].n_arguments) && flag)
      {
	if ((*fnrec).out[k-counter-1].type!=st_constant) 
	  flag=FALSE;
	counter++;
      }
  else
    {
      //      while ((counter<(*fnrec).out[i].var_arg) && flag)
	{
	  if ((*fnrec).out[k-counter-1].type!=st_constant) 
	    flag=FALSE;
	  counter++;
	}
    }
  return flag;
}

S_INT_8_T lb_pa_op_can_be_reduced(FN_RECORD_T *fnrec, S_INT_16_T i, S_INT_16_T k)
{
  S_INT_8_T flag;
  /* We assert the type of argument is an operator */
  if ((*fnrec).out[i].type!=st_operator)
    {
      printf("Error: lb_pa_fn_can_be_reduced:  parameter is not an operator, index=%d\r\n",i);
      exit(1);
    }

  flag=TRUE;
  
  if ((*fnrec).out[k-1].type!=st_constant)
    flag=FALSE;

  if (ops[(*fnrec).out[i].index].op_type==op_binary)
    {
      if ((*fnrec).out[k-2].type!=st_constant)
	flag=FALSE;
    }
  return flag;
}

void lb_pa_adjust_arguments(FN_RECORD_T *fnrec, S_INT_16_T i)
{
  S_INT_16_T n,m;
  S_INT_16_T start,end;
  
  // printf("n=%i  oq=%i\r\n",n,(*fnrec).oq);
  // printf("bool = %i\r\n",(*fnrec).items[n].type==st_function);
      
  /* Must adjust the number of arguments for certain functions */
    for (n=i;n<(*fnrec).oq;n++) 
      if((*fnrec).out[n].type==st_function)
	{ 
	  if(0 && fncs[(*fnrec).out[n].index].n_arguments==-1)
	    { 
	      if ((n-i)<=((*fnrec).out[n].var_arg[0]))
		switch((*fnrec).out[i].type)
		  {
		  case st_function:
		    printf("adjusting arguments for function %d --> delta= %d\r\n",
			   (*fnrec).out[n].index,fncs[(*fnrec).out[i].index].n_arguments);
		    (*fnrec).out[n].var_arg[0]-=fncs[(*fnrec).out[i].index].n_arguments;
		    break;
		  case st_operator:
		    if (ops[(*fnrec).out[i].index].op_type==op_binary)
		      (*fnrec).out[n].var_arg[0]-=2;
		    else
		      (*fnrec).out[n].var_arg[0]--;
		    break;
		  default:
		    printf("Error: lb_pa_adjust_arguments: unhandled operator type\r\n");
		    exit(1);
		    break;
		  }
	    }
	  else
	    {
	      // if ((fncs[(*fnrec).out[n].index].var_mask>>m) & 0x01)
	      end=n-1;	   
	      printf("----------------------------------------------\r\n");
	      m=fncs[(*fnrec).out[n].index].n_arguments-1;
	      while ((m>=0) && (end>=i))
		{
		  start=end-(*fnrec).out[n].var_arg[m]+1;
	      	  
		  printf("n=%d  start=%d  end=%d arg=%d m0=%d\r\n",
			 n,start,end,(*fnrec).out[n].var_arg[m],fncs[(*fnrec).out[n].index].n_arguments);
		  if ((start<=i) && (i<=end))
		    
		      switch((*fnrec).out[i].type)
			{
			case st_function:
			  printf("Adjusting arguments on item %d due to collapsing function. a%d=%d",
				 n,m,(*fnrec).out[n].var_arg[m]);
			  (*fnrec).out[n].var_arg[m]-=fncs[(*fnrec).out[i].index].n_arguments;
			  printf(" --> a%d=%d\r\n",m,(*fnrec).out[n].var_arg[m]);
			  break;
			case st_operator:
			  printf("Adjusting arguments on item %d due to collapsing operator. a%d=%d",
				 n,m,(*fnrec).out[n].var_arg[m]);
			  if (ops[(*fnrec).out[i].index].op_type==op_binary)
			    (*fnrec).out[n].var_arg[m]-=2;
			  else
			    (*fnrec).out[n].var_arg[m]--;
			  printf(" --> a%d=%d\r\n",m,(*fnrec).out[n].var_arg[m]);
			  break;
			default:
			  printf("Error: lb_pa_adjust_arguments: unhandled operator type\r\n");
			  exit(1);
			  break;
			}
		  end=start-1;
		  m--;
		}
	    }	    
	} /* function */
}

S_INT_8_T lb_pa_reduce_constant(FN_RECORD_T *fnrec, ERR_T *error, S_INT_16_T a, S_INT_16_T b, S_INT_16_T c, S_INT_16_T d)
{
  FLOAT_T temp_float;

#ifdef DEBUG_REDUCE_CONSTANT
  printf("-------------------------------------------------------------------------------\r\n");
  printf("fnrec[a = %d]: (expected constant) ",a);  lb_pa_print_item((*fnrec).out[a],values);
  printf("fnrec[b = %d]: (expected + or -)   ",b);  lb_pa_print_item((*fnrec).out[b],values);
  printf("fnrec[c = %d]: (expected constant) ",c);  lb_pa_print_item((*fnrec).out[c],values);
  printf("fnrec[d = %d]: (expected + or -)   ",d);  lb_pa_print_item((*fnrec).out[d],values);
#endif

  if ( (a>=0) && (b>=1) && (c>=2) && (d+1)<=(*fnrec).oq )
    { 
      if ( ((*fnrec).out[a].type==st_constant) && 		   
	   ((*fnrec).out[b].type==st_operator) && 
	   (((*fnrec).out[b].index==op_addition) || ((*fnrec).out[b].index==op_substraction)) &&
	   ((*fnrec).out[c].type==st_constant) && 		   
	   ((*fnrec).out[d].type==st_operator) && 
	   (((*fnrec).out[d].index==op_addition) || ((*fnrec).out[d].index==op_substraction)) )
	{
	  if ((*fnrec).out[b].index==op_addition)
	    temp_float = (*fnrec).out[a].float_value;
	  if ((*fnrec).out[b].index==op_substraction)
	    temp_float = -(*fnrec).out[a].float_value;
	  if ((*fnrec).out[d].index==op_addition)
	    temp_float += (*fnrec).out[c].float_value;
	  if ((*fnrec).out[d].index==op_substraction)
	    temp_float -= (*fnrec).out[c].float_value;
	
	  (*fnrec).out[a].float_value = temp_float;
	  (*fnrec).out[b].type=st_operator;
	  (*fnrec).out[b].index=op_addition;
	  lb_pa_adjust_arguments(fnrec,d);
	  return TRUE;
	}
      else
	if ( ((*fnrec).out[a].type==st_constant) && 		   
	     ((*fnrec).out[b].type==st_operator) && 		   
	     (((*fnrec).out[b].index==op_product) || ((*fnrec).out[b].index==op_division)) &&
	     ((*fnrec).out[c].type==st_constant) && 		   
	     ((*fnrec).out[d].type==st_operator) && 		   
	     (((*fnrec).out[d].index==op_product) || ((*fnrec).out[d].index==op_division)) )
	  {
	    if ((*fnrec).out[b].index==op_product)
	      temp_float = (*fnrec).out[a].float_value;
	    if ((*fnrec).out[b].index==op_division)
	      temp_float = lb_re_divide(1.0,(*fnrec).out[a].float_value,error);
	    if ((*fnrec).out[d].index==op_product)
	      temp_float *= (*fnrec).out[c].float_value;
	    if ((*fnrec).out[d].index==op_division)
	      temp_float = lb_re_divide(temp_float,(*fnrec).out[c].float_value,error);

	    (*fnrec).out[a].float_value = temp_float;
	    (*fnrec).out[b].type=st_operator;
	    (*fnrec).out[b].index=op_product;
	    lb_pa_adjust_arguments(fnrec,d); 
	    return TRUE;
      }
    }
  return FALSE;
}

void lb_pa_reduce(FN_RECORD_T *fnrec, ERR_T *error)
{
  S_INT_16_T i, k, n;
  S_INT_8_T flag_eval_error=FALSE;
  FLOAT_T temp_float;

  i=0;
  k=0;
  *error=e_none;
 
  while((i<(*fnrec).oq) && !flag_eval_error)
    {
      switch ((*fnrec).out[i].type)
	{
	case st_constant:
	  if (lb_pa_reduce_constant(fnrec, error, k-2, k-1, i, i+1))
	    i+=2;
	  else
	    {
	      (*fnrec).out[k]=(*fnrec).out[i];
	      k++;
	      i++;
	    }
	  break;
	case st_variable:
	  (*fnrec).out[k]=(*fnrec).out[i];
	  i++;
	  k++;
	  break;
	case st_function:
	  switch ((*fnrec).out[i].index)
	    {
	    case fn_sin:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=sin((*fnrec).out[k-1].float_value);
		  lb_pa_adjust_arguments(fnrec,i);
		  //if (lb_pa_reduce_constant(fnrec, error, k-3,k-2,k-1,i+1))
		  //{
		  //  k--;
		  //  i++;
		  //}
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case fn_cos:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=cos((*fnrec).out[k-1].float_value);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case fn_tan:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=lb_re_tan((*fnrec).out[k-1].float_value,error);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case fn_asin:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=lb_re_asin((*fnrec).out[k-1].float_value,error);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case fn_acos:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=lb_re_acos((*fnrec).out[k-1].float_value,error);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case fn_atan:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=atan((*fnrec).out[k-1].float_value);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i]; 
		  k++;
		}
	      i++;
	      break;
	    case fn_exp:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=lb_re_exp((*fnrec).out[k-1].float_value,error);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case fn_ln:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=lb_re_ln((*fnrec).out[k-1].float_value,error);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case fn_log:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=lb_re_log((*fnrec).out[k-1].float_value,error);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case fn_log2:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=lb_re_log2((*fnrec).out[k-1].float_value,error);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case fn_max2:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-2].float_value=lb_re_max((*fnrec).out[k-2].float_value,(*fnrec).out[k-1].float_value);
		  k--;
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case fn_min2:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-2].float_value=lb_re_min((*fnrec).out[k-2].float_value,(*fnrec).out[k-1].float_value);
		  k--;
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case fn_if:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
	      	{
		  if (!lb_re_equal((*fnrec).out[k-3].float_value,0))
		    (*fnrec).out[k-3].float_value=(*fnrec).out[k-2].float_value;
		  else
		    (*fnrec).out[k-3].float_value=(*fnrec).out[k-1].float_value;
		  k-=2;
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      i++;
	      break;
	    case fn_sqr:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=lb_re_sqr((*fnrec).out[k-1].float_value,error);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case fn_sqrt:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=lb_re_sqrt((*fnrec).out[k-1].float_value,error);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break; // fn_erf, fn_ erfc, fn_rand, fn_marsa
	    case fn_abs:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=fabs((*fnrec).out[k-1].float_value);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break; // fn_erf, fn_ erfc, fn_rand, fn_marsa
	    case fn_erf:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=lb_st_erf((*fnrec).out[k-1].float_value);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break; // fn_erf, fn_ erfc, fn_rand, fn_marsa
	    case fn_erfc:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=lb_st_erfc((*fnrec).out[k-1].float_value);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case fn_rand:
	      (*fnrec).out[k]=(*fnrec).out[i];
	      k++;
	      i++;
	      break;
	    case fn_marsa:
	      (*fnrec).out[k]=(*fnrec).out[i];
	      k++;
	      i++;
  	      break;
	    case fn_dstd:
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=lb_st_dist_std((*fnrec).out[k-1].float_value);
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case fn_avg:
	      //printf("AVG can be reduced? = %d\r\n",lb_pa_fn_can_be_reduced(fnrec,i,k));
	      if (lb_pa_fn_can_be_reduced(fnrec,i,k))
	      	{
	      	  temp_float=0.0;
	      	  for (n=(*fnrec).out[i].var_arg[0];n>0;n--)
		    temp_float+=(*fnrec).out[k-n].float_value;
		  (*fnrec).out[k-(*fnrec).out[i].var_arg[0]].float_value=temp_float/(*fnrec).out[i].var_arg[0];
		  k+=1-(*fnrec).out[i].var_arg[0];
		  lb_pa_adjust_arguments(fnrec,i);
	      	}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    default:
	      (*fnrec).out[k]=(*fnrec).out[i];
	      k++;
	      i++;
	      break;
	    }
	  break;
	case st_separator:
	  printf("Error: lb_pa_reduce(): separator in output queue \r\n");
	  break;
	case st_operator:
	  switch ((*fnrec).out[i].index)
	    {
	    case op_positive:
	      lb_pa_adjust_arguments(fnrec,i);
	      i++;
	      break;
	    case op_negative:
	      if (lb_pa_op_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-1].float_value=-(*fnrec).out[k-1].float_value;
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case op_factorial:
	      break;
	    case op_power:
	      if (lb_pa_op_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-2].float_value=lb_re_pow((*fnrec).out[k-2].float_value,
							    (*fnrec).out[k-1].float_value,error);
	  	  k--;
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case op_division:
	      if (lb_pa_op_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-2].float_value=lb_re_divide((*fnrec).out[k-2].float_value,
							       (*fnrec).out[k-1].float_value,error);
		  k--;
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case op_product:
	      if (lb_pa_op_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-2].float_value*=(*fnrec).out[k-1].float_value;
		  k--;
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case op_substraction:
	      if (lb_pa_op_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-2].float_value-=(*fnrec).out[k-1].float_value;
		  k--;
		  lb_pa_adjust_arguments(fnrec,i);
	 	}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case op_addition:
 	      if (lb_pa_op_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-2].float_value+=(*fnrec).out[k-1].float_value;
		  k--;
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case op_equal_than:
	      if (lb_pa_op_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-2].float_value=lb_re_equal((*fnrec).out[k-2].float_value,(*fnrec).out[k-1].float_value);
		  k--;
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case op_larger_than:
	      if (lb_pa_op_can_be_reduced(fnrec,i,k))
		{
		  if ((*fnrec).out[k-2].float_value>(*fnrec).out[k-1].float_value)
		    (*fnrec).out[k-2].float_value=1.0;
		  else
		    (*fnrec).out[k-2].float_value=0.0;
		  k--;
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case op_smaller_than:
	      if (lb_pa_op_can_be_reduced(fnrec,i,k))
		{
		  if ((*fnrec).out[k-2].float_value<(*fnrec).out[k-1].float_value)
		    (*fnrec).out[k-2].float_value=1.0;
		  else
		    (*fnrec).out[k-2].float_value=0.0;
		  k--;
		  lb_pa_adjust_arguments(fnrec,i);
		}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case op_different_than:
	      if (lb_pa_op_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-2].float_value=!lb_re_equal((*fnrec).out[k-2].float_value,(*fnrec).out[k-1].float_value);
		  k--;
		  lb_pa_adjust_arguments(fnrec,i);
	      	}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case op_larger_or_equal_than:
	      if (lb_pa_op_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-2].float_value=lb_re_larger_or_equal((*fnrec).out[k-2].float_value,(*fnrec).out[k-1].float_value);
		  k--;
		  lb_pa_adjust_arguments(fnrec,i);
	      	}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
	      break;
	    case op_smaller_or_equal_than:
	      if (lb_pa_op_can_be_reduced(fnrec,i,k))
		{
		  (*fnrec).out[k-2].float_value=lb_re_smaller_or_equal((*fnrec).out[k-2].float_value,(*fnrec).out[k-1].float_value);
		  k--;
		  lb_pa_adjust_arguments(fnrec,i);
	      	}
	      else
		{
		  (*fnrec).out[k]=(*fnrec).out[i];
		  k++;
		}
	      i++;
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
    }
  (*fnrec).oq=k;
}


FLOAT_T lb_pa_eval(FN_RECORD_T *fnrec, FLOAT_T *values, ERR_T *error)
{
  FLOAT_T F[MAX_STACK_SIZE];
  S_INT_8_T  flag_eval_error=FALSE;
  S_INT_16_T i, k, n;
  S_INT_16_T pos2, pos3;
  FLOAT_T temp_x, temp_float, a, b, delta;

  /* Special constants */
  FLOAT_T max_float;
  FLOAT_T min_float;

  FLOAT_T vec1[MAX_NUMBER_VARIABLES];
  FN_RECORD_T fnrec1;                  

  if (sizeof(FLOAT_T)==4)
    {
      max_float=FLT_MAX;
      min_float=FLT_MIN;
    } else
    if (sizeof(FLOAT_T)==8)
    {
      max_float=DBL_MAX;
      min_float=DBL_MIN;
    } else
      {
	printf("Error: lb_pa_eval(): invalid floating data type: [size = %d]\r\n",sizeof(FLOAT_T));
	exit(1);
      }
  
  //printf("Max_float = %.12e\r\n",max_float); 
  //printf("Min_float = %.12e\r\n",min_float);
  //printf("1/Min_float = %.12e\r\n",1/min_float);

  k=0;
  i=0;
  errno=0;

  while((i<(*fnrec).oq) && !flag_eval_error)    
    {
      switch ((*fnrec).out[i].type)
	{
	case st_constant: 
	  if (fabs((*fnrec).out[i].float_value)<max_float)
	    F[k]=(*fnrec).out[i].float_value;
	  else
	    {
	      F[k]=0;
	      *error=e_range_too_big;
	    }
	  k++;
	  break;
	case st_variable:
	  if (fabs(values[(*fnrec).out[i].index])<max_float)
	    F[k]=values[(*fnrec).out[i].index];
	  else
	    {
	      F[k]=0;
	      *error=e_range_too_big;
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
	      F[k-1]=lb_st_dist_std(F[k-1]);
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
	      pos2=lb_pa_startpos(fnrec,i,2);
	      pos3=lb_pa_startpos(fnrec,i,3);

	      for(n=0;n<3;n++)
		vec1[n]=values[n];

	      //for (aux=0;aux<3;aux++)
	      // printf("aux[%d]=%f\r\n",aux,vec1[aux]);

	      for(n=0;n<(*fnrec).out[i].var_arg[2];n++)
	      {
		fnrec1.out[fnrec1.oq]=(*fnrec).out[pos2+n];
		lb_pa_inc_out("lb_pa_eval()-->idef",&(fnrec1.oq));
	      }
	      
	      lb_pa_print_output(&fnrec1, values, TEXT_COLOR_BACKGROUND+TEXT_COLOR_YELLOW);
	      
	      a=F[k-5];
	      b=F[k-4];
      	      delta=(b-a)/F[k-1];

	      temp_float=0.0;
	      if (F[k-1]<0)
		*error=e_arg_negative;
	      else
	      	if (F[k-1]<2)
	      	  *error=e_arg_less_two;
	      	else
		  if (F[k-1]>65534.0)
		    *error=e_arg_too_big;
	        else
	          if(!lb_re_equal(F[k-1]-(S_INT_16_T)F[k-1],0))
	            *error=e_arg_non_int;
	          else
	            if (!lb_in_is_even((S_INT_16_T)F[k-1]))
		      *error=e_arg_non_even;

	      if ((*error)!=e_none)
		return 0;
		  
	      for(n=1;n<=0.5*F[k-1];n++)
		{
		  vec1[(*fnrec).out[pos3].index]=a+delta*(2*n-2);
		  temp_float+=lb_pa_eval(&fnrec1, vec1, error);
	   
		  vec1[(*fnrec).out[pos3].index]=a+delta*(2*n-1);
		  temp_float+=4.0*lb_pa_eval(&fnrec1, vec1, error);

		  vec1[(*fnrec).out[pos3].index]=a+delta*(2*n);
		  temp_float+=lb_pa_eval(&fnrec1, vec1, error);
		}
	      temp_float=temp_float*delta/3.0;
	      //printf("IDEF = %f\r\n",temp_float);     
	     	      
	      F[k- fncs[(*fnrec).out[i].index].n_arguments]=temp_float;
	      k+=1-fncs[(*fnrec).out[i].index].n_arguments;
	      break;
	    case fn_sigma:
	      fnrec1.oq=0;
	      
	      pos2=lb_pa_startpos(fnrec,i,2);
	      pos3=lb_pa_startpos(fnrec,i,3);

	      for(n=0;n<3;n++)
		vec1[n]=values[n];

	      //for (aux=0;aux<3;aux++)
	      // printf("aux[%d]=%f\r\n",aux,vec1[aux]);

	      for(n=0;n<(*fnrec).out[i].var_arg[2];n++)
	      {
		fnrec1.out[fnrec1.oq]=(*fnrec).out[pos2+n];
		lb_pa_inc_out("lb_pa_eval()-->idef",&(fnrec1.oq));
	      }
	      
	      lb_pa_print_output(&fnrec1, values, TEXT_COLOR_BACKGROUND+TEXT_COLOR_YELLOW);
	      
	      a=F[k-5];
	      b=F[k-4];
	      
	      if (((b>a) && (F[k-1]<=0)) || ((b<a) && (F[k-1]>=0)))
		*error=e_arg_inf_loop;
	      
	      if ((*error)!=e_none)
		return 0;
	
	      temp_x=a;
	      temp_float=0.0;
	      while (  (((b>a) && (temp_x<=b)) || ((b<a) && (temp_x>=b))) && ((*error)==e_none) )
	     	{
		  vec1[(*fnrec).out[pos3].index]=temp_x;
		  temp_float+=lb_pa_eval(&fnrec1, vec1, error);
		  temp_x+=F[k-1];
		}
	      printf("SIGMA = %f\r\n",temp_float);     
	     	      
	      printf("fncs[(*fnrec).out[i].index].n_arguments=%d\r\n",fncs[(*fnrec).out[i].index].n_arguments);
	      F[k- fncs[(*fnrec).out[i].index].n_arguments]=temp_float;
	      printf("F[%d]=\r\n",k- fncs[(*fnrec).out[i].index].n_arguments);
	       k+=1-fncs[(*fnrec).out[i].index].n_arguments;
	      printf("k=%d\r\n",k);
	      printf("i=%d\r\n",i);
	      break;
	    case fn_diff:
	      fnrec1.oq=0;
	      printf("startpos DIFF=%d\r\n",lb_pa_startpos(fnrec,i,0));     
	      
	      if (F[k-1]<0)
		*error=e_arg_negative;
		else
		  if (sizeof(FLOAT_T)==4)
		    if (F[k-1]<10.0*FLT_EPSILON)
		      *error=e_arg_too_small;
		    else ;
		  else
		    if (sizeof(FLOAT_T)==8)
		      if (F[k-1]<10.0*DBL_EPSILON)
			*error=e_arg_too_small;
		      else ;
		    else
		      {
			printf("Error: lb_re_eval(): invalid floating data type: [size = %d]\r\n",sizeof(FLOAT_T));
			exit(1);
		      }
	      if ((*error)!=e_none)
		return 0;
	      
	      for(n=0;n<(*fnrec).out[i].var_arg[0];n++)
		{
		  fnrec1.out[fnrec1.oq]=(*fnrec).out[lb_pa_startpos(fnrec,i,0)+n];
		  if(fnrec1.out[fnrec1.oq].type==st_variable) 
		    vec1[fnrec1.out[fnrec1.oq].index]=values[fnrec1.out[fnrec1.oq].index];
		  lb_pa_inc_out("lb_pa_eval()-->idef",&(fnrec1.oq));
		}
	      lb_pa_print_output(&fnrec1,values,TEXT_COLOR_BACKGROUND+TEXT_COLOR_BLUE);
  
	      vec1[(*fnrec).out[lb_pa_startpos(fnrec,i,1)].index]=F[k-2]+F[k-1];
	      temp_float=lb_pa_eval(&fnrec1, vec1, error);
	      
	      if ((*error)!=e_none)
		return 0;
 
	      vec1[(*fnrec).out[lb_pa_startpos(fnrec,i,1)].index]=F[k-2];

	      // for (aux=0;aux<3;aux++)
	      //  printf("aux[%d]=%f\r\n",aux,vec1[aux]);

	      temp_float-=lb_pa_eval(&fnrec1, vec1, error);

	      if ((*error)!=e_none)
		return 0;

	      temp_float/=(*fnrec).out[lb_pa_startpos(fnrec,i,3)].float_value;
	      	      	      
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
	      printf("F[%d]=%f\r\n",k-1,F[k-1]);
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
	      printf("sumando %f  y  %f\r\n",F[k-2],F[k-1]);
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
      i++;
    }
  if (k!=1)
    {
      printf("Error: lb_pa_eval(): output queue has more than one element: %d\r\n",k);
      for (i=0;i<k;i++)
      	printf("F[%i]=%f\r\n",i,F[i]);
      return 0;
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
  S_INT_16_T st, i, oq_size;
  S_INT_8_T f_cond1, f_cond2, f_cond3;
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
  
  st=0;
  for (i=0;i<oq_size;i++)
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
	break;
      case st_separator:
	args.arguments[args.z]++;
	if(args.arguments[args.z]==MAX_ARGUMENTS)
	{
	  printf("Error: lb_pa_shunting_yard(): too many arguments [%d] > max [%d]\r\n",
		 args.arguments[args.z]+1,MAX_ARGUMENTS);
	  exit(1);
	}
	args.subitems[args.arguments[args.z]][args.z]=0;
	
	/* Rule No. 03:
	   Until the token at the top of the stack is a left parenthesis, pop operators off the stack onto the output queue. 
	   If no left parentheses are encountered, either the separator was misplaced or parentheses were mismatched. */
	while ( stack[st-1].type!= st_lpar )
	  {
	    if (st>0)
	      lb_pa_pop(fnrec,&stack[st-1],&st,&args);
	    else
	      {
		printf("Error: lb_pa_shunting_yard(): misplaced separator or mismatched parenthesis at %i \r\n",i);
		exit(1);
	      }
	  }
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
	//lb_pa_print_output(fnrec);
 	break;
      case st_lpar:
	/* Rule 05: 
	   If the token is a left parenthesis (i.e. "("), then push it onto the stack. */
	stack[st]=(*fnrec).out[i];
	lb_pa_inc_out("shunting_yard -> st_lpar",&st);
	break;
      case st_rpar:
	/* Rule 06:
	   If the token is a right parenthesis (i.e. ")"):
	   - Until the token at the top of the stack is a left parenthesis, pop operators off the stack onto the output queue.
	   - Pop the left parenthesis from the stack, but not onto the output queue.
	   - If the token at the top of the stack is a function token, pop it onto the output queue.
	   - If the stack runs out without finding a left parenthesis, then there are mismatched parentheses. */
	while (stack[st-1].type!=st_lpar)
	  {
	    lb_pa_pop(fnrec,&stack[st-1],&st,&args);
	    if (st<=0)
	      {
		printf("Error in Rule 06: missing left parenthesis\r\n");
		exit(1);
	      }
	  }
	
	if (st<=0)
	  {
	    printf("Error: lb_pa_shunting_yard() --> st_rpar st<=0: %d\r\n",st);
	    exit(1);
	  }
	st--;
	
	if( (stack[st-1].type==st_function) || 
	    (stack[st-1].type==st_variable) )
	  lb_pa_pop(fnrec,&stack[st-1],&st,&args);
	break;
      }
    
  /* Final Rule: 
     While there are still operator tokens in the stack:
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
  // lb_pa_print_output(fnrec);
  //lb_pa_print_stack(stack, st);
}

void lb_pa_parse(char *fnstr, FN_RECORD_T *fnrec, char *vars, S_INT_16_T *n_vars, FLOAT_T *val_vars)
{
  char vars_list[MAX_NUMBER_VARIABLES][MAX_VARIABLE_SIZE+1];
  char token[MAX_FORMULA_SIZE+1];
  S_INT_16_T i, k, length_fn, length_token, length_number;
  S_INT_8_T decimal_flag, match_flag=FALSE, match_flag2=FALSE;
  S_INT_8_T f_is_first, f_prev_is_lpar, f_prev_is_separator, f_prev_is_plus_or_minus, temp_operator;
  FLOAT_T temp_float;
 
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
		    f_is_first          = FALSE;
		    // f_prev_is_op        = FALSE;
		    f_prev_is_lpar      = FALSE;
		    f_prev_is_separator = FALSE;

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
 // (f_is_first  || ((!f_is_first) && (f_prev_is_op || f_prev_is_lpar || f_prev_is_separator)))) 
		      {
			match_flag=TRUE;
			(*fnrec).out[(*fnrec).oq].type=st_operator;
			(*fnrec).out[(*fnrec).oq].index=k;
			lb_pa_inc_out("lb_pa_parse()-->operator dual",&(*fnrec).oq);
		      }
		    k++;
		  }
		/* oxo */
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
				printf("Hola2\r\n");
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

void lb_pa_parse_vars(char *vars, char vars_list[MAX_NUMBER_VARIABLES][MAX_VARIABLE_SIZE+1], S_INT_16_T *n_vars)
/* This function parses the variables in a null-terminated strings
   Rules:  1. Variables must start with a letter 
           2. Variables can be followed by letters or numbers
           3. Other characters are considered invalid 
           4. Variables are not case sensitive */
{
  S_INT_16_T i, j, k, length_string;
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

void lb_pa_print_item(STACK_ELEMENT_T item, FLOAT_T *values)
{
  S_INT_16_T n;
  switch(item.type)
    {
    case st_constant: 
      printf("CONSTANT=%f\r\n",item.float_value);
      break;
    case st_variable: 
      printf("VARIABLE[%i]=%f\r\n",item.index, values[item.index]);
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
    }
}

void lb_pa_print_output(FN_RECORD_T *fnrec, FLOAT_T *values, S_INT_16_T color)
{
  lb_co_textcolor(color);
  lb_co_cls_from_cursor();
  S_INT_16_T i;
  if ((*fnrec).oq==0)
    printf("Output queue is empty\r\n");
  for (i=0;i<(*fnrec).oq;i++)
    {
      printf("item %d of %d = ",i,(*fnrec).oq-1);
      lb_pa_print_item((*fnrec).out[i], values);
    }
  lb_co_reset_colors();
  lb_co_cls_from_cursor();
 }

void lb_pa_print_stack(STACK_ELEMENT_T *stack, FLOAT_T *values, S_INT_16_T st)
{
  S_INT_16_T i;
  if (st==0)
    printf("Stack is empty\r\n");
  for (i=0;i<st;i++)
    {
      printf("item %d of %d = ",i,st-1);
      lb_pa_print_item(stack[i], values);
    }
}

FLOAT_T lb_pa_formula(char *fnstr, char *vars, FLOAT_T f1, FLOAT_T f2, FLOAT_T f3, ERR_T *error)
{
  FN_RECORD_T fnrec;
  FLOAT_T vector[3];
  S_INT_16_T n_vars;
 
  vector[0]=f1;
  vector[1]=f2;
  vector[2]=f3;

  lb_pa_parse(fnstr, &fnrec, vars, &n_vars, vector);
  lb_pa_print_output(&fnrec,vector,TEXT_COLOR_BACKGROUND+TEXT_COLOR_CYAN);
  if (n_vars>3)
    {
      printf("Error: lb_pa_formula(): too many variables: %d\r\n",n_vars);
      exit(1);
    }
  //printf("Parsed formula:\r\n");
  //lb_pa_print_output(&fnrec,TEXT_COLOR_BACKGROUND+TEXT_COLOR_GREEN);

  printf("After shunting yard:\r\n");
  lb_pa_shunting_yard(&fnrec);
  lb_pa_print_output(&fnrec,vector,TEXT_COLOR_BACKGROUND+TEXT_COLOR_RED);

  //printf("After reducing:\r\n");
  //lb_pa_reduce(&fnrec,error);
  //lb_pa_print_output(&fnrec,TEXT_COLOR_BACKGROUND+TEXT_COLOR_CYAN);
  
  return lb_pa_eval(&fnrec, vector, error);
}



