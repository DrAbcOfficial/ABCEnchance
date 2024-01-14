#ifndef OPTIONADVANCEDLG_H
#define OPTIONADVANCEDLG_H

#pragma once
#include "vgui_controls/PropertyPage.h"
#include "vgui_controls/PropertyDialog.h"

namespace vgui {
	class GaussianBlurPanel;
	class ModelViewPanel;
	class TextEntry;
	class Slider;
	class Label;
}

class COptionsAdvanceSubMultiPlay : public vgui::PropertyPage
{
	DECLARE_CLASS_SIMPLE(COptionsAdvanceSubMultiPlay, vgui::PropertyPage);
public:
	COptionsAdvanceSubMultiPlay(vgui::Panel* parent);
	void ResetModel();
protected:
	virtual void OnMessage(const KeyValues* params, vgui::VPANEL ifromPanel) override;
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
private:
	vgui::ModelViewPanel* m_pModelViewer;
	vgui::Slider* m_pModelController;
	vgui::Label* m_pPlayerName;
};

class COptionsAdvanceDialog : public vgui::PropertyDialog
{
	DECLARE_CLASS_SIMPLE(COptionsAdvanceDialog, vgui::PropertyDialog);
public:
	COptionsAdvanceDialog(vgui::Panel* parent);
	virtual void Activate() override;
protected:
	virtual void OnCommand(const char* command) override;
	virtual void ApplySettings(KeyValues* inResourceData) override;
	virtual void PaintBackground() override;
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
private:
	vgui::GaussianBlurPanel* m_pBlur;
	COptionsAdvanceSubMultiPlay* m_pMultiPlayPage;
};

#endif // !OPTIONADVANCEDLG_H