
#include "vgui/ISurface.h"
#include "CGameLoading.h"

CGameLoading::CGameLoading(Panel* parent, const char* name) : Panel(parent, name)
{
	int ScreenWidth, ScreenHeight;
	vgui::surface()->GetScreenSize(ScreenWidth, ScreenHeight);

	SetBounds(0, 0, ScreenWidth, ScreenHeight);

	m_pStatusText = new vgui::Label(this, "Label", L"");
	m_pStatusText->SetBounds(ScreenWidth / 2 - 200, ScreenHeight / 5 * 4, 200, 30);
	m_pStatusText->SetContentAlignment(vgui::Label::a_center);
	m_pStatusText->SetPaintBackgroundEnabled(false);
	m_pStatusText->SetVisible(true);
	m_pStatusText->SetFgColor(Color(255, 255, 255, 50));
	m_pStatusText->SetTextColorState(vgui::Label::CS_DULL);
	vgui::IScheme* pScheme = vgui::scheme()->GetIScheme(m_pStatusText->GetScheme());
	if (pScheme)
	{
		m_pStatusText->SetFont(pScheme->GetFont("Legacy_CreditsFont"));
	}

	m_pProgress = new vgui::ProgressBar(this, "Progress");
	m_pProgress->SetBounds(60 * ScreenWidth / 1024.0, ScreenHeight - 40 * ScreenHeight / 768.0, ScreenWidth - 60 * 2 * ScreenWidth / 1024.0, 26);
	m_pProgress->SetPaintBackgroundEnabled(false);
	m_pProgress->SetFgColor(Color(75, 75, 75, 75));
	m_pProgress->SetVisible(false);

	Activate();
}

CGameLoading::~CGameLoading()
{
}

void CGameLoading::PerformLayout(void)
{
	BaseClass::PerformLayout();
}

void CGameLoading::Activate()
{
	SetVisible(true);
	MakePopup(false, false);
	MoveToFront();
	vgui::surface()->RestrictPaintToSinglePanel(GetVPanel());
}

void CGameLoading::SetStatusText(const char* szStatusText)
{
	m_pStatusText->SetText(szStatusText);
}

bool CGameLoading::SetProgressPoint(int progressPoint)
{
	int nOldDrawnSegments = m_pProgress->GetDrawnSegmentCount();
	m_pProgress->SetProgress((float)progressPoint / (m_iRangeMax - m_iRangeMin));
	int nNewDrawSegments = m_pProgress->GetDrawnSegmentCount();
	return (nOldDrawnSegments != nNewDrawSegments);
}

void CGameLoading::SetProgressRange(int min, int max)
{
	m_iRangeMin = min;
	m_iRangeMax = max;
}

void CGameLoading::SetProgressVisible(bool bSet)
{
	m_pProgress->SetVisible(bSet);
}

void CGameLoading::PaintBackground()
{
	Panel::PaintBackground();
}