#include <metahook.h>
#include <cmath>
#include "mymathlib.h"

#include "pm_defs.h"
#include "pmtrace.h"
#include "event_api.h"

#include "local.h"
#include "exportfuncs.h"
#include "viewmodellag.h"

//copied and edited from here: https://github.com/SNMetamorph/PrimeXT/blob/master/client/r_view.cpp#L266
void V_CalcViewModelLag(ref_params_t* pparams, Vector& origin, Vector& angles, const Vector& original_angles)
{
	if (gCVars.pModelLag->value <= 0)
		return;
	//��������
	extern float GetCurrentFOV();
	if (gCVars.pModelLagAutoStop->value > 0 && GetCurrentFOV() != CMathlib::clamp<float>(gCVars.pCvarDefaultFOV->value, 10.0f, 110.0f))
		return;
	static Vector m_vecLastFacing;
	Vector vOriginalOrigin = origin;
	Vector vOriginalAngles = angles;

	// Calculate our drift
	Vector forward, right, up;

	CMathlib::AngleVectors(angles, forward, right, up);

	if (pparams->frametime != 0.0f)	// not in paused
	{
		Vector vDifference;

		vDifference = forward - m_vecLastFacing;

		float flSpeed = 5.0f;

		// If we start to lag too far behind, we'll increase the "catch up" speed.
		// Solves the problem with fast cl_yawspeed, m_yaw or joysticks rotating quickly.
		// The old code would slam lastfacing with origin causing the viewmodel to pop to a new position
		float flDiff = vDifference.Length();
		if ((flDiff > gCVars.pModelLagValue->value) && (gCVars.pModelLagValue->value > 0.0f))
		{
			float flScale = flDiff / gCVars.pModelLagValue->value;
			flSpeed *= flScale;
		}

		// FIXME:  Needs to be predictable?
		m_vecLastFacing = m_vecLastFacing + vDifference * (flSpeed * pparams->frametime);
		// Make sure it doesn't grow out of control!!!
		m_vecLastFacing = m_vecLastFacing.Normalize();
		origin = origin + (vDifference * -1.0f) * flSpeed;
	}

	CMathlib::AngleVectors(original_angles, forward, right, up);

	float pitch = original_angles[CMathlib::Q_PITCH];

	if (pitch > 180.0f)
	{
		pitch -= 360.0f;
	}
	else if (pitch < -180.0f)
	{
		pitch += 360.0f;
	}

	if (gCVars.pModelLagValue->value <= 0.0f)
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
void V_CalcModelSlide(ref_params_t* pparams) {
	if (gCVars.pModelSlide->value <= 0)
		return;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	pmtrace_t tr;
	Vector vecEnd = local->curstate.origin;
	vecEnd.z += -4;
	gEngfuncs.pEventAPI->EV_SetTraceHull(0);
	gEngfuncs.pEventAPI->EV_PlayerTrace(local->curstate.origin, vecEnd, PM_STUDIO_IGNORE, local->index, &tr);
	if (tr.fraction >= 1)
		return;
	float flViewHeight = CMathlib::clamp<float>(pparams->viewheight[2], 12.0f, 28.0f);
	//12 Duck
	//28 Stand
	float flSlideRatio = 1 - ((flViewHeight - 12) / 16);
	cl_entity_t* view = gEngfuncs.GetViewModel();
	Vector vecRight;
	CMathlib::AngleVectors(view->angles, nullptr, vecRight, nullptr);
	vecRight = -vecRight.Normalize() * gCVars.pModelSlideLength->value * flSlideRatio;
	view->curstate.angles[2] = -gCVars.pModelSlideAngle->value * flSlideRatio;
	CMathlib::VectorAdd(view->origin, vecRight, view->origin);
	view->origin[2] -= gCVars.pModelSlideHeight->value * flSlideRatio;
}