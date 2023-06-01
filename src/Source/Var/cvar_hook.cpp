#include <metahook.h>
#include "cvar_hook.h"

std::map<cvar_t*, void(*)(cvar_t* cvars)> gCVarsHookMap;
cvar_t* RegisterHookedVariable(char* szName, char* szValue, int flags, void(*pCallBack)(cvar_t* cvars)) {
	cvar_t* temp = gEngfuncs.pfnRegisterVariable(szName, szValue, flags);
	gCVarsHookMap[temp] = pCallBack;
	return temp;
}