#ifndef NETEASEMUSICDIALOG_H
#define NETEASEMUSICDIALOG_H

#pragma once
#include "vgui_controls/Frame.h"

namespace vgui {
	class Button;
	class Label;

	class CNeteaseMusicDialog : public Frame {
		DECLARE_CLASS_SIMPLE(CNeteaseMusicDialog, Frame);
	public:
		CNeteaseMusicDialog(Panel* parent);
		virtual ~CNeteaseMusicDialog();
	protected:

	private:
		Label* m_pUserInfo;
		Panel* m_pDivideLine;
	};
}

#endif // !NETEASEMUSICDIALOG_H