
#include "vgui_controls/CrossHairDisplay.h"

// memdbgon must be the last include file in a .cpp file!!!
//#include <tier0/memdbgon.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CrossHairDisplay::CrossHairDisplay(Panel *parent, const char *panelName) : Panel(parent, panelName){
	m_aryCrosshairBorder = {
		new vgui::Panel(this, "center"),
		new vgui::Panel(this, "top"),
		new vgui::Panel(this, "bottom"),
		new vgui::Panel(this, "left"),
		new vgui::Panel(this, "right")
	};
	m_aryCrosshair = {
		new vgui::Panel(this, "center"),
		new vgui::Panel(this, "top"),
		new vgui::Panel(this, "bottom"),
		new vgui::Panel(this, "left"),
		new vgui::Panel(this, "right")
	};
}

void vgui::CrossHairDisplay::SetDot(bool state){
	m_bHasDot = state;
}
void vgui::CrossHairDisplay::SetT(bool T){
	m_bT = T;
}
void vgui::CrossHairDisplay::SetOutline(bool o){
	m_bOutline = o;
}
void vgui::CrossHairDisplay::SetWidth(int w){
	m_iWidth = w;
}
void vgui::CrossHairDisplay::SetLength(int l){
	m_iLength = l;
}
void vgui::CrossHairDisplay::SetOutlineWidth(int o){
	m_iOutlineWidth = o;
}
void vgui::CrossHairDisplay::SetOffset(int o){
	m_iOffset = o;
}
void vgui::CrossHairDisplay::SetCrossHairColor(Color c){
	m_cCrosshair = c;
}
void vgui::CrossHairDisplay::SetOutlineColor(Color c){
	m_cOutline = c;
}
void vgui::CrossHairDisplay::PerformLayout() {
	int iWidth = m_iWidth;
	int iLength = m_iLength;
	int iOutLineWidth = m_iOutlineWidth;
	int iWidthOffset = iWidth / 2;
	int iFinalOffset = m_iOffset + iWidthOffset;
	//上色
	for (auto iter = m_aryCrosshair.begin() + 2; iter != m_aryCrosshair.end(); iter++) {
		(*iter)->SetBgColor(m_cCrosshair);
	}
	m_aryCrosshair[0]->SetBgColor((m_bHasDot) ? m_cCrosshair : Color(0, 0, 0, 0));
	m_aryCrosshair[1]->SetBgColor((m_bT) ? m_cCrosshair : Color(0, 0, 0, 0));

	int w, h;
	GetSize(w, h);
	int cx = w / 2;
	int cy = h / 2;
	//位置大小
	//顺序中心上下左右
	m_aryCrosshair[0]->SetBounds(cx - iWidthOffset, cy - iWidthOffset, iWidth, iWidth);
	m_aryCrosshair[1]->SetBounds(cx - iWidthOffset, cy - iFinalOffset - iLength, iWidth, iLength);
	m_aryCrosshair[2]->SetBounds(cx - iWidthOffset, cy + iFinalOffset, iWidth, iLength);
	m_aryCrosshair[3]->SetBounds(cx - iFinalOffset - iLength, cy - iWidthOffset, iLength, iWidth);
	m_aryCrosshair[4]->SetBounds(cx + iFinalOffset, cy - iWidthOffset, iLength, iWidth);
	//描边
	for (auto iter = m_aryCrosshairBorder.begin(); iter != m_aryCrosshairBorder.end(); iter++) {
		(*iter)->SetVisible(m_bOutline);
		(*iter)->SetBgColor(m_cOutline);
	}
	m_aryCrosshairBorder[0]->SetAlpha((m_bHasDot) ? m_cOutline.a() : 0);
	m_aryCrosshairBorder[1]->SetAlpha((!m_bT) ? m_cOutline.a() : 0);
	m_aryCrosshairBorder[0]->SetBounds(
		cx - iWidthOffset - iOutLineWidth,
		cy - iWidthOffset - iOutLineWidth,
		2 * iOutLineWidth + iWidth,
		2 * iOutLineWidth + iWidth
	);
	m_aryCrosshairBorder[1]->SetBounds(
		cx - iWidthOffset - iOutLineWidth,
		cy - iFinalOffset - iLength - iOutLineWidth,
		2 * iOutLineWidth + iWidth,
		2 * iOutLineWidth + iLength
	);
	m_aryCrosshairBorder[2]->SetBounds(
		cx - iWidthOffset - iOutLineWidth,
		cy + iFinalOffset - iOutLineWidth,
		2 * iOutLineWidth + iWidth,
		2 * iOutLineWidth + iLength
	);
	m_aryCrosshairBorder[3]->SetBounds(
		cx - iFinalOffset - iLength - iOutLineWidth,
		cy - iWidthOffset - iOutLineWidth,
		2 * iOutLineWidth + iLength,
		2 * iOutLineWidth + iWidth
	);
	m_aryCrosshairBorder[4]->SetBounds(
		cx + iFinalOffset - iOutLineWidth,
		cy - iWidthOffset - iOutLineWidth,
		2 * iOutLineWidth + iLength,
		2 * iOutLineWidth + iWidth
	);
}