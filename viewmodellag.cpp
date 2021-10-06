#include <metahook.h>
#include <cmath>
#include "mathlib.h"
#include "local.h"
#include "exportfuncs.h"
#include "viewmodellag.h"
//copied and edited from here: https://github.com/SNMetamorph/PrimeXT/blob/master/client/r_view.cpp#L266
void V_CalcViewModelLag(ref_params_t* pparams){
	//¼¦ÃçÎäÆ÷
	if (gCVars.pModelLagAutoStop->value > 0 && m_hfov != gCVars.pCvarDefaultFOV->value)
		return;
	static vec3_t m_vecLastFacing;
	cl_entity_t* view = gEngfuncs.GetViewModel();
	vec3_t vOriginalOrigin;
	vec3_t vOriginalAngles;
	VectorCopy(view->origin, vOriginalOrigin);
	VectorCopy(view->angles, vOriginalAngles);
	// Calculate our drift
	vec3_t forward, right, up;
	AngleVectors(view->angles, forward, right, up);
	// not in paused
	if (pparams->frametime != 0.0f){
		vec3_t vDifference;
		vDifference[0] = forward[0] - m_vecLastFacing[0];
		vDifference[1] = forward[1] - m_vecLastFacing[1];
		vDifference[2] = forward[2] - m_vecLastFacing[2];

		float flSpeed = 5.0f;
		// If we start to lag too far behind, we'll increase the "catch up" speed.
		// Solves the problem with fast cl_yawspeed, m_yaw or joysticks rotating quickly.
		// The old code would slam lastfacing with origin causing the viewmodel to pop to a new position
		float flDiff = VectorLength(vDifference);
		if ((flDiff > gCVars.pModelLagValue->value) && (gCVars.pModelLagValue->value > 0.0f))
			flSpeed *= (flDiff / gCVars.pModelLagValue->value);
		m_vecLastFacing[0] = m_vecLastFacing[0] + vDifference[0] * (flSpeed * pparams->frametime);
		m_vecLastFacing[1] = m_vecLastFacing[1] + vDifference[1] * (flSpeed * pparams->frametime);
		m_vecLastFacing[2] = m_vecLastFacing[2] + vDifference[2] * (flSpeed * pparams->frametime);
		// Make sure it doesn't grow out of control!!!
		VectorNormalize(m_vecLastFacing);
		view->origin[0] += (vDifference[0] * -1.0f) * flSpeed;
		view->origin[1] += (vDifference[1] * -1.0f) * flSpeed;
		view->origin[2] += (vDifference[2] * -1.0f) * flSpeed;
	}
	AngleVectors(pparams->cl_viewangles, forward, right, up);
	float pitch = pparams->cl_viewangles[PITCH];
	pitch += pitch > 180.0f ? -360.0f : pitch < -180.0f ? 360.0f : 0;
	if (gCVars.pModelLagValue->value <= 0.0f){
		VectorCopy(vOriginalOrigin, view->origin);
		VectorCopy(vOriginalAngles, view->angles);
	}
	else{
		for (int i = 0; i < 3; i++) {
			view->origin[i] += (forward[i] * (-pitch * 0.035f));
			view->origin[i] += (right[i] * (-pitch * 0.03f));
			view->origin[i] += (up[i] * (-pitch * 0.02f));
		}
	}
}