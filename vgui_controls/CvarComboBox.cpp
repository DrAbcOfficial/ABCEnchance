#include <algorithm>
#include "cvardef.h"

#include "CvarComboBox.h"

using namespace vgui;

extern float CVAR_GET_FLOAT(const char* x);
extern void CVAR_SET_FLOAT(const char* x, float v);
extern cvar_t* CVAR_GET_POINTER(const char* x);

CCvarComboBox::CCvarComboBox(Panel *parent, const char *panelName, const char *text, int maxitem, char const *cvarname) : BaseClass(parent, panelName){
	SetMouseInputEnabled(true);
	SetProportional(true);
	if (cvarname){
		Q_strncpy(m_szCvarName, cvarname, 63);
		m_szCvarName[63] = 0;
	}
	else
		m_szCvarName[0] = 0;
	m_pComboBox = new ComboBox(this, "ComboBox", maxitem, false);
	m_pLabel = new Label(this, "Label", text);
	Reset();
	AddActionSignalTarget(this);
}

CCvarComboBox::~CCvarComboBox(void){
	m_szCvarName[0] = 0;
}

void CCvarComboBox::ApplyChanges(void)
{
	if (!m_szCvarName[0])
		return;
	auto active = m_pComboBox->GetActiveItemUserData();
	if (active)
		CVAR_SET_FLOAT(m_szCvarName, active->GetFloat("value"));
	else {
		int item = m_pComboBox->GetActiveItem();
		if (item >= 0) 
			CVAR_SET_FLOAT(m_szCvarName, item);
	}
}

void CCvarComboBox::Reset(void){
	if (!m_szCvarName[0])
		return;

	auto cvar = CVAR_GET_POINTER(m_szCvarName);
	if (!cvar){
		SetEnabled(false);
		return;
	}
	float value = cvar->value;
	int index = -1;
	for (int i = 0; i < m_pComboBox->GetItemCount(); i++) {
		auto kv = m_pComboBox->GetItemUserData(i);
		if (kv) {
			float kvvalue = kv->GetFloat("value");
			if (value == kvvalue) {
				index = i;
				break;
			}
		}
	}
	if(index < 0)
		index = std::clamp<int>(value, 0, m_pComboBox->GetItemCount() - 1);
	m_iStartValue = index;
	m_pComboBox->ActivateItem(index);
}

const char* CCvarComboBox::GetCvarName(void){
	return m_szCvarName;
}

int CCvarComboBox::AddItem(const char* itemText, const KeyValues* userData){
	return m_pComboBox->AddItem(itemText, userData);
}

int CCvarComboBox::AddItem(const wchar_t* itemText, const KeyValues* userData){
	return m_pComboBox->AddItem(itemText,userData);
}

void CCvarComboBox::SetSelected(int select){
	m_iStartValue = std::clamp<int>(CVAR_GET_FLOAT(m_szCvarName), 0, m_pComboBox->GetItemCount() - 1);
	m_pComboBox->ActivateItem(m_iStartValue);
	PostActionSignal(new KeyValues("SelectChange", "select", select));
}

void CCvarComboBox::PerformLayout(){
	BaseClass::PerformLayout();
	int w, h;
	GetSize(w, h);
	m_pLabel->SetSize(w * 0.3333, h);
	m_pComboBox->SetSize(w * 0.6666, h);
	m_pComboBox->SetPos(w * 0.3333, 0);
	Reset();
}
