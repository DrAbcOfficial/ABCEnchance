#ifndef NETEASEMUSICDIALOG_H
#define NETEASEMUSICDIALOG_H

#pragma once
#include "vgui_controls/Frame.h"

namespace vgui {
	class Button;
	class ImageButton;
	class Label;
	class ImagePanel;
	class ListViewPanel;

	class CNeteaseMusicDialog : public Frame {
		DECLARE_CLASS_SIMPLE(CNeteaseMusicDialog, Frame);
	public:
		CNeteaseMusicDialog(Panel* parent);
		virtual ~CNeteaseMusicDialog();
	protected:

	private:
		ImagePanel* m_pIcon;
		Label* m_pTitle;
		Label* m_pUserInfo;
		Panel* m_pDivideLine;
		ImagePanel* m_pAlubmImage;
		Label* m_pSongName;
		Label* m_pSongTime;
		Label* m_pLyric;
		ImageButton* m_pVolumn;
		ImageButton* m_pPlay;
		ImageButton* m_pNext;
		ImageButton* m_pLast;
		ImageButton* m_pClose;

		ListViewPanel* m_pSideBar;
		Panel* m_pSearchPage;
	};
}

#endif // !NETEASEMUSICDIALOG_H