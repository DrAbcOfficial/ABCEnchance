#ifndef OPTIONADVANCEDLG_H
#define OPTIONADVANCEDLG_H

#pragma once
#include <string>
#include <set>

#include "vgui_controls/PropertyPage.h"
#include "vgui_controls/PropertyDialog.h"

namespace vgui {
	class CrossHairDisplay;
	class GaussianBlurPanel;
	class ModelViewPanel;
	class TextEntry;
	class Slider;
	class Label;
	class ListPanel;
	class Button;
	class CCvarSlider;
	class CCvarToggleCheckButton;

class COptionsAdvanceSubMultiPlay : public PropertyPage{
	DECLARE_CLASS_SIMPLE(COptionsAdvanceSubMultiPlay, PropertyPage);
public:
	COptionsAdvanceSubMultiPlay(Panel* parent);
	void ResetModel();
	void ChangeModel(const char* mdl);
	void BuildModelList(const char* filter = nullptr);
protected:
	MESSAGE_FUNC(OnSliderMoved, "SliderMoved");
	MESSAGE_FUNC(OnItemSelected, "ItemSelected");
	MESSAGE_FUNC(OnButtonChanged, "ButtonChanged");
	virtual void OnResetData() override;
	virtual void OnCommand(const char* cmd) override;
	virtual void ApplySchemeSettings(IScheme* pScheme) override;
private:
	std::set<std::string> m_aryModelList;
	ModelViewPanel* m_pModelViewer;
	Slider* m_pModelController;
	Label* m_pPlayerName;
	ListPanel* m_pModelList;
	TextEntry* m_pModelFilter;
	Button* m_pModelFilterButton;

	CrossHairDisplay* m_pCrosshairDisplay;

	Label* m_pColorLabel;
	CCvarSlider* m_pCrosshairr;
	CCvarSlider* m_pCrosshairg;
	CCvarSlider* m_pCrosshairb;
	CCvarSlider* m_pCrosshaira;

	Label* m_OutlineLabel;
	CCvarSlider* m_pCrosshairOutliner;
	CCvarSlider* m_pCrosshairOutlineg;
	CCvarSlider* m_pCrosshairOutlineb;
	CCvarSlider* m_pCrosshairOutlinea;
	Label* m_pWidthLabel;
	CCvarSlider* m_pCrosshairWidth;
	Label* m_pLengthLabel;
	CCvarSlider* m_pCrosshairLength;
	Label* m_pOutlineWidthLabel;
	CCvarSlider* m_pCrosshairOutlineWidth;
	Label* m_pOffsetLable;
	CCvarSlider* m_pCrosshairOffset;
	CCvarToggleCheckButton* m_pCrosshairDot;
	CCvarToggleCheckButton* m_pCrosshairT;
	CCvarToggleCheckButton* m_pCrosshairOutline;
};

class COptionsAdvanceDialog : public PropertyDialog{
	DECLARE_CLASS_SIMPLE(COptionsAdvanceDialog, PropertyDialog);
public:
	COptionsAdvanceDialog(Panel* parent);
	virtual void Activate() override;
protected:
	virtual void ApplySettings(KeyValues* inResourceData) override;
	virtual void PaintBackground() override;
	virtual void ApplySchemeSettings(IScheme* pScheme) override;
private:
	GaussianBlurPanel* m_pBlur;
	COptionsAdvanceSubMultiPlay* m_pMultiPlayPage;
};

}

#endif // !OPTIONADVANCEDLG_H