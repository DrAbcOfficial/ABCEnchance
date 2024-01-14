#include "vgui_controls/PropertySheet.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/Slider.h"

#include <GaussianBlurPanel.h>
#include <ModelViewPanel.h>

#include "OptionAdvancedDlg.h"

extern const char* CVAR_GET_STRING(const char* x);

COptionsAdvanceSubMultiPlay::COptionsAdvanceSubMultiPlay(vgui::Panel* parent) : BaseClass(parent, "OptionsAdvanceSubMultiPlay"){
	m_pModelViewer = new vgui::ModelViewPanel(this, "ModelViewer");
	m_pModelController = new vgui::Slider(this, "ModelController");
	m_pPlayerName = new vgui::Label(this, "PlayerName", CVAR_GET_STRING("name"));

	LoadControlSettings("abcenchance\\res\\gameui\\OptionsAdvanceSubMultiPlay.res");
	m_pModelViewer->SetupTexBuffer();
	ResetModel();
}
void COptionsAdvanceSubMultiPlay::ResetModel(){
	char path[MAX_PATH] = {};
	const char* mdl = CVAR_GET_STRING("model");
	std::snprintf(path, MAX_PATH, "models/player/%s/%s.mdl", mdl, mdl);
	m_pModelViewer->ChangeModel(path);
}
void COptionsAdvanceSubMultiPlay::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	Color empty = Color(0, 0, 0, 0);
	m_pModelViewer->SetBgColor(empty);
}

COptionsAdvanceDialog::COptionsAdvanceDialog(vgui::Panel* parent) : BaseClass(parent, "OptionsAdvanceDialog") {
	SetDeleteSelfOnClose(true);

	m_pBlur = new vgui::GaussianBlurPanel(this, "BlurPanel");
	LoadControlSettings("abcenchance\\res\\gameui\\OptionsAdvanceDialog.res");
	auto sheet = GetPropertySheet();
	m_pMultiPlayPage = new COptionsAdvanceSubMultiPlay(this);
	AddPage(m_pMultiPlayPage, "#GameUI_ABC_MultiPlayPage");
	SetApplyButtonVisible(true);
}
void COptionsAdvanceDialog::Activate(){
	BaseClass::Activate();
	vgui::input()->SetAppModalSurface(GetVPanel());
}
void COptionsAdvanceDialog::OnCommand(const char* command){
	if (!stricmp(command, "OK")){
		ApplyChanges();
		Close();
	}
	else if (!stricmp(command, "Apply"))
		ApplyChanges();
	else
		BaseClass::OnCommand(command);
}
void COptionsAdvanceDialog::ApplySettings(KeyValues* inResourceData){
	BaseClass::ApplySettings(inResourceData);
}
void COptionsAdvanceDialog::PaintBackground(){
}
void COptionsAdvanceDialog::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	auto sheet = GetPropertySheet();
	Color empty = Color(0, 0, 0, 0);
	SetBgColor(empty);
	SetBorder(nullptr);
	m_pMultiPlayPage->SetBgColor(empty);
	m_pMultiPlayPage->SetBorder(nullptr);
	sheet->SetBgColor(empty);
	sheet->SetBorder(nullptr);
	m_pBlur->SetBgColor(Color(255, 255, 255, 255));
}
