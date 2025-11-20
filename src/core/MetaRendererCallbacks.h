#pragma once
#include <IMetaRenderer.h>

class CMetaRendererCallbacks : public IMetaRendererCallbacks
{
public:
	/*
	 Called from GL_BeginRendering
	*/
	void OnBeginRendering(int* x, int* y, int* width, int* height) override;

	/*
		Called from GL_BeginRendering -> R_RenderFrameStart
	*/
	void OnRenderFrameStart() override;

	/*
		Called from GL_BeginRendering
	*/
	void OnBeginRenderingPost(int* x, int* y, int* width, int* height) override;

	/*
		Called from GL_FreeFrameBuffers
	*/
	void OnFreeFrameBuffers() override;

	/*
		Called from GL_GenerateFrameBuffers
	*/
	void OnGenerateFrameBuffers()  override;

	/*
		Called from R_PreRenderView
	*/
	void OnPreRenderView() override;

	/*
		Called from R_PreRenderView, after switching to target FBO
	*/
	void OnPreRenderViewPost() override;

	/*
		Called from R_PostRenderView
	*/
	void OnPostRenderView() override;

	/*
		Called from R_PostRenderView, after executing all built-in post-processes
	*/
	void OnPostRenderViewPost() override;

	/*
		Called from R_RenderEndFrame
	*/
	void OnRenderEndFrame()  override;
};
extern CMetaRendererCallbacks g_MetaRendererCallbacks;