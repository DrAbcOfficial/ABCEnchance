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
#include <vguilocal.h>


using namespace vgui;
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
GaussianBlurPanel::GaussianBlurPanel(Panel *parent, const char *name) : Panel(parent, name){
	glGenFramebuffers(1, &m_hBufferFBO);
	m_hBufferTex = GL_GenTextureRGBA8(ScreenWidth(), ScreenHeight());
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hBufferFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_hBufferTex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);
}

vgui::GaussianBlurPanel::~GaussianBlurPanel(){
	if (m_hBufferFBO)
		glDeleteFramebuffers(1, &m_hBufferFBO);
	if (m_hBufferTex)
		glDeleteTextures(1, &m_hBufferTex);
}


float vgui::GaussianBlurPanel::GetBlurRatio(){
	return m_flRatio;
}

void vgui::GaussianBlurPanel::SetBlurRatio(float f){
	m_flRatio = f;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void GaussianBlurPanel::PaintBackground(){
	int x, y, w, h;
	GetBounds(x, y, w, h);
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
	GL_BlitFrameBufferToFrameBufferColorOnly(m_oldFrameBuffer, m_hBufferFBO, ScreenWidth(), ScreenHeight(), ScreenWidth(), ScreenHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, m_hBufferFBO);
	
	DrawGaussianBlur(m_hBufferTex, m_flRatio, ScreenWidth(), ScreenHeight());

	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);

	glEnable(GL_TEXTURE_2D);
	glBind(m_hBufferTex);
	Color bgcolor = GetBgColor();
	glColor4ub(bgcolor.r(), bgcolor.g(), bgcolor.b(), bgcolor.a());

	int absx, absy;
	vgui::ipanel()->GetAbsPos(GetVPanel(), absx, absy);
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
}