#include <metahook.h>
#include "mathlib.h"
void V_CalcViewModelLag(ref_params_t* pparams, vec3_t origin, vec3_t angles, const vec3_t original_angles)
{
	static vec3_t m_vecLastFacing;
	vec3_t vOriginalOrigin;
	vec3_t vOriginalAngles;
	VectorCopy(origin,vOriginalOrigin);
	VectorCopy(angles, vOriginalAngles);

	// Calculate our drift
	vec3_t forward, right, up;

	AngleVectors(angles, forward, right, up);

	if (pparams->frametime != 0.0f)	// not in paused
	{
		vec3_t vDifference;
		vDifference[0] = forward[0] - m_vecLastFacing[0];
		vDifference[1] = forward[1] - m_vecLastFacing[1];
		vDifference[2] = forward[2] - m_vecLastFacing[2];

		float flSpeed = 5.0f;

		float flDiff = VectorLength(vDifference);
		if ((flDiff > cl_weaponlag->value) && (cl_weaponlag->value > 0.0f))
		{
			float flScale = flDiff / cl_weaponlag->value;
			flSpeed *= flScale;
		}
		m_vecLastFacing[0] = m_vecLastFacing[0] + vDifference[0] * (flSpeed * pparams->frametime);
		m_vecLastFacing[1] = m_vecLastFacing[1] + vDifference[1] * (flSpeed * pparams->frametime);
		m_vecLastFacing[2] = m_vecLastFacing[2] + vDifference[2] * (flSpeed * pparams->frametime);
		// Make sure it doesn't grow out of control!!!
		VectorNormalize(m_vecLastFacing);

		origin = origin + (vDifference * -1.0f) * flSpeed;
	}
	AngleVectors(original_angles, forward, right, up);
	float pitch = original_angles[PITCH];

	if (pitch > 180.0f)
	{
		pitch -= 360.0f;
	}
	else if (pitch < -180.0f)
	{
		pitch += 360.0f;
	}

	if (cl_weaponlag->value <= 0.0f)
	{
		origin = vOriginalOrigin;
		angles = vOriginalAngles;
	}
	else
	{
		// FIXME: These are the old settings that caused too many exposed polys on some models
		origin = origin + forward * (-pitch * 0.035f);
		origin = origin + right * (-pitch * 0.03f);
		origin = origin + up * (-pitch * 0.02f);
	}
}