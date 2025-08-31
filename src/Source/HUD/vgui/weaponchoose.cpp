#include <metahook.h>

#include"local.h"

#include <vguilocal.h>
#include <map>
#include <string>
#include "cvardef.h"
#include "weaponinfo.h"
#include "weapon.h"
#include "weaponbank.h"

#include "hud.h"
#include "CCustomHud.h"

#include "vgui_controls/spr_image.h"
#include "vgui_controls/ImageSprPanel.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/AnimationController.h"
#include "weaponchoose.h"

extern const clientdata_t* gClientData;
static int g_iRainbowColorCounter = 0;
CWeaponChooseItem::CWeaponChooseItem(vgui::Panel* parent, WEAPON* wep) : BaseClass(parent, "WeaponItem") {
	m_pWeapon = wep;
	m_pWeaponPanel = new vgui::ImageSprPanel(this, "Weapon");
	m_pWeaponInactivePanel = new vgui::ImageSprPanel(this, "WeaponInactive");
	m_pAmmoBarBg1 = new vgui::Panel(this, "AmmoBarBg1");
	m_pAmmoBar1 = new vgui::Panel(this, "AmmoBar1");
	m_pAmmoBarBg2 = new vgui::Panel(this, "AmmoBarBg2");
	m_pAmmoBar2 = new vgui::Panel(this, "AmmoBar2");
	m_pWeaponPanel->SetVisible(false);
	ReloadWeaponSpr();
}
void CWeaponChooseItem::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("WeaponChoose.ItemBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));

	m_cFgColor = GetSchemeColor("WeaponChoose.ActiveColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	m_cInactiveColor = GetSchemeColor("WeaponChoose.InactiveColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);

	m_cEmptyColor = GetSchemeColor("WeaponChoose.EmptyColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	m_cEmptyInactiveColor = GetSchemeColor("WeaponChoose.EmptyInactiveColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);

	m_cAmmoBarBgColor = GetSchemeColor("WeaponChoose.AmmoBarBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme);
	m_cAmmoBarColor = GetSchemeColor("WeaponChoose.AmmoBarColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);
	m_cAmmoBarEmptyColor = GetSchemeColor("WeaponChoose.AmmoBarEmptyColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme);

	m_pAmmoBarBg1->SetBgColor(m_cAmmoBarBgColor);
	m_pAmmoBarBg2->SetBgColor(m_cAmmoBarBgColor);
	m_pAmmoBar1->SetBgColor(m_cAmmoBarColor);
	m_pAmmoBar2->SetBgColor(m_cAmmoBarColor);
}
void CWeaponChooseItem::PerformLayout(){
	BaseClass::PerformLayout();
	int w, h;
	GetSize(w, h);
	int abw = w / 10;
	int abx = w / 20;
	int abg = w / 30;
	int abh = h / 10;

	int x = abx;
	int y = 0;
	float ratio = clamp<float>(static_cast<float>(gWR.CountAmmo(m_pWeapon->iAmmoType)) / m_pWeapon->iMax1, 0.0f, 1.0f);
	m_pAmmoBarBg1->SetBounds(x, y, abw, abh);
	m_pAmmoBar1->SetBounds(x, y, abw * ratio, abh);
	
	x += abw + abg;
	ratio = clamp<float>(static_cast<float>(gWR.CountAmmo(m_pWeapon->iAmmo2Type)) / m_pWeapon->iMax2, 0.0f, 1.0f);;
	m_pAmmoBarBg2->SetBounds(x, y, abw, abh);
	m_pAmmoBar2->SetBounds(x, y, abw * ratio, abh);
}
void CWeaponChooseItem::SetBounds(int x, int y, int w, int h){
	SetPos(x, y);
	SetSize(w, h);
	m_pWeaponPanel->SetSize(w, h);
	m_pWeaponInactivePanel->SetSize(w, h);
}
void CWeaponChooseItem::ReloadWeaponSpr(){
	m_pWeaponInactivePanel->SetImage(m_pWeapon->hInactive);
	m_pWeaponInactivePanel->SetRect(m_pWeapon->rcInactive.left, m_pWeapon->rcInactive.right, m_pWeapon->rcInactive.top, m_pWeapon->rcInactive.bottom);
	m_pWeaponInactivePanel->SetRenderMode(kRenderTransAdd);
	m_pWeaponPanel->SetImage(m_pWeapon->hActive);
	m_pWeaponPanel->SetRect(m_pWeapon->rcActive.left, m_pWeapon->rcActive.right, m_pWeapon->rcActive.top, m_pWeapon->rcActive.bottom);
	m_pWeaponPanel->SetRenderMode(kRenderTransAdd);
}
void CWeaponChooseItem::UpdateColor(){
	static auto rainbow = [](Color& c) {
		if (gCVars.pAmmoMenuDrawRainbow->value > 0) {
			Vector temp = { (float)c.r() / 255.0f,(float)c.g() / 255.0f, (float)c.b() / 255.0f };
			RGBtoHSV(temp, temp);
			temp.x += g_iRainbowColorCounter * 20;
			temp.x = fmodf(temp.x, 360.0f);
			HSVtoRGB(temp, temp);
			temp *= 255.0f;
			c = Color(temp.x, temp.y, temp.z, c.a());
		}
	};
	Color ac = gWR.HasAmmo(m_pWeapon) ? m_cFgColor : m_cEmptyColor;
	rainbow(ac);
	m_pWeaponPanel->SetDrawColor(ac);
	Color ic = gWR.HasAmmo(m_pWeapon) ? m_cInactiveColor : m_cEmptyInactiveColor;
	rainbow(ic);
	m_pWeaponInactivePanel->SetDrawColor(ic);
	if (m_pWeapon->iAmmoType < 0) {
		m_pAmmoBarBg1->SetVisible(false);
		m_pAmmoBar1->SetVisible(false);
	}
	else {
		m_pAmmoBarBg1->SetVisible(true);
		m_pAmmoBar1->SetVisible(true);
		m_pAmmoBar1->SetBgColor(gWR.CountAmmo(m_pWeapon->iAmmoType) ? m_cAmmoBarColor : m_cAmmoBarEmptyColor);
	}
	if (m_pWeapon->iAmmo2Type < 0) {
		m_pAmmoBarBg2->SetVisible(false);
		m_pAmmoBar2->SetVisible(false);
	}
	else {
		m_pAmmoBarBg2->SetVisible(true);
		m_pAmmoBar2->SetVisible(true);
		m_pAmmoBar2->SetBgColor(gWR.CountAmmo(m_pWeapon->iAmmo2Type) ? m_cAmmoBarColor : m_cAmmoBarEmptyColor);
	}
}
void CWeaponChooseItem::SetActivate(bool state) {
	m_pWeaponPanel->SetVisible(state);
	m_pWeaponInactivePanel->SetVisible(!state);
}
WEAPON* CWeaponChooseItem::GetWeapon() {
	return m_pWeapon;
}

#define VIEWPORT_WEAPONCHOOSE_NAME "WeaponChoosePanel"
extern vgui::HScheme GetViewPortBaseScheme();
CWeaponChoosePanel::CWeaponChoosePanel()
	: BaseClass(nullptr, VIEWPORT_WEAPONCHOOSE_NAME) {

	gCVars.pAmmoMenuDrawRainbow = CREATE_CVAR("hud_rainbow_menu", "0", FCVAR_VALUE, NULL);

	m_arySlotPanel = {
		new vgui::ImagePanel(this,"Slot1"),
		new vgui::ImagePanel(this,"Slot2"),
		new vgui::ImagePanel(this,"Slot3"),
		new vgui::ImagePanel(this,"Slot4"),
		new vgui::ImagePanel(this,"Slot5"),
		new vgui::ImagePanel(this,"Slot6"),
		new vgui::ImagePanel(this,"Slot7"),
		new vgui::ImagePanel(this,"Slot8"),
		new vgui::ImagePanel(this,"Slot9"),
		new vgui::ImagePanel(this,"Slot0"),
	};
	m_pSelectBucket = new vgui::ImagePanel(this, "Bucket");
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());
	LoadControlSettings(VGUI2_ROOT_DIR "WeaponChoosePanel.res");
	SetVisible(false);
}
void CWeaponChoosePanel::ApplySettings(KeyValues* inResourceData){
	BaseClass::ApplySettings(inResourceData);
	m_iItemInactiveHeight = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetInt("item_inactive_height", 10));
	m_iItemInactiveWide = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetInt("item_inactive_wide", 10));
	m_iItemHeight = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetInt("item_height", 10));
	m_iItemWide = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetInt("item_wide", 30));
	m_iItemXGap = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetInt("item_xgap", 1));
	m_iItemYGap = vgui::scheme()->GetProportionalScaledValue(inResourceData->GetInt("item_ygap", 1));
	m_flCloseTime = inResourceData->GetFloat("close_time", 5.0f);
}
void CWeaponChoosePanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("WeaponChoose.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	Color temp = GetSchemeColor("WeaponChoose.SlotColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme);
	for (auto iter = m_arySlotPanel.begin(); iter != m_arySlotPanel.end(); iter++) {
		(*iter)->SetDrawColor(temp);
	}
	m_pSelectBucket->SetDrawColor(GetSchemeColor("WeaponChoose.BucketColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}
const char* CWeaponChoosePanel::GetName() {
	return VIEWPORT_WEAPONCHOOSE_NAME;
}
void CWeaponChoosePanel::Reset() {
	if (IsVisible())
		ShowPanel(false);
	for (auto iter1 = m_aryPanelList.begin(); iter1 != m_aryPanelList.end(); iter1++) {
		auto& list = *iter1;
		for (auto iter2 = list.begin(); iter2 != list.end(); iter2++) {
			(*iter2)->DeletePanel();
		}
		list.clear();
	}
	InvalidateLayout();
}
void CWeaponChoosePanel::ShowPanel(bool state) {
	if (state == IsVisible())
		return;
	if (state && !ShouldDraw())
		return;
	SetVisible(state);
	if (state) {
		vgui::GetAnimationController()->CancelAnimationsForPanel(GetParent());
		vgui::GetAnimationController()->StartAnimationSequence(GetParent(), "WeaponChooseIn");
		InvalidateLayout();
	}
}
bool CWeaponChoosePanel::IsVisible() {
	return BaseClass::IsVisible();
}
vgui::VPANEL CWeaponChoosePanel::GetVPanel() {
	return BaseClass::GetVPanel();
}
void CWeaponChoosePanel::SetParent(vgui::VPANEL parent) {
	BaseClass::SetParent(parent);
}
void CWeaponChoosePanel::PerformLayout(){
	BaseClass::PerformLayout();
	WEAPON* select = gWR.m_pNowSelected;
	int sslot = -1;
	if (select)
		sslot = select->iSlot;
	int x = 0;
	g_iRainbowColorCounter = 0;
	for (size_t i = 0; i < m_aryPanelList.size(); i++) {
		int y = 0;
		//���ƶ���ͼ��
		auto slot = m_arySlotPanel[i];
		slot->SetPos(x, y);
		y += slot->GetTall();
		auto& list = m_aryPanelList[i];
		//防止槽位为空时展开选择框
		bool ss = sslot >= 0 && sslot == i && gWR.HasWeapon(select); 
		for (auto iter2 = list.begin(); iter2 != list.end(); iter2++) {
			auto& item = *iter2;
			if (ss)
				item->SetBounds(x, y, m_iItemWide, m_iItemHeight);
			else
				item->SetBounds(x, y, m_iItemInactiveWide, m_iItemInactiveHeight);
			y += (ss ? m_iItemHeight : m_iItemInactiveHeight) + m_iItemYGap;
			item->UpdateColor();
			if (gCVars.pAmmoMenuDrawRainbow->value > 0)
				g_iRainbowColorCounter++;
		}
		x += (ss ? m_iItemWide : m_iItemInactiveWide) + m_iItemXGap;
	}
	if (m_pHandledWeapon) {
		int bx, by, bw, bh;
		m_pHandledWeapon.Get()->GetBounds(bx, by, bw, bh);
		m_pSelectBucket->SetSize(bw, bh);
		float xy[2] = { (float)bx,(float)by };
		vgui::GetAnimationController()->RunAnimationCommandEx(m_pSelectBucket, "position", xy, 2, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR, 0.0f);
	}
	m_flNextClosePanelTime = gEngfuncs.GetClientTime() + m_flCloseTime;
}
void CWeaponChoosePanel::OnThink(){
	if (gEngfuncs.GetClientTime() >= m_flNextClosePanelTime)
		ShowPanel(false);
}
void CWeaponChoosePanel::ReloadWeaponSpr(){
	for (auto iter1 = m_aryPanelList.begin(); iter1 != m_aryPanelList.end(); iter1++) {
		auto& list = *iter1;
		for (auto iter2 = list.begin(); iter2 != list.end(); iter2++) {
			(*iter2)->ReloadWeaponSpr();
		}
	}
}
bool CWeaponChoosePanel::ShouldDraw(){
	if (gCustomHud.IsInSpectate())
		return false;
	if (gCustomHud.IsHudHide(HUD_HIDEALL | HUD_HIDEWEAPONS))
		return false;
	if (!gCustomHud.HasSuit())
		return false;
	if (gClientData->health <= 0)
		return false;
	return true;
}
void CWeaponChoosePanel::SelectWeapon(){
	if (!IsVisible())
		return;
	if (CVAR_GET_FLOAT("hud_fastswitch") > 0)
		return;
	if (m_pHandledWeapon) {
		auto wep = reinterpret_cast<CWeaponChooseItem*>(m_pHandledWeapon.Get())->GetWeapon();
		if (!(wep->iFlags & ITEM_FLAG_SELECTONEMPTY) && !gWR.HasAmmo(wep))
			return;
		ServerCmd(wep->szName);
		PlaySoundByName("common/wpn_select.wav", 1);
		ShowPanel(false);
		m_bSelectBlock = true;
	}
}
void CWeaponChoosePanel::ChooseWeapon(WEAPON* weapon){
	ShowPanel(true);

	//如果选到空槽位，关闭上次的选择框和高亮spr
	if (!weapon || !gWR.HasWeapon(weapon)) {
		if (m_pHandledWeapon)
			reinterpret_cast<CWeaponChooseItem*>(m_pHandledWeapon.Get())->SetActivate(false);
		m_pSelectBucket->SetVisible(false);
		return;
	}

	m_pSelectBucket->SetVisible(true);

	for (auto iter1 = m_aryPanelList.begin(); iter1 != m_aryPanelList.end(); iter1++) {
		auto& list = *iter1;
		for (auto iter2 = list.begin(); iter2 != list.end(); iter2++) {
			auto& item = *iter2;
			if (item && item->GetWeapon() == weapon) {
				if(m_pHandledWeapon)
					reinterpret_cast<CWeaponChooseItem*>(m_pHandledWeapon.Get())->SetActivate(false);
				item->SetActivate(true);
				m_pHandledWeapon = item;
				InvalidateLayout();
				return;
			}
		}
	}
}
void CWeaponChoosePanel::InsertWeapon(WEAPON* weapon){
	size_t slot = weapon->iSlot;
	size_t pos = weapon->iSlotPos;
	if (slot < 0 || slot > 9)
		return;
	auto& list = m_aryPanelList[slot];
	for (auto iter = list.begin(); iter != list.end(); iter++) {
		auto item = (*iter)->GetWeapon();
		if (item == weapon || item->iId == weapon->iId)
			return;
		if (item->iSlotPos > pos) {
			list.insert(iter, new CWeaponChooseItem(this, weapon));
			InvalidateLayout();
			return;
		}
	}
	list.push_back(new CWeaponChooseItem(this, weapon));
}
void CWeaponChoosePanel::RemoveWeapon(WEAPON* weapon){
	int slot = weapon->iSlot;
	if (slot < 0 || slot > 9)
		return;
	auto& list = m_aryPanelList[slot];
	for (auto iter = list.begin(); iter != list.end(); iter++) {
		if ((*iter)->GetWeapon() == weapon) {
			(*iter)->DeletePanel();
			list.erase(iter);
			break;
		}
	}
	InvalidateLayout();
}
bool CWeaponChoosePanel::BlockAttackOnce() {
	if (m_bSelectBlock) {
		m_bSelectBlock = false;
		return true;
	}
	return false;
}