#include <metahook.h>
#include "plugins.h"
#include "MetaRenderer.h"

static HINTERFACEMODULE g_hMetaRenderer;
static IMetaRenderer* g_pMetaRenderer;

IMetaRenderer* MetaRenderer()
{
	return g_pMetaRenderer;
}

void MetaRenderer_Init()
{
	g_hMetaRenderer = (HINTERFACEMODULE)GetModuleHandleA("Renderer.dll");

	if (g_hMetaRenderer)
	{
		auto factory = Sys_GetFactory(g_hMetaRenderer);

		if (!factory)
		{
			SYS_ERROR("Could not get factory from Renderer.dll");
			return;
		}

		g_pMetaRenderer = (decltype(g_pMetaRenderer))factory(METARENDERER_INTERFACE_VERSION, NULL);

		if (!g_pMetaRenderer)
		{
			SYS_ERROR("Could not get g_pMetaRenderer from Renderer.dll");
			return;
		}

		return;
	}
}

void MetaRenderer_Shutdown()
{
	//Never unload by ourself!
	if (g_hMetaRenderer)
	{
		g_hMetaRenderer = NULL;
	}
}
