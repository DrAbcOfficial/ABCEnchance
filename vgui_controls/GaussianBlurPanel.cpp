//========= Copyright ?1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "GaussianBlurPanel.h"
#include <glew.h>
#include <gl_utility.h>

using namespace vgui;
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
GaussianBlurPanel::GaussianBlurPanel(Panel *parent, const char *name) : Panel(parent, name){
}

vgui::GaussianBlurPanel::~GaussianBlurPanel(){
	if (m_hBufferFBO)
		glDeleteFramebuffers(1, &m_hBufferFBO);
	if (m_hBufferTex)
		glDeleteTextures(1, &m_hBufferTex);
}
void vgui::GaussianBlurPanel::SetupTexture(){
	if (!m_hBufferFBO && !m_hBufferTex) {
		glGenFramebuffers(1, &m_hBufferFBO);
		int w, h;
		GetSize(w, h);
		m_hBufferTex = GL_GenTextureRGBA8(w, h);
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_hBufferFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_hBufferTex, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void GaussianBlurPanel::PaintBackground(){
	int w, h;
	GetSize(w, h);
	int x, y;
	vgui::ipanel()->GetAbsPos(GetVPanel(), x, y);
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hBufferFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_hBufferFBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_oldFrameBuffer);
	glBlitFramebuffer(x, y, x + w, x + h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

//-----------------------------------------------------------------------------
// Purpose: Applies designer settings from res file
//-----------------------------------------------------------------------------
void GaussianBlurPanel::ApplySettings(KeyValues *inResourceData){
	BaseClass::ApplySettings(inResourceData);

}