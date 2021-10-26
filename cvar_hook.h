#pragma once
#include "cvardef.h"
#include <map>
cvar_t* RegisterHookedVariable(char* szName, char* szValue, int flags, void(*pCallBack)(cvar_t* cvars) = NULL);
#define CREATE_CVAR(name,value,flag,callback) RegisterHookedVariable((char*)name,(char*)value,flag,callback)
extern std::map<cvar_t*, void(*)(cvar_t* cvars)> gCVarsHookMap;