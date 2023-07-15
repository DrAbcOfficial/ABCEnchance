#include <metahook.h>
#include <triangleapi.h>

#include <vgui2/SurfaceHook.h>

#include "local.h"
#include "spr_image.h"

CSPRImage::CSPRImage(const char *pFilePath){
	LoadSprImage(pFilePath);
}

void CSPRImage::LoadSprImage(const char *pFilePath){
	m_iTextureID = SPR_Load(pFilePath);
	m_iAllFrames = SPR_Frames(m_iTextureID);
	strcpy(m_szPath, pFilePath);
}

void CSPRImage::Reset(){
	if(m_szPath[0] != '\0')
		LoadSprImage(m_szPath);
	m_flAnimeTime = 0;
}

void CSPRImage::SetFramerate(float flTime){
	m_flFrameRate = flTime;
}

void CSPRImage::SetLoop(bool loop){
	m_bSequenceLoop = loop;
}

void CSPRImage::Animate(){
	float flTime = ClientTime();
	float flInterval = (flTime - m_flAnimeTime);
	if (flInterval <= 0.001){
		m_flAnimeTime = flTime;
		return;
	}
	if (m_flAnimeTime <= 0)
		flInterval = 0.0;

	m_flFrame += flInterval * m_flFrameRate;
	m_flAnimeTime = flTime;

	if (m_flFrame < 0.0 || m_flFrame >= m_iAllFrames){
		if (m_bSequenceLoop)
			m_flFrame -= (int)(m_flFrame / m_iAllFrames) * m_iAllFrames;
		else
			m_flFrame = (m_flFrame < 0.0) ? 0 : 255;
	}
}

void CSPRImage::SetRect(int l, int r, int t, int b){
	m_nLeft = l;
	m_nRight = r;
	m_nTop = t;
	m_nBottom = b;
}

void CSPRImage::SetRenderMode(int mode){
	m_iRenderMode = mode;
}

void CSPRImage::Paint(){
	if (m_iTextureID != -1){
		rect_s rect = { m_nLeft, m_nRight, m_nTop, m_nBottom };
		float flLeft = static_cast<float>(rect.left) / m_wide;
		float flRight = static_cast<float>(rect.right) / m_wide;
		float flTop = static_cast<float>(rect.top) / m_tall;
		float flBottom = static_cast<float>(rect.bottom) / m_tall;
		int r, g, b, a;
		m_Color.GetColor(r, g, b, a);
		gEngfuncs.pTriAPI->SpriteTexture(const_cast<struct model_s*>(gEngfuncs.GetSpritePointer(m_iTextureID, m_iTextureID)), m_flFrame);
		gEngfuncs.pTriAPI->RenderMode(m_iRenderMode);
		gEngfuncs.pTriAPI->CullFace(TRI_NONE);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		gEngfuncs.pTriAPI->Color4ub(r, g, b, a);
		gEngfuncs.pTriAPI->Brightness(1);
		gEngfuncs.pTriAPI->TexCoord2f(flLeft, flTop);
		gEngfuncs.pTriAPI->Vertex3f(m_nX, m_nY, 0);
		gEngfuncs.pTriAPI->Brightness(1);
		gEngfuncs.pTriAPI->TexCoord2f(flLeft, flBottom);
		gEngfuncs.pTriAPI->Vertex3f(m_nX, m_nY + m_tall, 0);
		gEngfuncs.pTriAPI->Brightness(1);
		gEngfuncs.pTriAPI->TexCoord2f(flRight, flBottom);
		gEngfuncs.pTriAPI->Vertex3f(m_nX + m_wide, m_nY + m_tall, 0);
		gEngfuncs.pTriAPI->Brightness(1);
		gEngfuncs.pTriAPI->TexCoord2f(flRight, flTop);
		gEngfuncs.pTriAPI->Vertex3f(m_nX + m_wide, m_nY, 0);
		gEngfuncs.pTriAPI->End();
		gEngfuncs.pTriAPI->CullFace(TRI_FRONT);
		gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
	}
}

void CSPRImage::SetPos(int x, int y)
{
	m_nX = x;
	m_nY = y;
}

void CSPRImage::GetContentSize(int &wide, int &tall)
{
	wide = m_wide;
	tall = m_tall;
}

void CSPRImage::GetSize(int &wide, int &tall){
	GetContentSize(wide, tall);
}

void CSPRImage::SetSize(int wide, int tall){
	m_wide = wide;
	m_tall = tall;
}

void CSPRImage::SetColor(Color col){
	m_Color = col;
}
