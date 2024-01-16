#ifndef JUSTANOTHERCATGIRL_3D
#define JUSTANOTHERCATGIRL_3D

#include "types.h"

//multiply vector by a matrix
v3d mulvm(mrx3_3* matrix, v3d* vector);

//multiply point by a matrix (basically a bad overload of mulvm)
p3d mulpm(mrx3_3 *matrix, p3d *point);

//construct vector from two points
v3d p2v(p3d start, p3d end);

//pretty self-explanatory
void printv(v3d* vector);

//yes, i absolutely stole this.
//credits to Quake III
float Q_rsqrt( float number );

v3d cross_prod(v3d v1, v3d v2);
float dot_prod(v3d v1, v3d v2);

//normalizes vector in-place
void normalize(v3d* vector);

// TODO: FINISH

#endif // JUSTANOTHERCATGIRL_3D

