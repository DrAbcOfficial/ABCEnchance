typedef float vec_t;
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

typedef int fixed4_t;
typedef int fixed8_t;
typedef int fixed16_t;

#define PITCH 0
#define YAW 1
#define ROLL 2

#define SIDE_FRONT 0
#define SIDE_ON 2
#define SIDE_BACK 1
#define SIDE_CROSS -2

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define Q_PI 3.14159265358979323846

extern vec3_t vec3_origin;

#define EQUAL_EPSILON 0.001
#define COLINEAR_EPSILON 0.001


#pragma once
#ifndef MAX_PLAYER_NAME_LENGTH
#define MAX_PLAYER_NAME_LENGTH 128
#endif
#ifndef RADIAN_PER_DEGREE
#define RADIAN_PER_DEGREE 57.29577951308232087684
#endif
#ifndef DEGREE_PER_RADIAN
#define DEGREE_PER_RADIAN 0.017453292519943295769
#endif
#ifndef PERIGON_ANGLE
#define PERIGON_ANGLE 360.0
#endif
#ifndef FLAT_ANGLE
#define FLAT_ANGLE 180.0
#endif

struct mplane_s;

extern int nanmask;

#define IS_NAN(x) (((*(int *)&x)&nanmask) == nanmask)
#define DEG2RAD(a) (a * M_PI) / 180.0F
#define RAD2DEG(r) (r * 180.0F) / M_PI
#define clamp(num, minn, maxn) max(min(num, maxn),minn)
#define METERS_PER_INCH 0.0254f
#define METER2INCH(x) (float)(x * (1.0f / METERS_PER_INCH))
#define INCH2METER(x) (float)(x * (METERS_PER_INCH / 1.0f))

#define RANDOM_LONG(a,b) gEngfuncs.pfnRandomLong(a, b)
#define RANDOM_FLOAT(a,b) gEngfuncs.pfnRandomFloat(a, b)

#define VectorSubtract(a, b, c) { (c)[0] = (a)[0] - (b)[0]; (c)[1] = (a)[1] - (b)[1]; (c)[2] = (a)[2] - (b)[2]; }
#define VectorAdd(a, b, c) { (c)[0] = (a)[0] + (b)[0]; (c)[1] = (a)[1] + (b)[1]; (c)[2] = (a)[2] + (b)[2]; }
#define VectorAddFloat(a,f) {a[0]+=f;a[1]+=f;a[2]+=f;}
#define VectorMultipiler(a,f) {a[0]*=f;a[1]*=f;a[2]*=f;}
#define VectorCopy(a, b) { (b)[0] = (a)[0]; (b)[1] = (a)[1]; (b)[2] = (a)[2]; }
#define VectorReverse(a) {(a[0])=-a[0];(a[1])=-a[1];(a[2])=-a[2]; }
#define Vector2Copy(a, b) { (b)[0] = (a)[0]; (b)[1] = (a)[1]; }
#define VectorClear(a) { (a)[0] = 0.0; (a)[1] = 0.0; (a)[2] = 0.0; }
#define DotProduct(x, y) ((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2])
#define ColorCalcuAlpha(r, g, b, a) {(r)*=(float)(a)/255;(g)*=(float)(a)/255;(b)*=(float)(a)/255;}
#define Vector2Rotate(out, x, y, rotate) {out[0] = x * cos(rotate) - y * sin(rotate);out[1] = x * sin(rotate) + y * cos(rotate);}
#define Vector2Rotate(out, x, y, ca, sa) {out[0] = x * ca - y * sa;out[1] = x * sa + y * ca;}
#define max3(a,b,c) max(a,max(b,c))
#define min3(a,b,c) min(a,min(b,c))

void MatrixCopy(float in[4][3], float out[4][3]);
void VectorMA(const vec3_t veca, float scale, const vec3_t vecb, vec3_t vecc);

vec_t _DotProduct(vec3_t v1, vec3_t v2);
void _VectorSubtract(vec3_t veca, vec3_t vecb, vec3_t out);
void _VectorAdd(vec3_t veca, vec3_t vecb, vec3_t out);
void _VectorCopy(vec3_t in, vec3_t out);

int VectorCompare(const vec3_t v1, const vec3_t v2);
float FVectorLength(vec3_t v);
double VectorLength(vec3_t v);
void CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross);
float VectorNormalize(vec3_t v);
void VectorInverse(vec3_t v);
void VectorScale(const vec3_t in, vec_t scale, vec3_t out);
int Q_log2(int val);
void ClearBounds(vec3_t mins, vec3_t maxs);

void R_ConcatRotations(float in1[3][3], float in2[3][3], float out[3][3]);
void R_ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4]);
void Matrix4x4_CreateFromEntity(float out[4][4], const vec3_t angles, const vec3_t origin, float scale);

void FloorDivMod(double numer, double denom, int *quotient, int *rem);
int GreatestCommonDivisor(int i1, int i2);
void RotatePointAroundVector(vec3_t dst, const vec3_t dir, const vec3_t point, float degrees);

void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
void AngleVectorsTranspose(const vec3_t angles, vec3_t *forward, vec3_t *right, vec3_t *up);

#define AngleIVectors AngleVectorsTranspose

void AngleMatrix(const vec3_t angles, float (*matrix)[4]);
void VectorTransform(const vec3_t in1, float in2[3][4], vec3_t out);

void VectorAngles(const vec3_t forward, vec3_t angles);

int InvertMatrix(const float * m, float *out);

void Matrix4x4_ConcatTransforms(float out[4][4], float in1[4][4], float in2[4][4]);

void VectorRotate(const vec3_t in1, const float in2[3][4], vec3_t out);
void VectorIRotate(const vec3_t in1, const float in2[3][4], vec3_t out);
float anglemod(float a);

float fsqrt(float x);