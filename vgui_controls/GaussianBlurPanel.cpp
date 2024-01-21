//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "GaussianBlurPanel.h"
#include <glew.h>
#include <gl_draw.h>
#include <gl_utility.h>
#include <gl_shader.h>
#include <gl_def.h>

extern size_t ScreenWidth();
extern size_t ScreenHeight();

using namespace vgui;
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
GaussianBlurPanel::GaussianBlurPanel(Panel *parent, const char *name) : Panel(parent, name){
	glGenFramebuffers(1, &m_hBufferFBO);
	m_hBufferTex = GL_GenTextureRGBA8(ScreenWidth() / 2, ScreenHeight() / 2);
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hBufferFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_hBufferTex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);
}

GaussianBlurPanel::~GaussianBlurPanel(){
	if (m_hBufferFBO)
		glDeleteFramebuffers(1, &m_hBufferFBO);
	if (m_hBufferTex)
		glDeleteTextures(1, &m_hBufferTex);
}


float GaussianBlurPanel::GetBlurRatio(){
	return m_flRatio;
}

void GaussianBlurPanel::SetBlurRatio(float f){
	m_flRatio = f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void GaussianBlurPanel::PaintBackground(){
	int hw = ScreenWidth() / 2;
	int hh = ScreenHeight() / 2;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
	GL_BlitFrameBufferToFrameBufferColorOnly(m_oldFrameBuffer, m_hBufferFBO, ScreenWidth(), ScreenHeight(), hw, hh);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hBufferFBO);

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, hw, hh);
	for (size_t i = 0; i < m_iBlurness; i++) {
		glEnable(GL_TEXTURE_2D);
		glBind(m_hBufferTex);
		GL_UseProgram(pp_gaussianblur.program);
		GL_Uniform1f(pp_gaussianblur.du, m_flRatio);
		GL_Uniform2f(pp_gaussianblur.res, hw, hh);
		glColor4ub(255, 255, 255, 255);
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0);
		glVertex2i(0, 0);
		glTexCoord2i(0, 1);
		glVertex2i(0, ScreenHeight());
		glTexCoord2i(1, 1);
		glVertex2i(ScreenWidth(), ScreenHeight());
		glTexCoord2i(1, 0);
		glVertex2i(ScreenWidth(), 0);
		glEnd();
		GL_UseProgram(0);
		glDisable(GL_TEXTURE_2D);
	}
	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);

	glEnable(GL_TEXTURE_2D);
	glBind(m_hBufferTex);
	Color bgcolor = GetBgColor();
	glColor4ub(bgcolor.r(), bgcolor.g(), bgcolor.b(), bgcolor.a());

	int x, y, w, h;
	GetBounds(x, y, w, h);
	int absx, absy;
	ipanel()->GetAbsPos(GetVPanel(), absx, absy);
	absy = ScreenHeight() - absy - h;
	float rx = (float)absx / ScreenWidth();
	float ry = (float)absy / ScreenHeight();
	float rw = (float)w / ScreenWidth();
	float rh = (float)h / ScreenHeight();
	glBegin(GL_QUADS);
	glTexCoord2f(rx, ry);
	glVertex2f(x, y + h);
	glTexCoord2f(rx, ry + rh);
	glVertex2f(x, y);
	glTexCoord2f(rx + rw, ry + rh);
	glVertex2f(x + w, y);
	glTexCoord2f(rx + rw, ry);
	glVertex2f(x + w, y + h);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

//-----------------------------------------------------------------------------
// Purpose: Applies designer settings from res file
//-----------------------------------------------------------------------------
void GaussianBlurPanel::ApplySettings(KeyValues *inResourceData){
	BaseClass::ApplySettings(inResourceData);
	m_flRatio = inResourceData->GetFloat("ratio", 1.0f);
	m_iBlurness = inResourceData->GetInt("blurness", 16);
}