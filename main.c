#include <stdio.h>
 
typedef struct 
{
    float x, y, z;
} p3d;
 
typedef struct
{
    float x, y;
} p2d;
 
typedef struct
{
    float x, y, z;
} v3d;
 
typedef struct
{
    p3d m;
    v3d n, x, y;
} plane;
 
typedef struct 
{
    p3d m;
    v3d s;
} line;
 
v3d p2v(p3d a, p3d b)
{
    v3d ret = {b.x-a.x, b.y-a.y, b.z-a.z};
    return ret;
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
    p3d ret1 = {D*t+d, E*t+e, F*t+f};
    p2d ret2 = {};
    return ret2;
}
    
#include <stdlib.h>
 
main()
{
        srand(10);
        p3d *ps = malloc(10*sizeof(p3d));
        for(int i = 0; i < 10; ++i)
        {       
                p3d rp = {rand()%40, rand()%40, rand()%40};
                *(ps+i) = rp;
		printf("(%f, %f, %f)\n", ps[i].x, ps[i].y, ps[i].z);
        }      
	plane screen = {};
	point3d view = {100, 100, 100};
	p2d *ps2 = malloc(10*sizeof(p2d));
	for(int i = 0; i < 10; ++i)
	{
		p2d cp = project(*(ps+i), view, screen);
		printf("projected dot: (%.2f, %.2f)", cp.x, cp.y);
	}
	
}
