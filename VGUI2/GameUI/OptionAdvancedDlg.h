#ifndef OPTIONADVANCEDLG_H
#define OPTIONADVANCEDLG_H

#pragma once
#include <string>
#include <map>

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
	class URLLabel;
	class CheckButton;
	class CCvarLabelSlider;
	class CCvarComboBox;
	class GroupBox;

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
	MESSAGE_FUNC_INT(OnOpenContextMenu, "OpenContextMenu", itemID);
	MESSAGE_FUNC_INT(OnFilterModelChecked, "FilterModelChecked", state);
	MESSAGE_FUNC_INT_INT(OnFavChange, "FavChange", itemID, add);
	virtual void OnResetData() override;
	virtual void OnApplyChanges() override;
	virtual void OnCommand(const char* cmd) override;
	virtual void ApplySchemeSettings(IScheme* pScheme) override;
private:
	void FilterModel();
	void GetValidateSparySize(size_t& ow, size_t& oh);
	void SetSparyPixel(unsigned char* pixels, size_t wide, size_t height);

	std::map<std::string, bool> m_aryModelList;

	ModelViewPanel* m_pModelViewer = nullptr;
	Slider* m_pModelController = nullptr;
	Label* m_pPlayerName = nullptr;
	ListPanel* m_pModelList = nullptr;
	TextEntry* m_pModelFilter = nullptr;
	Button* m_pModelFilterButton = nullptr;
	CheckButton* m_pFavCheckButton = nullptr;

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

	URLLabel* m_pUrl = nullptr;

	KeyValues* m_pFileKeyvalues = nullptr;
	WadFile* m_pSparyWad = nullptr;
};

class COptionsAdvanceSubOtherOption : public PropertyPage {
	DECLARE_CLASS_SIMPLE(COptionsAdvanceSubOtherOption, PropertyPage);
public:
	COptionsAdvanceSubOtherOption(Panel* parent);
protected:
	virtual void OnResetData() override;
	virtual void OnApplyChanges() override;
	virtual void OnCommand(const char* cmd) override;

	MESSAGE_FUNC_PTR_INT(OnKeyBinded, "KeyBinded", target, code);
private:
	CCvarToggleCheckButton* m_pNewHud;
	CCvarToggleCheckButton* m_pDynamicBackground;

	GroupBox* m_pScoreBoard;
	CCvarToggleCheckButton* m_pScoreShowAvatar;
	CCvarToggleCheckButton* m_pScoreShowLoss;
	CCvarToggleCheckButton* m_pScoreShowSteamId;
	CCvarComboBox* m_pScoreShowSteamIdType;
	CCvarToggleCheckButton* m_pScoreShowRealName;
	CCvarLabelSlider* m_pScoreSpacingNormal;
	CCvarLabelSlider* m_pScoreSpacingCompact;
	CCvarComboBox* m_pScoreSize;

	CCvarToggleCheckButton* m_pPopNumber;

	GroupBox* m_pPlayerTitleGroup;
	CCvarToggleCheckButton* m_pPlayerTitle;
	CCvarLabelSlider* m_pPlayerTitleDanger;

	GroupBox* m_pCameraGroup;
	CCvarLabelSlider* m_pCameraHeightValue;
	CCvarLabelSlider* m_pCameraRightValue;

	GroupBox* m_pModelLagGroup;
	CCvarToggleCheckButton* m_pModelLag;
	CCvarToggleCheckButton* m_pModelLagAutoStop;
	CCvarLabelSlider* m_pModelLagAutoValue;
	CCvarToggleCheckButton* m_pModelSlide;
	CCvarLabelSlider* m_pModelSlideAngle;
	CCvarLabelSlider* m_pModelSlideLength;
	CCvarLabelSlider* m_pModelSlideHeight;

	GroupBox* m_pHealthArmor;
	CCvarToggleCheckButton* m_pDamageShock;
	CCvarLabelSlider* m_pDamageShockFactor;
	CCvarLabelSlider* m_pDamageShockDmg;
	CCvarLabelSlider* m_pDangerHealth;
	CCvarLabelSlider* m_pDangerArmor;

	GroupBox* m_pItemHighLightGroup;
	CCvarToggleCheckButton* m_pItemHighLight;
	CCvarLabelSlider* m_pItemHighLightRange;

	CCvarToggleCheckButton* m_pAutojump;
	CCvarToggleCheckButton* m_pRainbowMenu;
	CCvarToggleCheckButton* m_pEfxHud;
	CCvarToggleCheckButton* m_pSidePanel;
	CCvarComboBox* m_pMotd;

	GroupBox* m_pDeathNoticeGroup;
	CCvarToggleCheckButton* m_pDeathNotice;
	CCvarLabelSlider* m_pDeathNoticeTime;
	CCvarLabelSlider* m_pDeathNoticeCount;

	GroupBox* m_pGrenadeIndicatorGroup;
	CCvarToggleCheckButton* m_pGrenadeIndicator;
	CCvarLabelSlider* m_pGrenadeIndicatorTime;
	CCvarLabelSlider* m_pGrenadeIndicatorRange;

	GroupBox* m_pVoteGroup;
	CCvarToggleCheckButton* m_pVote;
	Label* m_pVoteYes;
	Label* m_pVoteNo;
	Button* m_pVoteYesButton;
	Button* m_pVoteNoButton;

	GroupBox* m_pEfxGroup;
	CCvarToggleCheckButton* m_pShellEfx;
	CCvarToggleCheckButton* m_pGaussEfx;
	CCvarToggleCheckButton* m_pBloodEfx;
	CCvarLabelSlider* m_pBloodSpriteSpeed;
	CCvarLabelSlider* m_pBloodSpriteCount;
};


class COptionsAdvanceDialog : public PropertyDialog{
	DECLARE_CLASS_SIMPLE(COptionsAdvanceDialog, PropertyDialog);
public:
	COptionsAdvanceDialog(Panel* parent);
protected:
	virtual void ApplySettings(KeyValues* inResourceData) override;
	virtual void ApplySchemeSettings(IScheme* pScheme) override;
private:

	GaussianBlurPanel* m_pBlur;
	COptionsAdvanceSubMultiPlay* m_pMultiPlayPage;
	COptionsAdvanceSubOtherOption* m_pOtherOption;
};

}

#endif // !OPTIONADVANCEDLG_H