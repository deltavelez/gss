#include "lb_types.h"

#ifndef LB_GEOMETRY_H
#define LB_GEOMETRY_H

REAL_T lb_ge_distance_point_to_line_coordinates(REAL_T x1, REAL_T y1, REAL_T x2, REAL_T y2, REAL_T x, REAL_T y, MATHERROR_T *error);
REAL_T lb_ge_angle_from_length_ellipse_q1(REAL_T a, REAL_T b, REAL_T length, REAL_T R);
REAL_T lb_ge_arclength_ellipse(REAL_T a, REAL_T b);
REAL_T lb_ge_arclength_ellipse_angle(REAL_T a, REAL_T b, REAL_T t);
REAL_T lb_ge_arclength_ellipse_angle_q1(REAL_T a, REAL_T b, REAL_T t, REAL_T R);
REAL_T lb_ge_arclength_ellipse_angles(REAL_T a, REAL_T b, REAL_T t1, REAL_T t2);
void lb_ge_tangents_to_circle_point(REAL_T xc, REAL_T yc, REAL_T r, REAL_T xo, REAL_T yo, REAL_T *tetha1, REAL_T *tetha2);

#endif /* LB_GEOMETRY_H */
