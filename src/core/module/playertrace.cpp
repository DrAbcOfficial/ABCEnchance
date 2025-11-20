#include <metahook.h>
#include "pm_defs.h"
#include "event_api.h"

#include "mathlib/vector.h"
#include "core/library/mymathlib.h"

#include "utility/vgui_util.h"
#include "hud/Viewport.h"

#include "playertrace.h"

extern cl_enginefunc_t gEngfuncs;

static CPlayerTrace s_PlayerTrace;

void CPlayerTrace::Update(){
	if (!GetBaseViewPort()->IsVisible())
		return;
	auto local = gEngfuncs.GetLocalPlayer();
	if (!local)
		return;
	//vp
	Vector vecAngles;
	gEngfuncs.GetViewAngles(vecAngles);
	Vector vecForward;
	CMathlib::AngleVectors(vecAngles, vecForward, nullptr, nullptr);
	Vector vecEnd;
	Vector vecSrc = local->origin;
	VectorMA(vecSrc, 8192, vecForward, vecEnd);
	pmtrace_t tr;
	Vector vecOfs;
	gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(vecOfs);
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc + vecOfs, vecEnd, PM_STUDIO_BOX, local->index, &tr);
	memcpy_s(&m_trViewpoint, sizeof(m_trViewpoint), &tr, sizeof(tr));
	//head
	gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecSrc + Vector(0, 0, 8192), PM_WORLD_ONLY, local->index, &tr);
	memcpy_s(&m_trHead, sizeof(m_trHead), &tr, sizeof(tr));
	//foot
	gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecSrc + Vector(0, 0, -8192), PM_WORLD_ONLY, local->index, &tr);
	memcpy_s(&m_trFoot, sizeof(m_trFoot), &tr, sizeof(tr));
}

pmtrace_t* CPlayerTrace::Get(TRACE_TYPE ty){
	switch (ty){
	case CPlayerTrace::TRACE_TYPE::HEAD:return &m_trHead; break;
	case CPlayerTrace::TRACE_TYPE::FOOT:return &m_trFoot; break;
	case CPlayerTrace::TRACE_TYPE::VIEW: return &m_trViewpoint; break;
	}
	return nullptr;
}

CPlayerTrace* GetPlayerTrace(){
	return &s_PlayerTrace;
}
