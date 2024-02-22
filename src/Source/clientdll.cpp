#include <metahook.h>

#include "CCustomHud.h"

class IClientDLL : public IBaseInterface {
	virtual int Init(char* interface_char_ptr) = 0;
	virtual void PlaySoundFX(float* param_1, int param_2, char* param_3, float param_4, float param_5, int param_6, int param_7) = 0;
	virtual void RenderBegin(ref_params_s* param_1) = 0;
	virtual void RenderEnd1() = 0;
	virtual void RenderEnd2() = 0;
};
static int(__fastcall* m_pfnCClientDLL_Init)(void* pthis, int, char* interface_char_ptr) = nullptr;
static void(__fastcall* m_pfnCClientDLL_PlaySoundFX)(void* pthis, int, float* param_1, int param_2, char* param_3, float param_4, float param_5, int param_6, int param_7) = nullptr;
static void(__fastcall* m_pfnCClientDLL_RenderBegin)(void* pthis, int, ref_params_s* param_1) = nullptr;
static void(__fastcall* m_pfnCClientDLL_RenderEnd1)(void* pthis, int) = nullptr;
static void(__fastcall* m_pfnCClientDLL_RenderEnd2)(void* pthis, int) = nullptr;
class CNewClientDLL : public IClientDLL {
	virtual int Init(char* interface_char_ptr) override {
		return m_pfnCClientDLL_Init(this, 0, interface_char_ptr);
	}
	virtual void PlaySoundFX(float* param_1, int param_2, char* param_3, float param_4, float param_5, int param_6, int param_7) override {
		m_pfnCClientDLL_PlaySoundFX(this, 0, param_1, param_2, param_3, param_4, param_5, param_6, param_7);
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
	g_pMetaHookAPI->VFTHook(interface_ptr, 0, 1, ProxyVFTable[1], (void**)&m_pfnCClientDLL_Init);
	g_pMetaHookAPI->VFTHook(interface_ptr, 0, 2, ProxyVFTable[2], (void**)&m_pfnCClientDLL_PlaySoundFX);
	g_pMetaHookAPI->VFTHook(interface_ptr, 0, 3, ProxyVFTable[3], (void**)&m_pfnCClientDLL_RenderBegin);
	g_pMetaHookAPI->VFTHook(interface_ptr, 0, 4, ProxyVFTable[4], (void**)&m_pfnCClientDLL_RenderEnd1);
	g_pMetaHookAPI->VFTHook(interface_ptr, 0, 5, ProxyVFTable[5], (void**)&m_pfnCClientDLL_RenderEnd2);
}