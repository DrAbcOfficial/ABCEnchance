#ifndef CVARCOMBOBOX_H
#define CVARCOMBOBOX_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/ComboBox.h>
namespace vgui {
	class CCvarComboBox : public Panel
	{
		DECLARE_CLASS_SIMPLE(CCvarComboBox, Panel);

	public:
		CCvarComboBox(Panel* parent, const char* panelName, const char* text, int maxitem, char const* cvarname);
		~CCvarComboBox(void);

	public:
		virtual int AddItem(const char* itemText, const KeyValues* userData);
		virtual int AddItem(const wchar_t* itemText, const KeyValues* userData);
		virtual void PerformLayout();
	public:
		void Reset(void);
		void ApplyChanges(void);
		void SetSelected(int select);
		const char* GetCvarName(void);

	private:
		char m_szCvarName[64];
		int m_iStartValue;
		ComboBox* m_pComboBox;
		Label* m_pLabel;
	};
}
#endif