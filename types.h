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
} rotation;

typedef struct {
    float val[3][3];
} mrx3_3;

typedef p3d point3d;
typedef v3d vector3d;
typedef p2d point2d;
typedef rotation rotation_cosines;

