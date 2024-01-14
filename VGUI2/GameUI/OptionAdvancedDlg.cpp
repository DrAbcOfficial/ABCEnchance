#include <GaussianBlurPanel.h>
#include <ModelViewPanel.h>

#include "OptionAdvancedDlg.h"

extern const char* CVAR_GET_STRING(const char* x);

COptionsAdvanceDlg::COptionsAdvanceDlg(vgui::Panel* parent) : BaseClass(parent, "OptionAdvancedDlg"){
	SetDeleteSelfOnClose(true);
	SetTitle("", true);
	SetCloseButtonVisible(false);
	SetMoveable(false);
	SetSizeable(false);
	SetProportional(true);
	SetKeyBoardInputEnabled(false);

	m_pBlur = new vgui::GaussianBlurPanel(this, "BlurPanel");
	m_pModelViewer = new vgui::ModelViewPanel(this, "ModelViewr");
	LoadControlSettings("abcenchance\\res\\gameui\\OptionAdvancedDlg.res");
	m_pModelViewer->SetupTexBuffer();
	ResetModel();
}

void COptionsAdvanceDlg::ResetModel(){
	char path[MAX_PATH] = {};
	const char* mdl = CVAR_GET_STRING("model");
	std::snprintf(path, MAX_PATH, "models/player/%s/%s.mdl", mdl, mdl);
	m_pModelViewer->ChangeModel(path);
}

void COptionsAdvanceDlg::PaintBackground(){
	//nothing
}

void COptionsAdvanceDlg::PaintBorder(){

}

void COptionsAdvanceDlg::Activate(){
	BaseClass::Activate();
}

void COptionsAdvanceDlg::ApplyChangesToConVar(const char* pConVarName, int value)
{
}

void COptionsAdvanceDlg::ApplyChanges(void)
{
}

void COptionsAdvanceDlg::OnResetData(void)
{
}

void COptionsAdvanceDlg::OnCommand(const char* command)
{
}

void COptionsAdvanceDlg::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	Color empty = Color(0, 0, 0, 0);
	SetBgColor(empty);
	SetFgColor(empty);
	m_pModelViewer->SetBgColor(empty);
	m_pBlur->SetBgColor(Color(255, 255, 255, 255));
}

void COptionsAdvanceDlg::OnTextChanged(vgui::Panel* panel)
{
}

void COptionsAdvanceDlg::OnGameUIHidden(void)
{
}

void COptionsAdvanceDlg::OK_Confirmed(void)
{
}
