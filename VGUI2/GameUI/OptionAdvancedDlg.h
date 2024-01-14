#ifndef OPTIONADVANCEDLG_H
#define OPTIONADVANCEDLG_H

#pragma once
#include "vgui_controls/Frame.h"

namespace vgui {
	class GaussianBlurPanel;
	class ModelViewPanel;
}
class COptionsAdvanceDlg : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(COptionsAdvanceDlg, vgui::Frame);
public:
	COptionsAdvanceDlg(vgui::Panel* parent);
	void ResetModel();
	
	virtual void PaintBackground();
	virtual void PaintBorder() override;
	virtual void Activate() override;
	MESSAGE_FUNC_PTR(OnTextChanged, "TextChanged", panel);
	MESSAGE_FUNC(OnGameUIHidden, "GameUIHidden");
	MESSAGE_FUNC(OK_Confirmed, "OK_Confirmed");
	void ApplyChangesToConVar(const char* pConVarName, int value);
	virtual void ApplyChanges();
	virtual void OnResetData();
	virtual void OnCommand(const char* command) override;
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
private:
	vgui::GaussianBlurPanel* m_pBlur;
	vgui::ModelViewPanel* m_pModelViewer;
};


#endif // !OPTIONADVANCEDLG_H