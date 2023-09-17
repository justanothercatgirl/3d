#include <stdio.h>

typedef struct {
    float x, y, z;
} p3d;

typedef struct {
    float x, y;
} p2d;

typedef struct {
    float x, y, z;
} v3d;

typedef struct {
    p3d m; //ASSUMING THIS IS STARTING POINT OF COORDINATES
    v3d n;
    v3d x, y;
} plane;

typedef struct {
    p3d m;
	v3d s;
} line;

typedef struct {
	float cosx, cosy, cosz;
} rot_mrx;

typedef struct {
    float val[3][3];
} mrx3_3;

v3d mulvm(mrx3_3 *mrx_u, v3d v)
{   //this code is soooo memory-unsafe
	v3d ret = { 0.0f, 0.0f, 0.0f};
	float *mrx = (float*)mrx_u,
		  *varr = (float*)&v, 
		  *retarr = (float*)&ret;
	for(int i = 0; i < 3; ++i)
		for(int j = 0; j < 3; ++j)
			retarr[i] += *(mrx+3*i+j) * varr[j];
	return ret;
}   //handling memory like it's nuclear warhead



p3d mulpm(mrx3_3 *mrx_u, p3d p)
{
	v3d tmp = mulvm(mrx_u, *(p3d*)&p);
	return *(p3d*)&tmp;
}

v3d p2v(p3d a, p3d b)
{
    v3d ret = {b.x-a.x, b.y-a.y, b.z-a.z};
    return ret;
}

void printv(v3d v)
{
	printf("vector: (%f. %f, %f)\n", v.x, v.y, v.z);
}

//yes, i absolutely stole this.
//credits to Quake III
float Q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

 	return y;
}

float scalar_prod(v3d v1, v3d v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

v3d cross_prod(v3d v1, v3d v2)
{
	v3d ret = {v1.y*v2.z-v1.z*v2.y, v1.x*v2.z-v1.z*v2.x, v1.x*v2.y-v1.y*v2.x};
	return ret;
}

void normalize(v3d* v)
{
	float scale = Q_rsqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x *= scale;
	v->y *= scale;
	v->z *= scale;
}

void create_coordinates(plane *p)
{
	float x1=0, y1=8, z1,
	      x2=2, y2=4, z2;
	p3d p1 = {0, 8, 0},
	    p2 = {2, 4, 0};
	float A=p->n.x, B=p->n.y, C=p->n.z,
	      a=p->m.x, b=p->m.y, c=p->m.z;
	p1.z = (A*(a-p1.x)+B*(b-p1.y))/C + c;
	p2.z = (A*(a-p2.x)+B*(b-p2.y))/C + c;
	
	v3d base = p2v(p1, p2);
	p->x = base;
	p->y = cross_prod(base, p->n);
	normalize(&(p->x));
	normalize(&(p->y));
}


p2d project(p3d orig, p3d view, plane scr)
{
    line l = {view,p2v(view, orig)};
    float
        A=scr.n.x, B=scr.n.y, C=scr.n.z,
        a=scr.m.x, b=scr.m.y, c=scr.m.z,
        D=l.s.x, E=l.s.y, F=l.s.z,
        d=l.m.x, e=l.m.y, f=l.m.z;
    float t = (A*(a-d)+B*(b-e)+C*(c-f))/(A*D+B*E+C*F);

    v3d ret1 = {a-D*t+d, b-E*t+e, c-F*t+f};

    p2d ret2 = {scalar_prod(ret1, scr.x), scalar_prod(ret1, scr.y)};
    return ret2;
}
    
#include <stdlib.h>
 
int main()
{
        srand(10);
        p3d *ps = malloc(10*sizeof(p3d));
        for(int i = 0; i < 10; ++i)
        {       
                p3d rp = {rand()%40, rand()%40, rand()%40};
                *(ps+i) = rp;
		printf("(%f, %f, %f)\n", ps[i].x, ps[i].y, ps[i].z);
        }

	p3d tmpp = {0.0f,  0.0f, 0.0f};
	*ps = tmpp;
	p3d tmpp1 = {10.0f, 10.0f, 10.0f};
	*(ps+1) = tmpp1;

	plane screen = {.m={50.0f, 50.0f, 50.0f}, .n={0.7071f, 0.7071f, 0.7071f}};
	create_coordinates(&screen);
	p3d view = {100.0f, 100.0f, 100.0f};
	p2d *ps2 = malloc(10*sizeof(p2d));
	for(int i = 0; i < 10; ++i)
	{
		p2d cp = project(*(ps+i), view, screen);
		printf("projected dot: (%.2f, %.2f)\n", cp.x, cp.y);
	}
	return 0;	
}
