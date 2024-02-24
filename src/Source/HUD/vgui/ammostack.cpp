#include <metahook.h>

#include <vguilocal.h>
#include <wrect.h>

#include <map>
#include <string>
#include "cvardef.h"
#include "weaponinfo.h"
class WEAPON;
typedef int HSPRITE;
#include "weaponbank.h"

#include "vgui_controls/ImageSprPanel.h"
#include "vgui_controls/Label.h"

#include "ammostack.h"

#define VIEWPORT_AMMOSTACK_NAME "AmmoStackPanel"

CAmmoStackItem::CAmmoStackItem(Panel* parent, int iSpridx, int value, int l, int r, int t, int b, float expire, float fi, float fo)
	: BaseClass(parent, iSpridx, l, r, t, b, expire, fi, fo) {
	m_pText = new vgui::Label(this, "Text", std::to_string(value).c_str());
	iValue = value;
}
void CAmmoStackItem::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("AmmoStack.ListViewBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pPanel->SetDrawColor(GetSchemeColor("AmmoStack.IconColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
	m_pText->SetFgColor(GetSchemeColor("AmmoStack.TextColor", GetSchemeColor("Label.FgColor", pScheme), pScheme));
}
void CAmmoStackItem::PerformLayout() {
	BaseClass::PerformLayout();
	int w, h;
	GetSize(w, h);
	int img = min(w * 0.25, h);
	m_pPanel->SetSize(img, img);
	m_pText->SetPos(w * 0.25, 0);
	m_pText->SetSize(w * 0.75, h);
}
int CAmmoStackItem::GetValue() {
	return iValue;
}

extern vgui::HScheme GetViewPortBaseScheme();
CAmmoStackPanel::CAmmoStackPanel()
	: BaseClass(nullptr, VIEWPORT_AMMOSTACK_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());
	LoadControlSettings(VGUI2_ROOT_DIR "AmmoStackPanel.res");
	SetVisible(true);
}
const char* CAmmoStackPanel::GetName() {
	return VIEWPORT_AMMOSTACK_NAME;
}
void CAmmoStackPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("AmmoStack.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}
void CAmmoStackPanel::AddAmmoPickup(int id, int count){
	wrect_t rcPic;
	HSPRITE* spr = gWR.GetAmmoPicFromWeapon(id, rcPic);
	if (!spr)
		return;
	CAmmoStackItem* item = new CAmmoStackItem(this, *spr, count, rcPic.left, rcPic.right, rcPic.top, rcPic.bottom, 
		m_flKeepTime, m_flFadeinTime, m_flFadeoutTime);
	item->SetSize(m_iItemWide, m_iItemTall);
	m_aryPanels.push_back(item);
}
void CAmmoStackPanel::PaintBackground() {
	if (m_aryPanels.size() == 0)
		return;
	int w = GetWide();
	int y = GetTall();
	for (auto iter = m_aryPanels.rbegin(); iter != m_aryPanels.rend(); iter++) {
		auto item = *iter;
		if (item->IsVisible()) {
			y -= item->GetTall() + m_iStackGap;
			item->SetWide(w);
			item->SetPos(0, y);
		}
	}
	BaseClass::PaintBackground();
}
