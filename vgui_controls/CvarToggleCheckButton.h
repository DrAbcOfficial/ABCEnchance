#ifndef CVARTOGGLECHECKBUTTON_H
#define CVARTOGGLECHECKBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/CheckButton.h>
namespace vgui {
	class CCvarToggleCheckButton : public CheckButton
	{
		DECLARE_CLASS_SIMPLE(CCvarToggleCheckButton, CheckButton);

	public:
		CCvarToggleCheckButton(Panel* parent, const char* panelName, const char* text, char const* cvarname);
		~CCvarToggleCheckButton(void);

	public:
		virtual void SetSelected(bool state);
		virtual void Paint(void);

	public:
		void Reset(void);
		void ApplyChanges(void);
		bool HasBeenModified(void);
		void ApplySettings(KeyValues* inResourceData);
		const char* GetCvarName(void);
	private:
		MESSAGE_FUNC(OnButtonChecked, "CheckButtonChecked");

	private:
		char m_szCvarName[64];
		bool m_bStartValue;
	};
}
#endif