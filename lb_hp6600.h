#ifndef LB_HP6600_H
#define LB_HP6600_H

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

float t_previous, t_delta;
float volt, curr;
int current_stage=0;
int flag_volt_sent=FALSE, flag_volt_parsed=FALSE, flag_curr_sent=FALSE, flag_curr_parsed=FALSE, 
    flag_record_printed=FALSE, flag_stage_checked=FALSE, flag_stage_changed=FALSE;


/*****************************************************************************************************/
/* BATTERY PROFILE DEFINITIONS                                                                       */ 
/*****************************************************************************************************/
/* #define LEAD_ACID_DISCHARGE            */
/* #define LEAD_ACID_RECOVERY_AND_CHARGE  */
/* #define LITHIUM_TEST                   */
#define LEAD_ACID_TEST                 
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
#define N_CELLS 3.0
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


#endif  /* LB_HP6600_H */

