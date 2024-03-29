#ifndef CVARSLIDER_H
#define CVARSLIDER_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Slider.h>

namespace vgui {
	class CCvarSlider : public Slider
	{
		DECLARE_CLASS_SIMPLE(CCvarSlider, Slider);

	public:
		CCvarSlider(Panel* parent, const char* panelName);
		CCvarSlider(Panel* parent, const char* panelName, const char* caption, float minValue, float maxValue, char const* cvarname, bool bAllowOutOfRange = false, bool floatvalue = false);
		~CCvarSlider(void);

	public:
		void SetupSlider(float minValue, float maxValue, const char* cvarname, bool bAllowOutOfRange);
		void SetCVarName(char const* cvarname);
		void SetMinMaxValues(float minValue, float maxValue, bool bSetTickdisplay = true);
		void SetTickColor(Color color);
		void SetFloatValue(bool state);

	public:
		virtual void Paint(void);
		virtual void ApplySettings(KeyValues* inResourceData);
		virtual void GetSettings(KeyValues* outResourceData);

	public:
		void ApplyChanges(void);
		float GetSliderValue(void);
		void SetSliderValue(float fValue);
		void Reset(void);
		bool HasBeenModified(void);

	public:
		MESSAGE_FUNC(OnSliderMoved, "SliderMoved");
		MESSAGE_FUNC(OnApplyChanges, "ApplyChanges");

	private:
		bool m_bAllowOutOfRange;
		bool m_bModifiedOnce;
		float m_fStartValue;
		int m_iStartValue;
		int m_iLastSliderValue;
		float m_fCurrentValue;
		char m_szCvarName[64];
		bool m_bCreatedInCode;
		float m_flMinValue;
		float m_flMaxValue;
		bool m_bIsFloat;
	};
}

#endif