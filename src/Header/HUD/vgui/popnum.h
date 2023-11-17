#ifndef CPOPNUMPANEL_H
#define CPOPNUMPANEL_H
#include <string>

#include <vgui_controls/EditablePanel.h>
#include "IViewportPanel.h"

namespace vgui {
	class Label;
}
class CVector;

class CPopNumberPanel : public vgui::EditablePanel, public IViewportPanel
{
public:
	DECLARE_CLASS_SIMPLE(CPopNumberPanel, vgui::EditablePanel);

	CPopNumberPanel(vec3_t vecOrigin, Color& pColor, int value);
	~CPopNumberPanel();
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
	int m_iValue = 0;
	vgui::Label* m_pNumberLable = nullptr;
	CVector* m_vecOrigin = nullptr;
	Color m_hColor;
};
#endif