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

#define RANDOM_LONG(a, b) gEngfuncs.pfnRandomLong(a, b)
#define RANDOM_FLOAT(a, b) gEngfuncs.pfnRandomFloat(a, b)

struct mplane_s;
#define AngleIVectors AngleVectorsTranspose
namespace mathlib {
	const double M_PI = 3.14159265358979323846;
	const double Q_PI = 3.14159265358979323846;
	const float EQUAL_EPSILON = 0.001;
	const float COLINEAR_EPSILON = 0.001;
	const int MAX_PLAYER_NAME_LENGTH = 128;
	const float RADIAN_PER_DEGREE = 57.29577951308232087684;
	const float DEGREE_PER_RADIAN = 0.017453292519943295769;
	const float PERIGON_ANGLE = 360.0;
	const float FLAT_ANGLE = 180.0;
	const vec3_t vec3_origin = { 0, 0, 0 };
	const float METERS_PER_INCH = 0.0254f;
	constexpr int nanmask = 255 << 23;

	bool IS_NAN(float x);
	float DEG2RAD(float a);
	float RAD2DEG(float r);
	float clamp(float num, float minn, float maxn);
	int clamp(int num, int minn, int maxn);
	float METER2INCH(float x);
	float INCH2METER(float x);
	void VectorSubtract(vec3_t a, vec3_t b, vec3_t c);
	void VectorAdd(vec3_t a, vec3_t b, vec3_t c);
	void VectorAddFloat(vec3_t a, float f);
	void VectorMultipiler(vec3_t a, float f);
	void VectorCopy(vec3_t a, vec3_t b);
	void VectorReverse(vec3_t a);
	void Vector2Copy(vec2_t a, vec2_t b);
	void VectorClear(vec3_t a);
	float DotProduct(vec3_t x, vec3_t y);
	float DotProduct(const vec3_t x, const vec3_t y);
	void ColorCalcuAlpha(int& r, int& g, int& b, int a);
	void Vector2RotateCASA(vec2_t out, float x, float y, float ca, float sa);
	float max3(float a, float b, float c);
	int max3(int a, int b, int c);
	float min3(float a, float b, float c);
	int min3(int a, int b, int c);

	void Vector2Rotate(vec2_t out, float x, float y, float rotate);

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

	void FloorDivMod(double numer, double denom, int* quotient, int* rem);
	int GreatestCommonDivisor(int i1, int i2);
	void RotatePointAroundVector(vec3_t dst, const vec3_t dir, const vec3_t point, float degrees);

	void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
	void AngleVectorsTranspose(const vec3_t angles, vec3_t* forward, vec3_t* right, vec3_t* up);
	void AngleMatrix(const vec3_t angles, float(*matrix)[4]);
	void VectorTransform(const vec3_t in1, float in2[3][4], vec3_t out);

	void VectorAngles(const vec3_t forward, vec3_t angles);

	int InvertMatrix(const float* m, float* out);

	void Matrix4x4_ConcatTransforms(float out[4][4], float in1[4][4], float in2[4][4]);

	void VectorRotate(const vec3_t in1, const float in2[3][4], vec3_t out);
	void VectorIRotate(const vec3_t in1, const float in2[3][4], vec3_t out);
	float anglemod(float a);

	float fsqrt(float x);
};