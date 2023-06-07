#include <interface.h>
#include <metahook.h>
#include <plugins.h>
#include "IParticleMan.h"

IParticleMan* g_pParticleMan;
HINTERFACEMODULE g_hParticleman;
void InitCreateParticleMan() {
	char szPDir[512];
	CreateInterfaceFn ParticleManCreateInterface = nullptr;
	if (!gEngfuncs.COM_ExpandFilename(PARTICLEMAN_DLLNAME, szPDir, sizeof(szPDir))){
		g_pMetaHookAPI->SysError("[ABCEnchance] Failed find particleman.dll!");
		return;
	}
	g_hParticleman = (HINTERFACEMODULE)Sys_LoadModule(szPDir);
	ParticleManCreateInterface = (CreateInterfaceFn)Sys_GetFactory(particleman);
	if (!ParticleManCreateInterface)
		g_pMetaHookAPI->SysError("[ABCEnchance] Can not create ParticleMan Interface!");
	else {
		g_pParticleMan = (IParticleMan*)ParticleManCreateInterface(PARTICLEMAN_INTERFACE, NULL);
		if (g_pParticleMan) {
			g_pParticleMan->SetUp(&gEngfuncs);
			// Add custom particle classes here BEFORE calling anything else or you will die.
			g_pParticleMan->AddCustomParticleClassSize(sizeof(CBaseParticle));
		}
		else
			g_pMetaHookAPI->SysError("[ABCEnchance] Can not setup ParticleMan!");
	}
}
void FreeParticleMan() {
	Sys_FreeModule(g_hParticleman);
}