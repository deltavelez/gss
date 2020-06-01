#ifndef LB_HP6600_H
#define LB_HP6600_H

#include <stdio.h>
#include "lb_serial.h"
#include "lb_types.h"
#include "lb_hp6600.h"
#include "lb_time.h"


#define ONE_DAY 86400
#define ONE_HOUR 3600 
#define T_INTERVAL 10.0

typedef struct {
  char label[10];
  float lim_volt;
  float lim_curr;
  int output_enabled;
  
  float v_min;
  int v_min_trigger;
  int v_min_pointer;

  float v_max;
  int v_max_trigger;
  int v_max_pointer;

  float i_min;
  int i_min_trigger;
  int i_min_pointer;
  
  float i_max;
  int i_max_trigger;
  int i_max_pointer;

  float t_max;
  int t_max_trigger;
  int t_max_pointer;

  float watt_hour;
  float t_cycle;
} t_charge_stage;

void UpdateStageOnce(COMM_PORT_T *port, float t1, float t2, float dt, float t, float v,float i,int *flag_already_updated, int *stage_changed);
void RunProcess(COMM_PORT_T *port, FILE *text_file);
void PrintRecordOnce(FILE *text_file, float t1, float t2, float dt, float t, float v,float i,int *flag_already_printed);
void SetupPSU(COMM_PORT_T *port, int n);
void SendCmdOnce(COMM_PORT_T *port, float t1, float t2, float t, const char *str, int *already_executed_flag);
void ParseBufferOnce(COMM_PORT_T *port, float t1, float t2, float dt, float t, int *already_parsed_flag, float *val, int *success);

#endif  /* LB_HP6600_H */

