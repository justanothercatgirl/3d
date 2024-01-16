#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "types.h"

v3d mulvm(mrx3_3 *mrx_u, v3d *v)
{   //this code is soooo memory-unsafe
	v3d ret = { 0.0f, 0.0f, 0.0f};
	float *mrx = (float*)mrx_u,
	      *varr = (float*)v, 
	      *retarr = (float*)&ret;
	for(int i = 0; i < 3; ++i)
		for(int j = 0; j < 3; ++j)
			retarr[i] += *(mrx+3*i+j) * varr[j];
	return ret;
}   //handling memory like it's nuclear warhead

p3d mulpm(mrx3_3 *mrx_u, p3d *p)
{
	v3d tmp = mulvm(mrx_u, (v3d*)p);
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
	/* y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed */

	return y;
}

v3d cross_prod(v3d v1, v3d v2)
{   //i don't know why would anyone ever need documentation on this
	v3d ret = {v1.y*v2.z-v1.z*v2.y, v1.x*v2.z-v1.z*v2.x, v1.x*v2.y-v1.y*v2.x};
	return ret;
}

float dot_prod(v3d v1, v3d v2)
{   //same here
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

void normalize(v3d* v)
{   
	float scale = Q_rsqrt(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x *= scale;
	v->y *= scale;
	v->z *= scale;
}

//creates a coordinate system (sets values to plane.x and plane.y)
//since I don't know how to rotate coordinates initially, 
//i've just chosen random numbers and solved equations for them
void create_coordinates(plane *p)
{
	float x1=0, y1=8, z1,   //the random numbers 
	      x2=2, y2=4, z2;   //mentioned above
	p3d p1 = {x1, y1, 0},     //????????????
	    p2 = {x2, y2, 0};     //I have a lot of questions to my sanity
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

// find intersection point between two lines
p3d line_plane_intersec(line* l, plane* p) {
	float	A = p->n.x, B = p->n.y, C = p->n.z,
		a = p->m.x, b = p->m.y, c = p->m.z,
		D = l->s.x, E = l->s.y, F = l->s.y,
		d = l->m.x, e = l->m.y, f = l->m.z;
	float t = (A*(a-d) + B*(b-e) + C*(c-f)) / (A*D + B*E + C*F);
	p3d inter = {d + D*t, e + E*t, f + F*t};
	return inter;
}

//calculate a projection of point orig on plane scr relative to viewer view
//basically construct a vector p2v(view, orig) and calculate where it intersects the plane
p2d project(p3d *orig, p3d *view, plane *scr)
{
	line l = {.m = *view, .s = p2v(*view, *orig)};
	p3d intersection = line_plane_intersec(&l, scr);
	v3d z_coord_tmp = {0, 0, 0};
	v3d new_basis[3] = {scr->x, scr->y, z_coord_tmp};
	p3d projected = mulpm((mrx3_3*)new_basis, &intersection);
	p2d ret = {ret.x, ret.y};
	return ret;
}

//calculate how much the plane needs to be scaled 
//for all points to fit in array [sx][sy] 
float getscale(p2d* ps, int size, int sx, int sy)
{
	p2d min = {10e6, 10e6}, max = {-10e6, -10e6};
	for(int i = 0; i < size; ++i)
	{
		printv((v3d*)(ps+i));

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

	//assuming the scale factor is same for x and y.
	//might have to rework this later
	return sclx > scly ? scly : sclx;
}

//convert plane in memory to screen 2d-array
//for each point in ps* multiply it by scale factor 
//and increment the 2d-array at the received position
void plntscr(char* grid, int gridw, int gridh, plane p, p2d* ps, int pss, float scale)
{	//pass scale = -1 for it to be determned automatically
	if (scale == -1) scale = getscale(ps, pss, gridw, gridh);

	memset(grid, 0, sizeof(char)*gridw*gridh);
	for(int i = 0; i < pss; ++i)
	{
		int x = ps[i].x*scale, y = ps[i].y*scale;
		if (x < 0 || x >= gridw || y < 0 || y >= gridh)
		{
			printf("invalid x or y: %i or %i\n", x, y);
			continue;
		}
		printf("%i, %i, addr: %p\n", x, y, grid+gridw*x+y);
		++*(grid + gridw*x +y);
		printf("point: (%f, %f); point on plane: (%i, %i); value: %hhi\n", 
				ps[i].x, ps[i].y, 
				x, y, 
				*(grid+gridw*x+y));
	}
}

//print plane
void printpln(char* grid, int gridw, int gridh)
{	//it tries to print it based on brightness?..
	for(int i = 0; i < gridw; ++i)
	{
		for(int j = 0; j < gridh; ++j)
		{
			int ind = *(grid+i*gridw+j);
			printf("%c", " .,-+*%&&$#@"[ind > 11 ? 11 : ind]);
		}
		printf("\n");
	}
}


#define N_POINTS 5
int main()
{
	//construct n points
	const float points[N_POINTS][3] = 
	{{0, 0, 0}, {1, 1, 1}, {2, 6, 2}, {3, 3, 3}, {4, 4, 4}};
	p3d *ps = malloc(N_POINTS*sizeof(p3d));
	for(int i = 0; i < N_POINTS; ++i)
	{
		(ps+i)->x = points[i][0];
		(ps+i)->y = points[i][1];
		(ps+i)->z = points[i][2];
	}

	//create screen, coordinates, view point and array for projected points
	plane screen = {.m={50.0f, 50.0f, 50.0f}, .n={0.7071f, 0.7071f, 0.7071f}};
	create_coordinates(&screen);
	p3d view = {100.0f, 100.0f, 100.0f};
	p2d *ps2 = malloc(N_POINTS*sizeof(p2d));

	//project the points
	for(int i = 0; i < N_POINTS; ++i)
		ps2[i] = project((ps+i), &view, &screen);

	//construct the screen mapping, fill it with point data and print out
	char pixels[50][40];
	memset(pixels, 0, 50*40*sizeof(char));
	plntscr((char*)pixels, 50, 40, screen, ps2, N_POINTS, -1);
	printpln((char*)pixels, 50, 40);

	return 0;	
}
