#include <metahook.h>
#include <vguilocal.h>
#include "vgui_controls/ImageSprPanel.h"
#include "itemstack.h"

#define VIEWPORT_ITEMSTACK_NAME "ItemStackPanel"

CItemStackItem::CItemStackItem(vgui::Panel* parent, int spridx, int l, int r, int t, int b, float expire, float fi, float fo, float is) :
	BaseClass(parent, spridx, l, r, t, b, expire, fi, fo, is) {
	int w = vgui::scheme()->GetProportionalScaledValue(r - l) * is;
	int h = vgui::scheme()->GetProportionalScaledValue(b - t) * is;
	SetSize(w, h);
	m_pPanel->SetSize(w, h);
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
	SetVisible(true);
}
void CItemStackPanel::PaintBackground() {
	if (m_aryPanels.size() == 0)
		return;
	int y = GetTall();
	for (auto iter = m_aryPanels.rbegin(); iter != m_aryPanels.rend(); iter++) {
		auto item = *iter;
		if (item->IsVisible()) {
			y -= item->GetTall() + m_iStackGap;
			item->SetPos(0, y);
		}
	}
	BaseClass::PaintBackground();
}
const char* CItemStackPanel::GetName() {
	return VIEWPORT_ITEMSTACK_NAME;
}
void CItemStackPanel::AddItemPickup(int sprindex, int l, int r, int t, int b){
	CItemStackItem* item = new CItemStackItem(this, sprindex, l, r, t, b, m_flKeepTime, m_flFadeinTime, m_flFadeoutTime, m_flIconSize);
	m_aryPanels.push_back(item);
}
void CItemStackPanel::ApplySchemeSettings(vgui::IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	SetBgColor(GetSchemeColor("ItemStack.BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}