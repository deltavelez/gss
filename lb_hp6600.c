
void UpdateStageOnce(float t1, float t2, float dt, float t, float v,float i,int *flag_already_updated, int *stage_changed)
{
  if ((t1 <= dt) && (dt < t2) && !(*flag_already_updated))
    {
      *stage_changed=TRUE;
      
      if ((v<=stages[current_stage].v_min) && stages[current_stage].v_min_trigger)
	current_stage=stages[current_stage].v_min_pointer;
      else
	if ((v>=stages[current_stage].v_max) && stages[current_stage].v_max_trigger)
	  current_stage=stages[current_stage].v_max_pointer;
	else
	  if ((i<=stages[current_stage].i_min) && stages[current_stage].i_min_trigger)
	    current_stage=stages[current_stage].i_min_pointer;
	  else
	    if ((i>=stages[current_stage].i_max) && stages[current_stage].i_max_trigger)
	      current_stage=stages[current_stage].i_max_pointer;
	    else
	      if ((stages[current_stage].t_cycle>=stages[current_stage].t_max) && stages[current_stage].t_max_trigger)
		current_stage=stages[current_stage].t_max_pointer;
	      else
		*stage_changed=FALSE;
  
      if (*stage_changed && (current_stage!=N_STAGES))
	  SetupPSU(current_stage);
     
     else
	{
	  stages[current_stage].t_cycle+=T_INTERVAL;
	  stages[current_stage].watt_hour+=volt*curr*T_INTERVAL/3600.0;
	}
      *flag_already_updated=TRUE;
    }
}


void RunProcess()
{
  float t;
  int f_conv_volt, f_conv_curr;
 
  t=GetTimeStamp();
  t_delta+=t-t_previous;

  SERCOM_ProcessRx();

  SendCmdOnce(0.0, T_INTERVAL, t_delta,"MEAS:VOLT?\r\n", &flag_volt_sent); 
  ParseBufferOnce(1.0,T_INTERVAL, t_delta, t, &flag_volt_parsed, &volt,  &f_conv_volt);  
  SendCmdOnce(2.0, T_INTERVAL, t_delta,"MEAS:CURR?\r\n", &flag_curr_sent); 
  ParseBufferOnce(3.0, T_INTERVAL, t_delta, t, &flag_curr_parsed, &curr, &f_conv_curr);
  
  PrintRecordOnce(4.0, T_INTERVAL, t_delta, t-4.0,volt,curr,&flag_record_printed); 
  UpdateStageOnce(4.5, T_INTERVAL, t_delta, t,volt,curr,&flag_stage_checked,&flag_stage_changed);
  
  if (t_delta>T_INTERVAL)
    {
      t_delta-=T_INTERVAL;
      flag_volt_sent=FALSE;
      flag_volt_parsed=FALSE;
      flag_curr_sent=FALSE;
      flag_curr_parsed=FALSE;
      flag_record_printed=FALSE;
      flag_stage_checked=FALSE;
    }
  t_previous=t;
}

void PrintRecordOnce(float t1, float t2, float dt, float t, float v,float i,int *flag_already_printed)
{
  if ((t1 <= dt) && (dt < t2) && !(*flag_already_printed))
    {
      fprintf(output,"S:%d [",current_stage);
      fprintf(text_file,"%d\t",current_stage);
     
      fprintf(output,"%s]",stages[current_stage].label);
      fprintf(text_file,"%s\t",stages[current_stage].label);
    
      Gotox(15);
      
      fprintf(output,"t= %2.4f",t/3600.0);
      fprintf(text_file,"%2.4f\t",t/3600.0);
      
      Gotox(25);
      
      fprintf(output,"dt= %2.4f",stages[current_stage].t_cycle/3600.0);
      fprintf(text_file,"%2.4f\t",stages[current_stage].t_cycle/3600.0);
      
      Gotox(37);
      
      fprintf(output,"V=%2.2f",v);
      fprintf(text_file,"%2.2f\t",v);
      
      Gotox(45);
      
      fprintf(output,"I= %2.3f",i);
      fprintf(text_file,"%2.3f\t",i);
      
      Gotox(55);

      fprintf(output,"P= %2.2f",v*i);
      fprintf(text_file,"%2.2f\t",v*i);

      Gotox(65);
      fprintf(output,"W*H= %2.3f\r\n",stages[current_stage].watt_hour);
      fprintf(text_file,"%2.3f\r\n",stages[current_stage].watt_hour);

      *flag_already_printed=TRUE;
    }
}


void SetupPSU(int n)
{
  char str[20];
 
  sprintf(str,"VOLT:LEV %f\r\n",stages[n].lim_volt);  
  SERCOM_TxStr(str);

  /* fprintf(output,str); */
  sprintf(str,"CURR:LEV %f\r\n",fabs(stages[n].lim_curr)); 
  SERCOM_TxStr(str);
  stages[n].t_cycle=0;
  
  if (stages[n].output_enabled)
    SERCOM_TxStr("OUTP:STAT 1\r\n");
  else
    SERCOM_TxStr("OUTP:STAT 0\r\n");
}
void SendCmdOnce(float t1, float t2, float t, const char *str, int *already_executed_flag)
{
  if ((t1 <= t) && (t < t2) && !*already_executed_flag)
    {
      SERCOM_TxStr(str);
      *already_executed_flag=TRUE;
   }
}

void ParseBufferOnce(float t1, float t2, float dt, float t, int *already_parsed_flag, float *val, int *success)
{
  char str[90];

  if ((t1 <= dt) && (dt < t2) && !*already_parsed_flag)
    {
      SERCOM_DataBufferToStr(str);
      
      if (strlen(str)!=0)
	{
	  *val=atof(str);
	  *success=TRUE;
	} 
      else 
	*success=FALSE;
      SERCOM_ClearBuffer();
      *already_parsed_flag=TRUE;
    }
}

