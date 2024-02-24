#ifndef CBASESTACKPANEL_H
#define CBASESTACKPANEL_H
#include <vector>
#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class ImageSprPanel;
}

class CBaseSprStackItem : public vgui::EditablePanel {
public:
	DECLARE_CLASS_SIMPLE(CBaseSprStackItem, vgui::EditablePanel);
	CBaseSprStackItem(vgui::Panel* parent, int spridx, int l, int r, int t, int b, float expire, float fi, float fo, float is);
	void Show(float flTime);

	void SetExpire(float f);
	void CheckExpire();
protected:
	vgui::ImageSprPanel* m_pPanel;

	int iSprIdx;
	float fKeepTime;
	float fIn;
	float fOut;
};

class CBaseStackPanel : public vgui::EditablePanel, public IViewportPanel{
public:
	DECLARE_CLASS_SIMPLE(CBaseStackPanel, vgui::EditablePanel);
	CBaseStackPanel(vgui::Panel* parent, const char* name);
	virtual const char* GetName() = 0;
	virtual void Reset();
	virtual void ShowPanel(bool state);
	virtual bool IsVisible();
	virtual vgui::VPANEL GetVPanel();
	virtual void SetParent(vgui::VPANEL parent);

	virtual void ApplySettings(KeyValues* inResourceData) override;
	virtual void OnThink() override;

protected:
	float m_flFadeinTime;
	float m_flFadeoutTime;
	float m_flKeepTime;
	float m_flIconSize;
	std::vector<CBaseSprStackItem*> m_aryPanels;
};
#endif