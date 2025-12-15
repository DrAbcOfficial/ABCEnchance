//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "GaussianBlurPanel.h"
#include <glew.h>
#include "core/opengl/gl_common.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------

GaussianBlurPanel::GaussianBlurPanel(Panel *parent, const char *name) : Panel(parent, name)
{
	if (MetaRenderer())
	{
		m_BlurFBO[0].iWidth = MetaRenderer()->GetSwapChainWidth() / 2;
		m_BlurFBO[0].iHeight = MetaRenderer()->GetSwapChainWidth() / 2;

		MetaRenderer()->GenFrameBuffer(&m_BlurFBO[0], "GaussianBlurFBO_Vertical");
		MetaRenderer()->FrameBufferColorTexture(&m_BlurFBO[0], GL_RGBA8);

		m_BlurFBO[1].iWidth = MetaRenderer()->GetSwapChainWidth() / 2;
		m_BlurFBO[1].iHeight = MetaRenderer()->GetSwapChainWidth() / 2;

		MetaRenderer()->GenFrameBuffer(&m_BlurFBO[1], "GaussianBlurFBO_Horizontal");
		MetaRenderer()->FrameBufferColorTexture(&m_BlurFBO[1], GL_RGBA8);
	}
}

GaussianBlurPanel::~GaussianBlurPanel(){

	if (MetaRenderer())
	{
		MetaRenderer()->FreeFBO(&m_BlurFBO[0]);
		MetaRenderer()->FreeFBO(&m_BlurFBO[1]);
	}
}


size_t GaussianBlurPanel::GetBlurness(){
	return m_iBlurIteration;
}

void GaussianBlurPanel::SetBlurness(size_t f){
	m_iBlurIteration = f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void GaussianBlurPanel::PaintBackground()
{
	if (MetaRenderer())
	{
		auto CurrentRenderingFBO = MetaRenderer()->GetCurrentRenderingFBO();

		//Note that BlurPass will modify proj & world matrix, so we need to backup first
		MetaRenderer()->PushProjectionMatrix();
		MetaRenderer()->PushWorldMatrix();

		MetaRenderer()->BlurPass(CurrentRenderingFBO, &m_BlurFBO[0], 1.0f, false);
		MetaRenderer()->BlurPass(&m_BlurFBO[0], &m_BlurFBO[1], 1.0f, true);

		//Switch back to current rendering frame buffer
		MetaRenderer()->BindFrameBuffer(CurrentRenderingFBO);

		MetaRenderer()->PopProjectionMatrix();
		MetaRenderer()->PopWorldMatrix();

		//Note that BlurPass will modify the state of GL_DEPTH_TEST and GL_CULL_FACE, so we need to revert them
		glViewport(0, 0, CurrentRenderingFBO->iWidth, CurrentRenderingFBO->iHeight);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		int w{}, h{};
		GetSize(w, h);

		int absX{}, absY{};
		ipanel()->GetAbsPos(GetVPanel(), absX, absY);

		int screenW{}, screenH{};
		screenW = CurrentRenderingFBO->iWidth;
		screenH = CurrentRenderingFBO->iHeight;

		// 计算纹理坐标，只采样Panel所遮挡的区域
		// BlurFBO的尺寸是屏幕的一半
		float u0 = (float)absX / (float)screenW;
		float u1 = (float)(absX + w) / (float)screenW;
		// OpenGL纹理坐标原点在左下角，需要翻转v坐标
		float v0 = 1.0f - (float)absY / (float)screenH;
		float v1 = 1.0f - (float)(absY + h) / (float)screenH;

		Color BgColor = GetBgColor();
		vec4_t vColor4 = { BgColor.r() / 255.0f, BgColor.g() / 255.0f, BgColor.b() / 255.0f, BgColor.a() / 255.0f };

		// 使用DrawTexturedRect手动指定纹理坐标
		texturedrectvertex_t vertices[4];

		// 左下角 (屏幕坐标: 0, h)
		vertices[0].pos[0] = 0;
		vertices[0].pos[1] = h;
		vertices[0].texcoord[0] = u0;
		vertices[0].texcoord[1] = v1;  // 屏幕下方对应纹理的下方
		vertices[0].col[0] = vColor4[0];
		vertices[0].col[1] = vColor4[1];
		vertices[0].col[2] = vColor4[2];
		vertices[0].col[3] = vColor4[3];

		// 右下角 (屏幕坐标: w, h)
		vertices[1].pos[0] = w;
		vertices[1].pos[1] = h;
		vertices[1].texcoord[0] = u1;
		vertices[1].texcoord[1] = v1;  // 屏幕下方对应纹理的下方
		vertices[1].col[0] = vColor4[0];
		vertices[1].col[1] = vColor4[1];
		vertices[1].col[2] = vColor4[2];
		vertices[1].col[3] = vColor4[3];

		// 右上角 (屏幕坐标: w, 0)
		vertices[2].pos[0] = w;
		vertices[2].pos[1] = 0;
		vertices[2].texcoord[0] = u1;
		vertices[2].texcoord[1] = v0;  // 屏幕上方对应纹理的上方
		vertices[2].col[0] = vColor4[0];
		vertices[2].col[1] = vColor4[1];
		vertices[2].col[2] = vColor4[2];
		vertices[2].col[3] = vColor4[3];

		// 左上角 (屏幕坐标: 0, 0)
		vertices[3].pos[0] = 0;
		vertices[3].pos[1] = 0;
		vertices[3].texcoord[0] = u0;
		vertices[3].texcoord[1] = v0;  // 屏幕上方对应纹理的上方
		vertices[3].col[0] = vColor4[0];
		vertices[3].col[1] = vColor4[1];
		vertices[3].col[2] = vColor4[2];
		vertices[3].col[3] = vColor4[3];

		const uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

		MetaRenderer()->DrawTexturedRect(m_BlurFBO[1].s_hBackBufferTex, vertices, 4, indices, 6, DRAW_TEXTURED_RECT_ALPHA_BLEND_ENABLED, "GaussianBlurPanel::PaintBackground");
	}
}

//-----------------------------------------------------------------------------
// Purpose: Applies designer settings from res file
//-----------------------------------------------------------------------------
void GaussianBlurPanel::ApplySettings(KeyValues *inResourceData){
	BaseClass::ApplySettings(inResourceData);
	m_iBlurOffset = inResourceData->GetInt("offset", 5);
	m_iBlurIteration = inResourceData->GetInt("iteration", 3);
}