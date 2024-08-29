#include <metahook.h>

#include "CCustomHud.h"

class IClientDLL : public IBaseInterface {
	virtual int Init(char*) = 0;
	virtual void PlaySoundFX(float*, int, char*, float, float, int, int) = 0;
	virtual void RenderBegin(ref_params_s*) = 0;
	virtual void RenderEnd1() = 0;
	virtual void RenderEnd2() = 0;
};
static int(__fastcall* m_pfnCClientDLL_Init)(void* pthis, int, char*) = nullptr;
static void(__fastcall* m_pfnCClientDLL_PlaySoundFX)(void* pthis, int, float*, int, char*, float, float, int, int) = nullptr;
static void(__fastcall* m_pfnCClientDLL_RenderBegin)(void* pthis, int, ref_params_s*) = nullptr;
static void(__fastcall* m_pfnCClientDLL_RenderEnd1)(void* pthis, int) = nullptr;
static void(__fastcall* m_pfnCClientDLL_RenderEnd2)(void* pthis, int) = nullptr;

class CNewClientDLL : public IClientDLL {
	virtual int Init(char* interface_char_ptr) override {
		return m_pfnCClientDLL_Init(this, 0, interface_char_ptr);
	}
	virtual void PlaySoundFX(float* origin, int channel, char* name, float fvol, float attenuation, int extraflags, int pitch) override {
		m_pfnCClientDLL_PlaySoundFX(this, 0, origin, channel, name, fvol, attenuation, extraflags, pitch);
	}
	virtual void RenderBegin(ref_params_s* param_1) {
		gCustomHud.RenderRadar();
		m_pfnCClientDLL_RenderBegin(this, 0, param_1);
	}
	virtual void RenderEnd1() {
		m_pfnCClientDLL_RenderEnd1(this, 0);
	}
	virtual void RenderEnd2() {
		m_pfnCClientDLL_RenderEnd2(this, 0);
	}
};

static CNewClientDLL s_newclientdll;

void ClientDLLHook(void* interface_ptr) {
	PVOID* ProxyVFTable = *(PVOID**)&s_newclientdll;
	//g_pMetaHookAPI->VFTHook(interface_ptr, 0, 1, ProxyVFTable[1], (void**)&m_pfnCClientDLL_Init);
	//g_pMetaHookAPI->VFTHook(interface_ptr, 0, 2, ProxyVFTable[2], (void**)&m_pfnCClientDLL_PlaySoundFX);
	g_pMetaHookAPI->VFTHook(interface_ptr, 0, 3, ProxyVFTable[3], (void**)&m_pfnCClientDLL_RenderBegin);
	//g_pMetaHookAPI->VFTHook(interface_ptr, 0, 4, ProxyVFTable[4], (void**)&m_pfnCClientDLL_RenderEnd1);
	//g_pMetaHookAPI->VFTHook(interface_ptr, 0, 5, ProxyVFTable[5], (void**)&m_pfnCClientDLL_RenderEnd2);
}