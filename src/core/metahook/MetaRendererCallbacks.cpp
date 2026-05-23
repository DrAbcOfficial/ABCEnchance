#include "hud/vgui/radar.h"
#include "hud/Viewport.h"
#include "MetaRendererCallbacks.h"
#include <vgui_controls/ModelViewPanel.h>

CMetaRendererCallbacks g_MetaRendererCallbacks;
/*
 Called from GL_BeginRendering
*/
void CMetaRendererCallbacks::OnBeginRendering(int* x, int* y, int* width, int* height)
{

}

/*
	Called from GL_BeginRendering -> R_RenderFrameStart
*/
void CMetaRendererCallbacks::OnRenderFrameStart()
{

}

/*
	Called from GL_BeginRendering
*/
void CMetaRendererCallbacks::OnBeginRenderingPost(int* x, int* y, int* width, int* height)
{

}

/*
	Called from GL_FreeFrameBuffers
*/
void CMetaRendererCallbacks::OnFreeFrameBuffers()
{

}

/*
	Called from GL_GenerateFrameBuffers
*/
void CMetaRendererCallbacks::OnGenerateFrameBuffers()
{

}

/*
	Called from R_PreRenderView
*/
void CMetaRendererCallbacks::OnPreRenderView()
{
	GetBaseViewPort()->GetRadarPanel()->RenderRadar();
	if (vgui::g_pModelViewPanel)
		vgui::g_pModelViewPanel->RenderModel();
}

/*
	Called from R_PreRenderView, after switching to target FBO
*/
void CMetaRendererCallbacks::OnPreRenderViewPost()
{

}

/*
	Called from R_PostRenderView
*/
void CMetaRendererCallbacks::OnPostRenderView()
{

}

/*
	Called from R_PostRenderView, after executing all built-in post-processes
*/
void CMetaRendererCallbacks::OnPostRenderViewPost()
{

}

/*
	Called from R_RenderEndFrame
*/
void CMetaRendererCallbacks::OnRenderEndFrame()
{
}