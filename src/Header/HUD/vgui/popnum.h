#ifndef CPOPNUMPANEL_H
#define CPOPNUMPANEL_H
#include <string>

#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
}

class CPopNumberPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CPopNumberPanel, vgui::EditablePanel);

	CPopNumberPanel(int x, int y, Color& pColor, int value);
	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
	// IViewportPanel overrides
	virtual const char* GetName() override;
	virtual void SetSize(int w, int t);
	virtual void Reset() override;
	virtual void ShowPanel(bool state) override;
	virtual bool IsVisible() override;
	virtual vgui::VPANEL GetVPanel() override;
	virtual void SetParent(vgui::VPANEL parent) override;
	virtual void OnThink() override;
private:
	void Destory();
	float m_flCreateTime = 0;
	int m_iOriginalY = 0;
	int m_iOriginalTall = 0;
	int m_iValue = 0;
	vgui::Label* m_pNumberLable = nullptr;
};
#endif