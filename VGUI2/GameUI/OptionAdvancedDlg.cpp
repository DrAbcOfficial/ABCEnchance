#include "interface.h"
#include "IFileSystem.h"

#include "vgui_controls/PropertySheet.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/Slider.h"
#include "vgui_controls/ListPanel.h"
#include "vgui_controls/TextEntry.h"

#include <GaussianBlurPanel.h>
#include <ModelViewPanel.h>

#include "OptionAdvancedDlg.h"

extern const char* CVAR_GET_STRING(const char* x);

COptionsAdvanceSubMultiPlay::COptionsAdvanceSubMultiPlay(vgui::Panel* parent) : BaseClass(parent, "OptionsAdvanceSubMultiPlay"){
	m_pModelViewer = new vgui::ModelViewPanel(this, "ModelViewer");
	m_pModelController = new vgui::Slider(this, "ModelController");
	m_pModelList = new vgui::ListPanel(this, "ModelList");
	m_pModelFilter = new vgui::TextEntry(this, "ModelFilter");
	m_pPlayerName = new vgui::Label(this, "PlayerName", CVAR_GET_STRING("name"));
	LoadControlSettings("abcenchance/res/gameui/OptionsAdvanceSubMultiPlay.res");

	float y, r;
	m_pModelViewer->SetupTexBuffer();
	m_pModelViewer->GetModelRotate(r, y, r);

	m_pModelController->SetRange(0, 360);
	m_pModelController->AddActionSignalTarget(this);
	m_pModelController->SetValue(y, true);

	m_pModelList->AddColumnHeader(0, "modelname", "#GameUI_ABC_Model", 0, vgui::ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW);
	m_pModelList->AddActionSignalTarget(this);
	m_pModelList->SetMultiselectEnabled(false);

	m_pModelFilter->AddActionSignalTarget(this);
	m_pModelFilter->RequestFocus();

	ResetModel();
	BuildModelList();
}
void COptionsAdvanceSubMultiPlay::ResetModel(){
	ChangeModel(CVAR_GET_STRING("model"));
}
void COptionsAdvanceSubMultiPlay::ChangeModel(const char* mdl){
	char path[MAX_PATH] = {};
	std::snprintf(path, MAX_PATH, "models/player/%s/%s.mdl", mdl, mdl);
	m_pModelViewer->ChangeModel(path);
}
void COptionsAdvanceSubMultiPlay::BuildModelList(const char* filter){
	FileFindHandle_t findHandle;
	auto pszFileName = vgui::filesystem()->FindFirst("models/player/*.*", &findHandle);
	if (!pszFileName)
		return;
	if (pszFileName){
		do{
			if (vgui::filesystem()->FindIsDirectory(findHandle) && *pszFileName != '.') {
				std::string str = pszFileName;
				if(!filter || str.find(filter) != std::string::npos)
					m_aryModelList.insert(str);
			}
		} while ((pszFileName = vgui::filesystem()->FindNext(findHandle)) != nullptr);
	}
	vgui::filesystem()->FindClose(findHandle);

	size_t counter = 0;
	size_t plr = 0;
	const char* playermdl = CVAR_GET_STRING("model");
	for (auto iter = m_aryModelList.begin(); iter != m_aryModelList.end(); iter++) {
		const char* mdl = iter->c_str();
		if (!std::strcmp(mdl, playermdl))
			plr = counter;
		counter++;
		KeyValues kv(mdl, "modelname", mdl);
		m_pModelList->AddItem(&kv, 0, plr == counter, false);
	}
	m_pModelList->SetSingleSelectedItem(plr);
}

void COptionsAdvanceSubMultiPlay::OnSliderMoved() {
	m_pModelViewer->SetModelRotate(0, m_pModelController->GetValue(), 0);
}

void COptionsAdvanceSubMultiPlay::OnItemSelected() {
	ChangeModel(m_pModelList->GetItem(m_pModelList->GetSelectedItem(0))->GetName());
}

void COptionsAdvanceSubMultiPlay::ApplySchemeSettings(vgui::IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	Color empty = Color(0, 0, 0, 0);
	m_pModelViewer->SetBgColor(empty);
}

COptionsAdvanceDialog::COptionsAdvanceDialog(vgui::Panel* parent) : BaseClass(parent, "OptionsAdvanceDialog") {
	SetDeleteSelfOnClose(true);
	SetSizeable(false);
	SetProportional(true);

	m_pBlur = new vgui::GaussianBlurPanel(this, "BlurPanel");
	LoadControlSettings("abcenchance/res/gameui/OptionsAdvanceDialog.res");
	m_pMultiPlayPage = new COptionsAdvanceSubMultiPlay(this);
	AddPage(m_pMultiPlayPage, "#GameUI_ABC_MultiPlayPage");
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
