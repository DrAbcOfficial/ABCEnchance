#ifndef OPTIONADVANCEDLG_H
#define OPTIONADVANCEDLG_H

#pragma once
#include <string>
#include <set>

#include "vgui_controls/PropertyPage.h"
#include "vgui_controls/PropertyDialog.h"

class WadFile;
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
	class ImagePanel;
	class Button;
	class FileOpenDialog;

class COptionsAdvanceSubMultiPlay : public PropertyPage{
	DECLARE_CLASS_SIMPLE(COptionsAdvanceSubMultiPlay, PropertyPage);
public:
	COptionsAdvanceSubMultiPlay(Panel* parent);
	~COptionsAdvanceSubMultiPlay();
	void ResetModel();
	void ChangeModel(const char* mdl);
	void BuildModelList(const char* filter = nullptr);
protected:
	MESSAGE_FUNC(OnSliderMoved, "SliderMoved");
	MESSAGE_FUNC(OnItemSelected, "ItemSelected");
	MESSAGE_FUNC(OnButtonChanged, "ButtonChanged");
	MESSAGE_FUNC_CHARPTR(OnFileSelected, "FileSelected", fullpath);
	virtual void OnResetData() override;
	virtual void OnCommand(const char* cmd) override;
	virtual void ApplySchemeSettings(IScheme* pScheme) override;
private:
	void GetValidateSparySize(size_t& ow, size_t& oh);
	void SetSparyPixel(unsigned char* pixels, size_t wide, size_t height);
	std::set<std::string> m_aryModelList;
	ModelViewPanel* m_pModelViewer = nullptr;
	Slider* m_pModelController = nullptr;
	Label* m_pPlayerName = nullptr;
	ListPanel* m_pModelList = nullptr;
	TextEntry* m_pModelFilter = nullptr;
	Button* m_pModelFilterButton = nullptr;

	CrossHairDisplay* m_pCrosshairDisplay = nullptr;
	Label* m_pColorLabel = nullptr;
	CCvarSlider* m_pCrosshairr = nullptr;
	CCvarSlider* m_pCrosshairg = nullptr;
	CCvarSlider* m_pCrosshairb = nullptr;
	CCvarSlider* m_pCrosshaira = nullptr;
	Label* m_OutlineLabel = nullptr;
	CCvarSlider* m_pCrosshairOutliner = nullptr;
	CCvarSlider* m_pCrosshairOutlineg = nullptr;
	CCvarSlider* m_pCrosshairOutlineb = nullptr;
	CCvarSlider* m_pCrosshairOutlinea = nullptr;
	Label* m_pWidthLabel = nullptr;
	CCvarSlider* m_pCrosshairWidth = nullptr;
	Label* m_pLengthLabel = nullptr;
	CCvarSlider* m_pCrosshairLength = nullptr;
	Label* m_pOutlineWidthLabel = nullptr;
	CCvarSlider* m_pCrosshairOutlineWidth = nullptr;
	Label* m_pOffsetLable = nullptr;
	CCvarSlider* m_pCrosshairOffset = nullptr;
	CCvarToggleCheckButton* m_pCrosshairDot = nullptr;
	CCvarToggleCheckButton* m_pCrosshairT = nullptr;
	CCvarToggleCheckButton* m_pCrosshairOutline = nullptr;

	Panel* m_pSpliter = nullptr;

	ImagePanel* m_pSpary = nullptr;
	Button* m_pLoadSpary = nullptr;
	FileOpenDialog* m_pFileDialog = nullptr;

	KeyValues* m_pFileKeyvalues = nullptr;
	WadFile* m_pSparyWad = nullptr;
};

class COptionsAdvanceDialog : public PropertyDialog{
	DECLARE_CLASS_SIMPLE(COptionsAdvanceDialog, PropertyDialog);
public:
	COptionsAdvanceDialog(Panel* parent);
	virtual void Activate() override;
protected:
	virtual void ApplySettings(KeyValues* inResourceData) override;
	virtual void ApplySchemeSettings(IScheme* pScheme) override;
private:

	GaussianBlurPanel* m_pBlur;
	COptionsAdvanceSubMultiPlay* m_pMultiPlayPage;
};

}

#endif // !OPTIONADVANCEDLG_H