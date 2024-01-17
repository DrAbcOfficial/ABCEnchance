#include <interface.h>
#include <plugins.h>
#include "IParticleMan.h"

extern void SysError(const char* message ...);

IParticleMan* g_pParticleMan;
HINTERFACEMODULE g_hParticleman;
void InitCreateParticleMan() {
	char szPDir[512];
	CreateInterfaceFn ParticleManCreateInterface = nullptr;
	if (!gEngfuncs.COM_ExpandFilename(PARTICLEMAN_DLLNAME, szPDir, sizeof(szPDir))){
		SysError("Failed find particleman.dll!");
		return;
	}
	g_hParticleman = (HINTERFACEMODULE)Sys_LoadModule(szPDir);
	ParticleManCreateInterface = (CreateInterfaceFn)Sys_GetFactory(g_hParticleman);
	if (!ParticleManCreateInterface)
		SysError("Can not create ParticleMan Interface!");
	else {
		g_pParticleMan = (IParticleMan*)ParticleManCreateInterface(PARTICLEMAN_INTERFACE, NULL);
		if (g_pParticleMan) {
			g_pParticleMan->SetUp(&gEngfuncs);
			// Add custom particle classes here BEFORE calling anything else or you will die.
			g_pParticleMan->AddCustomParticleClassSize(sizeof(CBaseParticle));
		}
		else
			SysError("Can not setup ParticleMan!");
	}
}
void FreeParticleMan() {
	Sys_FreeModule(g_hParticleman);
}