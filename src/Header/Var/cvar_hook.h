#pragma once
#include "cvardef.h"
#include <map>
cvar_t* RegisterHookedVariable(char* szName, char* szValue, int flags, void(*pCallBack)(cvar_t* cvars) = nullptr);
#define CREATE_CVAR(name,value,flag,callback) RegisterHookedVariable(const_cast<char*>(name),const_cast<char*>(value),flag,callback)
extern std::map<cvar_t*, void(*)(cvar_t* cvars)> gCVarsHookMap;