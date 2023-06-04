#include <interface.h>
#include <metahook.h>
#include <plugins.h>
#include "IParticleMan.h"

IParticleMan* g_pParticleMan;
void InitCreateParticleMan() {
	CreateInterfaceFn ParticleManCreateInterface = nullptr;
	HINTERFACEMODULE particleman = (HINTERFACEMODULE)LoadLibrary(PARTICLEMAN_DLLNAME);
	ParticleManCreateInterface = (CreateInterfaceFn)Sys_GetFactory(particleman);
	if (!ParticleManCreateInterface)
		g_pMetaHookAPI->SysError("[ABCEnchance] Can not create ParticleMan Interface!");
	else {
		g_pParticleMan = (IParticleMan*)ParticleManCreateInterface(PARTICLEMAN_INTERFACE, NULL);
		if (g_pParticleMan)
			g_pParticleMan->SetUp(&gEngfuncs);
		else
			g_pMetaHookAPI->SysError("[ABCEnchance] Can not setup ParticleMan!");
	}
}