#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "lb_hp6600.h"


/*****************************************************************************************************/
/* BATTERY PROFILE DEFINITIONS                                                                       */ 
/*****************************************************************************************************/
//#define LEAD_ACID_DISCHARGE            */
//#define LEAD_ACID_RECOVERY_AND_CHARGE  */
#define LITHIUM_TEST                   */
//#define LEAD_ACID_TEST                 
/*****************************************************************************************************/

#ifdef BATTERY_TEST
#define N_STAGES 3

t_charge_stage stages[N_STAGES] = {
  /* STAGE No. 0 sampe charge cycle at constant current */
  { "Rvy Chg",
    14.0, 0.1, TRUE,      /* nominal v and i */
    0.0, FALSE,  0,   /* v_min settings */
    12.0, TRUE,  1,  /* v_max settings */
    0.00, FALSE, 0,   /* i_min settings */
    0.00, FALSE, 0,   /* i_max settings */
    86400.0, TRUE, 3, /* t_max settings */
    0,0 },          /* always zero */

  { "Fst Chg",
    14.4, 1.5, TRUE,      /* nominal v and i */
    0.0, FALSE,  0,   /* v_min settings */
    12.5, TRUE,  2,  /* v_max settings */
    0.00, FALSE, 0,   /* i_min settings */
    0.00, FALSE, 0,   /* i_max settings */
    ONE_HOUR*2.0, TRUE, 3, /* t_max settings */
    0,0 },          /* always zero */

  /* STAGE No. 0: sample discharge cycle at constant current */
  { "Discharge", 
    11.0, -1.25, TRUE,       /* nominal v and i */
    11.95, TRUE,  3,   /* v_min settings */
    0.0, FALSE,  0,   /* v_max settings */
    0.0, FALSE, 0,    /* i_min settings */
    0.0, FALSE, 0,    /* i_max settings */
    86400.0, TRUE, 3, /* t_max settings */
    0,0 }};          /* always zero */
#endif


#ifdef LEAD_ACID_RECOVERY_AND_CHARGE
#define N_STAGES 3

t_charge_stage stages[N_STAGES] = {
  { "Rvy Chg",
    12.5, 0.05, TRUE,      /* nominal v and i */
    0.0, FALSE,  0,   /* v_min settings */
    10.5, TRUE,  1,  /* v_max settings */
    0.00, FALSE, 0,   /* i_min settings */
    0.00, FALSE, 0,   /* i_max settings */
    ONE_DAY*5.0, TRUE, 3, /* t_max settings */
    0,0 },          /* always zero */

  { "Fst Chg",
    14.4, 5.0, TRUE,      /* nominal v and i */
    11.5, TRUE,  0,   /* v_min settings */
    13.65, TRUE,  2,  /* v_max settings */
    0.00, FALSE, 0,   /* i_min settings */
    0.00, FALSE, 0,   /* i_max settings */
    ONE_DAY, TRUE, 3, /* t_max settings */
    0,0 },          /* always zero */

  { "Top Chg",
    14.4, 5.0, TRUE,      /* nominal v and i */
    13.0, TRUE,  1,   /* v_min settings */
    0.0, FALSE, 3,  /* v_max settings */
    0.5, TRUE, 3,   /* i_min settings */
    0.00, FALSE, 0,   /* i_max settings */
    ONE_DAY, TRUE, 3, /* t_max settings */
    0,0 },          /* always zero */
};
#endif

#ifdef LEAD_ACID_TEST
#define N_STAGES 7
#define N_CELLS 6
#define LEAD_VOLT_UCH 1.67
#define LEAD_VOLT_OCH 2.4
#define LEAD_NOM_CURR 2.5 

t_charge_stage stages[N_STAGES] = {
  { "Rvy Chg",
    LEAD_VOLT_OCH*N_CELLS, 0.025*LEAD_NOM_CURR, TRUE,      /* nominal v and i */
    0.0, FALSE,  0,   /* v_min settings */
    LEAD_VOLT_UCH*N_CELLS, TRUE,  1,  /* v_max settings */
    0.00, FALSE, 0,   /* i_min settings */
    0.00, FALSE, 0,   /* i_max settings */
    ONE_DAY*3.0, TRUE, N_STAGES, /* t_max settings */
    0,0 },          /* always zero */
  
  { "Fst Chg",
    LEAD_VOLT_OCH*N_CELLS, LEAD_NOM_CURR, TRUE,      /* nominal v and i */
    LEAD_VOLT_UCH*N_CELLS, TRUE,  0,   /* v_min settings */
    0, FALSE,  0,  /* v_max settings */
    0.1*LEAD_NOM_CURR, TRUE, 2,   /* i_min settings */
    0.00, FALSE, 0,   /* i_max settings */
    ONE_DAY, TRUE, N_STAGES, /* t_max settings */
    0,0 },          /* always zero */
  
  { "Discharge", 
    0.8*LEAD_VOLT_UCH*N_CELLS, 5.0, TRUE,       /* nominal v and i */
    LEAD_VOLT_UCH*N_CELLS, TRUE,  3,   /* v_min settings */
    0.0, FALSE,  0,   /* v_max settings */
    0.0, FALSE, 0,    /* i_min settings */
    0.0, FALSE, 0,    /* i_max settings */
    ONE_DAY, TRUE, N_STAGES, /* t_max settings */
    0,0 },          /* always zero */
  
  { "Rvy Chg",
    LEAD_VOLT_OCH*N_CELLS, 0.025*LEAD_NOM_CURR, TRUE,      /* nominal v and i */
    0.0, FALSE,  0,   /* v_min settings */
    LEAD_VOLT_UCH*N_CELLS, TRUE,  4,  /* v_max settings */
    0.00, FALSE, 0,   /* i_min settings */
    0.00, FALSE, 0,   /* i_max settings */
    ONE_DAY*3.0, TRUE, N_STAGES, /* t_max settings */
    0,0 },          /* always zero */
  
  { "Fst Chg",
    LEAD_VOLT_OCH*N_CELLS, LEAD_NOM_CURR, TRUE,      /* nominal v and i */
    LEAD_VOLT_UCH*N_CELLS, TRUE,  0,   /* v_min settings */
    0, FALSE,  0,  /* v_max settings */
    0.1*LEAD_NOM_CURR, TRUE, N_STAGES,   /* i_min settings */
    0.00, FALSE, 0,   /* i_max settings */
    ONE_DAY, TRUE, N_STAGES, /* t_max settings */
    0,0 },          /* always zero */
  
};
#endif


#ifdef LEAD_ACID_DISCHARGE
#define N_STAGES 1

t_charge_stage stages[N_STAGES] = 
  { "Discharge", 
    7.0, -2.0, TRUE,       /* nominal v and i */
    10.5, TRUE,  1,   /* v_min settings */
    0.0, FALSE,  0,   /* v_max settings */
    0.0, FALSE, 0,    /* i_min settings */
    0.0, FALSE, 0,    /* i_max settings */
    ONE_DAY, TRUE, 1, /* t_max settings */
    0,0 };          /* always zero */
#endif


#ifdef LITHIUM_TEST
#define N_STAGES 5
#define N_CELLS 1.0
#define LI_VOLT_CHG 4.2
#define LI_VOLT_DCH 2.8
#define LI_NOM_CURR 1.0 

t_charge_stage stages[N_STAGES] = {
  { "Rvy Chg",
    N_CELLS*LI_VOLT_CHG, LI_NOM_CURR*0.05, TRUE,      /* nominal v and i */
    0.0, FALSE,  0,   /* v_min settings */
    N_CELLS*LI_VOLT_DCH, TRUE,  1,  /* v_max settings */
    0.00, FALSE, 0,   /* i_min settings */
    0.00, FALSE, 0,   /* i_max settings */
    ONE_DAY, TRUE, N_STAGES, /* t_max settings */
    0,0 },          /* always zero */

  { "Fst Chg",
    N_CELLS*LI_VOLT_CHG, LI_NOM_CURR, TRUE,      /* nominal v and i */
    N_CELLS*LI_VOLT_DCH, TRUE,  0,   /* v_min settings */
    0.0, FALSE,  0,  /* v_max settings */
    LI_NOM_CURR*0.05, TRUE, 2,   /* i_min settings */
    0.0, FALSE, 0,   /* i_max settings */
    ONE_DAY, TRUE, N_STAGES, /* t_max settings */
    0,0 },          /* always zero */

  { "Dis Chg", 
    N_CELLS*0.9*LI_VOLT_DCH, LI_NOM_CURR, TRUE,       /* nominal v and i */
    N_CELLS*LI_VOLT_DCH, TRUE,  3,   /* v_min settings */
    0.0, FALSE,  0,   /* v_max settings */
    0.0, FALSE, 0,    /* i_min settings */
    0.0, FALSE, 0,    /* i_max settings */
    ONE_DAY, TRUE, N_STAGES, /* t_max settings */
    0,0 },
  
  { "Rvy Chg",
    N_CELLS*LI_VOLT_CHG, 0.1, TRUE,      /* nominal v and i */
    0.0, FALSE,  0,   /* v_min settings */
    N_CELLS*LI_VOLT_DCH, TRUE,  4,  /* v_max settings */
    0.00, FALSE, 0,   /* i_min settings */
    0.00, FALSE, 0,   /* i_max settings */
    ONE_DAY, TRUE, N_STAGES, /* t_max settings */
    0,0 },          /* always zero */

  { "Fst Chg",
    N_CELLS*LI_VOLT_CHG, LI_NOM_CURR, TRUE,      /* nominal v and i */
    N_CELLS*LI_VOLT_DCH, TRUE,  3,   /* v_min settings */
    0.0, FALSE,  0,  /* v_max settings */
    LI_NOM_CURR*0.2, TRUE, 5,   /* i_min settings */
    0.0, FALSE, 0,   /* i_max settings */
    ONE_DAY, TRUE, N_STAGES, /* t_max settings */
    0,0 },          /* always zero */
  
};          /* always zero */
#endif


float t_previous, t_delta;
float volt, curr;
int current_stage=0;
int flag_volt_sent=FALSE, flag_volt_parsed=FALSE, flag_curr_sent=FALSE, flag_curr_parsed=FALSE, 
    flag_record_printed=FALSE, flag_stage_checked=FALSE, flag_stage_changed=FALSE;

void UpdateStageOnce(COMM_PORT_T *port, float t1, float t2, float dt, float t, float v,float i,int *flag_already_updated, int *stage_changed)
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
	SetupPSU(port, current_stage);
     
     else
	{
	  stages[current_stage].t_cycle+=T_INTERVAL;
	  stages[current_stage].watt_hour+=volt*curr*T_INTERVAL/3600.0;
	}
      *flag_already_updated=TRUE;
    }
}


void RunProcess(COMM_PORT_T *port, FILE *text_file)
{
  float t;
  int f_conv_volt, f_conv_curr;
 
  t=lb_ti_time_wall();
  t_delta+=t-t_previous;

  lb_se_process_rx(port);

  SendCmdOnce(port, 0.0, T_INTERVAL, t_delta,"MEAS:VOLT?\r\n", &flag_volt_sent); 
  ParseBufferOnce(port, 1.0,T_INTERVAL, t_delta, t, &flag_volt_parsed, &volt,  &f_conv_volt);  
  SendCmdOnce(port, 2.0, T_INTERVAL, t_delta,"MEAS:CURR?\r\n", &flag_curr_sent); 
  ParseBufferOnce(port, 3.0, T_INTERVAL, t_delta, t, &flag_curr_parsed, &curr, &f_conv_curr);
  
  PrintRecordOnce(text_file, 4.0, T_INTERVAL, t_delta, t-4.0,volt,curr,&flag_record_printed); 
  UpdateStageOnce(port, 4.5, T_INTERVAL, t_delta, t,volt,curr,&flag_stage_checked,&flag_stage_changed);
  
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

void PrintRecordOnce(FILE *text_file, float t1, float t2, float dt, float t, float v,float i,int *flag_already_printed)
{
  if ((t1 <= dt) && (dt < t2) && !(*flag_already_printed))
    {
      printf("S:%d [",current_stage);
      fprintf(text_file,"%d\t",current_stage);
     
      printf("%s]",stages[current_stage].label);
      fprintf(text_file,"%s\t",stages[current_stage].label);
    
      //Gotox(15);
      
      printf("t= %2.4f",t/3600.0);
      fprintf(text_file,"%2.4f\t",t/3600.0);
      
      //Gotox(25);
      
      printf("dt= %2.4f",stages[current_stage].t_cycle/3600.0);
      fprintf(text_file,"%2.4f\t",stages[current_stage].t_cycle/3600.0);
      
      //Gotox(37);
      
      printf("V=%2.2f",v);
      fprintf(text_file,"%2.2f\t",v);
      
      //Gotox(45);
      
      printf("I= %2.3f",i);
      fprintf(text_file,"%2.3f\t",i);
      
      //Gotox(55);

      printf("P= %2.2f",v*i);
      fprintf(text_file,"%2.2f\t",v*i);

      //Gotox(65);
      printf("W*H= %2.3f\r\n",stages[current_stage].watt_hour);
      fprintf(text_file,"%2.3f\r\n",stages[current_stage].watt_hour);

      *flag_already_printed=TRUE;
    }
}


void SetupPSU(COMM_PORT_T *port, int n)
{
  char str[20];
 
  sprintf(str,"VOLT:LEV %f\r\n",stages[n].lim_volt);  
  lb_se_tx_str(port,str);

  /* fprintf(output,str); */
  sprintf(str,"CURR:LEV %f\r\n",fabs(stages[n].lim_curr)); 
  lb_se_tx_str(port,str);
  stages[n].t_cycle=0;
  
  if (stages[n].output_enabled)
    lb_se_tx_str(port,"OUTP:STAT 1\r\n");
  else
    lb_se_tx_str(port,"OUTP:STAT 0\r\n");
}

void SendCmdOnce(COMM_PORT_T *port, float t1, float t2, float t, const char *str, int *already_executed_flag)
{
  if ((t1 <= t) && (t < t2) && !*already_executed_flag)
    {
      lb_se_tx_str(port,str);
      *already_executed_flag=TRUE;
   }
}

void ParseBufferOnce(COMM_PORT_T *port, float t1, float t2, float dt, float t, int *already_parsed_flag, float *val, int *success)
{
  char str[90];

  if ((t1 <= dt) && (dt < t2) && !*already_parsed_flag)
    {
      lb_se_tx_str(port,str);
      
      if (strlen(str)!=0)
	{
	  *val=atof(str);
	  *success=TRUE;
	} 
      else 
	*success=FALSE;
      lb_se_clear_buffer(port);
      *already_parsed_flag=TRUE;
    }
}

