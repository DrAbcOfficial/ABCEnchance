//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef CONSOLEDIALOG_H
#define CONSOLEDIALOG_H

#ifdef _WIN32
#pragma once
#endif

#include <Color.h>
#include "tier1/utlvector.h"
#include "vgui_controls/EditablePanel.h"
#include "vgui_controls/Frame.h"

typedef struct cvar_s cvar_t;

namespace vgui
{

	// Things the user typed in and hit submit/return with
	class CHistoryItem
	{
	public:
		CHistoryItem(void);
		CHistoryItem(const char* text, const char* extra = NULL);
		CHistoryItem(const CHistoryItem& src);
		~CHistoryItem(void);

		const char* GetText() const;
		const char* GetExtra() const;
		void SetText(const char* text, const char* extra);
		bool HasExtra() { return m_bHasExtra; }

	private:
		char* m_text;
		char* m_extraText;
		bool		m_bHasExtra;
	};

	//-----------------------------------------------------------------------------
	// Purpose: Game/dev console dialog
	//-----------------------------------------------------------------------------
	class CConsolePanel : public EditablePanel
	{
		DECLARE_CLASS_SIMPLE(CConsolePanel, EditablePanel);

	public:
		CConsolePanel(Panel* pParent, const char* pName, bool bStatusVersion);
		~CConsolePanel();

		// Inherited from IConsoleDisplayFunc
		virtual void ColorPrint(const Color& clr, const char* pMessage);
		virtual void Print(const char* pMessage);
		virtual void DPrint(const char* pMessage);
		virtual void GetConsoleText(char* pchText, size_t bufSize) const;

		// clears the console
		void Clear();

		// writes console to a file
		void DumpConsoleTextToFile();

		// Hides the console
		void Hide();

		bool TextEntryHasFocus() const;
		void TextEntryRequestFocus();



	private:
		enum
		{
			MAX_HISTORY_ITEMS = 100,
		};

		class CompletionItem
		{
		public:
			CompletionItem(void);
			CompletionItem(const CompletionItem& src);
			CompletionItem& operator =(const CompletionItem& src);
			~CompletionItem(void);
			const char* GetItemText(void);
			const char* GetCommand(void) const;
			const char* GetName() const;

			bool	m_bIsCommand;
			bool	m_bIsProtected;
			cvar_t* m_pCVar;
			unsigned int m_iCmd;
			CHistoryItem* m_pText;
		};

	protected:
		// methods
		void OnAutoComplete(bool reverse);
		MESSAGE_FUNC_PTR(OnTextChanged, "TextChanged", panel);
		void RebuildCompletionList(const char* partialText);
		void UpdateCompletionListPosition();
		MESSAGE_FUNC(CloseCompletionList, "CloseCompletionList");
		MESSAGE_FUNC_CHARPTR(OnMenuItemSelected, "CompletionCommand", command);
		void ClearCompletionList();
		void AddToHistory(const char* commandText, const char* extraText);

		// vgui overrides
		virtual void PerformLayout();
		virtual void ApplySchemeSettings(IScheme* pScheme);
		virtual void ApplySettings(KeyValues* inResourceData);
		virtual void OnCommand(const char* command);
		virtual void OnKeyCodePressed(KeyCode code);
		virtual void OnKeyCodeTyped(KeyCode code);
		virtual void OnThink();

		RichText* m_pHistory;
		TextEntry* m_pEntry;
		Button* m_pSubmit;
		Menu* m_pCompletionList;
		Color m_PrintColor;
		Color m_ErrorPrintColor;
		Color m_WarnPrintColor;
		Color m_DPrintColor;

		int m_iNextCompletion;		// the completion that we'll next go to
		char m_szPartialText[256];
		char m_szPreviousPartialText[256];
		bool m_bAutoCompleteMode;	// true if the user is currently tabbing through completion options
		bool m_bWasBackspacing;
		bool m_bStatusVersion;
		char m_szSubmitedCommandPrefix[64];

		CUtlVector< CompletionItem* > m_CompletionList;
		CUtlVector< CHistoryItem >	m_CommandHistory;

		friend class CConsoleDialog;
	};


	class CConsoleDialog : public Frame
	{
		DECLARE_CLASS_SIMPLE(CConsoleDialog, Frame);

	public:
		CConsoleDialog(Panel* pParent, const char* pName, bool bStatusVersion);

		virtual void OnScreenSizeChanged(int iOldWide, int iOldTall);
		virtual void Close();
		virtual void PerformLayout();

		// brings dialog to the fore
		MESSAGE_FUNC(Activate, "Activate");
		MESSAGE_FUNC_CHARPTR(OnCommandSubmitted, "CommandSubmitted", command);

		// hides the console
		void Hide();

		// Chain to the page
		void Print(const char* msg);
		void DPrint(const char* msg);
		void ColorPrint(const Color& clr, const char* msg);
		void Clear();
		void DumpConsoleTextToFile();

		virtual void OnKeyCodePressed(KeyCode code);

	protected:
		CConsolePanel* m_pConsolePanel;
	};

} // end namespace vgui2

#endif // CONSOLEDIALOG_H