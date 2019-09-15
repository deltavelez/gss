#include "lb_types.h"

#ifndef LB_GEOMETRY_H
#define LB_GEOMETRY_H

FLOAT_T lb_ge_distance_point_to_line_coordinates(FLOAT_T x1, FLOAT_T y1, FLOAT_T x2, FLOAT_T y2, FLOAT_T x, FLOAT_T y, ERR_T *error);
FLOAT_T lb_ge_angle_from_length_ellipse_q1(FLOAT_T a, FLOAT_T b, FLOAT_T length, FLOAT_T R);
FLOAT_T lb_ge_arclength_ellipse(FLOAT_T a, FLOAT_T b);
FLOAT_T lb_ge_arclength_ellipse_angle(FLOAT_T a, FLOAT_T b, FLOAT_T t);
FLOAT_T lb_ge_arclength_ellipse_angle_q1(FLOAT_T a, FLOAT_T b, FLOAT_T t, FLOAT_T R);
FLOAT_T lb_ge_arclength_ellipse_angles(FLOAT_T a, FLOAT_T b, FLOAT_T t1, FLOAT_T t2);
void lb_ge_tangents_to_circle_point(FLOAT_T xc, FLOAT_T yc, FLOAT_T r, FLOAT_T xo, FLOAT_T yo, FLOAT_T *tetha1, FLOAT_T *tetha2);

#endif /* LB_GEOMETRY_H */
