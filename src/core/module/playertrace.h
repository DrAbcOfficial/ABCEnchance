#pragma once
#ifndef _PLAYER_TRACE_H_
#define _PLAYER_TRACE_H_
using qboolean = int;
using vec3_t = float[3];

#include "pmtrace.h"

class CPlayerTrace {
public:
	enum class TRACE_TYPE
	{
		HEAD,
		FOOT,
		VIEW
	};
	void Update();
	pmtrace_t* Get(TRACE_TYPE ty);
private:
	pmtrace_t m_trViewpoint;
	pmtrace_t m_trFoot;
	pmtrace_t m_trHead;
};

CPlayerTrace* GetPlayerTrace();
#endif // !_PLAYER_TRACE_H_
