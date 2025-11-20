#include <metahook.h>

#include <map>
#include <string>
#include "cvardef.h"
#include "weaponinfo.h"

#include "utility/vgui_util.h"
#include "core/resource/weaponresource.h"

#include "vgui_controls/ImageSprPanel.h"
#include "weaponstack.h"

#define VIEWPORT_WEAPONSTACK_NAME "WeaponStackPanel"

CWeaponStackItem::CWeaponStackItem(vgui::Panel* parent, int spridx, int l, int r, int t, int b, float expire, float fi, float fo, size_t wi) :
	BaseClass(parent, spridx, l, r, t, b, expire, fi, fo) {
	m_iWeapon = wi;
}
void CWeaponStackItem::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("WeaponStack.ListViewBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_cEmpty = GetSchemeColor("WeaponStack.IconEmptyColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	m_cDraw = GetSchemeColor("WeaponStack.IconColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	m_pPanel->SetDrawColor(m_cDraw);
}

void CWeaponStackItem::CheckExpire(){
	BaseClass::CheckExpire();
	auto weapon = gWR.GetWeapon(m_iWeapon);
	if (weapon == nullptr)
		return;
	if(gWR.HasAmmo(weapon))
		m_pPanel->SetDrawColor(m_cDraw);
	else
		m_pPanel->SetDrawColor(m_cEmpty);
}
void CWeaponStackItem::ForceReload() {
	auto weapon = gWR.GetWeapon(m_iWeapon);
	if (weapon == nullptr)
		return;
	m_pPanel->SetImage(weapon->hInactive);
	m_pPanel->SetRect(weapon->rcInactive.left, weapon->rcInactive.right, weapon->rcInactive.top, weapon->rcInactive.bottom);
	m_pPanel->SetRenderMode(kRenderTransAdd);
	iSprIdx = weapon->hInactive;
}

extern vgui::HScheme GetViewPortBaseScheme();
CWeaponStackPanel::CWeaponStackPanel()
	: BaseClass(nullptr, VIEWPORT_WEAPONSTACK_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());
	LoadControlSettings(VGUI2_ROOT_DIR "WeaponStackPanel.res");
	CalculateMaxItemCount();
}
const char* CWeaponStackPanel::GetName() {
	return VIEWPORT_WEAPONSTACK_NAME;
}
void CWeaponStackPanel::AddItemPickup(int wepindex){
	auto weapon = gWR.GetWeapon(wepindex);
	if (weapon == nullptr)
		return;
	CWeaponStackItem* item = new CWeaponStackItem(this, weapon->hInactive, 
		weapon->rcInactive.left, weapon->rcInactive.right, weapon->rcInactive.top, weapon->rcInactive.bottom, 
		m_flKeepTime, m_flFadeinTime, m_flFadeoutTime, wepindex);
	item->SetSize(m_iItemWide, m_iItemTall);
	m_aryPanels.push_back(item);
	if (m_aryPanels.size() > m_iMaxItem) {
		m_aryPanels.front()->DeletePanel();
		m_aryPanels.pop_front();
	}
	InvalidateLayout();
}
void CWeaponStackPanel::ReloadWeaponSpr(){
	for (auto iter = m_aryPanels.begin(); iter != m_aryPanels.end(); iter++) {
		auto item = reinterpret_cast<CWeaponStackItem*>(*iter);
		item->ForceReload();
	}
}
void CWeaponStackPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("WeaponStack.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}