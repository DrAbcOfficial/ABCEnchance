#include <metahook.h>
#include <vgui/IScheme.h>

using namespace vgui;

class CSchemeManagerProxy: public ISchemeManager
{
public:
	virtual HScheme LoadSchemeFromFile(const char *fileName, const char *tag);
	virtual void ReloadSchemes(void);
	virtual HScheme GetDefaultScheme(void);
	virtual HScheme GetScheme(const char *tag);
	virtual IImage *GetImage(const char *imageName, bool hardwareFiltered);
	virtual HTexture GetImageID(const char *imageName, bool hardwareFiltered);
	virtual IScheme *GetIScheme(HScheme scheme);
	virtual void Shutdown(bool full = true);
	virtual int GetProportionalScaledValue(int normalizedValue);
	virtual int GetProportionalNormalizedValue(int scaledValue);
};

static HScheme (__fastcall * m_pfnLoadSchemeFromFile)(void *pthis, int, const char *fileName, const char *tag) = NULL;
static void (__fastcall * m_pfnReloadSchemes)(void *pthis, int) = NULL;
static HScheme (__fastcall * m_pfnGetDefaultScheme)(void *pthis, int) = NULL;
static HScheme (__fastcall * m_pfnGetScheme)(void *pthis, int, const char *tag) = NULL;
static IImage *(__fastcall * m_pfnGetImage)(void *pthis, int, const char *imageName, bool hardwareFiltered) = NULL;
static HTexture (__fastcall * m_pfnGetImageID)(void *pthis, int, const char *imageName, bool hardwareFiltered) = NULL;
static IScheme *(__fastcall * m_pfnGetIScheme)(void *pthis, int, HScheme scheme) = NULL;
static void (__fastcall * m_pfnShutdown)(void *pthis, int, bool full) = NULL;
static int (__fastcall * m_pfnGetProportionalScaledValue)(void *pthis, int, int normalizedValue) = NULL;
static int (__fastcall * m_pfnGetProportionalNormalizedValue)(void *pthis, int, int scaledValue) = NULL;

extern vgui::ISchemeManager *g_pVGuiSchemeManager;

HScheme CSchemeManagerProxy::LoadSchemeFromFile(const char *fileName, const char *tag)
{
	return g_pVGuiSchemeManager->LoadSchemeFromFile(fileName, tag);
}

void CSchemeManagerProxy::ReloadSchemes(void)
{
	g_pVGuiSchemeManager->ReloadSchemes();
}

HScheme CSchemeManagerProxy::GetDefaultScheme(void)
{
	return g_pVGuiSchemeManager->GetDefaultScheme();
}

HScheme CSchemeManagerProxy::GetScheme(const char *tag)
{
	return g_pVGuiSchemeManager->GetScheme(tag);
}

IImage *CSchemeManagerProxy::GetImage(const char *imageName, bool hardwareFiltered)
{
	return g_pVGuiSchemeManager->GetImage(imageName, hardwareFiltered);
}

HTexture CSchemeManagerProxy::GetImageID(const char *imageName, bool hardwareFiltered)
{
	return g_pVGuiSchemeManager->GetImageID(imageName, hardwareFiltered);
}

IScheme *CSchemeManagerProxy::GetIScheme(HScheme scheme)
{
	return g_pVGuiSchemeManager->GetIScheme(scheme);
}

void CSchemeManagerProxy::Shutdown(bool full)
{
	g_pVGuiSchemeManager->Shutdown(full);
}

int CSchemeManagerProxy::GetProportionalScaledValue(int normalizedValue)
{
	return g_pVGuiSchemeManager->GetProportionalScaledValue(normalizedValue);
}

int CSchemeManagerProxy::GetProportionalNormalizedValue(int scaledValue)
{
	return g_pVGuiSchemeManager->GetProportionalNormalizedValue(scaledValue);
}

static CSchemeManagerProxy g_SchemeProxy;
extern vgui::ISchemeManager *g_pScheme;

void Scheme_InstallHook(void)
{
	DWORD *pVFTable = *(DWORD **)&g_SchemeProxy;

	g_pMetaHookAPI->VFTHook(g_pScheme, 0, 1, (void *)pVFTable[1], (void **)&m_pfnLoadSchemeFromFile);
	g_pMetaHookAPI->VFTHook(g_pScheme, 0, 2, (void *)pVFTable[2], (void **)&m_pfnReloadSchemes);
	g_pMetaHookAPI->VFTHook(g_pScheme, 0, 3, (void *)pVFTable[3], (void **)&m_pfnGetDefaultScheme);
	g_pMetaHookAPI->VFTHook(g_pScheme, 0, 4, (void *)pVFTable[4], (void **)&m_pfnGetScheme);
	g_pMetaHookAPI->VFTHook(g_pScheme, 0, 5, (void *)pVFTable[5], (void **)&m_pfnGetImage);
	g_pMetaHookAPI->VFTHook(g_pScheme, 0, 6, (void *)pVFTable[6], (void **)&m_pfnGetImageID);
	g_pMetaHookAPI->VFTHook(g_pScheme, 0, 7, (void *)pVFTable[7], (void **)&m_pfnGetIScheme);
	g_pMetaHookAPI->VFTHook(g_pScheme, 0, 8, (void *)pVFTable[8], (void **)&m_pfnShutdown);
	g_pMetaHookAPI->VFTHook(g_pScheme, 0, 9, (void *)pVFTable[9], (void **)&m_pfnGetProportionalScaledValue);
	g_pMetaHookAPI->VFTHook(g_pScheme, 0, 10, (void *)pVFTable[10], (void **)&m_pfnGetProportionalNormalizedValue);
}