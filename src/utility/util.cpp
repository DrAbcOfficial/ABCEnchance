#include <metahook.h>
#include <algorithm>
#include "utility/util.h"
#include "utility/vgui_util.h"
#include "triangleapi.h"
#include <Controls.h>

cl_refHookfunc_t gHookFuncs;
cl_cvars_t gCVars;
char m_szCurrentLanguage[128] = { 0 };

xcommand_t Q_HOOK_COMMAND(const char* cmd, xcommand_t func) {
	return g_pMetaHookAPI->HookCmd(cmd, func);
};
void ADD_COMMAND(const char* cmd, void (*pfnEngSrc_function)(void)) {
	gEngfuncs.pfnAddCommand(cmd, pfnEngSrc_function);
}
void ConsoleWriteline(const char* x) {
	gEngfuncs.Con_Printf(x);
}
cvar_t* CVAR_GET_POINTER(const char* x) {
	return gEngfuncs.pfnGetCvarPointer(x);
}
float CVAR_GET_FLOAT(const char* x) {
	return gEngfuncs.pfnGetCvarFloat(x);
}
const char* CVAR_GET_STRING(const char* x) {
	return gEngfuncs.pfnGetCvarString(x);
}
void CVAR_SET_STRING(const char* x, const char* v) {
	gEngfuncs.Cvar_Set(x, v);
}
void CVAR_SET_FLOAT(const char* x, float v) {
	gEngfuncs.Cvar_SetValue(x, v);
}
cvar_t* CREATE_CVAR(const char* name, const char* val, int flag, cvar_callback_t callback) {
	cvar_t* cvar = gEngfuncs.pfnRegisterVariable(const_cast<char*>(name), const_cast<char*>(val), flag);
	if (callback)
		g_pMetaHookAPI->RegisterCvarCallback(name, callback, nullptr); 
	return cvar;
}
void ServerCmd(const char* x) {
	(*gEngfuncs.pfnServerCmd)(const_cast<char*>(x));
}
void EngineClientCmd(const char* x) {
	(*gEngfuncs.pfnClientCmd)(const_cast<char*>(x));
}
void PlaySoundByName(const char* x, float volum) {
	(*gEngfuncs.pfnPlaySoundByName)(const_cast<char*>(x), volum);
}
void VEC_WorldToScreen(vec3_t world, vec3_t screen ) {
	int screen_w, screen_h;
	vgui::surface()->GetScreenSize(screen_w, screen_h);
	(*gEngfuncs.pTriAPI->WorldToScreen)(world, screen);
	screen[0] = std::max<float>(0, (1.0f + screen[0]) * screen_w / 2); 
	screen[1] = std::max<float>(0, (1.0f - screen[1]) * screen_h / 2);
}
float ClientTime() {
	return gEngfuncs.GetClientTime();
}