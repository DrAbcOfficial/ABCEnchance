#ifndef GROUPBOX_H
#define GROUPBOX_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Panel.h>
namespace vgui {
	class Label;

	class GroupBox : public Panel
	{
		DECLARE_CLASS_SIMPLE(GroupBox, Panel);
	public:
		GroupBox(Panel* parent, const char* panelName, const char* text, int size);
	public:
		virtual void PerformLayout() override;
		virtual void ApplySchemeSettings(IScheme* pScheme) override;
	private:
		Label* m_pLabel;
		int m_iSize;
	};
}
#endif