#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "lb_types.h"
#include "lb_fonts.h"
#include "lb_algebra.h"
#include "lb_real.h"
#include "lb_complex.h"

void lb_al_add_vector_c(VECTOR_C_T *A, VECTOR_C_T *B, VECTOR_C_T *C)
{ 
  U_INT16_T i;

  if (!lb_al_assert_dimensions_vector_c(A))
    {
      printf("Error: lb_al_add_vector_c() --> invalid dimension A[%d]\r\n",(*A).items);
      exit(EXIT_FAILURE);
    }
  
  if (!lb_al_assert_dimensions_vector_c(B) || !lb_al_assert_dimensions_vector_c(C))
    {
      printf("Error: lb_al_add_vector_c() --> invalid dimension B[%d]",(*B).items);
      exit(EXIT_FAILURE);
    }
  
  if (((*A).items != (*B).items) || ((*A).items != (*C).items))
    {
      printf("Error: lb_al_add_vector_c() --> invalid dimension A[%d] B[%d] C[%d]\r\n",(*A).items,(*B).items,(*C).items);
      exit(EXIT_FAILURE);
    }
   
  for (i=0;i<(*A).items;i++)
    (*C).array[i]=lb_cp_add((*A).array[i],(*B).array[i]);
}

void lb_al_add_array_r_r(VECTOR_R_T *A, VECTOR_R_T *B, VECTOR_R_T *C)
{ 
  U_INT16_T i;

  if (!lb_al_assert_dimensions_vector_r(A))
    {
      printf("Error: lb_al_add_vector_r() --> invalid dimension A[%d]\r\n",(*A).items);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_vector_r(B))
    {
      printf("Error: lb_al_add_vector_r() --> invalid dimension B[%d]\r\n",(*B).items);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_vector_r(C))
    {
      printf("Error: lb_al_add_vector_r() --> invalid dimension C[%d]\r\n",(*C).items);
      exit(EXIT_FAILURE);
    }

  if (((*A).items != (*B).items) || ((*A).items != (*C).items))
    {
      printf("Error: lb_al_add_vector_r() --> invalid dimension A[%d] B[%d] C[%d]\r\n",(*A).items, (*B).items, (*C).items);
      exit(EXIT_FAILURE);
    }
    
  for (i=0;i<(*A).items;i++)
    (*C).array[i]=(*A).array[i]+(*B).array[i];
}

S_INT8_T lb_al_assert_dimensions_matrix_p2d(MATRIX_POINT_2D_T *S)
{
  if ( ((*S).rows <= 0) || ((*S).rows > MATRIX_MAX_ROWS) ||
       ((*S).cols <= 0) || ((*S).cols > MATRIX_MAX_COLS) )
    return 0;
  return 1;
}

S_INT8_T lb_al_assert_dimensions_matrix_p3d(MATRIX_POINT_3D_T *S)
{
  if ( ((*S).rows <= 0) || ((*S).rows > MATRIX_MAX_ROWS) ||
       ((*S).cols <= 0) || ((*S).cols > MATRIX_MAX_COLS) )
    return 0;
  return 1;
}

S_INT8_T lb_al_assert_dimensions_array_r(ARRAY_R_T *M)
{
  S_INT16_T  i;

  if ( ((*M).n<1) || ((*M).n>ARRAY_MAX_DIM) ) 
    return FALSE;

  for (i=0; i<(*M).n; i++)
    if ( ((*M).dim[i]<1) || ((*M).dim[i]>ARRAY_MAX_N) )
      return FALSE;
  
  return TRUE;
}

S_INT8_T lb_al_assert_dimensions_matrix_r(MATRIX_R_T *M)
{
  if ( ((*M).rows <= 0) || ((*M).rows > MATRIX_MAX_ROWS) ||
       ((*M).cols <= 0) || ((*M).cols > MATRIX_MAX_COLS) )
    return 0;
  return 1;
}

S_INT8_T lb_al_assert_dimensions_matrix_si8(MATRIX_S_INT8_T *M)
{
  if ( ((*M).rows <= 0) || ((*M).rows > MATRIX_MAX_ROWS) ||
       ((*M).cols <= 0) || ((*M).cols > MATRIX_MAX_COLS) )
    return 0;
  return 1;
}

S_INT8_T lb_al_assert_dimensions_vector_c(VECTOR_C_T *V)
{
  if ( ((*V).items < 0) || ((*V).items > VECTOR_MAX_ITEMS) )
    return 0;
  return 1;
}

S_INT8_T lb_al_assert_dimensions_vector_r(VECTOR_R_T *V)
{
  if ( ((*V).items < 0) || ((*V).items > VECTOR_MAX_ITEMS) )
    return 0;
  return 1;
}

S_INT8_T lb_al_assert_dimensions_vector_si8(VECTOR_S_INT8_T *V)
{
  if ( ((*V).items < 0) || ((*V).items > VECTOR_MAX_ITEMS) )
    return 0;
  return 1;
}

S_INT8_T lb_al_assert_dimensions_vector_si16(VECTOR_S_INT16_T *V)
{
  if ( ((*V).items < 0) || ((*V).items > VECTOR_MAX_ITEMS) )
    return 0;
  return 1;
}

void lb_al_copy_matrix_r_col_to_vector_r(MATRIX_R_T *M, VECTOR_R_T *V, U_INT16_T j)
{
  U_INT16_T i;

  if (!lb_al_assert_dimensions_matrix_r(M))
    {
      printf("Error: lb_al_copy_matrix_r_col_to_vector_r() --> invalid dimension M[%d,%d]\r\n",(*M).rows,(*M).cols);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_vector_r(V))
    {
      printf("Error: lb_al_copy_matrix_r_col_to_vector_r() --> invalid dimension V[%d]\r\n",(*V).items);
      exit(EXIT_FAILURE);
    }
    
  if ( (*M).rows != (*V).items)
    {
      printf("Error: lb_al_copy_matrix_r_col_to_vector_r() --> invalid dimension M[==>%d,%d] V[==>%d]\r\n",(*M).rows,(*M).cols,(*V).items);
      exit(EXIT_FAILURE);
    }
  
  if ( (j<0) || (j>=(*M).cols) )
    {
      printf("Error: lb_al_copy_matrix_r_col_to_vector_r() --> invalid column [ %d ]\r\n",j);
      exit(EXIT_FAILURE);
    }
     
  for(i=0;i<(*M).rows;i++)
    (*V).array[i]=(*M).array[i][j];
}

void lb_al_copy_matrix_r_row_to_vector_r(MATRIX_R_T *M, VECTOR_R_T *V, U_INT16_T i)
{
  U_INT16_T j;
  
  if (!lb_al_assert_dimensions_matrix_r(M))
    {
      printf("Error: lb_al_copy_matrix_r_row_to_vector_r() --> invalid dimension M[%d,%d]\r\n",(*M).rows,(*M).cols);
      exit(EXIT_FAILURE);
    }
    
  if (!lb_al_assert_dimensions_vector_r(V))
    {
      printf("Error: lb_al_copy_matrix_r_row_to_vector_r() --> invalid dimension V[%d]\r\n",(*V).items);
      exit(EXIT_FAILURE);
    }
    
  if ( (*M).cols != (*V).items)
    {
      printf("Error: lb_al_copy_matrix_r_row_to_vector_r() --> invalid dimension M[%d,==>%d] V[==>%d]\r\n",(*M).rows,(*M).cols,(*V).items);
      exit(EXIT_FAILURE);
    }
    
  if ( (i<0) || (i>=(*M).rows))
    {
      printf("Error: lb_al_copy_matrix_r_row_to_vector_r() --> invalid row [ %d ]\r\n",i);
      exit(EXIT_FAILURE);
    }
     
  for(j=0;j<(*M).cols;j++)
    (*V).array[j]=(*M).array[i][j];
}

void lb_al_copy_matrix_r(MATRIX_R_T *M1, MATRIX_R_T *M2)
{ 
  U_INT16_T i, j;

  if (!lb_al_assert_dimensions_matrix_r(M1))
    {
      printf("Error: lb_al_copy_matrix_r() --> invalid dimension M1[%d,%d]\r\n",(*M1).rows,(*M1).cols);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_matrix_r(M2))
    {
      printf("Error: lb_al_copy_matrix_r() --> invalid dimension M2[%d,%d]\r\n",(*M2).rows,(*M2).cols);
      exit(EXIT_FAILURE);
    }

  if ( ((*M1).rows != (*M2).rows) || ((*M1).cols != (*M2).cols) )
    {
      printf("Error: lb_al_copy_matrix_r() --> invalid dimension M1[%d,%d] M2[%d,%d]\r\n",(*M1).rows,(*M1).cols,(*M2).rows,(*M2).cols);
      exit(EXIT_FAILURE);
    }
     
  for (i=0;i<(*M1).rows;i++)
    for (j=0;j<(*M1).cols;j++)
      (*M2).array[i][j]=(*M1).array[i][j];
}

void lb_al_copy_matrix33_r(FLOAT_T M1[3][3], FLOAT_T M2[3][3])
{ 
  U_INT16_T i, j;

  for (i=0;i<3;i++)
    for (j=0;j<3;j++)
      M2[i][j]=M1[i][j];
}


void lb_al_copy_minor_matrix_r(MATRIX_R_T *M1, MATRIX_R_T *M2, U_INT16_T io, U_INT16_T jo)
{
  U_INT16_T i, j, i_minor, j_minor;

  /* We assert the size */
  if (!lb_al_assert_dimensions_matrix_r(M1))
    {
      printf("Error: lb_al_copy_minor_matrix_r() --> invalid dimension M1[%d,%d]\r\n",(*M1).rows,(*M1).cols);
      exit(EXIT_FAILURE);
    }
  
  if (!lb_al_assert_dimensions_matrix_r(M2))
    {
      printf("Error: lb_al_copy_minor_matrix_r() --> invalid dimension M2[%d,%d]\r\n",(*M2).rows,(*M2).cols);
      exit(EXIT_FAILURE);
    }
 
  if ( ((*M1).rows<=1) || ((*M1).cols<=1) ||  
       (((*M1).rows -(*M2).rows) !=1) ||
       (((*M1).cols -(*M2).cols) !=1) ) 
    {
      printf("Error: lb_al_copy_minor_matrix_r() --> invalid dimension M1[%d,%d] M2[%d,%d]\r\n",(*M1).rows,(*M1).cols,(*M2).rows,(*M2).cols);
      exit(EXIT_FAILURE);
    }
  
  if ( ((*M1).rows<=io) || ((*M1).cols<=jo))
    {
      printf("Error: lb_al_copy_minor_matrix_r() --> invalid cell\r\n");
      exit(EXIT_FAILURE);
    }

  i_minor=0;
  for (i=0;i<(*M1).rows;i++)
    {
      j_minor=0;
      for (j=0;j<(*M1).cols;j++)
	{
	  if ( (i!=io) && (j!=jo)) 
	    (*M2).array[i_minor][j_minor]=(*M1).array[i][j];
	  if (j!=jo) j_minor++;
	}
      if (i!=io) i_minor++;
    }
}

void lb_al_copy_vector_c(VECTOR_C_T *V1, VECTOR_C_T *V2)
{ 
  U_INT16_T i;

  if (!lb_al_assert_dimensions_vector_c(V1))
    {
      printf("Error: lb_al_copy_vector_c() --> invalid dimension V1[%d]\r\n",(*V1).items);
      exit(EXIT_FAILURE);
    }
  
  if (!lb_al_assert_dimensions_vector_c(V2))
    {
      printf("Error: lb_al_copy_vector_c() --> invalid dimension V2[%d]\r\n",(*V2).items);
      exit(EXIT_FAILURE);
    }
   
  if ((*V1).items != (*V2).items)
    {
      printf("Error: lb_al_copy_vector_c() --> invalid dimension V1[%d] != V2[%d]\r\n",(*V1).items,(*V2).items);
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*V1).items;i++)
    (*V2).array[i]=(*V1).array[i];
}

void lb_al_copy_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2)
{ 
  U_INT16_T i;

  if (!lb_al_assert_dimensions_vector_r(V1))
    {
      printf("Error: lb_al_copy_vector_r() --> invalid dimension V1[%d]\r\n",(*V1).items);
      exit(EXIT_FAILURE);
    }
  
  if (!lb_al_assert_dimensions_vector_r(V2))
    {
      printf("Error: lb_al_copy_vector_r() --> invalid dimension V2[%d]\r\n",(*V2).items);
      exit(EXIT_FAILURE);
    }
  
  if ((*V1).items != (*V2).items)
    {
      printf("Error: lb_al_copy_vector_r() --> invalid dimension V1[%d] != V2[%d]\r\n",(*V1).items,(*V2).items);
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*V1).items;i++)
    (*V2).array[i]=(*V1).array[i];
}

void lb_al_create_array_r(ARRAY_R_T *M)
{
  U_INT16_T j[5];

  if (!lb_al_assert_dimensions_array_r(M))
    {
      printf("Error: lb_al_create_array_r() --> invalid dimension, dim=%d, [%d][%d][%d][%d][%d]\r\n",
		   (*M).n,(*M).dim[4],(*M).dim[3],(*M).dim[2],(*M).dim[1],(*M).dim[0]);
      exit(EXIT_FAILURE);
    }
  
  switch ((*M).n)
    {
    case 1:
      printf("Creating M[%d]\r\n",(*M).dim[0]);
      (*M).a1=(FLOAT_T*)malloc((*M).dim[0]*sizeof(FLOAT_T));
      if ((*M).a1==NULL)
	{
	  printf("Error: lb_al_create_array_r() --> malloc failed 1.0\r\n");
	  exit(EXIT_FAILURE);
	}
      
      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
	(*M).a1[j[0]]=0.0;
      break;
    case 2:
      (*M).a2=malloc((*M).dim[1]*sizeof(FLOAT_T*));
      if ((*M).a2==NULL)
	{
	  printf("Error: lb_al_create_array_r() --> malloc failed 2.0\r\n");
	  exit(EXIT_FAILURE);
	}
      for (j[1]=0;j[1]<(*M).dim[1];j[1]++)
	{
	  (*M).a2[j[1]]=malloc((*M).dim[0]*sizeof(FLOAT_T));
	  if ((*M).a2[j[1]]==NULL)
	    {
	      printf("Error: lb_al_create_array_r() -- malloc failed 2.1 [%d]\r\n",j[1]);
	      exit(EXIT_FAILURE);
	    }
	  for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
	    (*M).a2[j[1]][j[0]]=0.0;
	}
      break;
    case 3:
      (*M).a3=malloc((*M).dim[2]*sizeof(FLOAT_T**));
      if ((*M).a3==NULL)
	{
	  printf("Error: lb_al_create_array_r() --> malloc failed 3.0\r\n");
	  exit(EXIT_FAILURE);
	}

      for (j[2]=0;j[2]<(*M).dim[2];j[2]++)
	{ 
	  (*M).a3[j[2]]=malloc((*M).dim[1]*sizeof(FLOAT_T*));
	  if ((*M).a3[j[2]]==NULL)
	    {
	      printf("Error: lb_al_create_array_r() --> malloc failed 3.1 [%d]\r\n",j[2]);
	      exit(EXIT_FAILURE);
	    }
	  for (j[1]=0;j[1]<(*M).dim[1];j[1]++)
	    {
	      (*M).a3[j[2]][j[1]]=malloc((*M).dim[0]*sizeof(FLOAT_T));
	      if ((*M).a3[j[2]][j[1]]==NULL)
		{
		  printf("Error: lb_al_create_array_r() -- malloc failed 3.2 [%d] [%d]\r\n",j[2],j[1]);
		  exit(EXIT_FAILURE);
		}
	      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
		(*M).a3[j[2]][j[1]][j[0]]=0.0;
	    }
	}
      break;
    case 4:
      (*M).a4=malloc((*M).dim[3]*sizeof(FLOAT_T***));
      if ((*M).a4==NULL)
	{
	  printf("Error: lb_al_create_array_r() --> malloc failed 4.0\r\n");
	  exit(EXIT_FAILURE);
   	}
	 
      for (j[3]=0;j[3]<(*M).dim[3];j[3]++)
	{
	  (*M).a4[j[3]]=(FLOAT_T***)malloc((*M).dim[2]*sizeof(FLOAT_T**));
	  if ((*M).a4[j[3]]==NULL)
	    {
	      printf("Error: lb_al_create_array_r() --> malloc failed 4.1 [%d]\r\n",j[3]);
	     exit(EXIT_FAILURE);
   	    }

	  for (j[2]=0;j[2]<(*M).dim[2];j[2]++)
	    {
	      (*M).a4[j[3]][j[2]]=(FLOAT_T**)malloc((*M).dim[1]*sizeof(FLOAT_T*));
	      if ((*M).a4[j[3]][j[2]]==NULL)
		{
		  printf("Error: lb_al_create_array_r() --> malloc failed 4.2 [%d][%d]\r\n",j[3],j[2]);
		  exit(EXIT_FAILURE);
		}

	      for (j[1]=0;j[1]<(*M).dim[1];j[1]++)
		{
		  (*M).a4[j[3]][j[2]][j[1]]=(FLOAT_T*)malloc((*M).dim[0]*sizeof(FLOAT_T));
		  if ((*M).a4[j[3]][j[2]][j[1]]==NULL)
		    {
		      printf("Error: lb_al_create_array_r() --> malloc failed 4.3 [%d][%d][%d]\r\n",j[3],j[2],j[1]);
		      exit(EXIT_FAILURE);
   		    }

		  for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
		    (*M).a4[j[3]][j[2]][j[1]][j[0]]=0.0;
		}
	    }
	}
      break;
    case 5:
      (*M).a5=malloc((*M).dim[4]*sizeof(FLOAT_T****));
      if ((*M).a5==NULL)
	{
	  printf("Error: lb_al_create_array_r() --> malloc failed: 5.0\r\n");
	  exit(EXIT_FAILURE);
   	}

      for (j[4]=0;j[4]<(*M).dim[4];j[4]++)
	{      
	  (*M).a5[j[4]]=malloc((*M).dim[3]*sizeof(FLOAT_T***));
	  if ((*M).a5[j[4]]==NULL)
	    {
	      printf("Error: lb_al_create_array_r() --> malloc failed 5.1 [%d]\r\n",j[4]);
	      exit(EXIT_FAILURE);
      	    }
	 
	  for (j[3]=0;j[3]<(*M).dim[3];j[3]++)
	    {
	      (*M).a5[j[4]][j[3]]=(FLOAT_T***)malloc((*M).dim[2]*sizeof(FLOAT_T**));
	      if ((*M).a5[j[4]][j[3]]==NULL)
		{
		  printf("Error: lb_al_create_array_r() --> malloc failed 5.2 [%d][%d]\r\n",j[4],j[3]);
		  exit(EXIT_FAILURE);
  		}

	      for (j[2]=0;j[2]<(*M).dim[2];j[2]++)
		{
		  (*M).a5[j[4]][j[3]][j[2]]=(FLOAT_T**)malloc((*M).dim[1]*sizeof(FLOAT_T*));
		  if ((*M).a5[j[4]][j[3]][j[2]]==NULL)
		    {
		      printf("Error: lb_al_create_array_r() --> malloc failed 5.3 [%d][%d][%d]\r\n",j[4],j[3],j[2]);
		      exit(EXIT_FAILURE);
  		    }

		  for (j[1]=0;j[1]<(*M).dim[1];j[1]++)
		    {
		      (*M).a5[j[4]][j[3]][j[2]][j[1]]=(FLOAT_T*)malloc((*M).dim[0]*sizeof(FLOAT_T));
		      if ((*M).a5[j[4]][j[3]][j[2]][j[1]]==NULL)
			{
			  printf("Error: lb_al_create_array_r() --> malloc failed 5.4 [%d][%d][%d][%d]\r\n",j[4],j[3],j[2],j[1]);
			  exit(EXIT_FAILURE);
   			}
		      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
			(*M).a5[j[4]][j[3]][j[2]][j[1]][j[0]]=0.0;
		    }
		}
	    }
	}
      break;
    default:
      break;
    }
}

void  lb_al_create_matrix_p2d(MATRIX_POINT_2D_T *S)
{
  U_INT16_T i,j;

  if (!lb_al_assert_dimensions_matrix_p2d(S))
    {
      printf("Error: lb_al_create_surface_2df() --> invalid dimension S[%d,%d]\r\n", (*S).rows, (*S).cols);
      exit(EXIT_FAILURE);
    }
  
  (*S).array=malloc((*S).rows*sizeof(POINT_2D_FLOAT_T*));

  if ((*S).array==NULL)
    {
      printf("Error: lb_al_create_matrix_p2d() --> out of memory (1)\r\n");
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*S).rows;i++)
    { 
      (*S).array[i]=malloc((*S).cols*sizeof(POINT_2D_FLOAT_T));
      if ((*S).array[i]== NULL)
	{
	  printf("Error: lb_al_create_matrix_p2d() --> out of memory (2)\r\n");
	  exit(EXIT_FAILURE);
   	}
    }

  for (i=0;i<(*S).rows;i++)
    for (j=0;j<(*S).cols;j++)
      {
	(*S).array[i][j].x=0.0;
	(*S).array[i][j].y=0.0;
      }
}


void  lb_al_create_matrix_p3d(MATRIX_POINT_3D_T *S)
{
  U_INT16_T i,j;

  if (!lb_al_assert_dimensions_matrix_p3d(S))
    {
      printf("Error: lb_al_create_matrix_p3d() --> invalid dimension S[%d,%d]\r\n",(*S).rows,(*S).cols);
      exit(EXIT_FAILURE);
    }
  
  (*S).array=malloc((*S).rows*sizeof(POINT_3D_FLOAT_T*));

  if ((*S).array==NULL)
    {
      printf("Error: lb_al_create_matrix_p3d() --> out of memory (1)\r\n");
      exit(EXIT_FAILURE);
    }
  for (i=0;i<(*S).rows;i++)
    { 
      (*S).array[i]=malloc((*S).cols*sizeof(POINT_3D_FLOAT_T));
      if ((*S).array[i]== NULL)
	{
	  printf("Error: lb_al_create_matrix_p3d() --> out of memory (2)\r\n");
	  exit(EXIT_FAILURE);
	}
    }

  for (i=0;i<(*S).rows;i++)
    for (j=0;j<(*S).cols;j++)
      {
	(*S).array[i][j].x=0.0;
	(*S).array[i][j].y=0.0;
	(*S).array[i][j].z=0.0;
      }
}

void lb_al_create_matrix_r(MATRIX_R_T *M)
{
  U_INT16_T i,j;

  if (!lb_al_assert_dimensions_matrix_r(M))
    {
      printf("Error: lb_al_create_matrix_r() --> invalid dimension M[%d,%d]\r\n",(*M).rows, (*M).cols);
      exit(EXIT_FAILURE);
    }

  (*M).array=malloc((*M).rows*sizeof(FLOAT_T*));

  if ((*M).array==NULL)
    {
      printf("Error: lb_al_create_matrix_r() --> out of memory (1)\r\n");
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*M).rows;i++)
    { 
      (*M).array[i]=malloc((*M).cols*sizeof(FLOAT_T));
      if ((*M).array[i]== NULL)
	{
	  printf("Error: lb_al_create_matrix_r() --> out of memory (2)\r\n");
	  exit(EXIT_FAILURE);
	}
    }
  
  for (i=0;i<(*M).rows;i++)
    for (j=0;j<(*M).cols;j++)
      (*M).array[i][j]=0.0;
} 

void  lb_al_create_matrix_si8(MATRIX_S_INT8_T *M)
{
  U_INT16_T i, j;

  if (!lb_al_assert_dimensions_matrix_si8(M))
    {
      printf("Error: lb_al_create_matrix_si8() --> invalid dimension M[%d,%d]\r\n",(*M).rows, (*M).cols);
      exit(EXIT_FAILURE);
    }
  
  (*M).array=malloc((*M).rows*sizeof(S_INT8_T*));

  if ((*M).array==NULL)
    {
      printf("Error: lb_al_create_matrix_si8() --> out of memory (1)\r\n");
      exit(EXIT_FAILURE);
    }
  for (i=0;i<(*M).rows;i++)
    { 
      (*M).array[i]=malloc((*M).cols*sizeof(S_INT8_T));
      if ((*M).array[i]== NULL)
	{
	  printf("Error: lb_al_create_matrix_si8() --> out of memory (2)\r\n");
	  exit(EXIT_FAILURE);
	}
    }
  for (i=0;i<(*M).rows;i++)
    for (j=0;j<(*M).cols;j++)
      (*M).array[i][j]=0;
}


void lb_al_create_vector_c(VECTOR_C_T *V)
{
  U_INT16_T i;

  if (!lb_al_assert_dimensions_vector_c(V))
    {
      printf("Error: lb_al_create_vector_c() --> invalid dimension V[%d]\r\n",(*V).items);
      exit(EXIT_FAILURE);
    }
     
  (*V).array=malloc((*V).items*sizeof(COMPLEX_T));

  if ((*V).array==NULL)
    {
      printf("Error: lb_al_create_vector_c() --> Out of memory\r\n");
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*V).items;i++)
    {
      (*V).array[i].r=0.0;
      (*V).array[i].i=0.0;
    }
}

void lb_al_create_vector_r(VECTOR_R_T *V)
{
  U_INT16_T i;
  if (!lb_al_assert_dimensions_vector_r(V))
    {
      printf("Error: lb_al_create_vector_r() --> invalid dimension [%d]\r\n",(*V).items);
      exit(EXIT_FAILURE);
    }
  
  if ((*V).items!=0)
    {
      (*V).array=(FLOAT_T *)malloc((*V).items*sizeof(FLOAT_T));
  
      if ((*V).array==NULL)
	{
	  printf("Error: lb_al_create_vector_r() --> failed malloc\r\n");
	  exit(EXIT_FAILURE);
	}
      
      for (i=0;i<(*V).items;i++)
	(*V).array[i]=0.0;
    }
}

void lb_al_create_vector_si8(VECTOR_S_INT8_T *V)
{
  U_INT16_T i;

  if (!lb_al_assert_dimensions_vector_si8(V))
    {
      printf("Error: lb_al_create_vector_si8() --> invalid dimension V[%d]\r\n",(*V).items);
      exit(EXIT_FAILURE);
    }
  
  (*V).array=malloc((*V).items*sizeof(S_INT8_T));

  if ((*V).array==NULL)
    {
      printf("Error: lb_al_create_vector_si8() --> out of memory\r\n");
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*V).items;i++)
    (*V).array[i]=0;
}


void lb_al_create_vector_si16(VECTOR_S_INT16_T *V)
{
  U_INT16_T i;

  if (!lb_al_assert_dimensions_vector_si16(V))
    {
      printf("Error: lb_al_create_vector_i() --> invalid dimension V[%d]\r\n",(*V).items);
      exit(EXIT_FAILURE);
    }
  
  (*V).array=malloc((*V).items*sizeof(S_INT16_T));
  if ((*V).array==NULL)
    {
      printf("Error: lb_al_create_vector_si16() --> out of memory\r\n");
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*V).items;i++)
    (*V).array[i]=0;
}

void lb_al_cross_product_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2, VECTOR_R_T *V3)
{
  if (!lb_al_assert_dimensions_vector_r(V1))
    {
      printf("Error: lb_al_cross_product_vector_r() --> invalid dimension V1[%d]\r\n",(*V1).items);
      exit(EXIT_FAILURE);
    }
  
  if (!lb_al_assert_dimensions_vector_r(V2))
    {
      printf("Error: lb_al_cross_product_vector_r() --> invalid dimension V2[%d]\r\n",(*V2).items);
      exit(EXIT_FAILURE);
    }
  
  if (!lb_al_assert_dimensions_vector_r(V3))
    {
      printf("Error: lb_al_cross_product_vector_r() --> invalid dimension V3[%d]\r\n",(*V3).items);
      exit(EXIT_FAILURE);
    }
  
  if ( ((*V1).items != 3) || ((*V2).items != 3) || ((*V3).items != 3))
    {
      printf("Error: lb_al_cross_product_vector_r() --> invalid dimension [dim!=3] V1[%d] V2[%d] V3[%d]\r\n",
	     (*V1).items,(*V2).items,(*V3).items);
      exit(EXIT_FAILURE);
    }
  
  (*V3).array[0]=(*V1).array[1]*(*V2).array[2]-(*V1).array[2]*(*V2).array[1];
  (*V3).array[1]=(*V1).array[2]*(*V2).array[0]-(*V1).array[0]*(*V2).array[2];
  (*V3).array[2]=(*V1).array[0]*(*V2).array[1]-(*V1).array[1]*(*V2).array[0];
}

void lb_al_cross_product_vector3_r(FLOAT_T V1[3], FLOAT_T V2[3], FLOAT_T V3[3])
{
  V3[0]=V1[1]*V2[2]-V1[2]*V2[1];
  V3[1]=V1[2]*V2[0]-V1[0]*V2[2];
  V3[2]=V1[0]*V2[1]-V1[1]*V2[0];
}

void lb_al_delete_item_vector_r(VECTOR_R_T *V, U_INT16_T pos)
{
  lb_al_delete_n_items_vector_r(V, pos, 1);
}

#ifdef NADA
void lb_al_delete_item_vector_r(VECTOR_R_T *V, U_INT16_T pos)
{
  VECTOR_R_T V_temp;
  U_INT16_T i,j;
  
  if (!lb_al_assert_dimensions_vector_r(V))
    {
      printf("Error: lb_al_delete_item_vector_r() --> invalid dimension V[%d]\r\n",(*V).items);
      exit(EXIT_FAILURE);
    }
   
  if ((*V).items==1)
    {
      (*V).items=0;
      lb_al_release_vector_r(V); /* We discard the old memory block */
    }
  else
    {
      if (pos>=(*V).items)
	{
	  printf("Error: lb_al_delete_item_vector_r() --> pos [%d] >= vector.size [%d]\r\n",pos,(*V).items);
	  exit(EXIT_FAILURE);
	}
      V_temp.items=(*V).items-1;
      lb_al_create_vector_r(&V_temp);

      j=0;
      for(i=0;i<(*V).items;i++)
	{
	  if (i!=pos)
	    {
	      V_temp.array[j]=(*V).array[i];
	      j++;
	    }
	}
      lb_al_release_vector_r(V);
      (*V).items=V_temp.items;
      (*V).array=V_temp.array;
    }
}
#endif

#ifdef NADA
/* This is a different implementation in which a fill copy if the vector is made.
   It is slower but may be useful later */
void lb_al_delete_item_vector_r(VECTOR_R_T *V, U_INT16_T pos)
{
  VECTOR_R_T V_copy;
  U_INT16_T i,j;
  
  if (!lb_al_assert_dimensions_vector_r(V))
    {
      printf(ty_C,"Error: lb_al_delete_item_vector_r() --> invalid dimension V[%d]\r\n", (*V).items);
      exit(EXIT_FAILURE);
    }
  
  if ((*V).items==1)
    {
      (*V).items=0;
      lb_al_release_vector_r(V); /* We discard the old memory block */
    }
  else
    {
      if (pos>=(*V).items)
	{
	  printf("Error: lb_al_delete_item_vector_r --> pos [%d] >= vector.size [%d]\r\n",pos,(*V).items);
	  exit(EXIT_FAILURE);
	}
      
      lb_ft_printf(ty_C,"hello\r\n");
      V_copy.items=(*V).items;
      lb_al_create_vector_r(&V_copy);
      for(i=0;i<(*V).items;i++)
	V_copy.array[i]=(*V).array[i];

      lb_al_release_vector_r(V);
      (*V).items=V_copy.items-1;
      lb_al_create_vector_r(V);
      /*(*V).array=(FLOAT_T *)malloc((*V).items*sizeof(FLOAT_T));
	if ((*V).array==NULL)
	{
	printf(ty_C,"Error: lb_al_delete_teme_vector_r() --> Out of memory\r\n");
	exit(EXIT_FAILURE);
				  
	}*/

      j=0;
      for(i=0;i<V_copy.items;i++)
	{
	  lb_ft_printf(ty_C,"i=%d, j=%d, pos=%d\r\n",i,j,pos);
	  if (i!=pos)
	    {
	      (*V).array[j]=V_copy.array[i];
	      j++;
	    }
	}
      lb_al_release_vector_r(&V_copy);
    }
}
#endif


void lb_al_delete_n_items_vector_r(VECTOR_R_T *V, U_INT16_T pos, U_INT16_T n)
{
  VECTOR_R_T V_temp;
  U_INT16_T i, j;
  
  if (!lb_al_assert_dimensions_vector_r(V))
    {
      printf("Error: lb_al_delete_n_items_vector_r() --> invalid dimension V[%d]\r\n",(*V).items);
      exit(EXIT_FAILURE);
    }
  
  if ((pos+(n-1))>=(*V).items)
    {
      printf("Error: lb_al_delete_n_items_vector_r() --> deleting pos >= %d\r\n",(*V).items-1);
      exit(EXIT_FAILURE);
    }
  
  if (((*V).items-n)==0)
    {
      (*V).items=0;
      lb_al_release_vector_r(V); /* We discard the old memory block */
      return;
    }

  if(n==0)  /* Deleting 0 items means no operation is to be performed */
    return;
    
  V_temp.items=(*V).items-n;
  //lb_ft_printf(ty_C,"V_temp.items=%d = %d minus %d \r\n",V_temp.items,(*V).items,n);
  lb_al_create_vector_r(&V_temp);
    
  j=0;
  for(i=0;i<(*V).items;i++)
    {
      if((i<pos) || (i>pos+(n-1)))
	{
	  V_temp.array[j]=(*V).array[i];
	  j++;
	}
    }
  lb_al_release_vector_r(V);
  (*V).items=V_temp.items;
  (*V).array=V_temp.array;
}

FLOAT_T lb_al_determinant_matrix_r(MATRIX_R_T *M)
{
  U_INT16_T j, n;
  FLOAT_T det = 0.0;
  MATRIX_R_T SUB;

  /* We assert the size */
  if (!lb_al_assert_dimensions_matrix_r(M))
    {
      printf("Error: lb_al_determinant_matrix_r() --> invalid dimension M[%d,%d]\r\n", (*M).rows,(*M).cols);
      exit(EXIT_FAILURE);
    }
   
  /* Determinant is only defined for square matrix */
  if ((*M).rows != (*M).cols)
    {
      printf("Error: lb_al_determinant_matrix_r() --> invalid dimension rows[%d] != cols[%d]\r\n",
	     (*M).rows,(*M).cols);
      exit(EXIT_FAILURE);
    }
  
  n=(*M).rows;  /* n makes the code clearer */

  if (n == 1)
    det = (*M).array[0][0];
  else if (n == 2)
    det =  (*M).array[0][0]*(*M).array[1][1] - (*M).array[1][0]*(*M).array[0][1];
  else
    {
      for (j=0; j<n; j++) 
	{
	  SUB.rows=n-1;
	  SUB.cols=n-1;
	  lb_al_create_matrix_r(&SUB);
	  lb_al_copy_minor_matrix_r(M,&SUB,0,j);
	  det += pow(-1.0,1.0 + j + 1.0)*(*M).array[0][j]*lb_al_determinant_matrix_r(&SUB);
	  lb_al_release_matrix_r(&SUB);
	}
    }	
  return det;
}

FLOAT_T lb_al_determinant_matrix_r_gauss(MATRIX_R_T *M)
{
  MATRIX_R_T NEW_M;
  S_INT16_T i, j, k, n;
  FLOAT_T factor, temp, det;
  
  /* We assert the size */
  if (!lb_al_assert_dimensions_matrix_r(M))
    {
      printf("Error: lb_al_determinant_matrix_r_gauss() --> invalid dimension M[%d,%d]\r\n",(*M).rows,(*M).cols);
      exit(EXIT_FAILURE);
    }
  
  /* Determinant is only defined for square matrix */
  if ((*M).rows != (*M).cols)
    {
      printf("Error: lb_al_determinant_matrix_r_gauss() --> invalid dimension rows[%d] != cols[%d]\r\n",
	     (*M).rows,(*M).cols);
      exit(EXIT_FAILURE);
    }
   
  n=(*M).rows;  /* n makes the code clearer */

  NEW_M.rows=n;
  NEW_M.cols=n;
  lb_al_create_matrix_r(&NEW_M);
  lb_al_copy_matrix_r(M,&NEW_M);
  
  det=1.0;
  /* Do the elimination */
  for(i=0;i<n;i++) 
    {
      /* if the main diagonal value is zero,  re-sort the array */
      if (NEW_M.array[i][i]==0) 
	{
	  for(j=i+1;j<n;j++)
	    {
	      if (NEW_M.array[j][i] != 0) 
		{
		  for(k=0;k<n;k++) 
		    {
		      temp=NEW_M.array[i][k];
		      NEW_M.array[i][k]=NEW_M.array[j][k];
		      NEW_M.array[j][k]=temp;
		    }
		  /* For Gauss-Jordan Elimination, if we do a switch, the determinant, switches sign */
		  det = -det;
		  break;
		}
	    }
	}

      /* if after the resorting, the value is still zero, then the determinant is definitely zero */
      if (NEW_M.array[i][i]== 0) 
	return 0.0;

      /* eliminate the lower rows to achieve triangular zeroes */
      for(j=i+1;j<n;j++) 
	{
	  if (NEW_M.array[j][i]!=0) 
	    {
	      factor = (NEW_M.array[j][i] * 1.0) / NEW_M.array[i][i];
	      for(k=i;k<n;k++)
		{
		  NEW_M.array[j][k]=NEW_M.array[j][k] - factor * NEW_M.array[i][k];
		  //{2}
		}
	      //{1}
	    }
	}
    }
  lb_al_print_matrix_r(&NEW_M,"N",FLOAT_FORMAT_MATRIX);

  /* calculate the main diagonal */
  for(i=0;i<n;i++)
    det*=NEW_M.array[i][i];
  lb_al_release_matrix_r(&NEW_M);
  return det;
}

FLOAT_T   lb_al_determinant_matrix22_r(FLOAT_T M[2][2])
{
  return M[0][0]*M[1][1] -  M[0][1]*M[1][0];
}


FLOAT_T   lb_al_determinant_matrix33_r(FLOAT_T M[3][3])
{
  return M[0][0]*(M[1][1]*M[2][2]-M[1][2]*M[2][1]) -
    M[0][1]*(M[1][0]*M[2][2]-M[1][2]*M[2][0]) +
    M[0][2]*(M[1][0]*M[2][1]-M[1][1]*M[2][0]);
}

FLOAT_T lb_al_dot_product_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2)
{
  U_INT16_T i;
  FLOAT_T temp_f;

  if (!lb_al_assert_dimensions_vector_r(V1))
    {
      printf("Error: lb_al_dot_product_vector_r() --> invalid dimension V1[%d]\r\n",(*V1).items);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_vector_r(V2))
    {
      printf("Error: lb_al_dot_product_vector_r() --> invalid dimension V2[%d]\r\n",(*V2).items);
      exit(EXIT_FAILURE);
    }

  if ((*V1).items != (*V2).items)
    {
      printf("Error: lb_al_dot_product_vector_r() --> invalid dimension V1[%d] != V2[%d]\r\n",
	     (*V1).items,(*V2).items);
      exit(EXIT_FAILURE);
    }
       
  temp_f=0.0;
  for (i=0;i<(*V1).items;i++)
    temp_f+=(*V1).array[i]*(*V2).array[i];
  return temp_f;
}

void lb_al_fill_identity_matrix_r(MATRIX_R_T *M)
{
  U_INT16_T i, j;
  /* We assert the size */
  if (!lb_al_assert_dimensions_matrix_r(M))
    {
      printf("Error: lb_al_fill_identity_matrix_r() --> invalid dimension M[%d,%d]\r\n",(*M).rows,(*M).cols);
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*M).rows;i++)
    for (j=0;j<(*M).cols;j++)
      if (i==j) 
	(*M).array[i][j]=1.0;
      else
	(*M).array[i][j]=0.0;
}

void lb_al_fill_rotation_matrix_tait_bryan_ZYX(MATRIX_R_T *R, FLOAT_T yaw, FLOAT_T pitch, FLOAT_T roll)
{
  MATRIX_R_T RZ, RX, RY, temp;

  RX.rows=3;
  RX.cols=3;
  lb_al_create_matrix_r(&RX);

  RY.rows=3;
  RY.cols=3;
  lb_al_create_matrix_r(&RY);

  RZ.rows=3;
  RZ.cols=3;
  lb_al_create_matrix_r(&RZ);
  
  temp.rows=3;
  temp.cols=3;
  lb_al_create_matrix_r(&temp);

  /* Matrix must be 3x3 */
  if ( ((*R).rows != 3)  || ((*R).cols != 3))
    {
      printf("Error: lb_al_fill_rotation_matrix_tait_bryan_ZYX() --> invalid dimension: must be 3x3 R[%d,%d]\r\n",
	     (*R).rows, (*R).cols);
      exit(EXIT_FAILURE);
    }
    
  lb_al_fill_rotation_matrix_Z(&RZ, yaw);   /* yaw */
  lb_al_fill_rotation_matrix_Y(&RY, pitch); /* pitch */
  lb_al_fill_rotation_matrix_X(&RX, roll);  /* roll */ 

  lb_al_multiply_matrix_r(&RZ, &RY, &temp);
  lb_al_multiply_matrix_r(&temp, &RX, R);

  lb_al_release_matrix_r(&RX);
  lb_al_release_matrix_r(&RY);
  lb_al_release_matrix_r(&RZ);
  lb_al_release_matrix_r(&temp);
}

/* This matrix allows rotating a point an angle around a vector */
void lb_al_fill_rotation_matrix_vector(MATRIX_R_T *R, FLOAT_T x, FLOAT_T y, FLOAT_T z, FLOAT_T angle)
{ 
  FLOAT_T c, s, t;
  
  /* Matrix must be 3x3 
     Vector must be 3x1 */

  if ( ((*R).rows != 3)  ||
       ((*R).cols != 3) )
    {
      printf("Error: lb_al_fill_rotation_matrix_vector() --> invalid dimension R[%d,%d]\r\n",(*R).rows,(*R).cols);
      exit(EXIT_FAILURE);
    }
  
  c=cos(angle);
  s=sin(angle);
  t=1.0-c;
  
  (*R).array[0][0] = t*x*x + c;      (*R).array[0][1] = t*x*y + s*z;   (*R).array[0][2] = t*x*z - x*y;
  (*R).array[1][0] = t*x*y - s*z;    (*R).array[1][1] = t*y*y + c;     (*R).array[1][2] = t*y*z + s*x;
  (*R).array[2][0] = t*x*z + s*y;    (*R).array[2][1] = t*y*z - x*s;   (*R).array[2][2] = t*z*z + c;
}

void lb_al_fill_rotation_matrix_X(MATRIX_R_T *R, FLOAT_T angle_x)
{
  FLOAT_T c, s;

  c=cos(angle_x);
  s=sin(angle_x);
  (*R).array[0][0] = 1.0;  (*R).array[0][1] = 0.0; (*R).array[0][2] = 0.0;
  (*R).array[1][0] = 0.0;  (*R).array[1][1] = c;   (*R).array[1][2] =  -s;
  (*R).array[2][0] = 0.0;  (*R).array[2][1] = s;   (*R).array[2][2] =   c;
}

void lb_al_fill_rotation_matrix_Y(MATRIX_R_T *R, FLOAT_T angle_y)
{
  FLOAT_T c, s;
  c=cos(angle_y);
  s=sin(angle_y);
  (*R).array[0][0] =   c;  (*R).array[0][1] = 0.0; (*R).array[0][2] =   s;
  (*R).array[1][0] = 0.0;  (*R).array[1][1] = 1.0; (*R).array[1][2] = 0.0;
  (*R).array[2][0] =  -s;  (*R).array[2][1] = 0.0; (*R).array[2][2] =   c;
}

void lb_al_fill_rotation_matrix_Z(MATRIX_R_T *R, FLOAT_T angle_z)
{
  FLOAT_T c, s;

  c=cos(angle_z);
  s=sin(angle_z);
  (*R).array[0][0] = c;    (*R).array[0][1] =  -s;   (*R).array[0][2] = 0.0;
  (*R).array[1][0] = s;    (*R).array[1][1] =   c;   (*R).array[1][2] = 0.0;
  (*R).array[2][0] = 0.0;  (*R).array[2][1] = 0.0;   (*R).array[2][2] = 1.0;
}

void lb_al_fill_rotation_matrix33_tait_bryan_ZYX(FLOAT_T R[3][3], FLOAT_T yaw, FLOAT_T pitch, FLOAT_T roll)
{
  FLOAT_T RZ[3][3], RX[3][3], RY[3][3], TEMP[3][3];

  lb_al_fill_rotation_matrix33_Z(RZ, yaw);   /* yaw */
  lb_al_fill_rotation_matrix33_Y(RY, pitch); /* pitch */
  lb_al_fill_rotation_matrix33_X(RX, roll);  /* roll */ 

  lb_al_multiply_matrix33_r(RZ, RY, TEMP);
  lb_al_multiply_matrix33_r(TEMP, RX, R);
}

void lb_al_fill_rotation_matrix33_X(FLOAT_T R[3][3], FLOAT_T angle_x)
{
  FLOAT_T c, s;

  c=cos(angle_x);
  s=sin(angle_x);
  R[0][0] = 1.0;  R[0][1] = 0.0; R[0][2] = 0.0;
  R[1][0] = 0.0;  R[1][1] = c;   R[1][2] =  -s;
  R[2][0] = 0.0;  R[2][1] = s;   R[2][2] =   c;
}

void lb_al_fill_rotation_matrix33_Y(FLOAT_T R[3][3], FLOAT_T angle_y)
{
  FLOAT_T c, s;
  c=cos(angle_y);
  s=sin(angle_y);
  R[0][0] =   c;  R[0][1] = 0.0; R[0][2] =   s;
  R[1][0] = 0.0;  R[1][1] = 1.0; R[1][2] = 0.0;
  R[2][0] =  -s;  R[2][1] = 0.0; R[2][2] =   c;
}

void lb_al_fill_rotation_matrix33_Z(FLOAT_T R[3][3], FLOAT_T angle_z)
{
  FLOAT_T c, s;

  c=cos(angle_z);
  s=sin(angle_z);
  R[0][0] = c;    R[0][1] =  -s;   R[0][2] = 0.0;
  R[1][0] = s;    R[1][1] =   c;   R[1][2] = 0.0;
  R[2][0] = 0.0;  R[2][1] = 0.0;   R[2][2] = 1.0;
}

void lb_al_fill_zeros_matrix_r(MATRIX_R_T *M)
{
  U_INT16_T i, j;
  /* We assert the size */
  if (!lb_al_assert_dimensions_matrix_r(M))
    {
      printf("Error: lb_al_fill_zeros_matrix_r() --> invalid dimension M[%d,%d]\r\n",(*M).rows,(*M).cols);
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*M).rows;i++)
    for (j=0;j<(*M).cols;j++)
      (*M).array[i][j]=0.0;
}

void lb_al_insert_item_vector_r(VECTOR_R_T *V, FLOAT_T x, U_INT16_T pos)
{
  VECTOR_R_T V2, temp_V;
  U_INT16_T i;
 
  if (!lb_al_assert_dimensions_vector_r(V))
    {
      if((*V).items==0)
	lb_ft_printf(ty_C,"Warning: lb_al_insert_item_vector_r(): vector_r has 0 elements\n");
      else
	{
	  printf("Error: lb_al_insert_item_vector_r() --> invalid dimension V[%d]\r\n",(*V).items);
	  exit(EXIT_FAILURE);
	}
    }
  
  V2.items=(*V).items+1;

  if (pos>=V2.items)
    {
      printf("Error: lb_al_insert_item_vector_r --> pos [%d] >= V[%d]\r\n",pos,V2.items);
      exit(EXIT_FAILURE);
    }
  
  lb_al_create_vector_r(&V2);
  for(i=0;i<pos;i++)
    V2.array[i]=(*V).array[i];
      
  V2.array[pos]=x;
   
  for(i=pos+1;i<V2.items;i++)
    V2.array[i]=(*V).array[i-1];
  
  if((*V).items!=0)
    temp_V.array=(*V).array;  /* We keep a copy of the original memory block */
  (*V).array=V2.array;      /* The new memory block is assigned to the variable */
  if(temp_V.items!=0)
    lb_al_release_vector_r(&temp_V); /* We discard the old memory block */
  (*V).items=V2.items;
}

void lb_al_inter_matrix(MATRIX_R_T *M, FLOAT_T ir, FLOAT_T jr, FLOAT_T *Q)
{
  ERR_T error;
  /* In order to interpolate, the matrix mus be at least 2x2 */
  if ( ((*M).rows < 2) || ((*M).rows > MATRIX_MAX_ROWS) ||
       ((*M).cols < 2) || ((*M).cols > MATRIX_MAX_COLS) )
    {
      printf("Error: lb_al_interpolate_from_matrix() --> invalid dimension M[%d,%d]\r\n",
	     (*M).rows,(*M).cols);
      exit(EXIT_FAILURE);
    }
    
  if ( ((S_INT16_T)ir<0) || ((S_INT16_T)ir>((*M).rows-1)) ||
       ((S_INT16_T)jr<0) || ((S_INT16_T)jr>((*M).cols-1)) )
    {
      printf("Error: lb_al_interpolate_from_matrix() --> index out of boundaries i[%d,%d]\r\n",
	     (S_INT16_T)ir,(S_INT16_T)jr);
      exit(EXIT_FAILURE);
    } 
  
  /* If jr is exactly at the corner of the matrix, we return the last item */ 
  if(lb_re_equal(ir,(*M).rows-1) && lb_re_equal(jr,(*M).cols-1))
    *Q=(*M).array[(U_INT16_T)ir][(U_INT16_T)jr];
  else
    if(lb_re_equal(jr,(*M).cols-1))
      lb_re_inter_linear((U_INT16_T)ir,   (*M).array[(U_INT16_T)ir][(*M).cols-1],
			 (U_INT16_T)ir+1, (*M).array[(U_INT16_T)ir+1][(*M).cols-1],
			 ir, Q, &error);
    else
      if(lb_re_equal(ir,(*M).rows-1))
	lb_re_inter_linear((U_INT16_T)jr,   (*M).array[(*M).rows-1][(U_INT16_T)jr],
			   (U_INT16_T)jr+1, (*M).array[(*M).rows-1][(U_INT16_T)jr+1],
			   jr, Q, &error);
      else
	lb_re_inter_bilinear((U_INT16_T)ir, (U_INT16_T)jr, (U_INT16_T)ir+1, (U_INT16_T)jr+1,
			     (*M).array[(U_INT16_T)ir][(U_INT16_T)jr],
			     (*M).array[(U_INT16_T)ir][(U_INT16_T)jr+1],
			     (*M).array[(U_INT16_T)ir+1][(U_INT16_T)jr],
			     (*M).array[(U_INT16_T)ir+1][(U_INT16_T)jr+1],
			     ir, jr, Q, &error);
}

void      lb_al_load_CSV_file(MATRIX_R_T *M, const char filename)
{


}

/* This function multiplies two matrices.  Parameter C cannot be A nor B */
void lb_al_multiply_matrix_r(MATRIX_R_T *A, MATRIX_R_T *B, MATRIX_R_T *C)
{
  U_INT16_T i,j,k;

  /* We assert sizes */
  if (!lb_al_assert_dimensions_matrix_r(A))
    {
      printf("Error: lb_al_multiply_matrix_r() --> invalid dimension A[%d,%d]\r\n",(*A).rows, (*A).cols);
      exit(EXIT_FAILURE);
    }
  
  if (!lb_al_assert_dimensions_matrix_r(B))
    {
      printf("Error: lb_al_multiply_matrix_r() --> invalid dimension B[%d,%d]\r\n",(*B).rows, (*B).cols);
      exit(EXIT_FAILURE);
    }
  
  if (!lb_al_assert_dimensions_matrix_r(C))
    {
      printf("Error: lb_al_multiply_matrix_r() --> invalid dimension C[%d,%d]\r\n",(*C).rows, (*C).cols);
      exit(EXIT_FAILURE);
    }

  if (((*A).rows != (*C).rows) ||
      ((*B).cols != (*C).cols) ||
      ((*A).cols != (*B).rows))
    {
      printf("Error: lb_al_multiply_matrix_r() --> invalid dimension A[%d,%d]*B[%d,%d]=C[%d,%d]\r\n",
	     (*A).rows,(*A).cols,(*B).rows,(*B).cols,(*C).rows,(*C).cols);
      exit(EXIT_FAILURE);
    }

  for (i=0;i<(*A).rows;i++)
    for (j=0;j<(*B).cols;j++)
      {
	(*C).array[i][j]=0.0;
	for(k=0;k<(*A).cols;k++)
	  (*C).array[i][j]+=(*A).array[i][k]*(*B).array[k][j];
      }
}

/* Multiplies two matrices, temporarily storing the result in a different matrix.
   After the product is computed, it copies the data to C, which can be any matrix, including A or B */

void lb_al_multiply_matrix_r_copy(MATRIX_R_T *A, MATRIX_R_T *B, MATRIX_R_T *C)
{
  U_INT16_T i,j,k;
  MATRIX_R_T temp;

  /* We assert sizes */
  if (!lb_al_assert_dimensions_matrix_r(A))
    {
      printf("Error: lb_al_multiply_matrix_r_copy() --> invalid dimension A[%d,%d]\r\n",(*A).rows, (*A).cols);
      exit(EXIT_FAILURE);
    }
  
  if (!lb_al_assert_dimensions_matrix_r(B))
    {
      printf("Error: lb_al_multiply_matrix_r_copy() --> invalid dimension B[%d,%d]\r\n",(*B).rows, (*B).cols);
      exit(EXIT_FAILURE);
    }
  
  if (!lb_al_assert_dimensions_matrix_r(C))
    {
      printf("Error: lb_al_multiply_matrix_r_copy() --> invalid dimension C[%d,%d]\r\n",(*C).rows, (*C).cols);
      exit(EXIT_FAILURE);
    }
  
  if (((*A).rows != (*C).rows) ||
      ((*B).cols != (*C).cols) ||
      ((*A).cols != (*B).rows))
    {
      printf("Error: lb_al_multiply_matrix_r_copy() --> invalid dimension A[%d,%d]*B[%d,%d]=C[%d,%d]\r\n",
		   (*A).rows,(*A).cols,(*B).rows,(*B).cols,(*C).rows,(*C).cols);
      exit(EXIT_FAILURE);
    }

  temp.rows=3;
  temp.cols=3;
  lb_al_create_matrix_r(&temp);

  for (i=0;i<(*A).rows;i++)
    for (j=0;j<(*B).cols;j++)
      {
	temp.array[i][j]=0.0;
	for(k=0;k<(*A).cols;k++)
	  temp.array[i][j]+=(*A).array[i][k]*(*B).array[k][j];
      }
  lb_al_copy_matrix_r(&temp,C);
  lb_al_release_matrix_r(&temp);
}

void lb_al_multiply_matrix_r_vector_r(MATRIX_R_T *A, VECTOR_R_T *B, VECTOR_R_T *C)
{
  U_INT16_T i,k;

  /* We assert sizes */
  if (((*A).rows != (*C).items) ||
      ((*A).cols != (*B).items))
    {
      printf("Error: lb_al_multiply_matrix_r_vector_r() --> invalid dimension\r\n");
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*A).rows;i++)
    {
      (*C).array[i]=0.0;
      for(k=0;k<(*A).cols;k++)
	(*C).array[i]+=(*A).array[i][k]*(*B).array[k];
    }
}

/* This function multiplies two 3x3 matrices.  Parameter C cannot be A nor B */
void lb_al_multiply_matrix33_r(FLOAT_T A[3][3], FLOAT_T B[3][3], FLOAT_T C[3][3])
{
  U_INT16_T i,j,k;

  for (i=0;i<3;i++)
    for (j=0;j<3;j++)
      {
	C[i][j]=0.0;
	for(k=0;k<3;k++)
	  C[i][j]+=A[i][k]*B[k][j];
      }
}


/* This function multiplies two 3x3 matrices, temporarily storing the result in a different matrix.
   After the product is computed, it copies the data to C, which can be any matrix, including A or B */

void lb_al_multiply_matrix33_r_copy(FLOAT_T A[3][3], FLOAT_T B[3][3], FLOAT_T C[3][3])
{
  U_INT16_T i,j,k;
  FLOAT_T TEMP[3][3];
  
  for (i=0;i<3;i++)
    for (j=0;j<3;j++)
      {
	TEMP[i][j]=0.0;
	for(k=0;k<3;k++)
	  TEMP[i][j]+=A[i][k]*B[k][j];
      }
  lb_al_copy_matrix33_r(TEMP,C);
}


void lb_al_multiply_matrix33_r_vector_r(FLOAT_T A[3][3], FLOAT_T B[3], FLOAT_T C[3])
{
  U_INT16_T i,k;

  for (i=0;i<3;i++)
    {
      C[i]=0.0;
      for(k=0;k<3;k++)
	C[i]+=A[i][k]*B[k];
    }
}

void lb_al_multiply_vector_c_real(VECTOR_C_T *V, FLOAT_T k)
{
  U_INT16_T i;

  if (!lb_al_assert_dimensions_vector_c(V))
    {
      printf("Error: lb_al_multiply_vector_c_real() --> invalid dimension [%d]",(*V).items);
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*V).items;i++)
    {
      (*V).array[i].r=k*(*V).array[i].r;
      (*V).array[i].i=k*(*V).array[i].i;
    }
}

void lb_al_multiply_vector_c_real_copy(VECTOR_C_T *V1, FLOAT_T k, VECTOR_C_T *V2)
{
  U_INT16_T i;

  if (!lb_al_assert_dimensions_vector_c(V1) || !lb_al_assert_dimensions_vector_c(V2))
    {
      printf("Error: lb_al_multiply_vector_c_real_copy() --> invalid dimension [%d],[%d]\r\n",
	     (*V1).items,(*V2).items);
      exit(EXIT_FAILURE);
    }
     
  if ((*V1).items != (*V2).items)
    {
      printf("Error: lb_al_multiply_vector_c_real_copy() --> invalid dimension [%d] != [%d]\r\n",
	     (*V1).items,(*V2).items);
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*V1).items;i++)
    {
      (*V2).array[i].r=k*(*V1).array[i].r;
      (*V2).array[i].i=k*(*V1).array[i].i;
    }
}


void lb_al_multiply_vector_r_real(VECTOR_R_T *V, FLOAT_T k)
{
  U_INT16_T i;

  if (!lb_al_assert_dimensions_vector_r(V))
    {
      printf("Error: lb_al_multiply_vector_r_real() --> invalid dimension [%d]",(*V).items);
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*V).items;i++)
    (*V).array[i]=k*(*V).array[i];
}

void lb_al_multiply_vector_r_real_copy(VECTOR_R_T *V1, FLOAT_T k, VECTOR_R_T *V2)
{
  U_INT16_T i;

  if (!lb_al_assert_dimensions_vector_r(V1))
    {
      printf("Error: lb_al_multiply_vector_r_real_copy() --> invalid dimension V1[%d]\r\n",
	     (*V1).items);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_vector_r(V2))
    {
      printf("Error: lb_al_multiply_vector_r_real_copy() --> invalid dimension V2[%d]\r\n",
	     (*V2).items);
      exit(EXIT_FAILURE);
    } 

  if ((*V1).items != (*V2).items)
    {
      printf("Error: lb_al_multiply_vector_r_real_copy() --> invalid dimension V1[%d] != V2[%d]\r\n",
	     (*V1).items,(*V2).items);
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*V1).items;i++)
    (*V2).array[i]=k*(*V1).array[i];
}




FLOAT_T lb_al_norm_matrix_r(MATRIX_R_T *M, ERR_T *error)
{
  FLOAT_T sum;
  U_INT16_T i,j;
 
  if (!lb_al_assert_dimensions_matrix_r(M))
    {
      printf("Error: lb_al_norm_matrix_r() --> invalid dimension M[%d,%d]\r\n",(*M).rows,(*M).cols);
      exit(EXIT_FAILURE);
    }
  
  sum=0;
  for (i=0;i<(*M).rows;i++)
    for (j=0;j<(*M).cols;j++)
      sum+=lb_re_sqr((*M).array[i][j], error);
  return sqrtf(sum);
}

FLOAT_T lb_al_norm_vector_r(VECTOR_R_T *V, ERR_T *error)
{
  FLOAT_T sum;
  U_INT16_T i;
 
  if (!lb_al_assert_dimensions_vector_r(V))
    {
      printf("Error: lb_al_norm_vector_r() --> invalid dimension V[%d]\r\n",(*V).items);
      exit(EXIT_FAILURE);
    }
  
  sum=0;
  for (i=0;i<(*V).items;i++)
    sum+=lb_re_sqr((*V).array[i], error);
  
  return sqrtf(sum);
}

void lb_al_normalize_matrix_r(MATRIX_R_T *M, MATRIX_R_T *Mnorm, ERR_T *error)
{
  FLOAT_T norm;
  U_INT16_T i,j;
 
  if (!lb_al_assert_dimensions_matrix_r(M))
    {
      printf("Error: lb_al_normalize_matrix_r() --> invalid dimension M[%d,%d]\r\n",(*M).rows,(*M).cols);
      exit(EXIT_FAILURE);
    }
     
  if (!lb_al_assert_dimensions_matrix_r(Mnorm))
    {
      printf("Error: lb_al_normalize_matrix_r() --> invalid dimension Mnorm[%d,%d]\r\n",(*Mnorm).rows,(*Mnorm).cols);
      exit(EXIT_FAILURE);
    }
     
  if ( ((*M).rows != (*Mnorm).rows) || ((*M).cols != (*Mnorm).cols) )
    {
      printf("Error: lb_al_normalize_matrix_r() --> invalid dimension M[%d,%d] != Mnorm[%d,%d]\r\n",
	     (*M).rows,(*M).cols,(*Mnorm).rows,(*Mnorm).cols);
      exit(EXIT_FAILURE);
    }
    
  norm=1.0/lb_al_norm_matrix_r(M,error);
  for (i=0;i<(*M).rows;i++)
    for (j=0;j<(*M).cols;j++)
      (*Mnorm).array[i][j]=norm*(*M).array[i][j]; 
}


void lb_al_normalize_vector_r(VECTOR_R_T *V, VECTOR_R_T *Vnorm, ERR_T *error)
{
  FLOAT_T norm;
  U_INT16_T i;
 
  if (!lb_al_assert_dimensions_vector_r(V))
    {
      printf("Error: lb_al_normalize_vector_r() --> invalid dimension V[%d]\r\n",(*V).items);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_vector_r(Vnorm))
    {
      printf("Error: lb_al_normalize_vector_r() --> invalid dimension Vnorm[%d]\r\n",(*Vnorm).items);
      exit(EXIT_FAILURE);
    }
  
  if ( (*V).items != (*Vnorm).items )
    {
      printf("Error: lb_al_normalize_vector_r() --> invalid dimension V[%d] != Vnorm[%d]\r\n",
	     (*V).items, (*Vnorm).items);
      exit(EXIT_FAILURE);
    }
      
  norm=1.0/lb_al_norm_vector_r(V,error);
  for (i=0;i<(*V).items;i++)
    (*Vnorm).array[i]=norm*(*V).array[i];
}

#define UC_C_LU "\u250c"
#define UC_C_RU "\u2510"
#define UC_C_LD "\u2514"
#define UC_C_RD "\u2518"
#define UC_BAR_H "\u2500"
#define UC_BAR_V "\u2502"
#define UC_T_0 "\u251c"
#define UC_T_90 "\u2534"
#define UC_T_180 "\u2524"
#define UC_T_270 "\u252c"
#define UC_CROSS "\u253c"


void lb_al_print_array_r(ARRAY_R_T *M, const char *text, S_INT8_T len, S_INT8_T dec)
{
  U_INT16_T x, j[ARRAY_MAX_DIM], digsize[ARRAY_MAX_DIM];
  S_INT8_T  title_size, n;
  char s0[10], s1[10], s2[10], s3[10], sval[10];

  for (x=0;x<ARRAY_MAX_DIM;x++)
    digsize[x]=lb_re_ndigits((*M).dim[x]-1);

  switch ((*M).n)
    {
    case 1:
      n=strlen(text);
      if (n>digsize[0])
	title_size=n;
      else
	title_size=digsize[0];

      /* Format specifiers */
      strcpy(s0,"");
      sprintf(s0,"%c %du",37,title_size);

      strcpy(sval,"");
      sprintf(sval,"%c%d.%de",37,len,dec);
	 
      /* Print first line */
      printf("\r\n");
      printf(UC_C_LU);
      for (x=0;x<title_size;x++)
	printf(UC_BAR_H);
      printf(UC_T_270);
      for (x=0;x<len;x++)
	printf(UC_BAR_H);
      printf(UC_C_RU);

      /* Second line */
      printf("\r\n");
      printf(UC_BAR_V);
      printf(text,""); 

      for (x=0;x<title_size-strlen(text);x++)
	printf(" ");
      printf(UC_BAR_V);
      for (x=0;x<len;x++)
	printf(" ");
      printf(UC_BAR_V);
      printf("\r\n");
      printf(UC_T_0);

      /* Third line */
      for (x=0;x<title_size;x++)
	printf(UC_BAR_H);
      printf(UC_CROSS);
      for (x=0;x<len;x++)
	printf(UC_BAR_H);
      printf(UC_T_180);
      printf("\r\n");

      /* Values */ 
      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
	{
	  printf(UC_BAR_V);
	  printf(s0,j[0]);
	  printf(UC_BAR_V);
	  
	  printf(sval,(*M).a1[j[0]]);
	  printf(UC_BAR_V);
	  printf("\r\n");
	}
      /* last line */
      printf(UC_C_LD);
      for (x=0;x<title_size;x++)
	printf(UC_BAR_H);
      printf(UC_T_90);
      for (x=0;x<len;x++)
	printf(UC_BAR_H);
      printf(UC_C_RD);
      break;
    case 2:
      n=strlen(text);
      if (n>digsize[1])
	title_size=n;
      else
	title_size=digsize[1];

      /* Format specifiers */
      strcpy(s0,"");
      sprintf(s0,"%c %du",37,digsize[0]);
      strcpy(s1,"");
      sprintf(s1,"%c %du",37,digsize[1]);
      strcpy(sval,"");
      sprintf(sval,"%c%d.%de",37,len,dec);
	      
      /* Print first line */
      printf("\r\n");
      printf(UC_C_LU);
      for (x=0;x<title_size;x++)
	printf(UC_BAR_H);
      printf(UC_T_270);
      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
	for (x=0;x<len;x++)
	  printf(UC_BAR_H);
      
      printf(UC_C_RU);
      printf("\r\n");
      
      /* Print column titles */
      printf(UC_BAR_V);
      printf(text,"");
      if (n<title_size)
	for (x=0;x<title_size-n;x++)
	  printf(" ");
      printf(UC_BAR_V);

      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
	{
	  for (x=0;x<len-digsize[0];x++)
	    printf(" ");
	  printf(s0,j[0]);
	}
      printf(UC_BAR_V);
      printf("\r\n");

      /* Print column separator */
      printf(UC_T_0);
      
      for (x=0;x<title_size;x++)
	printf(UC_BAR_H);
      printf(UC_CROSS);

      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
	for (x=0;x<len;x++)
	  printf(UC_BAR_H);
      
      printf(UC_T_180);
      printf("\r\n");
	     
      /* printing row title */
      for (j[1]=0;j[1]<(*M).dim[1];j[1]++)
	{
	  printf(UC_BAR_V);
	  for (x=0;x<title_size-digsize[1];x++)
	    printf(" ");
	  printf(s1,j[1]);
	  printf(UC_BAR_V);

	  /* Values */
	  for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
	    printf(sval,(*M).a2[j[1]][j[0]]);
	    
	  printf(UC_BAR_V);
	  printf("\r\n");
	}

      /* Print last line */
      printf(UC_C_LD);
      for (x=0;x<title_size;x++)
	printf(UC_BAR_H);
      printf(UC_T_90);
      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
	for (x=0;x<len;x++)
	  printf(UC_BAR_H);
      
      printf(UC_C_RD);
      printf("\r\n");
      break;
    case 3:
      /* Format specifiers */
      strcpy(s0,"");
      sprintf(s0,"%c %du",37,digsize[0]);
      strcpy(s1,"");
      sprintf(s1,"%c %du",37,digsize[1]);
      strcpy(s2,"");
      sprintf(s2,"%c %du",37,digsize[2]);
      strcpy(sval,"");
      sprintf(sval,"%c%d.%de",37,len,dec);
	      
      printf("\r\n");

      for (j[2]=0;j[2]<(*M).dim[2];j[2]++)
	{
	  /* Print first line */
	  j[1]=0;
	  if ( (j[2]==0) && (j[1]==0) )
	    {
	      printf(UC_C_LU);
	      for (x=0;x<digsize[2];x++)
		printf(UC_BAR_H);
	
	      printf(UC_T_270);
	      for (x=0;x<digsize[1];x++)
		printf(UC_BAR_H);
	      printf(UC_T_270);
	      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
		for (x=0;x<len;x++)
		  printf(UC_BAR_H);
	      
	      printf(UC_C_RU);
	    }
	  else
	    {
	      printf(UC_T_0);
	      for (x=0;x<digsize[2];x++)
		printf(UC_BAR_H);
	
	      printf(UC_CROSS);
	      for (x=0;x<digsize[1];x++)
		printf(UC_BAR_H);
	      printf(UC_CROSS);
	      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
		for (x=0;x<len;x++)
		  printf(UC_BAR_H);
	      printf(UC_T_180);
	    }
	    
	  printf("\r\n");
      
	  /* Print column titles */

	  printf(UC_BAR_V);
	  for (x=0;x<digsize[2];x++)
	    printf(" ");
	  
	  printf(UC_BAR_V);
	  for (x=0;x<digsize[1];x++)
	    printf(" ");
	  printf(UC_BAR_V);

	  for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
	    {
	      for (x=0;x<len-digsize[0];x++)
		printf(" ");
	      printf(s0,j[0]);
	    }
	  printf(UC_BAR_V);
	  printf("\r\n");

	  /* Print column separator */
	  printf(UC_T_0);
	  for (x=0;x<digsize[2];x++)
	    printf(UC_BAR_H);

	  printf(UC_CROSS);
      
	  for (x=0;x<digsize[1];x++)
	    printf(UC_BAR_H);
	  printf(UC_CROSS);

	  for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
	    for (x=0;x<len;x++)
	      printf(UC_BAR_H);
      
	  printf(UC_T_180);
	  printf("\r\n");
	     
	  /* printing row title */
	  for (j[1]=0;j[1]<(*M).dim[1];j[1]++)
	    {
	      printf(UC_BAR_V);
	      for (x=0;x<digsize[1]-digsize[1];x++)
		printf(" ");

	      if (j[1]==0)
		printf(s2,j[2]);
	      else
		for (x=0;x<digsize[2];x++)
		  printf(" ");
	      
	      printf(UC_BAR_V);

	      printf(s1,j[1]);
	      printf(UC_BAR_V);

	      /* Values */
	      
	      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
		printf(sval,(*M).a3[j[2]][j[1]][j[0]]);
	    
	      printf(UC_BAR_V);
	      printf("\r\n");
	    }
	}
      printf(UC_C_LD);
      for (x=0;x<digsize[2];x++)
	printf(UC_BAR_H);
      
      printf(UC_T_90);
      for (x=0;x<digsize[1];x++)
	printf(UC_BAR_H);
      printf(UC_T_90);
      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
	{
	  for (x=0;x<len;x++)
	    printf(UC_BAR_H);
	}
      printf(UC_C_RD);
      break;
      /************************************************************************/
    case 4:
      /* Format specifiers */
      strcpy(s0,"");
      sprintf(s0,"%c %du",37,digsize[0]);
      strcpy(s1,"");
      sprintf(s1,"%c %du",37,digsize[1]);
      strcpy(s2,"");
      sprintf(s2,"%c %du",37,digsize[2]);
      strcpy(s3,"");
      sprintf(s3,"%c %du",37,digsize[3]);
      strcpy(sval,"");
      sprintf(sval,"%c%d.%de",37,len,dec);
	      
      printf("\r\n");

      j[3]=0;
      j[2]=0;
      j[1]=0;
      j[0]=0;

      printf(UC_C_LU);
      for (x=0;x<digsize[3];x++)
	printf(UC_BAR_H);

      printf(UC_T_270);
      for (x=0;x<digsize[2];x++)
	printf(UC_BAR_H);
	
      printf(UC_T_270);
      for (x=0;x<digsize[1];x++)
	printf(UC_BAR_H);
      printf(UC_T_270);
      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
	for (x=0;x<len;x++)
	  printf(UC_BAR_H);
      printf(UC_C_RU);
      printf("\r\n");

      while  (j[3]<(*M).dim[3])
	{
	  j[2]=0;
	  j[1]=0;
	  while (j[2]<(*M).dim[2])
	    {
	      if ((j[3]!=0) || (j[2]!=0) ) 
		{
		  if ( j[2]==0 )
		    {
		      printf(UC_T_0);
		      for (x=0;x<digsize[3];x++)
			printf(UC_BAR_H);
		      printf(UC_CROSS);
		      for (x=0;x<digsize[2];x++)
			printf(UC_BAR_H);
		      printf(UC_CROSS);
		      for (x=0;x<digsize[1];x++)
			printf(UC_BAR_H);
		      printf(UC_CROSS);
		    }
		  else
		    {
		      printf(UC_BAR_V);
		      for (x=0;x<digsize[3];x++)
		      	printf(" ");
		      printf(UC_T_0);
		
		      for (x=0;x<digsize[2];x++)
		      	printf(UC_BAR_H);
		      printf(UC_CROSS);
		      for (x=0;x<digsize[1];x++)
		      	printf(UC_BAR_H);
		      printf(UC_CROSS);
 		    }
		  for (x=0;x<len*(*M).dim[0];x++)
		    printf(UC_BAR_H);
		  printf(UC_T_180);
		  printf("\r\n");
		}
	      
	      /* Print column titles */
	      printf(UC_BAR_V);
	  
	      for (x=0;x<digsize[3];x++)
		printf(" ");
	      printf(UC_BAR_V);
	  
	      for (x=0;x<digsize[2];x++)
		printf(" ");

	      printf(UC_BAR_V);

	      for (x=0;x<digsize[1];x++)
		printf(" ");
	      printf(UC_BAR_V);

	      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
		{
		  for (x=0;x<len-digsize[0];x++)
		    printf(" ");
		  printf(s0,j[0]);
		}
	      printf(UC_BAR_V);
	      printf("\r\n");

	      if ( j[2]==0 )
		{
		  printf(UC_T_0);
		  for (x=0;x<digsize[3];x++)
		    printf(UC_BAR_H);
		  printf(UC_CROSS);
		  for (x=0;x<digsize[2];x++)
		    printf(UC_BAR_H);
		  printf(UC_CROSS);
		  for (x=0;x<digsize[1];x++)
		    printf(UC_BAR_H);
		  printf(UC_CROSS);
		}
	      else
		{
		  printf(UC_BAR_V);
		  for (x=0;x<digsize[3];x++)
		    printf(" ");
		  printf(UC_T_0);
		
		  for (x=0;x<digsize[2];x++)
		    printf(UC_BAR_H);
		  printf(UC_CROSS);
		  for (x=0;x<digsize[1];x++)
		    printf(UC_BAR_H);
		  printf(UC_CROSS);
		}
	      for (x=0;x<len*(*M).dim[0];x++)
		printf(UC_BAR_H);
	      printf(UC_T_180);
	      printf("\r\n");
	    
	      
	      /* printing row title */
	      for (j[1]=0;j[1]<(*M).dim[1];j[1]++)
		{
		  printf(UC_BAR_V);
		  for (x=0;x<digsize[2]-digsize[2];x++)
		    printf(" ");
		  if ( (j[2]==0) && (j[1]==0) )
		    printf(s3,j[3]);
		  else
		    for (x=0;x<digsize[3];x++)
		      printf(" ");

		 
		  printf(UC_BAR_V);
		  for (x=0;x<digsize[1]-digsize[1];x++)
		    printf(" ");

		  if (j[1]==0)
		    printf(s2,j[2]);
		  else
		    for (x=0;x<digsize[2];x++)
		      printf(" ");
	      
		  printf(UC_BAR_V);

		  printf(s1,j[1]);
		  printf(UC_BAR_V);

		  /* Values */
	      
		  for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
		    printf(sval,(*M).a4[j[3]][j[2]][j[1]][j[0]]);
	    
		  printf(UC_BAR_V);
		  printf("\r\n");
		}
	      j[2]++;
	    }
	  j[3]++;
	}
      printf(UC_C_LD);
      for (x=0;x<digsize[3];x++)
	printf(UC_BAR_H);

      printf(UC_T_90);
      for (x=0;x<digsize[2];x++)
	printf(UC_BAR_H);
	
      printf(UC_T_90);
      for (x=0;x<digsize[1];x++)
	printf(UC_BAR_H);
      printf(UC_T_90);
      for (j[0]=0;j[0]<(*M).dim[0];j[0]++)
	for (x=0;x<len;x++)
	  printf(UC_BAR_H);
      printf(UC_C_RD);

      break;
    }
}

  void lb_al_print_matrix_r(MATRIX_R_T *M, char *text, const char *format)
{ 
  U_INT16_T i, j;
  char s[40];

  strcpy(s,text);
  strcat(s,"[%02i,%02i]=");
  strcat(s,format);
  strcat(s," ");

  lb_ft_printf(ty_C,"-----------\r\n");

  if (!lb_al_assert_dimensions_matrix_r(M))
    {
      printf("Error: lb_al_print_matrix_r() --> invalid dimension M[%d,%d]\r\n",(*M).rows,(*M).cols);
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*M).rows;i++)
    {
      for (j=0;j<(*M).cols;j++)
	lb_ft_printf(ty_C,s,i,j,(*M).array[i][j]);
      // lb_ft_printf(ty_C,"%s[%02i,%02i]=%06.2f  ",text,i,j,(*M).array[i][j]);
      lb_ft_printf(ty_C,"\r\n");
    }
}

void lb_al_print_matrix33_r(FLOAT_T M[3][3], char *text, const char *format)
{ 
  U_INT16_T i, j;
  char s[40];

  strcpy(s,text);
  strcat(s,"[%02i,%02i]=");
  strcat(s,format);
  strcat(s," ");

  lb_ft_printf(ty_C,"-----------\r\n");

  for (i=0;i<3;i++)
    {
      for (j=0;j<3;j++)
	lb_ft_printf(ty_C,s,i,j,M[i][j]);
      // lb_ft_printf(ty_C,"%s[%02i,%02i]=%06.2f  ",text,i,j,(*M).array[i][j]);
      lb_ft_printf(ty_C,"\r\n");
    }
}

void lb_al_print_vector_c(VECTOR_C_T *V, char *text, const char *format)
{ 
  U_INT16_T i;
  char s[40];

  strcpy(s,text);
  strcat(s,"[%02i]=");
  strcat(s,format);
  
  lb_ft_printf(ty_C,"-----------\r\n");
  
  if (!lb_al_assert_dimensions_vector_c(V))
    {
      printf("Error: lb_al_print_vector_c() --> invalid dimension V[%d]\r\n",(*V).items);
      exit(EXIT_FAILURE);
    }
  
  for (i=0;i<(*V).items;i++)
    {
      lb_ft_printf(ty_C,s,i,(*V).array[i].r);
      lb_ft_printf(ty_C," , ");
      lb_ft_printf(ty_C,format,(*V).array[i].i);
      lb_ft_printf(ty_C,"\r\n");
    }
}

void lb_al_print_vector_r(VECTOR_R_T *V, char *text, const char *format)
{ 
  U_INT16_T i;
  char s[40];

  strcpy(s,text);
  strcat(s,"[%02i]=");
  strcat(s,format);
  
  lb_ft_printf(ty_C,"-----------\r\n");

  if (!lb_al_assert_dimensions_vector_r(V))
    {
      if((*V).items==0)
	lb_ft_printf(ty_C,"Warning: lb_al_print_vector_r(): %s has 0 elements",text);
      else
	{
	  printf("Error: lb_al_print_vector_r() --> invalid dimension V[%d]\r\n",(*V).items);
	  exit(EXIT_FAILURE);
	}
    }
  
  for (i=0;i<V->items;i++)
    lb_ft_printf(ty_C,s,i,V->array[i]);

  //lb_ft_printf(ty_C,"%s[%02i]=%06.2f  ",text,i,(*V).array[i]);
}

void lb_al_print_vector_si8(VECTOR_S_INT8_T *V, char *text)
{ 
  U_INT16_T i;
  char s[40];

  strcpy(s,text);
  strcat(s,"[%02i]=%i\r\n");
 
  lb_ft_printf(ty_C,"-----------\r\n");

  if (!lb_al_assert_dimensions_vector_si8(V))
    {
      if((*V).items==0)
	lb_ft_printf(ty_C,"Warning: lb_al_print_vector_i(): %s has 0 elements",text);
      else
	{
	  printf("Error: lb_al_print_vector_si8() --> invalid dimension V[%d]\r\n",(*V).items);
	  exit(EXIT_FAILURE);
	}			  
    }
  for (i=0;i<V->items;i++)
    lb_ft_printf(ty_C,s,i,V->array[i]);

  lb_ft_printf(ty_C,"\r\n");
}

void lb_al_print_vector_si16(VECTOR_S_INT16_T *V, char *text)
{ 
  U_INT16_T i;
  char s[40];

  strcpy(s,text);
  strcat(s,"[%02i]=%i\r\n");
 
  lb_ft_printf(ty_C,"-----------\r\n");

  if (!lb_al_assert_dimensions_vector_si16(V))
    {
      if((*V).items==0)
	lb_ft_printf(ty_C,"Warning: lb_al_print_vector_i(): %s has 0 elements",text);
      else
	{
	  printf("Error: lb_al_print_vector_si16() --> invalid dimension V[%d]\r\n",(*V).items);
	  exit(EXIT_FAILURE);
	}
    }
  for (i=0;i<V->items;i++)
    lb_ft_printf(ty_C,s,i,V->array[i]);

  lb_ft_printf(ty_C,"\r\n");
}

void lb_al_release_array_r(ARRAY_R_T *M)
{
  S_INT16_T j[5];

  if (!lb_al_assert_dimensions_array_r(M))
    {
      printf("Error: lb_al_release_array_r() --> invalid dimension, dim=%d, [%d][%d][%d][%d][%d]\r\n",
	     (*M).n,(*M).dim[4],(*M).dim[3],(*M).dim[2],(*M).dim[1],(*M).dim[0]);
      exit(EXIT_FAILURE);
    }
  
  switch ((*M).n)
    {
    case 1:
      free((*M).a1);
      break;
    case 2:
      for(j[1]=0; j[1]<(*M).dim[1]; j[1]++)
	free((*M).a2[j[1]]);
      free((*M).a2);
      break;
    case 3:
      for(j[2]=0; j[2]<(*M).dim[2]; j[2]++)
	{
	  for(j[1]=0; j[1]<(*M).dim[1]; j[1]++)
	    free((*M).a3[j[2]][j[1]]);
	  free((*M).a3[j[2]]);
	}
      free((*M).a3);
      break;
    case 4:
      for(j[3]=0; j[3]<(*M).dim[3]; j[3]++)
	{
	  for(j[2]=0; j[2]<(*M).dim[2]; j[2]++)
	    {
	      for(j[1]=0; j[1]<(*M).dim[1]; j[1]++)
		free((*M).a4[j[3]][j[2]][j[1]]);
	      free((*M).a4[j[3]][j[2]]);
	    }
	  free((*M).a4[j[3]]);
	}
      free((*M).a4);
      break;
    case 5:
      for(j[4]=0; j[4]<(*M).dim[4]; j[4]++)
	{
	  for(j[3]=0; j[3]<(*M).dim[3]; j[3]++)
	    {
	      for(j[2]=0; j[2]<(*M).dim[2]; j[2]++)
		{
		  for(j[1]=0; j[1]<(*M).dim[1]; j[1]++)
		    free((*M).a5[j[4]][j[3]][j[2]][j[1]]);
		  free((*M).a5[j[4]][j[3]][j[2]]);
		}
	      free((*M).a5[j[4]][j[3]]);
	    }
	  free((*M).a5[j[4]]);
	}
      free((*M).a5);
      break;
    default:
      break;
    }
}


void  lb_al_release_matrix_p2d(MATRIX_POINT_2D_T *S)
{
  U_INT16_T i;
  
  for(i=0; i<(*S).rows; i++)
    free((*S).array[i]);
  free((*S).array);
}

void  lb_al_release_matrix_p3d(MATRIX_POINT_3D_T *S)
{
  U_INT16_T i;
  
  for(i=0; i<(*S).rows; i++)
    free((*S).array[i]);
  free((*S).array);
}

void lb_al_release_matrix_r(MATRIX_R_T *M)
{
  U_INT16_T i;
  
  for(i=0; i<(*M).rows; i++)
    free((*M).array[i]);
  free((*M).array);
}

void lb_al_release_matrix_si8(MATRIX_S_INT8_T *M)
{
  U_INT16_T i;
  
  for(i=0; i<(*M).rows; i++)
    free((*M).array[i]);
  free((*M).array);
}

void lb_al_release_vector_c(VECTOR_C_T *V)
{
  /* oxo: compare with lb_al_release_vector_r() and improve */
  free((*V).array);
}

void lb_al_release_vector_r(VECTOR_R_T *V)
{
  if ((*V).array != NULL)
    free((*V).array);
  (*V).array=NULL;
}

void lb_al_release_vector_si8(VECTOR_S_INT8_T *V)
{
  free((*V).array);
}

void lb_al_release_vector_si16(VECTOR_S_INT16_T *V)
{
  free((*V).array);
}

void lb_al_resize_vector_r(VECTOR_R_T *V, U_INT16_T new_size)
{
  U_INT16_T i;
  
  if (!lb_al_assert_dimensions_vector_r(V))
    {
      printf("Error: lb_al_resize_vector_r() --> invalid dimension V[%d]\r\n",(*V).items);
      exit(EXIT_FAILURE);
    }
  
  if (new_size==0)
    {
      if ((*V).items != 0)
	lb_al_release_vector_r(V);
    }
  else
    {
      if ( (*V).items == 0 )
	{
	  (*V).items=new_size;
	  lb_al_create_vector_r(V);
	}
      else
	if ((*V).items != new_size)
	  {
	    (*V).array=(FLOAT_T *)realloc((*V).array,new_size*sizeof(FLOAT_T));
	    
	    if ( (*V).array==NULL )
	      {
		printf("Error: lb_al_resize_vector_r() --> out of memory\r\n");
		exit(EXIT_FAILURE);
	      }
	    else
	      {
		if (new_size>(*V).items)
		  for (i=(*V).items;i<new_size;i++)
		    (*V).array[i]=0.0;
		(*V).items=new_size;
	      }
	  }
    }
}

void lb_al_resize_vector_si16(VECTOR_S_INT16_T *V, U_INT16_T new_size)
{
  U_INT16_T i;
  
  if (!lb_al_assert_dimensions_vector_si16(V))
    {
      printf("Error: lb_al_resize_vector_si16() --> invalid dimension V[%d]\r\n",(*V).items);
      exit(EXIT_FAILURE);
    }
   
  if ((*V).items != new_size)
    {
      (*V).array=(S_INT16_T *)realloc((*V).array,new_size*sizeof(S_INT16_T));
      
      if ( ((*V).array==NULL) && (new_size!=0) )
	{
	  printf("Error: lb_al_resize_vector_si16() --> out of memory\r\n");
	  exit(EXIT_FAILURE);
	}
      else
      	{
      	  if (new_size>(*V).items)
      	    for (i=(*V).items;i<new_size;i++)
      	      (*V).array[i]=0;
     	  (*V).items=new_size;
	}
    }
}

void lb_al_reverse_order_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2)
{
  U_INT16_T  i;
  
  if (!lb_al_assert_dimensions_vector_r(V1))
    {
      printf("Error: lb_al_reverse_order_vector_r() --> invalid dimension V1=[%d]\r\n",(*V1).items);
      exit(EXIT_FAILURE);
    }
   
  if (!lb_al_assert_dimensions_vector_r(V2))
    {
      printf("Error: lb_al_reverse_order_vector_r() --> invalid dimension V2=[%d]\r\n",(*V2).items);
      exit(EXIT_FAILURE);
    }
  
  if ( (*V1).items != (*V2).items )
    {
      printf("Error: lb_al_reverse_order_vector_r() --> invalid dimension V1[%d] != V2[%d]\r\n",
	     (*V1).items, (*V2).items);
      exit(EXIT_FAILURE);
    }

  for (i=0;i<(*V1).items;i++)
    (*V2).array[(*V1).items-i-1]=(*V1).array[i];
}

void lb_al_solve_linear_system_r(MATRIX_R_T *M, VECTOR_R_T *X, VECTOR_R_T *S)
{
  MATRIX_R_T temp;
  U_INT16_T j, ip, jp, n;
  FLOAT_T det;

  if (!lb_al_assert_dimensions_matrix_r(M))
    {
      printf("Error: lb_al_solve_linear_system_r() --> invalid dimension M[%d,%d]\r\n",
	     (*M).rows, (*M).cols);
      exit(EXIT_FAILURE);
    }
  
  if (!lb_al_assert_dimensions_vector_r(X))
    {
      printf("Error: lb_al_solve_linear_system_r() --> invalid dimension X[%d]\r\n",
	     (*X).items);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_vector_r(S))
    {
      printf("Error: lb_al_solve_linear_system_r() --> invalid dimension S[%d]\r\n",
	     (*S).items);
      exit(EXIT_FAILURE);
    }

  if ( ((*M).rows != (*M).cols) || ((*M).rows != (*X).items) || ((*M).rows != (*S).items) )
    {
      printf("Error: lb_al_solve_linear_system_r() --> invalid dimension M[%d,%d] X[%d] S[%d]\r\n",
	     (*M).rows, (*M).cols, (*X).items, (*S).items);
      exit(EXIT_FAILURE);
    }
  
  n=(*M).rows; /* to make the code clearer */
  temp.rows=n;
  temp.cols=n;
  lb_al_create_matrix_r(&temp);

  det=lb_al_determinant_matrix_r(M);
  
  for (j=0;j<n;j++)
    {
      /* Copy M --> temp but substitute the j column with the solution vector */
      for (ip=0;ip<n;ip++)
	for (jp=0;jp<n;jp++)
	  if (jp!=j)
	    temp.array[ip][jp]=(*M).array[ip][jp];
	  else
	    temp.array[ip][jp]=(*X).array[ip];
      (*S).array[j]=lb_al_determinant_matrix_r(&temp)/det;
    }
  lb_al_release_matrix_r(&temp);
}

void lb_al_sort_bubble_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2)
{
  U_INT16_T i;
  S_INT8_T flag_swaps;
  FLOAT_T temp_f;
  
  if (!lb_al_assert_dimensions_vector_r(V1))
    {
      printf("Error: lb_al_sort_bubble_vector_r() --> invalid dimension V1[%d]\r\n", (*V1).items);
      exit(EXIT_FAILURE);
    }
  
  if (!lb_al_assert_dimensions_vector_r(V2))
    {
      printf("Error: lb_al_sort_bubble_vector_r() --> invalid dimension V2[%d]\r\n", (*V2).items);
      exit(EXIT_FAILURE);
    }
  
  if ( (*V1).items != (*V2).items )
    {
      printf("Error: lb_al_sort_bubble_vector_r() --> invalid dimension V1[%d] != V2[%d]\r\n",
		   (*V1).items, (*V2).items);
      exit(EXIT_FAILURE);
    } 
     
  lb_al_copy_vector_r(V1, V2);
  do {
    flag_swaps=FALSE;
    for (i=0;i<((*V2).items-1);i++)
      if ((*V2).array[i]>(*V2).array[i+1])
	{
	  temp_f=(*V2).array[i];
	  (*V2).array[i]=(*V2).array[i+1];
	  (*V2).array[i+1]=temp_f;
	  flag_swaps=TRUE;
	}
  } while (flag_swaps); 
}

/* Unregistered version of the bubble sorting algorithm.  
   n=number of cells in array */
void lb_al_sort_bubble_vector_r_unreg(FLOAT_T *V, U_INT16_T n)
{
  U_INT16_T i;
  S_INT8_T flag_swaps;
  FLOAT_T temp_f;
  
  do {
    flag_swaps=FALSE;
    for (i=0;i<n-1;i++)
      if (V[i]>V[i+1])
	{
	  temp_f=V[i];
	  V[i]=V[i+1];
	  V[i+1]=temp_f;
	  flag_swaps=TRUE;
	}
  } while (flag_swaps); 
}

void _lb_al_sort_quicksort_vector_r(VECTOR_R_T *V, U_INT16_T a, U_INT16_T b)
{
  U_INT16_T left, right;
  FLOAT_T pivot_f, temp_f;
 
  if (!lb_al_assert_dimensions_vector_r(V))
    {
      printf("Error: _lb_al_sort_quicksort_vector_r() --> invalid dimension V[%d]\r\n", (*V).items);
      exit(EXIT_FAILURE);
    }

  if ((b-a)<1)
    return;

  left=a;
  right=b;
  
  pivot_f=(*V).array[(a+b)>>0x01];
  
  while (left <= right)
    {
      while ((*V).array[left]  < pivot_f)
	left++;
      while ((*V).array[right] > pivot_f)
	right--;
      if (left <= right)
	{
	  temp_f=(*V).array[left];
	  (*V).array[left]=(*V).array[right];
	  (*V).array[right]=temp_f;
	  left++;
	  right--;
	}
    }
  _lb_al_sort_quicksort_vector_r(V,a,right);
  _lb_al_sort_quicksort_vector_r(V,left,b);
}

void lb_al_sort_quicksort_vector_r(VECTOR_R_T *V1, VECTOR_R_T *V2)
{
  if (!lb_al_assert_dimensions_vector_r(V1))
    {
      printf("Error: lb_al_sort_quicksort_vector_r() --> invalid dimension V1[%d]\r\n", (*V1).items);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_vector_r(V2))
    {
      printf("Error: lb_al_sort_quicksort_vector_r() --> invalid dimension V2[%d]\r\n", (*V2).items);
      exit(EXIT_FAILURE);
    }
 
  if ( (*V1).items != (*V2).items )
    {
      printf("Error: lb_al_sort_quicksort_vector_r() --> invalid dimension V1[%d] != V2[%d]\r\n",
	     (*V1).items, (*V2).items);
      exit(EXIT_FAILURE);
    }

  lb_al_copy_vector_r(V1, V2);
  _lb_al_sort_quicksort_vector_r(V2,0,(*V2).items-1);
}

void lb_al_substract_vector_c(VECTOR_C_T *A, VECTOR_C_T *B, VECTOR_C_T *C)
{ 
  U_INT16_T i;

  if (!lb_al_assert_dimensions_vector_c(A))
    {
      printf("Error: lb_al_substract_vector_c() --> invalid dimension A[%d]\r\n", (*A).items);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_vector_c(B))
    {
      printf("Error: lb_al_substract_vector_c() --> invalid dimension B[%d]\r\n", (*B).items);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_vector_c(C))
    {
      printf("Error: lb_al_substract_vector_c() --> invalid dimension C[%d]\r\n", (*C).items);
      exit(EXIT_FAILURE);
    }

  if (((*A).items != (*B).items) || ((*A).items != (*C).items))
    {
      printf("Error: lb_al_substract_vector_c() --> invalid dimension A[%d] B[%d] C[%d]\r\n",
	     (*A).items, (*B).items, (*C).items);
      exit(EXIT_FAILURE);
    }
     
  for (i=0;i<(*A).items;i++)
    (*C).array[i]=lb_cp_substract((*A).array[i],(*B).array[i]);
}

void lb_al_substract_vector_r(VECTOR_R_T *A, VECTOR_R_T *B, VECTOR_R_T *C)
{ 
  U_INT16_T i;

  if (!lb_al_assert_dimensions_vector_r(A))
    {
      printf("Error: lb_al_substract_vector_r() --> invalid dimension A[%d]\r\n", (*A).items);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_vector_r(B))
    {
      printf("Error: lb_al_substract_vector_r() --> invalid dimension B[%d]\r\n", (*B).items);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_vector_r(C))
    {
      printf("Error: lb_al_substract_vector_r() --> invalid dimension C[%d]\r\n", (*C).items);
      exit(EXIT_FAILURE);
    }
  
  if (((*A).items != (*B).items) || ((*A).items != (*C).items))
    {
      printf("Error: lb_al_substract_vector_r() --> invalid dimension A[%d] B[%d] C[%d]\r\n",
	     (*A).items, (*B).items, (*C).items);
      exit(EXIT_FAILURE);
    }

  for (i=0;i<(*A).items;i++)
    (*C).array[i]=(*A).array[i]-(*B).array[i];
}


FLOAT_T lb_al_sum_abs_matrix_r(MATRIX_R_T *M)
{
  FLOAT_T sum;
  U_INT16_T i,j;
 
  if (!lb_al_assert_dimensions_matrix_r(M))
    {
      printf("Error: lb_al_sum_abs_matrix_r() --> invalid dimension M[%d,%d]\r\n",
	     (*M).rows,(*M).cols);
      exit(EXIT_FAILURE);
    }
  
  sum=0;
  for (i=0;i<(*M).rows;i++)
    for (j=0;j<(*M).cols;j++)
      sum+=fabs((*M).array[i][j]);
  return sum;
}

FLOAT_T lb_al_sum_abs_vector_c(VECTOR_C_T *V, ERR_T *error)
{
  FLOAT_T sum;
  U_INT16_T i;
 
  if (!lb_al_assert_dimensions_vector_c(V))
    {
      printf("Error: lb_al_sum_abs_vector_c() --> invalid dimension V[%d]\r\n", (*V).items);
      exit(EXIT_FAILURE);
    }

  sum=0;
  for (i=0;i<(*V).items;i++)
    sum+=sqrtf(lb_re_sqr((*V).array[i].r,error) + lb_re_sqr((*V).array[i].i,error));
  return sum;
}

FLOAT_T lb_al_sum_abs_vector_r(VECTOR_R_T *V)
{
  FLOAT_T sum;
  U_INT16_T i;
 
  if (!lb_al_assert_dimensions_vector_r(V))
    {
      printf("Error: lb_al_sum_abs_vector_r() --> invalid dimension V[%d]\r\n", (*V).items);
      exit(EXIT_FAILURE);
    }

  sum=0;
  for (i=0;i<(*V).items;i++)
    sum+=fabs((*V).array[i]);
  return sum;
}

/* oxo: continue when complex matrices are worked on 

   COMPLEX_T lb_al_sum_matrix_c(MATRIX_C_T *M)
   {
   COMPLEX_T sum;
   U_INT16_T i,j;
 
   if (!lb_al_assert_dimensions_matrix_c(M))
   {
   printf("Error: lb_al_sum_matrix_c() --> invalid dimension M[%d,%d]\r\n", (*M).rows, (*M).cols);
   exit(EXIT_FAILURE);
   }

   sum.r=0;
   sum.i=0;
   for (i=0;i<(*M).rows;i++)
   for (j=0;j<(*M).cols;j++)
   sum=lb_cp_complex_add(sum,(*M).array[i][j]);
   return sum;
   } */

FLOAT_T lb_al_sum_matrix_r(MATRIX_R_T *M)
{
  FLOAT_T sum;
  U_INT16_T i,j;
 
  if (!lb_al_assert_dimensions_matrix_r(M))
    {
      printf("Error: lb_al_sum_matrix_r() --> invalid dimension M[%d,%d]\r\n", (*M).rows, (*M).cols);
      exit(EXIT_FAILURE);
    }
      
  sum=0;
  for (i=0;i<(*M).rows;i++)
    for (j=0;j<(*M).cols;j++)
      sum+=(*M).array[i][j];
  return sum;
}

COMPLEX_T lb_al_sum_vector_c(VECTOR_C_T *V)
{
  COMPLEX_T sum;
  U_INT16_T i;
 
  if (!lb_al_assert_dimensions_vector_c(V))
    {
      printf("Error: lb_al_sum_vector_c() --> invalid dimension V[%d]\r\n", (*V).items);
      exit(EXIT_FAILURE);
    }
  
  sum.r=0;
  sum.i=0;
  for (i=0;i<(*V).items;i++)
    sum=lb_cp_add(sum,(*V).array[i]);
  return sum;
}

FLOAT_T lb_al_sum_vector_r(VECTOR_R_T *V)
{
  FLOAT_T sum;
  U_INT16_T i;
 
  if (!lb_al_assert_dimensions_vector_r(V))
    {
      printf("Error: invalid dimension --> lb_al_sum_vector_r V[%d]\r\n", (*V).items);
      exit(EXIT_FAILURE);
    }
  
  sum=0;
  for (i=0;i<(*V).items;i++)
    sum+=(*V).array[i];
  return sum;
}

void lb_al_swap_vector_r(VECTOR_R_T *A, VECTOR_R_T *B)
{
  VECTOR_R_T temp;
  
  if (!lb_al_assert_dimensions_vector_r(A))
    {
      printf("Error: lb_al_swap_vector_r() --> invalid dimension A[%d]\r\n", (*A).items);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_vector_r(B))
    {
      printf("Error: lb_al_swap_vector_r() --> invalid dimension B[%d]\r\n", (*A).items);
      exit(EXIT_FAILURE);
    }
  temp=(*B);
  (*B)=(*A);
  (*A)=temp;
}


void lb_al_transpose_matrix_r(MATRIX_R_T *M1, MATRIX_R_T *M2)
{
  U_INT16_T i, j;

  /* We assert the size */
  if (!lb_al_assert_dimensions_matrix_r(M1))
    {
      printf("Error: lb_al_transpose_matrix_r() --> invalid dimension M1[%d,%d]\r\n",
	     (*M1).rows, (*M1).cols);
      exit(EXIT_FAILURE);
    }

  if (!lb_al_assert_dimensions_matrix_r(M2))
    {
      printf("Error: lb_al_transpose_matrix_r() --> invalid dimension M2[%d,%d]\r\n",
	     (*M2).rows, (*M2).cols);
      exit(EXIT_FAILURE);
    }
  
  if ( ((*M1).rows != (*M2).cols) || ((*M1).cols != (*M2).rows))
    {
      printf("Error: lb_al_transpose_matrix_r() --> invalid dimension M1[%d,%d] M2[%d,%d]\r\n",
		   (*M1).rows, (*M1).cols, (*M2).rows, (*M2).cols);
      exit(EXIT_FAILURE);
    }

  for (i=0;i<(*M1).rows;i++)
    for (j=0;j<(*M1).cols;j++)
      (*M2).array[j][i]=(*M1).array[i][j];
}


/* Examples:  
   MATRIX_T A;
   A.rows=3;
   A.cols=3;
   matrix_create(&A);

   VECTOR_R_T V;
   V.items=3;
   vector_create(&V);

   for (i=0;i<A.rows;i++)
   for (j=0;j<A.cols;j++)
   A.array[i][j]=10*i+j;

   for (i=0;i<V.items;i++)
   V.array[i]=10*i;
  
   lb_ft_printf(ty_C,"**************************\r\n");
   matrix_print(&A,"A");
   lb_ft_printf(ty_C,"**************************\r\n");

   vector_print(&V,"B");
   lb_ft_printf(ty_C,"**************************\r\n");
*/
