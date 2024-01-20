#ifndef CVARLABELSLIDER_H
#define CVARLABELSLIDER_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/CvarSlider.h>

namespace vgui {
	class CCvarLabelSlider : public Panel
	{
		DECLARE_CLASS_SIMPLE(CCvarLabelSlider, Panel);

	public:
		CCvarLabelSlider(Panel* parent, const char* panelName);
		CCvarLabelSlider(Panel* parent, const char* panelName, char const* labelcontent, const char* caption, float minValue, float maxValue, char const* cvarname, bool bAllowOutOfRange = false, bool isfloat = false);

		void SetupSlider(float minValue, float maxValue, const char* cvarname, bool bAllowOutOfRange);
		void SetCVarName(char const* cvarname);
		void SetMinMaxValues(float minValue, float maxValue, bool bSetTickdisplay = true);
		void SetTickColor(Color color);
		void SetFloatValue(bool state);
	public:
		virtual void PerformLayout(void) override;
	public:
		void ApplyChanges(void);
		float GetSliderValue(void);
		void SetSliderValue(float fValue);
		void Reset(void);
		bool HasBeenModified(void);
	private:
		MESSAGE_FUNC(OnSliderMoved, "SliderMoved");
		MESSAGE_FUNC(OnApplyChanges, "ApplyChanges");
	private:
		CCvarSlider* m_pSlider;
		Label* m_pLabel;
	};
}

#endif