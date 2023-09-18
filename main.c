#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
	v3d tmp = mulvm(mrx_u, *(v3d*)&p);
	return *(p3d*)&tmp;
}

v3d p2v(p3d a, p3d b)
{
    v3d ret = {b.x-a.x, b.y-a.y, b.z-a.z};
    return ret;
}

void printv(v3d *v)
{
	printf("vector: (%f. %f, %f)\n", v->x, v->y, v->z);
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

float getscale(p2d* ps, int size, int sx, int sy)
{
	printf("%s\n", __PRETTY_FUNCTION__);
	p2d min = {10e6, 10e6}, max = {-10e6, -10e6};
	for(int i = 0; i < size; ++i)
	{
		if (ps[i].x > max.x) 
			max.x = ps[i].x;
		else if(ps[i].x < min.x)
			min.x = ps[i].x;

		if (ps[i].y > max.y) 
			max.y = ps[i].y;
		else if(ps[i].y < min.y)
			min.y = ps[i].y;
	}
	float sclx = sx/(max.x-min.x), scly = sy/(max.y-min.y);
	return sclx > scly ? scly : sclx; //assuming the scale factor is same for x and y.
					  //might have to rework this later
}

void plntscr(char* grid, int gridw, int gridh, plane p, p2d* ps, int pss, float scale)
{	//pass scale == -1 for it to be determned automatically
	if (scale == -1) scale = getscale(ps, pss, gridw, gridh);

	memset(grid, 0, sizeof(int)*gridw*gridh); //uhuhm
	for(int i = 0; i < pss; ++i)
	{
		int x = ps[i].x*scale, y = ps[i].y*scale;
		++*(grid + gridw*x +y);
	}
}

void printpln(char* grid, int gridw, int gridh)
{	//it tries to print it based on brightness?..
	for(int i = 0; i < gridw; ++i)
	{
		for(int j = 0; j < gridh; ++j)
			printf("%c", " .,-+*%&&$#@@@@@@@@@@"[*(grid + i * gridw + j)]);
		printf("\n");
	}
}


int main()
{
	const float points[5][3] = {{0, 0, 0}, {1, 1, 1}, {2, 2, 2}, {3, 3, 3}, {4, 4, 4}};
        p3d *ps = malloc(5*sizeof(p3d));
        for(int i = 0; i < 5; ++i)
        {
		(ps+i)->x = points[i][0];
		(ps+i)->y = points[i][1];
		(ps+i)->z = points[i][2];
	}

	plane screen = {.m={50.0f, 50.0f, 50.0f}, .n={0.7071f, 0.7071f, 0.7071f}};
	create_coordinates(&screen);
	p3d view = {100.0f, 100.0f, 100.0f};
	p2d *ps2 = malloc(5*sizeof(p2d));
	char pixels[50][50];
	memset(pixels, 0, 50*50*sizeof(char));
	plntscr((char*)pixels, 50, 50, screen, ps2, 5, -1);
	printpln((char*)pixels, 50, 50);

	for(int i = 0; i < 10; ++i)
	{
		p2d cp = project(*(ps+i), view, screen);
		printf("projected dot: (%.2f, %.2f)\n", cp.x, cp.y);
	}
	return 0;	
}
