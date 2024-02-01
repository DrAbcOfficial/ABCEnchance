#include <cmath>
#include <metahook.h>
#include <mymathlib.h>

bool mathlib::Q_IS_NAN(float x){
	return ((*(int*)&x) & nanmask) == nanmask;
}
float mathlib::Q_DEG2RAD(float a) {
	return (a * F_PI) / 180.0f;
}
float mathlib::Q_RAD2DEG(float r) {
	return (r * 180.0f) / F_PI;
}
template<typename T>
T mathlib::clamp(T num, T minn, T maxn) {
	return max(min(num, maxn), minn);
}
template int mathlib::clamp(int, int, int);
template float mathlib::clamp(float, float, float);
template size_t mathlib::clamp(size_t, size_t, size_t);
float mathlib::METER2INCH(float x) {
	return (float)(x * (1.0f / mathlib::METERS_PER_INCH));
}
float mathlib::INCH2METER(float x) {
	return (float)(x * (mathlib::METERS_PER_INCH / 1.0f));
}
void mathlib::VectorSubtract(vec3_t a, vec3_t b, vec3_t c) {
	(c)[0] = (a)[0] - (b)[0]; (c)[1] = (a)[1] - (b)[1]; (c)[2] = (a)[2] - (b)[2];
}
void mathlib::VectorAdd(vec3_t a, vec3_t b, vec3_t c) {
	(c)[0] = (a)[0] + (b)[0]; (c)[1] = (a)[1] + (b)[1]; (c)[2] = (a)[2] + (b)[2];
}
void mathlib::VectorAddFloat(vec3_t a, float f) {
	a[0] += f; a[1] += f; a[2] += f;
}
void mathlib::VectorMultipiler(vec3_t a, float f) {
	a[0] *= f; a[1] *= f; a[2] *= f;
}
void mathlib::VectorCopy(vec3_t a, vec3_t b) {
	(b)[0] = (a)[0]; (b)[1] = (a)[1]; (b)[2] = (a)[2];
}
void mathlib::VectorReverse(vec3_t a) {
	(a[0]) = -a[0]; (a[1]) = -a[1]; (a[2]) = -a[2];
}
void mathlib::Q_Vector2Copy(vec2_t a, vec2_t b) {
	(b)[0] = (a)[0]; (b)[1] = (a)[1];
}
void mathlib::VectorClear(vec3_t a) {
	(a)[0] = (a)[1] = (a)[2] = 0.0;
}
float mathlib::DotProduct(vec3_t x, vec3_t y) {
	return ((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2]);
}
float mathlib::DotProduct(const vec3_t x, const vec3_t y) {
	return ((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2]);
}
void  mathlib::ColorCalcuAlpha(int& r, int& g, int& b, int a) {
	r *= static_cast<int>((float)(a) / 255); 
	g *= static_cast<int>((float)(a) / 255); 
	b *= static_cast<int>((float)(a) / 255);
}
void mathlib::Vector2RotateCASA(vec2_t out, float x, float y, float ca, float sa) {
	out[0] = x * ca - y * sa; out[1] = x * sa + y * ca;
}
template<typename T>
T mathlib::max3(T a, T b, T c) {
	return max(a, max(b, c));
}
template<typename T>
T mathlib::min3(T a, T b, T c) {
	return min(a, min(b, c));
}
void mathlib::CenterPos2OpenGLPos(vec2_t pos, int w, int h){
	pos[0] += w / 2; 
	pos[1] = h / 2 - pos[1];
}
size_t mathlib::GetScreenPixel(int length, double percent) {
	return (size_t)((float)length * clamp<float>(static_cast<float>(percent), 0.0f, 1.0f));
}
void mathlib::Vector2Rotate(vec2_t out, float x, float y, float rotate) {
	out[0] = x * cos(rotate) - y * sin(rotate); out[1] = x * sin(rotate) + y * cos(rotate);
}
void mathlib::ProjectPointOnPlane(vec3_t dst, const vec3_t p, const vec3_t normal){
	float d;
	vec3_t n;
	float inv_denom;

	inv_denom = 1.0F / mathlib::DotProduct(normal, normal);

	d = mathlib::DotProduct(normal, p) * inv_denom;

	n[0] = normal[0] * inv_denom;
	n[1] = normal[1] * inv_denom;
	n[2] = normal[2] * inv_denom;

	dst[0] = p[0] - d * n[0];
	dst[1] = p[1] - d * n[1];
	dst[2] = p[2] - d * n[2];
}
void mathlib::PerpendicularVector(vec3_t dst, const vec3_t src){
	int pos;
	int i;
	float minelem = 1.0F;
	vec3_t tempvec;

	for (pos = 0, i = 0; i < 3; i++){
		if (fabs(src[i]) < minelem){
			pos = i;
			minelem = fabs(src[i]);
		}
	}

	tempvec[0] = tempvec[1] = tempvec[2] = 0.0F;
	tempvec[pos] = 1.0F;

	ProjectPointOnPlane(dst, tempvec, src);

	mathlib::VectorNormalize(dst);
}
bool mathlib::PointInPolygen(vec2_t p1, vec2_t p2, vec2_t p3, vec2_t p4, int x, int y) {
	float z1 = (p2[0] - p1[0]) * (y - p1[1]) - (x - p1[0]) * (p2[1] - p1[1]);
	float z2 = (p3[0] - p2[0]) * (y - p2[1]) - (x - p2[0]) * (p3[1] - p2[1]);
	float z3 = (p4[0] - p3[0]) * (y - p3[1]) - (x - p3[0]) * (p4[1] - p3[1]);
	float z4 = (p1[0] - p4[0]) * (y - p4[1]) - (x - p4[0]) * (p1[1] - p4[1]);
	return z1 * z2 > 0 && z3 * z4 > 0 && z1 * z3 > 0;
}

#ifdef _WIN32
#pragma optimize("", off)
#endif

void mathlib::RotatePointAroundVector(vec3_t dst, const vec3_t dir, const vec3_t point, float degrees){
	float m[3][3];
	float im[3][3];
	float zrot[3][3];
	float tmpmat[3][3];
	float rot[3][3];
	int i;
	vec3_t vr, vup, vf;

	vf[0] = dir[0];
	vf[1] = dir[1];
	vf[2] = dir[2];

	PerpendicularVector(vr, dir);
	mathlib::CrossProduct(vr, vf, vup);

	m[0][0] = vr[0];
	m[1][0] = vr[1];
	m[2][0] = vr[2];

	m[0][1] = vup[0];
	m[1][1] = vup[1];
	m[2][1] = vup[2];

	m[0][2] = vf[0];
	m[1][2] = vf[1];
	m[2][2] = vf[2];

	memcpy(im, m, sizeof(im));

	im[0][1] = m[1][0];
	im[0][2] = m[2][0];
	im[1][0] = m[0][1];
	im[1][2] = m[2][1];
	im[2][0] = m[0][2];
	im[2][1] = m[1][2];

	memset(zrot, 0, sizeof(zrot));
	zrot[0][0] = zrot[1][1] = zrot[2][2] = 1.0F;

	zrot[0][0] = cos(mathlib::Q_DEG2RAD(degrees));
	zrot[0][1] = sin(mathlib::Q_DEG2RAD(degrees));
	zrot[1][0] = -sin(mathlib::Q_DEG2RAD(degrees));
	zrot[1][1] = cos(mathlib::Q_DEG2RAD(degrees));

	mathlib::R_ConcatRotations(m, zrot, tmpmat);
	mathlib::R_ConcatRotations(tmpmat, im, rot);

	for (i = 0; i < 3; i++)
		dst[i] = rot[i][0] * point[0] + rot[i][1] * point[1] + rot[i][2] * point[2];
}

#ifdef _WIN32
#pragma optimize("", on)
#endif

float mathlib::anglemod(float a){
	a = (360.0f / 65536) * ((int)(a * (65536 / 360.0f)) & 65535);
	return a;
}
void mathlib::AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up){
	float angle;
	vec2_t cs[3];
	for (int i = 0; i < 3; i++){
		angle = angles[i] * (F_PI / 180.0f);
		cs[i][0] = sin(angle);
		cs[i][1] = cos(angle);
	}
	if (forward){
		forward[0] = cs[0][1] * cs[1][1];
		forward[1] = cs[0][1] * cs[1][0];
		forward[2] = -cs[0][0];
	}
	if (right){
		right[0] = (-1 * cs[2][0] * cs[0][0] * cs[1][1] + -1 * cs[2][1] * -cs[1][0]);
		right[1] = (-1 * cs[2][0] * cs[0][0] * cs[1][0] + -1 * cs[2][1] * cs[1][1]);
		right[2] = -1 * cs[2][0] * cs[0][1];
	}
	if (up){
		up[0] = (cs[2][1] * cs[0][0] * cs[1][1] + -cs[2][0] * -cs[1][0]);
		up[1] = (cs[2][1] * cs[0][0] * cs[1][0] + -cs[2][0] * cs[1][1]);
		up[2] = cs[2][1] * cs[0][1];
	}
}
void mathlib::AngleVectorsTranspose(const vec3_t angles, vec3_t *forward, vec3_t *right, vec3_t *up){
	float angle;
	vec2_t cs[3];
	for (int i = 0; i < 3; i++){
		angle = angles[i] * (F_PI / 180.0f);
		cs[i][0] = sin(angle);
		cs[i][1] = cos(angle);
	}
	if (forward){
		(*forward)[0] = cs[0][1] * cs[1][1];
		(*forward)[1] = cs[0][1] * cs[1][0];
		(*forward)[2] = -cs[0][0];
	}
	if (right){
		(*right)[0] = (-1 * cs[2][0] * cs[0][0] * cs[1][1] + -1 * cs[2][1] * -cs[1][0]);
		(*right)[1] = (-1 * cs[2][0] * cs[0][0] * cs[1][0] + -1 * cs[2][1] * cs[1][1]);
		(*right)[2] = -1 * cs[2][0] * cs[0][1];
	}
	if (up){
		(*up)[0] = (cs[2][1] * cs[0][0] * cs[1][1] + -cs[2][0] * -cs[1][0]);
		(*up)[1] = (cs[2][1] * cs[0][0] * cs[1][0] + -cs[2][0] * cs[1][1]);
		(*up)[2] = cs[2][1] * cs[0][1];
	}
}
void mathlib::VectorAngles(const vec3_t forward, vec3_t angles){
	float yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0){
		yaw = 0;

		if (forward[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else{
		yaw = static_cast<float>((atan2((double)forward[1], (double)forward[0]) * 180 / Q_PI));

		if (yaw < 0)
			yaw += 360;

		float tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = static_cast<float>(atan2((double)forward[2], (double)tmp) * 180 / Q_PI);

		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}
float mathlib::FVectorLength(vec3_t v) {
	float length = 0;
	for (int i = 0; i < 3; i++)
		length += static_cast<float>(pow(v[i], 2));
	return sqrt(length);
}
double mathlib::VectorLength(vec3_t v){
	double length = 0;
	for (int i = 0; i < 3; i++)
		length += pow(v[i], 2);
	return sqrt(length);
}

#ifndef FLT_EPSILON
#define FLT_EPSILON (1.192093e-007)
#endif

int mathlib::VectorCompare(const vec3_t v1, const vec3_t v2){
	for (int i = 0; i < 3; i++){
		if (abs(v1[i] - v2[i]) > FLT_EPSILON)
			return 0;
	}
	return 1;
}
vec_t mathlib::Q_rint(vec_t in){
	return static_cast<float>(floor(in + 0.5f));
}
void mathlib::VectorMA(const vec3_t va, float scale, const vec3_t vb, vec3_t vc){
	vc[0] = va[0] + scale * vb[0];
	vc[1] = va[1] + scale * vb[1];
	vc[2] = va[2] + scale * vb[2];
}
void mathlib::CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross){
	cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
	cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
	cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
}
vec_t mathlib::_DotProduct(vec3_t v1, vec3_t v2){
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
void mathlib::_VectorSubtract(vec3_t va, vec3_t vb, vec3_t out){
	out[0] = va[0] - vb[0];
	out[1] = va[1] - vb[1];
	out[2] = va[2] - vb[2];
}
void mathlib::_VectorAdd(vec3_t va, vec3_t vb, vec3_t out){
	out[0] = va[0] + vb[0];
	out[1] = va[1] + vb[1];
	out[2] = va[2] + vb[2];
}
void mathlib::_VectorCopy(vec3_t in, vec3_t out){
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}
void mathlib::_VectorScale(vec3_t v, vec_t scale, vec3_t out){
	out[0] = v[0] * scale;
	out[1] = v[1] * scale;
	out[2] = v[2] * scale;
}
vec_t mathlib::Length(const vec3_t v){
	int i;
	float length;

	length = 0;

	for (i = 0; i< 3; i++)
		length += v[i] * v[i];

	length = sqrt(length);
	return length;
}
vec_t mathlib::VectorNormalize(vec3_t v){
	int i = 0;
	double length;

	if (fabs(v[1] - 0.000215956) < 0.0001)
		i = 1;
	length = 0;
	for (i; i < 3; i++)
		length += pow(v[i], 2);

	length = sqrt(length);

	if (length == 0)
		return 0;

	for (i = 0; i < 3; i++)
		v[i] /= static_cast<float>(length);

	return static_cast<float>(length);
}
void mathlib::VectorInverse(vec3_t v){
	v[0] = -v[0];
	v[1] = -v[1];
	v[2] = -v[2];
}
void mathlib::VectorScale(const vec3_t in, vec_t scale, vec3_t out){
	out[0] = in[0] * scale;
	out[1] = in[1] * scale;
	out[2] = in[2] * scale;
}
int mathlib::Q_log2(int val){
	int answer = 0;

	while (val >>= 1)
		answer++;

	return answer;
}
void mathlib::ClearBounds(vec3_t mins, vec3_t maxs){
	mins[0] = mins[1] = mins[2] = 99999;
	maxs[0] = maxs[1] = maxs[2] = -99999;
}
void mathlib::MatrixCopy(float in[4][3], float out[4][3]){
	memcpy(out, in, sizeof(float) * 3 * 4);
}
void mathlib::AddPointToBounds(vec3_t v, vec3_t mins, vec3_t maxs){
	vec_t val;
	for (int i = 0; i < 3; i++){
		val = v[i];

		if (val < mins[i])
			mins[i] = val;

		if (val > maxs[i])
			maxs[i] = val;
	}
}
void mathlib::AngleMatrix(const vec3_t angles, float (*matrix)[4]){
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[2] * (F_PI * 2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[1] * (F_PI * 2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[0] * (F_PI * 2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	matrix[0][0] = cp * cy;
	matrix[1][0] = cp * sy;
	matrix[2][0] = -sp;
	matrix[0][1] = sr * sp * cy + cr * -sy;
	matrix[1][1] = sr * sp * sy + cr * cy;
	matrix[2][1] = sr * cp;
	matrix[0][2] = (cr * sp * cy + -sr * -sy);
	matrix[1][2] = (cr * sp * sy + -sr * cy);
	matrix[2][2] = cr * cp;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
}
void mathlib::AngleIMatrix(const vec3_t angles, float matrix[3][4]){
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[2] * (F_PI * 2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[1] * (F_PI * 2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[0] * (F_PI * 2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	matrix[0][0] = cp * cy;
	matrix[0][1] = cp * sy;
	matrix[0][2] = -sp;
	matrix[1][0] = sr * sp * cy + cr * -sy;
	matrix[1][1] = sr * sp * sy + cr * cy;
	matrix[1][2] = sr * cp;
	matrix[2][0] = (cr * sp * cy + -sr * -sy);
	matrix[2][1] = (cr * sp * sy + -sr * cy);
	matrix[2][2] = cr * cp;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
}
void mathlib::R_ConcatRotations(float in1[3][3], float in2[3][3], float out[3][3]){
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
}
void mathlib::R_ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4]){
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
}
void mathlib::VectorRotate(const vec3_t in1, const float in2[3][4], vec3_t out){
	out[0] = mathlib::DotProduct(in1, in2[0]);
	out[1] = mathlib::DotProduct(in1, in2[1]);
	out[2] = mathlib::DotProduct(in1, in2[2]);
}
void mathlib::VectorIRotate(const vec3_t in1, const float in2[3][4], vec3_t out){
	out[0] = in1[0] * in2[0][0] + in1[1] * in2[1][0] + in1[2] * in2[2][0];
	out[1] = in1[0] * in2[0][1] + in1[1] * in2[1][1] + in1[2] * in2[2][1];
	out[2] = in1[0] * in2[0][2] + in1[1] * in2[1][2] + in1[2] * in2[2][2];
}
void mathlib::VectorTransform(const vec3_t in1, float in2[3][4], vec3_t out){
	out[0] = DotProduct(in1, in2[0]) + in2[0][3];
	out[1] = DotProduct(in1, in2[1]) + in2[1][3];
	out[2] = DotProduct(in1, in2[2]) + in2[2][3];
}
void mathlib::AngleQuaternion(const vec3_t angles, vec4_t quaternion){
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[2] * 0.5f;
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[1] * 0.5f;
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[0] * 0.5f;
	sr = sin(angle);
	cr = cos(angle);

	quaternion[0] = sr * cp * cy - cr * sp * sy;
	quaternion[1] = cr * sp * cy + sr * cp * sy;
	quaternion[2] = cr * cp * sy - sr * sp * cy;
	quaternion[3] = cr * cp * cy + sr * sp * sy;
}
void mathlib::QuaternionMatrix(const vec4_t quaternion, float (*matrix)[4]){
	matrix[0][0] = 1.0f - 2.0f * quaternion[1] * quaternion[1] - 2.0f * quaternion[2] * quaternion[2];
	matrix[1][0] = 2.0f * quaternion[0] * quaternion[1] + 2.0f * quaternion[3] * quaternion[2];
	matrix[2][0] = 2.0f * quaternion[0] * quaternion[2] - 2.0f * quaternion[3] * quaternion[1];

	matrix[0][1] = 2.0f * quaternion[0] * quaternion[1] - 2.0f * quaternion[3] * quaternion[2];
	matrix[1][1] = 1.0f - 2.0f * quaternion[0] * quaternion[0] - 2.0f * quaternion[2] * quaternion[2];
	matrix[2][1] = 2.0f * quaternion[1] * quaternion[2] + 2.0f * quaternion[3] * quaternion[0];

	matrix[0][2] = 2.0f * quaternion[0] * quaternion[2] + 2.0f * quaternion[3] * quaternion[1];
	matrix[1][2] = 2.0f * quaternion[1] * quaternion[2] - 2.0f * quaternion[3] * quaternion[0];
	matrix[2][2] = 1.0f - 2.0f * quaternion[0] * quaternion[0] - 2.0f * quaternion[1] * quaternion[1];
}
void mathlib::QuaternionSlerp(const vec4_t p, vec4_t q, float t, vec4_t qt){
	int i;
	float sclp, sclq;

	float a = 0;
	float b = 0;

	for (i = 0; i < 4; i++){
		a += (p[i] - q[i]) * (p[i] - q[i]);
		b += (p[i] + q[i]) * (p[i] + q[i]);
	}

	if (a > b){
		for (i = 0; i < 4; i++)
			q[i] = -q[i];
	}

	float cosom = p[0] * q[0] + p[1] * q[1] + p[2] * q[2] + p[3] * q[3];

	if ((1.0 + cosom) > 0.00000001){
		if ((1.0 - cosom) > 0.00000001){
			float omega = acos(cosom);
			float sinom = sin(omega);
			sclp = sin((1.0f - t)*omega) / sinom;
			sclq = sin(t * omega) / sinom;
		}
		else{
			sclp = 1.0f - t;
			sclq = t;
		}

		for (i = 0; i < 4; i++)
			qt[i] = sclp * p[i] + sclq * q[i];
	}
	else{
		qt[0] = -p[1];
		qt[1] = p[0];
		qt[2] = -p[3];
		qt[3] = p[2];
		sclp = sin((1.0f - t) * 0.5f * F_PI);
		sclq = sin(t * 0.5f * F_PI);

		for (i = 0; i < 3; i++)
			qt[i] = sclp * p[i] + sclq * qt[i];
	}
}
void mathlib::FloorDivMod(double numer, double denom, int *quotient, int *rem){
	int q, r;
	double x;

	if (numer >= 0.0){
		x = floor(numer / denom);
		q = (int)x;
		r = (int)floor(numer - (x * denom));
	}
	else{
		x = floor(-numer / denom);
		q = -(int)x;
		r = (int)floor(-numer - (x * denom));

		if (r != 0){
			q--;
			r = (int)denom - r;
		}
	}

	*quotient = q;
	*rem = r;
}
int mathlib::GreatestCommonDivisor(int i1, int i2){
	if (i1 > i2){
		if (i2 == 0)
			return (i1);

		return GreatestCommonDivisor(i2, i1 % i2);
	}
	else{
		if (i1 == 0)
			return (i2);

		return GreatestCommonDivisor(i1, i2 % i1);
	}
}
int mathlib::InvertMatrix(const float *m, float *out){
#define SWAP_ROWS(a, b) { float *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]

	float wtmp[4][8];
	float m0, m1, m2, m3, s;
	float *r0, *r1, *r2, *r3;

	r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

	r0[0] = MAT(m, 0, 0), r0[1] = MAT(m, 0, 1),
	r0[2] = MAT(m, 0, 2), r0[3] = MAT(m, 0, 3),
	r0[4] = 1.0f, r0[5] = r0[6] = r0[7] = 0.0f,

	r1[0] = MAT(m, 1, 0), r1[1] = MAT(m, 1, 1),
	r1[2] = MAT(m, 1, 2), r1[3] = MAT(m, 1, 3),
	r1[5] = 1.0f, r1[4] = r1[6] = r1[7] = 0.0f,

	r2[0] = MAT(m, 2, 0), r2[1] = MAT(m, 2, 1),
	r2[2] = MAT(m, 2, 2), r2[3] = MAT(m, 2, 3),
	r2[6] = 1.0f, r2[4] = r2[5] = r2[7] = 0.0f,

	r3[0] = MAT(m, 3, 0), r3[1] = MAT(m, 3, 1),
	r3[2] = MAT(m, 3, 2), r3[3] = MAT(m, 3, 3),
	r3[7] = 1.0f, r3[4] = r3[5] = r3[6] = 0.0f;

	if (fabs(r3[0]) > fabs(r2[0])) SWAP_ROWS(r3, r2);
	if (fabs(r2[0]) > fabs(r1[0])) SWAP_ROWS(r2, r1);
	if (fabs(r1[0]) > fabs(r0[0])) SWAP_ROWS(r1, r0);
	if (0.0f == r0[0]) return false;

	m1 = r1[0] / r0[0]; m2 = r2[0] / r0[0]; m3 = r3[0] / r0[0];
	s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
	s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
	s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
	s = r0[4];

	if (s != 0.0f){ r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r0[5];
	if (s != 0.0f) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r0[6];
	if (s != 0.0f) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r0[7];
	if (s != 0.0f) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

	if (fabs(r3[1])>fabs(r2[1])) SWAP_ROWS(r3, r2);
	if (fabs(r2[1])>fabs(r1[1])) SWAP_ROWS(r2, r1);
	if (0.0f == r1[1]) return false;

	m2 = r2[1] / r1[1]; m3 = r3[1] / r1[1];
	r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
	s = r1[4]; if (0.0f != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r1[5]; if (0.0f != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r1[6]; if (0.0f != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r1[7]; if (0.0f != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

	if (fabs(r3[2]) > fabs(r2[2])) SWAP_ROWS(r3, r2);
	if (0.0f == r2[2]) return false;

	m3 = r3[2] / r2[2];
	r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
	r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
	r3[7] -= m3 * r2[7];

	if (0.0f == r3[3]) return false;

	s = 1.0f / r3[3];
	r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

	m2 = r2[3];
	s = 1.0f / r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
	r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
	m1 = r1[3];
	r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
	r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
	m0 = r0[3];
	r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
	r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

	m1 = r1[2];
	s = 1.0f / r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
	r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
	m0 = r0[2];
	r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
	r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

	m0 = r0[1];
	s = 1.0f / r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
	r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

	MAT(out, 0, 0) = r0[4];
	MAT(out, 0, 1) = r0[5], MAT(out,0,2) = r0[6];
	MAT(out, 0, 3) = r0[7], MAT(out,1,0) = r1[4];
	MAT(out, 1, 1) = r1[5], MAT(out,1,2) = r1[6];
	MAT(out, 1, 3) = r1[7], MAT(out,2,0) = r2[4];
	MAT(out, 2, 1) = r2[5], MAT(out,2,2) = r2[6];
	MAT(out, 2, 3) = r2[7], MAT(out,3,0) = r3[4];
	MAT(out, 3, 1) = r3[5], MAT(out,3,2) = r3[6];
	MAT(out, 3, 3) = r3[7];

	return true;
#undef MAT
#undef SWAP_ROWS
}
void mathlib::SinCos(float radians, float *sine, float *cosine){
	*sine = sinf(radians);
	*cosine = cosf(radians);
}
void mathlib::Matrix4x4_CreateFromEntity(float out[4][4], const vec3_t angles, const vec3_t origin, float scale){
	float	angle, sr, sp, sy, cr, cp, cy;
	if (angles[Q_ROLL]){
		angle = angles[Q_YAW] * (F_PI * 2 / 360.0f);
		SinCos(angle, &sy, &cy);
		angle = angles[Q_PITCH] * (F_PI * 2 / 360.0f);
		SinCos(angle, &sp, &cp);
		angle = angles[Q_ROLL] * (F_PI * 2 / 360.0f);
		SinCos(angle, &sr, &cr);

		out[0][0] = (cp*cy) * scale;
		out[0][1] = (sr*sp*cy + cr * -sy) * scale;
		out[0][2] = (cr*sp*cy + -sr * -sy) * scale;
		out[0][3] = origin[0];
		out[1][0] = (cp*sy) * scale;
		out[1][1] = (sr*sp*sy + cr * cy) * scale;
		out[1][2] = (cr*sp*sy + -sr * cy) * scale;
		out[1][3] = origin[1];
		out[2][0] = (-sp) * scale;
		out[2][1] = (sr*cp) * scale;
		out[2][2] = (cr*cp) * scale;
		out[2][3] = origin[2];
	}
	else if (angles[Q_PITCH]){
		angle = angles[Q_YAW] * (F_PI * 2 / 360.0f);
		SinCos(angle, &sy, &cy);
		angle = angles[Q_PITCH] * (F_PI * 2 / 360.0f);
		SinCos(angle, &sp, &cp);

		out[0][0] = (cp*cy) * scale;
		out[0][1] = (-sy) * scale;
		out[0][2] = (sp*cy) * scale;
		out[0][3] = origin[0];
		out[1][0] = (cp*sy) * scale;
		out[1][1] = (cy)* scale;
		out[1][2] = (sp*sy) * scale;
		out[1][3] = origin[1];
		out[2][0] = (-sp) * scale;
		out[2][1] = 0.0f;
		out[2][2] = (cp)* scale;
		out[2][3] = origin[2];
	}
	else if (angles[Q_YAW]){
		angle = angles[Q_YAW] * (F_PI * 2 / 360.0f);
		SinCos(angle, &sy, &cy);

		out[0][0] = (cy)* scale;
		out[0][1] = (-sy) * scale;
		out[0][2] = 0.0f;
		out[0][3] = origin[0];
		out[1][0] = (sy)* scale;
		out[1][1] = (cy)* scale;
		out[1][2] = 0.0f;
		out[1][3] = origin[1];
		out[2][0] = 0.0f;
		out[2][1] = 0.0f;
		out[2][2] = scale;
		out[2][3] = origin[2];
	}
	else{
		out[0][0] = scale;
		out[0][1] = 0.0f;
		out[0][2] = 0.0f;
		out[0][3] = origin[0];
		out[1][0] = 0.0f;
		out[1][1] = scale;
		out[1][2] = 0.0f;
		out[1][3] = origin[1];
		out[2][0] = 0.0f;
		out[2][1] = 0.0f;
		out[2][2] = scale;
		out[2][3] = origin[2];
	}
}
void mathlib::Matrix4x4_ConcatTransforms(float out[4][4], float in1[4][4], float in2[4][4]){
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
}
void mathlib::ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4]){
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
		in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
		in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
		in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
		in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
		in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
		in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
		in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
		in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
		in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
		in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
		in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
		in1[2][2] * in2[2][3] + in1[2][3];
}
void mathlib::RGBToHSV(int r, int g, int b, float& h, float& s, float& v) {
	float fr = r / 255.0f, fg = g / 255.0f, fb = b / 255.0f;
	float max = max3(fr, fg, fb);
	float min = min3(fr, fg, fb);
	float range = max - min;
	//H
	if (range <= 0)
		h = 0;
	else if (max == r)
		h = 60 * (fg - fb) / range + (g >= b ? 0 : 360);
	else if (max == g)
		h = 60 * (fb - fr) / range + 120;
	else
		h = 60 * (fr - fg) / range + 240;
	if (abs(h) >= 360)
		h = static_cast<float>(fmod(h, 360));
	//S
	s = max <= 0 ? 0 : range / max;
	//V
	v = max <= 0 ? 0 : max;
}
void mathlib::HSVToRGB(float h, float s, float v, int& r, int& g, int& b) {
	//0<=h<360
	//0<=s<=1
	//0<=v<=1
	h = static_cast<float>(fmod(h, 360));
	s = clamp<float>(s, 0.0, 1.0);
	v = clamp<float>(v, 0.0, 1.0);
	float section = h / 60;
	float c = v * s;
	float x = static_cast<float>(c * (1 - abs(fmod(section, 2) - 1)));
	float hr = 0, hg = 0, hb = 0;
	switch ((int)section) {
	case 0:hr = c, hg = x, hb = 0; break;
	case 1:hr = x; hg = c; hb = 0; break;
	case 2:hr = 0; hg = c; hb = x; break;
	case 3:hr = 0; hg = x; hb = c; break;
	case 4:hr = x; hg = 0; hb = c; break;
	case 5:hr = c; hg = 0; hb = x; break;
	}
	float m = v - c;
	r = static_cast<int>((hr + m) * 255);
	g = static_cast<int>((hg + m) * 255);
	b = static_cast<int>((hb + m) * 255);
}
int mathlib::fatoi(const char* b){
	int res = 0;
	int sgn = 1;
	for (; 
		((*b) == ' ' || (*b) == '\t' || 
		(*b) == '\n' || (*b) == '\v' || 
		(*b) == '\f' || (*b) == '\r'); ++b);

	if (*b == '-'){
		sgn = -1;
		++b;
	}
	else if (*b == '+')
		++b;
	// Iterate through all characters of input string and update result
	for (; *b != 0; ++b){
		if (*b < '0' || *b > '9')
			return sgn * res;
		res = res * 10 + *b - '0';
	}
	// return result.
	return sgn * res;
}