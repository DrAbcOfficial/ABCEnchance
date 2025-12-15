#include <metahook.h>
#include <algorithm>

#include "vgui_controls/Panel.h"

#include "utility/util.h"
#include "spr_image.h"

using namespace vgui;

#undef clamp

CSPRImage::CSPRImage() {

}

CSPRImage::CSPRImage(const char *pFilePath) : CSPRImage(){
	LoadSprImage(pFilePath);
}

void CSPRImage::SetTextureID(int id) {
	m_iTextureID = id;
	if (id < 0)
		return;
	m_iAllFrames = SPR_Frames(m_iTextureID);
	if (m_nRight <= 0)
		m_nRight = gEngfuncs.pfnSPR_Width(m_iTextureID, 0);
	if (m_nBottom <= 0)
		m_nBottom = gEngfuncs.pfnSPR_Height(m_iTextureID, 0);
}
void CSPRImage::LoadSprImage(const char *pFilePath){
	m_szPath = pFilePath;
	SetTextureID(SPR_Load(m_szPath.c_str()));
}

void CSPRImage::Reset(){
	if(m_szPath.size())
		LoadSprImage(m_szPath.c_str());
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
			m_flFrame = (m_flFrame < 0) ? 0.0f : (float)(m_iAllFrames - 1);
	}
}

void CSPRImage::SetFrame(float flFrame){
	if (flFrame < 0.0f)
		m_flFrame = 0.0f;
	else if (m_iAllFrames > 0 && flFrame >= m_iAllFrames)
		m_flFrame = (float)(m_iAllFrames - 1);
	else
		m_flFrame = flFrame;
}

void CSPRImage::SetRect(int l, int r, int t, int b){
	m_nLeft = l;
	m_nRight = r;
	m_nTop = t;
	m_nBottom = b;
}

bool CSPRImage::IsInTheEnd(){
	return m_flFrame >= m_iAllFrames;
}

void CSPRImage::SetRenderMode(int mode){
	m_iRenderMode = mode;
}

void CSPRImage::Paint(){
	Animate();
	if (m_iTextureID != -1){
		float h = (float)gEngfuncs.pfnSPR_Height(m_iTextureID, (int)m_flFrame);
		float w = (float)gEngfuncs.pfnSPR_Width(m_iTextureID, (int)m_flFrame);
		rect_s rect = { m_nLeft, m_nRight, m_nTop, m_nBottom };
		float flLeft = static_cast<float>(rect.left) / w;
		float flRight = static_cast<float>(rect.right) / w;
		float flTop = static_cast<float>(rect.top) / h;
		float flBottom = static_cast<float>(rect.bottom) / h;
		int r, g, b, a;
		m_Color.GetColor(r, g, b, a);
		float fa = static_cast<float>(a);
		fa *= surface()->DrawGetAlphaMultiplier();
		fa = std::clamp(fa, 0.0f, 255.0f);

		extern void DrawSPRIconRect(int SprHandle, int mode, float x, float y, float w, float h, float left, float right, float top, float bottom,
			unsigned char r, unsigned char g, unsigned char b, unsigned char a, int frame);
		DrawSPRIconRect(m_iTextureID, m_iRenderMode, m_nX, m_nY, m_wide, m_tall, flLeft, flRight, flTop, flBottom, r, g, b, static_cast<byte>(fa), m_flFrame);
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
