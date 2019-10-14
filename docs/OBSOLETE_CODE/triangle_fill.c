/* Some old code for filling up a triangle.
I don remember what I did or why I have several copies */

#ifdef NADA

void lb_gr_draw_triangle_fill_i(PICTURE_T *Pic, POINT_2D_SINT16_T P0, POINT_2D_SINT16_T P1, POINT_2D_SINT16_T P2, PIXEL_T color, COPYMODE_T copymode)
{
  // http://www-users.mat.uni.torun.pl/~wrona/3d_tutor/tri_fillers.html
  POINT_2D_SINT16_T A, B, C, P_temp;
  SINT16_T i, x_start, x_end, line_y, temp;
  
  A=P0;
  B=P1;
  C=P2;
  
  if (B.y>C.y)
    {
      /* swap B and C */
      P_temp=B;
      B=C;
      C=P_temp;
    }

  if (A.y>B.y)
    {
      /* swap A and B */
      P_temp=A;
      A=B;
      B=P_temp;
    }

  if (B.y>C.y)
    {
      /* swap B and C */
      P_temp=B;
      B=C;
      C=P_temp;
    }

  printf("A = ( %d , %d )\r\n",A.x,A.y); 
  printf("B = ( %d , %d )\r\n",B.x,B.y); 
  printf("C = ( %d , %d )\r\n",C.x,C.y); 
  
  line_y=A.y;
  
  while (line_y<=B.y)
    {
      if (A.y!=B.y)
	x_start = B.x+(line_y-B.y)*(A.x-B.x)/(A.y-B.y);
      else
	{
	  x_start=A.x;
	  x_end=B.x;
	}
      if (A.y!=C.y)
	x_end   = C.x+(line_y-C.y)*(A.x-C.x)/(A.y-C.y);
      else
	{
	  if(A.x<=B.x)
	    x_start=A.x;
	  else
	    x_start=B.x;
	  if(C.x<x_start)
	    x_start=C.x;

	  if(A.x>=B.x)
	    x_end=A.x;
	  else
	    x_end=B.x;
	  if(C.x>x_end)
	    x_end=C.x;
	}
      printf("1) x_start= %d,  x_end=%d\r\n",x_start,x_end); 
      
      if (x_start>x_end)
	{
	  temp=x_end;
	  x_end=x_start;
	  x_start=temp;
	}
      	    
      for(i=x_start;i<=x_end;i++)
	lb_gr_draw_pixel(Pic, i, line_y, color, copymode);
      line_y++;
    }
  while (line_y <= C.y)
    {
      x_start = C.x+(line_y-C.y)*(B.x-C.x)/(B.y-C.y);
      x_end   = C.x+(line_y-C.y)*(A.x-C.x)/(A.y-C.y);
     
      printf("2) x_start= %d,  x_end=%d\r\n",x_start,x_end); 
   
      if (x_start>x_end)
	{
	  temp=x_end;
	  x_end=x_start;
	  x_start=temp;
	}
      for(i=x_start;i<=x_end;i++)
	lb_gr_draw_pixel(Pic, i, line_y, color, copymode);

      line_y++;
    }
}

void lb_gr_draw_triangle_fill_i(PICTURE_T *Pic, POINT_2D_REAL_T P0, POINT_2D_REAL_T P1, POINT_2D_REAL_T P2, PIXEL_T color, COPYMODE_T copymode)
{
  // http://www-users.mat.uni.torun.pl/~wrona/3d_tutor/tri_fillers.html
  POINT_2D_REAL_T A, B, C, P_temp;
  REAL_T i, x_start, x_end, line_y, temp;
  
  A=P0;
  B=P1;
  C=P2;
  
  if (B.y>C.y)
    {
      /* swap B and C */
      P_temp=B;
      B=C;
      C=P_temp;
    }

  if (A.y>B.y)
    {
      /* swap A and B */
      P_temp=A;
      A=B;
      B=P_temp;
    }

  if (B.y>C.y)
    {
      /* swap B and C */
      P_temp=B;
      B=C;
      C=P_temp;
    }

  printf("A = ( %f , %f )\r\n",A.x,A.y); 
  printf("B = ( %f , %f )\r\n",B.x,B.y); 
  printf("C = ( %f , %f )\r\n",C.x,C.y); 
  
  line_y=A.y;
  
  while (line_y<=B.y)
    {
      x_start = B.x+0.5+(line_y-0.5-B.y)*(A.x-B.x)/(A.y-B.y);
      x_end   = C.x+0.5+(line_y-0.5-C.y)*(A.x-C.x)/(A.y-C.y);

      if lb_ft_printf(ty_C, "1) x_start= %f,  x_end=%f\r\n",x_start,x_end); 
      
      //lb_gr_draw_pixel(Pic,x_start, line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_RED),  COPYMODE_BLEND);
      // lb_gr_draw_pixel(Pic,x_end,   line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_BLUE), COPYMODE_BLEND);
      if (x_start>x_end)
	{
	  temp=x_end;
	  x_end=x_start;
	  x_start=temp;
	}
	    
      for(i=round(x_start);i<round(x_end);i++)
	lb_gr_draw_pixel(Pic,round(i), round(line_y),lb_gr_12RGB(COLOR_SOLID | COLOR_WHITE),  COPYMODE_BLEND);
      line_y++;
      // lb_gr_delay(10);
    }
  while (line_y <= C.y)
    {
      x_start = C.x+0.5+(line_y-0.5-C.y)*(B.x-C.x)/(B.y-C.y);
      x_end   = C.x+0.5+(line_y-0.5-C.y)*(A.x-C.x)/(A.y-C.y);
     
      lb_ft_printf(ty_C, "2) x_start= %f,  x_end=%f\r\n",x_start,x_end); 
   
      if (x_start>x_end)
	{
	  temp=x_end;
	  x_end=x_start;
	  x_start=temp;
	}
      for(i=round(x_start);i<round(x_end);i++)
	lb_gr_draw_pixel(Pic, round(i), round(line_y),lb_gr_12RGB(COLOR_SOLID | COLOR_GREEN),  COPYMODE_BLEND);

      //lb_gr_draw_pixel(Pic,x_start, line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_RED),  COPYMODE_BLEND);
      //lb_gr_draw_pixel(Pic,x_end,   line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_BLUE), COPYMODE_BLEND);
      line_y++;
      // lb_gr_delay(10);
    }
  //lb_gr_draw_line1_f(NULL, A.x, A.y, B.x, B.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_BLUE), COPYMODE_BLEND);
  //lb_gr_draw_line1_f(NULL, B.x, B.y, C.x, C.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_BLUE), COPYMODE_BLEND);
  //lb_gr_draw_line1_f(NULL, C.x, C.y, A.x, A.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_BLUE), COPYMODE_BLEND);
  lb_gr_draw_pixel(NULL, A.x, A.y, lb_gr_12RGB(COLOR_YELLOW), copymode);
  lb_gr_draw_pixel(NULL, B.x, B.y, lb_gr_12RGB(COLOR_YELLOW), copymode);
  lb_gr_draw_pixel(NULL, C.x, C.y, lb_gr_12RGB(COLOR_YELLOW), copymode);


}
void lb_gr_draw_triangle_fill_i(PICTURE_T *Pic, POINT_2D_REAL_T P0, POINT_2D_REAL_T P1, POINT_2D_REAL_T P2, PIXEL_T color, COPYMODE_T copymode)
{
  // http://www-users.mat.uni.torun.pl/~wrona/3d_tutor/tri_fillers.html
  POINT_2D_REAL_T A, B, C, P_temp;
  REAL_T i, x_start, x_end, line_y, temp;
  
  A=P0;
  B=P1;
  C=P2;
  
  if (B.y>C.y)
    {
      /* swap B and C */
      P_temp=B;
      B=C;
      C=P_temp;
    }

  if (A.y>B.y)
    {
      /* swap A and B */
      P_temp=A;
      A=B;
      B=P_temp;
    }

  if (B.y>C.y)
    {
      /* swap B and C */
      P_temp=B;
      B=C;
      C=P_temp;
    }

  lb_ft_printf(ty_C, "A = ( %f , %f )\r\n",A.x,A.y); 
  lb_ft_printf(ty_C, "B = ( %f , %f )\r\n",B.x,B.y); 
  lb_ft_printf(ty_C, "C = ( %f , %f )\r\n",C.x,C.y); 
  
  line_y=A.y;
  
  while (line_y<=B.y)
    {
      x_start = B.x+(line_y-B.y)*(A.x-B.x)/(A.y-B.y);
      x_end   = C.x+(line_y-C.y)*(A.x-C.x)/(A.y-C.y);

      
      lb_ft_printf(ty_C, "1) x_start= %f,  x_end=%f\r\n",x_start,x_end); 
      
      //lb_gr_draw_pixel(Pic,x_start, line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_RED),  COPYMODE_BLEND);
      // lb_gr_draw_pixel(Pic,x_end,   line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_BLUE), COPYMODE_BLEND);
      if (x_start>x_end)
	{
	  temp=x_end;
	  x_end=x_start;
	  x_start=temp;
	}
	    
      for(i=round(x_start);i<=round(x_end);i++)
	lb_gr_draw_pixel(Pic,i, round(line_y),lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_WHITE),  COPYMODE_BLEND);
      line_y++;
      //lb_gr_delay(10);
    }
  while (line_y <= C.y)
    {
      x_start = C.x+(line_y-C.y)*(B.x-C.x)/(B.y-C.y);
      x_end   = C.x+(line_y-C.y)*(A.x-C.x)/(A.y-C.y);
     
      lb_ft_printf(ty_C, "2) x_start= %f,  x_end=%f\r\n",x_start,x_end); 
   
      if (x_start>x_end)
	{
	  temp=x_end;
	  x_end=x_start;
	  x_start=temp;
	}
      for(i=round(x_start);i<=round(x_end);i++)
	lb_gr_draw_pixel(Pic,i, round(line_y),lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_GRAY),  COPYMODE_BLEND);

      //lb_gr_draw_pixel(Pic,x_start, line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_RED),  COPYMODE_BLEND);
      //lb_gr_draw_pixel(Pic,x_end,   line_y,lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_BLUE), COPYMODE_BLEND);
      line_y++;
      //lb_gr_delay(10);
    }  
 

  //lb_gr_draw_line1_f(NULL, A.x, A.y, B.x, B.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_YELLOW), COPYMODE_BLEND);
  //lb_gr_draw_line1_f(NULL, B.x, B.y, C.x, C.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_YELLOW), COPYMODE_BLEND);
  //lb_gr_draw_line1_f(NULL, C.x, C.y, A.x, A.y, lb_gr_12RGB(COLOR_SEMI_SOLID | COLOR_YELLOW), COPYMODE_BLEND);
  lb_gr_draw_pixel(NULL, A.x, A.y, lb_gr_12RGB(COLOR_YELLOW), copymode);
  lb_gr_draw_pixel(NULL, B.x, B.y, lb_gr_12RGB(COLOR_YELLOW), copymode);
  lb_gr_draw_pixel(NULL, C.x, C.y, lb_gr_12RGB(COLOR_YELLOW), copymode);
}
#endif
