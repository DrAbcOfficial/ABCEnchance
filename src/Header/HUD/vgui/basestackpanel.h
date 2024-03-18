#ifndef CBASESTACKPANEL_H
#define CBASESTACKPANEL_H
#include <list>
#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class ImageSprPanel;
}

class CBaseSprStackItem : public vgui::EditablePanel {
public:
	DECLARE_CLASS_SIMPLE(CBaseSprStackItem, vgui::EditablePanel);
	CBaseSprStackItem(vgui::Panel* parent, int spridx, int l, int r, int t, int b, float expire, float fi, float fo);
	void Show(float flTime);

	void SetExpire(float f);
	virtual void CheckExpire();
	virtual void SetSize(int wide, int tall);
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
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;
	virtual void PerformLayout() override;
	virtual void PaintBackground() override;

	virtual void ApplySettings(KeyValues* inResourceData) override;
	virtual void OnThink() override;

	void CalculateMaxItemCount();

protected:
	float m_flFadeinTime;
	float m_flFadeoutTime;
	float m_flKeepTime;
	int m_iStackGap;
	int m_iItemTall;
	int m_iItemWide;

	size_t m_iMaxItem;
	std::list<CBaseSprStackItem*> m_aryPanels;
};
#endif