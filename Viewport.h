#pragma once

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <vgui/VGUI.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <vgui/IScheme.h>
#include <vgui/IVGui.h>
#include <vgui/IInput.h>
#include <VGUI2/IViewportPanel.h>
#include <VGUI_controls/Controls.h>
#include <VGUI_controls/Panel.h>
#include <VGUI_controls/Frame.h>

class ISchemel;

class CViewport : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CViewport, vgui::Panel);

public:
	CViewport(void);
	virtual ~CViewport(void);

public:
	//ClientVGUI Interface
	void Start(void);
	void Init(void);
	void VidInit(void);
	void Think(void);
	void Paint(void);
	void SetParent(vgui::VPANEL vPanel);
	void AddNewPanel(IViewportPanel* panel);
	void ActivateClientUI(void);
	void HideClientUI(void);
	bool IsScoreBoardVisible();
	void ShowScoreBoard();
	void HideScoreBoard();
	char* GetServerName();
	CScorePanel* GetScoreBoard();
	Color GetPlayerColor(int index);
private:
	std::vector<IViewportPanel*> m_Panels;

	char m_szLevelName[256];
	CScorePanel* m_pScorePanel = nullptr;
	char m_szServerName[MAX_SERVERNAME_LENGTH] = "<ERROR>";
};

extern CViewport *g_pViewPort;