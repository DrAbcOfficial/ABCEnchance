#pragma once

#include "vgui_controls/PropertyDialog.h"
#include "vgui_controls/KeyRepeat.h"

class COptionsDialog : public vgui::PropertyDialog
{
	DECLARE_CLASS_SIMPLE(COptionsDialog, vgui::PropertyDialog);

public:
	COptionsDialog(vgui::Panel *parent);
	~COptionsDialog();

public:
	void Run();
	void Activate();
	void Deactivate();

	virtual void ApplySchemeSettings(vgui::IScheme* pScheme) override;
public:
	void OnClose();
	virtual void OnThink() override;

public:
	MESSAGE_FUNC(OnGameUIHidden, "GameUIHidden");

private:
	bool m_bPushIn;
	bool m_bPushOut;
	float m_flAnimeTime;
	class COptionsSubMultiplayer *m_pOptionsSubMultiplayer;
	class COptionsSubKeyboard *m_pOptionsSubKeyboard;
	class COptionsSubMouse *m_pOptionsSubMouse;
	class COptionsSubAudio *m_pOptionsSubAudio;
	class COptionsSubVideo *m_pOptionsSubVideo;
	class COptionsSubVoice *m_pOptionsSubVoice;
	class COptionsSubAdvanced *m_pOptionsSubAdvanced;
};
COptionsDialog* CeateOptionDialog(vgui::Panel* parent);
COptionsDialog* OptionsDialog();