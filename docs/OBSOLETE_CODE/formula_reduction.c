/* Future work: simplifying an alredy-parsed function */

S_INT_8_T lb_pa_fn_can_be_reduced(FN_RECORD_T *fnrec, S_INT_16_T i, S_INT_16_T k);
S_INT_8_T lb_pa_op_can_be_reduced(FN_RECORD_T *fnrec, S_INT_16_T i, S_INT_16_T k);
void lb_pa_reduce(FN_RECORD_T *fnrec, ERR_T *error);
S_INT_8_T lb_pa_reduce_constant(FN_RECORD_T *fnrec, ERR_T *error, S_INT_16_T a, S_INT_16_T b, S_INT_16_T c, S_INT_16_T d);


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

