#include <metahook.h>
#include <cmath>
#include "mymathlib.h"
#include "CVector.h"

#include "pm_defs.h"
#include "pmtrace.h"
#include "event_api.h"

#include "local.h"
#include "exportfuncs.h"
#include "viewmodellag.h"
//copied and edited from here: https://github.com/SNMetamorph/PrimeXT/blob/master/client/r_view.cpp#L266

void V_CalcViewModelLag(ref_params_t* pparams){
	if (gCVars.pModelLag->value <= 0)
		return;
	//¼¦ÃçÎäÆ÷
	if (gCVars.pModelLagAutoStop->value > 0 && m_hfov != mathlib::clamp(gCVars.pCvarDefaultFOV->value, 10.0f, 110.0f))
		return;
	static vec3_t m_vecLastFacing;
	cl_entity_t* view = gEngfuncs.GetViewModel();
	vec3_t vOriginalOrigin;
	vec3_t vOriginalAngles;
	mathlib::VectorCopy(view->origin, vOriginalOrigin);
	mathlib::VectorCopy(view->angles, vOriginalAngles);
	// Calculate our drift
	vec3_t forward, right, up;
	mathlib::AngleVectors(view->angles, forward, right, up);
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
		float flDiff = mathlib::FVectorLength(vDifference);
		if ((flDiff > gCVars.pModelLagValue->value) && (gCVars.pModelLagValue->value > 0.0f))
			flSpeed *= (flDiff / gCVars.pModelLagValue->value);
		m_vecLastFacing[0] = m_vecLastFacing[0] + vDifference[0] * (flSpeed * pparams->frametime);
		m_vecLastFacing[1] = m_vecLastFacing[1] + vDifference[1] * (flSpeed * pparams->frametime);
		m_vecLastFacing[2] = m_vecLastFacing[2] + vDifference[2] * (flSpeed * pparams->frametime);
		// Make sure it doesn't grow out of control!!!
		mathlib::VectorNormalize(m_vecLastFacing);
		view->origin[0] += (vDifference[0] * -1.0f) * flSpeed;
		view->origin[1] += (vDifference[1] * -1.0f) * flSpeed;
		view->origin[2] += (vDifference[2] * -1.0f) * flSpeed;
	}
	mathlib::AngleVectors(pparams->cl_viewangles, forward, right, up);
	float pitch = pparams->cl_viewangles[Q_PITCH];
	pitch += pitch > 180.0f ? -360.0f : pitch < -180.0f ? 360.0f : 0;
	if (gCVars.pModelLagValue->value <= 0.0f){
		mathlib::VectorCopy(vOriginalOrigin, view->origin);
		mathlib::VectorCopy(vOriginalAngles, view->angles);
	}
	else{
		for (int i = 0; i < 3; i++) {
			view->origin[i] += (forward[i] * (-pitch * 0.035f));
			view->origin[i] += (right[i] * (-pitch * 0.03f));
			view->origin[i] += (up[i] * (-pitch * 0.02f));
		}
	}
}
void V_CalcModelSlide(ref_params_t* pparams) {
	if (gCVars.pModelSlide->value <= 0)
		return;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	pmtrace_t tr{};
	CVector vecEnd = local->curstate.origin;
	vecEnd.z += -4;
	gEngfuncs.pEventAPI->EV_SetTraceHull(0);
	gEngfuncs.pEventAPI->EV_PlayerTrace(local->curstate.origin, vecEnd, PM_STUDIO_IGNORE, local->index, &tr);
	if (tr.fraction >= 1)
		return;
	float flViewHeight = mathlib::clamp(pparams->viewheight[2], 12.0f, 28.0f);
	//12 Duck
	//28 Stand
	float flSlideRatio = 1 - ((flViewHeight - 12) / 16);
	cl_entity_t* view = gEngfuncs.GetViewModel();
	CVector vecRight;
	mathlib::AngleVectors(view->angles, nullptr, vecRight, nullptr);
	vecRight = -vecRight.Normalize() * gCVars.pModelSlideLength->value * flSlideRatio;
	view->curstate.angles[2] = -gCVars.pModelSlideAngle->value * flSlideRatio;
	mathlib::VectorAdd(view->origin, vecRight, view->origin);
	view->origin[2] -= gCVars.pModelSlideHeight->value * flSlideRatio;
}