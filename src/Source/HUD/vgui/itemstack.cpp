#include <metahook.h>
#include <vguilocal.h>
#include "vgui_controls/ImageSprPanel.h"
#include "itemstack.h"

#define VIEWPORT_ITEMSTACK_NAME "ItemStackPanel"

CItemStackItem::CItemStackItem(vgui::Panel* parent, int spridx, int l, int r, int t, int b, float expire, float fi, float fo) :
	BaseClass(parent, spridx, l, r, t, b, expire, fi, fo) {
}
void CItemStackItem::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("ItemStack.ListViewBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pPanel->SetDrawColor(GetSchemeColor("ItemStack.IconColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
}

extern vgui::HScheme GetViewPortBaseScheme();
CItemStackPanel::CItemStackPanel()
	: BaseClass(nullptr, VIEWPORT_ITEMSTACK_NAME) {
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme(GetViewPortBaseScheme());
	LoadControlSettings(VGUI2_ROOT_DIR "ItemStackPanel.res");
	CalculateMaxItemCount();
}
const char* CItemStackPanel::GetName() {
	return VIEWPORT_ITEMSTACK_NAME;
}
void CItemStackPanel::AddItemPickup(int sprindex, int l, int r, int t, int b){
	CItemStackItem* item = new CItemStackItem(this, sprindex, l, r, t, b, m_flKeepTime, m_flFadeinTime, m_flFadeoutTime);
	item->SetSize(m_iItemWide, m_iItemTall);
	m_aryPanels.push_back(item);
	if (m_aryPanels.size() > m_iMaxItem) {
		m_aryPanels.front()->DeletePanel();
		m_aryPanels.pop_front();
	}
	InvalidateLayout();
		
}
void CItemStackPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("ItemStack.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}