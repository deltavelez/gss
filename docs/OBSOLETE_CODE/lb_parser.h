#ifndef LB_PARSER_H
#define LB_PARSER_H

#define MAX_FORMULA_SIZE 80
#define MAX_FUNCTION_SIZE 8
#define MAX_OPERATOR_SIZE 2
#define MAX_VARIABLE_SIZE 4
#define MAX_CONSTANT_SIZE 3
#define MAX_ERROR_MESSAGE 50 
#define MAX_NUMBER_VARIABLES 10
#define MAX_STACK_SIZE 50
#define MAX_FUNCTIONS 25
#define MAX_OPERATORS 14
#define MAX_OPERATORS_DUAL 2 
#define MAX_CONSTANTS 2 
#define MAX_ERRORS 13
#define MAX_ARGUMENTS 16
#define MAX_FIXED_ARGUMENTS 5
#define MAX_DEPTH 16

typedef enum { op_unary, op_binary } ARGUMENTS_T;

typedef enum { a_left, a_right } ASSOCIATIVITY_T;

typedef enum { op_positive, op_negative, op_factorial, op_power, op_division, op_product, op_substraction, op_addition, 
	       op_equal_than, op_larger_than, op_smaller_than, op_different_than, op_larger_or_equal_than, 
	       op_smaller_or_equal_than } OPERATORS_T;

typedef enum { fn_sin, fn_cos, fn_tan, fn_asin, fn_acos, fn_atan, fn_exp, fn_ln, fn_log, fn_log2,  
	       fn_max2, fn_min2, fn_if, fn_sqr, fn_sqrt, fn_abs, fn_erf, fn_erfc, fn_rand, fn_marsa, fn_dstd,
	       fn_avg, fn_idef, fn_sigma, fn_diff } FNC_T;

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
  S_INT_8_T pre; 
  ARGUMENTS_T op_type;
} OPERATOR_T;

typedef struct
{
  FNC_T id;
  char name[MAX_FUNCTION_SIZE];
  S_INT_8_T n_arguments;
  U_INT_8_T var_mask; 
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
  S_INT_16_T index; 
  S_INT_8_T var_arg[MAX_FIXED_ARGUMENTS];
} STACK_ELEMENT_T; 

typedef struct
{
  STACK_ELEMENT_T out[MAX_STACK_SIZE];
  S_INT_16_T oq;  /* output queue grows from zero */
} FN_RECORD_T;

typedef struct
{
  S_INT_16_T z;
  S_INT_16_T arguments[MAX_DEPTH];
  S_INT_16_T subitems[MAX_ARGUMENTS][MAX_DEPTH];
} ARGS_T;

//void lb_pa_push(STACK_ELEMENT_T *stack, STACK_ELEMENT_T *item, S_INT_16_T *st, char* debug_msg);
void lb_pa_pop(FN_RECORD_T *fnrec, STACK_ELEMENT_T *item, S_INT_16_T *s, ARGS_T *args);
  
void lb_pa_inc_out(char *debug_msg,  S_INT_16_T *var);
FLOAT_T lb_pa_eval(FN_RECORD_T *fnrec, FLOAT_T *values, ERR_T *error);
S_INT_8_T lb_pa_fn_can_be_reduced(FN_RECORD_T *fnrec, S_INT_16_T i, S_INT_16_T k);
S_INT_8_T lb_pa_op_can_be_reduced(FN_RECORD_T *fnrec, S_INT_16_T i, S_INT_16_T k);
void lb_pa_reduce(FN_RECORD_T *fnrec, ERR_T *error);
S_INT_8_T lb_pa_reduce_constant(FN_RECORD_T *fnrec, ERR_T *error, S_INT_16_T a, S_INT_16_T b, S_INT_16_T c, S_INT_16_T d);

void lb_pa_parse(char *fnstr, FN_RECORD_T *fnrec, char *vars, S_INT_16_T *n_vars, FLOAT_T *val_vars);
void lb_pa_parse_vars(char *vars, char vars_list[MAX_NUMBER_VARIABLES][MAX_VARIABLE_SIZE+1], S_INT_16_T *n_vars);
void lb_pa_shunting_yard(FN_RECORD_T *fnrec);
void lb_pa_print_item(STACK_ELEMENT_T item, FLOAT_T *values);
void lb_pa_print_output(FN_RECORD_T *fnrec, FLOAT_T *values, S_INT_16_T color);
void lb_pa_print_stack(STACK_ELEMENT_T *stack, FLOAT_T *values, S_INT_16_T st);
FLOAT_T lb_pa_formula(char *fnstr, char *vars, FLOAT_T f1, FLOAT_T f2, FLOAT_T f3, ERR_T *error);
S_INT_16_T lb_pa_startpos(FN_RECORD_T *fnrec, S_INT_16_T i, S_INT_16_T argument);

#endif /* LB_PARSER_H */
