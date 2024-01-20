#include "CvarLabelSlider.h"
#include "Label.h"

using namespace vgui;

DECLARE_BUILD_FACTORY(CCvarLabelSlider);

CCvarLabelSlider::CCvarLabelSlider(Panel *parent, const char *name) : BaseClass(parent, name){
	m_pLabel = new Label(this, "CvarLabel", "");
	m_pSlider = new CCvarSlider(parent, "CvarSlider");
}

CCvarLabelSlider::CCvarLabelSlider(Panel *parent, const char *panelName, const char* labelcontent, const char *caption, float minValue, float maxValue, char const *cvarname, bool bAllowOutOfRange) : 
	BaseClass(parent, panelName) {
	m_pLabel = new Label(this, "CvarLabel", labelcontent);
	m_pSlider = new CCvarSlider(parent, "CvarSlider", caption, minValue, maxValue, cvarname, bAllowOutOfRange);
}

void vgui::CCvarLabelSlider::SetupSlider(float minValue, float maxValue, const char* cvarname, bool bAllowOutOfRange){
	m_pSlider->SetupSlider(minValue, maxValue, cvarname, bAllowOutOfRange);
}

void vgui::CCvarLabelSlider::SetCVarName(char const* cvarname){
	m_pSlider->SetCVarName(cvarname);
}

void vgui::CCvarLabelSlider::SetMinMaxValues(float minValue, float maxValue, bool bSetTickdisplay){
	m_pSlider->SetMinMaxValues(minValue, maxValue, bSetTickdisplay);
}

void vgui::CCvarLabelSlider::SetTickColor(Color color){
	m_pSlider->SetTickColor(color);
}

void CCvarLabelSlider::PerformLayout(void){
	BaseClass::PerformLayout();
	int w = GetWide();
	m_pLabel->SetWide(w / 10);
	m_pSlider->SetWide(w / 10 * 9);
	m_pSlider->SetPos(w / 10, 0);
}

void vgui::CCvarLabelSlider::ApplyChanges(void){
	m_pSlider->ApplyChanges();
}

float vgui::CCvarLabelSlider::GetSliderValue(void){
	return m_pSlider->GetSliderValue();
}

void vgui::CCvarLabelSlider::SetSliderValue(float fValue){
	m_pSlider->SetSliderValue(fValue);
}

void vgui::CCvarLabelSlider::Reset(void){
	m_pSlider->Reset();
}

bool vgui::CCvarLabelSlider::HasBeenModified(void){
	return m_pSlider->HasBeenModified();
}

void CCvarLabelSlider::OnSliderMoved() {
	m_pSlider->OnSliderMoved();
}

void CCvarLabelSlider::OnApplyChanges() {
	m_pSlider->OnApplyChanges();
}
