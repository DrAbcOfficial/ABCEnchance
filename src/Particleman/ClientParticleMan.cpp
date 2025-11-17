#include <interface.h>
#include <plugins.h>
#include "IParticleMan.h"
#include "core/events/networkmessage.h"

IParticleMan* g_pParticleMan = nullptr;
HINTERFACEMODULE g_hParticleman = nullptr;

void LoadParticleMan()
{
	char szPDir[512]{};
	CreateInterfaceFn ParticleManCreateInterface = nullptr;
	if (!gEngfuncs.COM_ExpandFilename(PARTICLEMAN_DLLNAME, szPDir, sizeof(szPDir))){
		SYS_ERROR("Could not get full path for particleman.dll!");
		return;
	}
	g_hParticleman = (HINTERFACEMODULE)Sys_LoadModule(szPDir);
	if (!g_hParticleman)
	{
		SYS_ERROR("Could not load particleman.dll!");
		return;
	}
	ParticleManCreateInterface = Sys_GetFactory(g_hParticleman);
	if (!ParticleManCreateInterface)
	{
		SYS_ERROR("Could not get factory from particleman.dll!");
		return;
	}
	g_pParticleMan = (IParticleMan*)ParticleManCreateInterface(PARTICLEMAN_INTERFACE, nullptr);
	if (!g_pParticleMan) {
		SYS_ERROR("Could not get interface \"" PARTICLEMAN_INTERFACE "\" from particleman.dll!");
		return;
	}

	g_pParticleMan->SetUp(&gEngfuncs);
	// Add custom particle classes here BEFORE calling anything else or you will die.
	g_pParticleMan->AddCustomParticleClassSize(sizeof(CBaseParticle));
	g_EventInitHUD.append([]() {
		if (g_pParticleMan)
			g_pParticleMan->ResetParticles();
	});
}

void FreeParticleMan()
{
	Sys_FreeModule(g_hParticleman);
}