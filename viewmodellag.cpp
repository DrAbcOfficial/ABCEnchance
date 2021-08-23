#include <metahook.h>
#include "mathlib.h"
#include "local.h"
#include "viewmodellag.h"
void V_CalcViewModelLag(ref_params_t* pparams)
{
	static vec3_t m_vecLastFacing;
	cl_entity_t* view = gEngfuncs.GetViewModel();
	vec3_t* origin = &view->origin;
	vec3_t* angles = &view->angles;

	vec3_t vOriginalOrigin;
	vec3_t vOriginalAngles;
	VectorCopy(*origin,vOriginalOrigin);
	VectorCopy(*angles, vOriginalAngles);

	// Calculate our drift
	vec3_t forward, right, up;

	AngleVectors(*angles, forward, right, up);

	if (pparams->frametime != 0.0f)	// not in paused
	{
		vec3_t vDifference;
		vDifference[0] = forward[0] - m_vecLastFacing[0];
		vDifference[1] = forward[1] - m_vecLastFacing[1];
		vDifference[2] = forward[2] - m_vecLastFacing[2];

		float flSpeed = 5.0f;

		float flDiff = VectorLength(vDifference);
		if ((flDiff > gCVars.pModelLagValue->value) && (gCVars.pModelLagValue->value > 0.0f))
		{
			float flScale = flDiff / gCVars.pModelLagValue->value;
			flSpeed *= flScale;
		}
		m_vecLastFacing[0] = m_vecLastFacing[0] + vDifference[0] * (flSpeed * pparams->frametime);
		m_vecLastFacing[1] = m_vecLastFacing[1] + vDifference[1] * (flSpeed * pparams->frametime);
		m_vecLastFacing[2] = m_vecLastFacing[2] + vDifference[2] * (flSpeed * pparams->frametime);
		// Make sure it doesn't grow out of control!!!
		VectorNormalize(m_vecLastFacing);

		*origin[0] += (vDifference[0] * -1.0f) * flSpeed;
		*origin[1] += (vDifference[1] * -1.0f) * flSpeed;
		*origin[2] += (vDifference[2] * -1.0f) * flSpeed;
	}
	AngleVectors(pparams->cl_viewangles, forward, right, up);
	float pitch = pparams->cl_viewangles[PITCH];
	if (pitch > 180.0f)
		pitch -= 360.0f;
	else if (pitch < -180.0f)
		pitch += 360.0f;
	if (gCVars.pModelLagValue->value <= 0.0f)
	{
		VectorCopy(*origin, vOriginalOrigin);
		VectorCopy(*angles, vOriginalAngles);
	}
	else
	{
		*origin[0] += forward[0] * (-pitch * 0.035f);
		*origin[1] += forward[1] * (-pitch * 0.035f);
		*origin[2] += forward[2] * (-pitch * 0.035f);

		*origin[0] += right[0] * (-pitch * 0.03f);
		*origin[1] += right[1] * (-pitch * 0.03f);
		*origin[2] += right[2] * (-pitch * 0.03f);

		*origin[0] += up[0] * (-pitch * 0.02f);
		*origin[1] += up[1] * (-pitch * 0.02f);
		*origin[2] += up[2] * (-pitch * 0.02f);
	}
}