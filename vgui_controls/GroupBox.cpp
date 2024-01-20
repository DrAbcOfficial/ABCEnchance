#include "vgui_controls/Label.h"

#include "GroupBox.h"

using namespace vgui;

GroupBox::GroupBox(Panel* parent, const char* panelName, const char* text, int size) : BaseClass(parent, panelName){
	m_pLabel = new Label(this, "Title", text);
	m_iSize = size;
}
void GroupBox::PerformLayout(){
	BaseClass::PerformLayout();
	int w, h;
	GetSize(w, h);
	m_pLabel->SetSize(w, h / (m_iSize + 1));
	m_pLabel->SetPos(0, 0);
}

void GroupBox::ApplySchemeSettings(IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	SetBorder(pScheme->GetBorder("GroupBox/Border"));
	SetBgColor(GetSchemeColor("GroupBox/BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	SetFgColor(GetSchemeColor("GroupBox/FgColor", GetSchemeColor("Panel.FgColor", pScheme), pScheme));
}
