#ifndef LB_PARSER_H
#define LB_PARSER_H

/******************************************************/
/* Parser data types */
/******************************************************/

#define MAX_FORMULA_SIZE 80
#define MAX_FUNCTION_SIZE 8
#define MAX_OPERATOR_SIZE 2
#define MAX_VARIABLE_SIZE 4
#define MAX_CONSTANT_SIZE 3
#define MAX_ERROR_MESSAGE 50 
#define MAX_NUMBER_VARIABLES 10
#define MAX_STACK_SIZE 50
#define MAX_FUNCTIONS 39
#define MAX_OPERATORS 14
#define MAX_OPERATORS_DUAL 2 
#define MAX_CONSTANTS 2 
#define MAX_ERRORS 17
#define MAX_ARGUMENTS 16
#define MAX_FIXED_ARGUMENTS 5
#define MAX_DEPTH 16
#define INDEX_TYPE S_INT16_T  /* Used by lb_parser.c and lb_numer.c */  

typedef enum { op_unary, op_binary } ARGUMENTS_T;

typedef enum { a_left, a_right } ASSOCIATIVITY_T;

typedef enum { op_positive, op_negative, op_factorial, op_power, op_division, op_product, op_substraction, op_addition, 
	       op_equal_than, op_larger_than, op_smaller_than, op_different_than, op_larger_or_equal_than, 
	       op_smaller_or_equal_than } OPERATORS_T;

typedef enum { fn_sin, fn_cos, fn_tan, fn_asin, fn_acos, fn_atan, fn_sinh, fn_cosh, fn_tanh, fn_asinh, 
	       fn_acosh, fn_atanh, fn_exp, fn_ln, fn_log, fn_log2, fn_logb, fn_max2, fn_min2, fn_if, fn_sqr, 
	       fn_sqrt, fn_abs, fn_erf, fn_erfc, fn_rand, fn_marsa, fn_dstd, fn_real, fn_imag, fn_polar, fn_rect, fn_avg, fn_idef, fn_idef2, fn_sigma, fn_prod, fn_diff, fn_diff2 } FNC_T;

typedef struct
{ 
  ERR_T id;
  char msg[MAX_ERROR_MESSAGE];
} ERRORS_T;

extern ERRORS_T errors[MAX_ERRORS];

typedef struct
{
  OPERATORS_T id;
  char symbol[MAX_OPERATOR_SIZE];
  ASSOCIATIVITY_T asso;
  S_INT8_T pre; 
  ARGUMENTS_T op_type;
} OPERATOR_T;

typedef struct
{
  FNC_T id;
  char name[MAX_FUNCTION_SIZE];
  S_INT8_T n_arguments;
  U_INT8_T var_mask; 
} FUNCTION_T;

typedef struct
{
  char name[MAX_CONSTANT_SIZE];
  FLOAT_T value;
} CONSTANT_T;

typedef enum { st_constant, st_variable, st_function, st_separator, st_operator, st_lpar, st_rpar } STACK_ELEMENT_TYPE_T;

typedef struct
{
  STACK_ELEMENT_TYPE_T type;
  FLOAT_T float_value;
  INDEX_TYPE index; 
  S_INT8_T var_arg[MAX_FIXED_ARGUMENTS];
} STACK_ELEMENT_T; 

typedef struct
{
  STACK_ELEMENT_T out[MAX_STACK_SIZE];
  S_INT16_T oq;  /* output queue grows from zero */
  S_INT16_T arg_count;
} FN_RECORD_T;

typedef struct
{
  S_INT16_T z;
  S_INT16_T arguments[MAX_DEPTH];
  S_INT16_T subitems[MAX_ARGUMENTS][MAX_DEPTH];
} ARGS_T;

//void lb_pa_push(STACK_ELEMENT_T *stack, STACK_ELEMENT_T *item, S_INT16_T *st, char* debug_msg);
void lb_pa_pop(FN_RECORD_T *fnrec, STACK_ELEMENT_T *item, S_INT16_T *s, ARGS_T *args);
  
void lb_pa_inc_out(char *debug_msg,  S_INT16_T *var);
FLOAT_T lb_pa_eval_real(FN_RECORD_T *fnrec, FLOAT_T *values, ERR_T *error);
COMPLEX_T lb_pa_eval_complex(FN_RECORD_T *fnrec, COMPLEX_T *values, ERR_T *error);

void lb_pa_parse(char *fnstr, FN_RECORD_T *fnrec, char *vars, S_INT16_T *n_vars);
void lb_pa_parse_vars(char *vars, char vars_list[MAX_NUMBER_VARIABLES][MAX_VARIABLE_SIZE+1], S_INT16_T *n_vars);
void lb_pa_shunting_yard(FN_RECORD_T *fnrec);
void lb_pa_print_item(STACK_ELEMENT_T item);
void lb_pa_print_output(FN_RECORD_T *fnrec, S_INT16_T color);
void lb_pa_print_stack(STACK_ELEMENT_T *stack, S_INT16_T st);
FLOAT_T lb_pa_formula(char *fnstr, char *vars, FLOAT_T f1, FLOAT_T f2, FLOAT_T f3, ERR_T *error);
COMPLEX_T lb_pa_formula_complex(char *fnstr, char *vars, COMPLEX_T f1, COMPLEX_T f2, COMPLEX_T f3, ERR_T *error);
S_INT16_T lb_pa_startpos(FN_RECORD_T *fnrec, S_INT16_T i, S_INT16_T argument);

#endif /* LB_PARSER_H */
