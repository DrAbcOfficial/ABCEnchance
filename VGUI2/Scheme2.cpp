#include <IScheme.h>
#include <KeyValues.h>
#include <Bitmap.h>
#include <Border.h>
#include <utlsymbol.h>
#include <Controls.h>

class CScheme : public vgui::IScheme {
public:
	const char* GetName(void) {
		return tag;
	}
	char const* GetFontName(const vgui::HFont& font) {
		for (int i = m_FontAliases.Count(); --i >= 0;) {
			vgui::HFont fnt = (vgui::HFont)m_FontAliases[i]._font;
			if (fnt == font)
				return m_FontAliases[i]._trueFontName.String();
		}
		return "<Unknown font>";
	}
	const char* GetFileName(void) { 
		return fileName; 
	}
private:
	char fileName[256];
	char tag[64];

	KeyValues* m_pData;
	KeyValues* m_pkvBaseSettings;
	KeyValues* m_pkvColors;

	struct SchemeBorder_t
	{
		vgui::Border* border;
		int borderSymbol;
		bool bSharedBorder;
	};

	CUtlVector<SchemeBorder_t> m_BorderList;
	vgui::IBorder* m_pBaseBorder;
	KeyValues* m_pkvBorders;

#pragma pack(1)
	struct fontalias_t
	{
		CUtlSymbol _fontName;
		CUtlSymbol _trueFontName;
		unsigned short _font : 15;
		unsigned short m_bProportional : 1;
		unsigned short m_bHD : 1;
	};
#pragma pack()

	friend fontalias_t;

	CUtlVector<fontalias_t>	m_FontAliases;
	vgui::VPANEL m_SizingPanel;
	int m_nScreenWide;
	int m_nScreenTall;
};

const char* vgui::IScheme::GetName(void) {
	return reinterpret_cast<CScheme*>(this)->GetName();
}
char const* vgui::IScheme::GetFontName(const vgui::HFont& font) {
	return reinterpret_cast<CScheme*>(this)->GetFontName(font);
}

class CSchemeManager : public vgui::ISchemeManager {
public:
	vgui::HScheme LoadSchemeFromFileEx(vgui::VPANEL sizingPanel, const char* fileName, const char* tag) {
		return LoadSchemeFromFile(fileName, tag);
	}
	int GetProportionalScaledValueEx(vgui::HScheme scheme, int normalizedValue) {
		return GetProportionalScaledValue(normalizedValue);
	}
	int GetProportionalNormalizedValueEx(vgui::HScheme scheme, int scaledValue) {
		return GetProportionalNormalizedValue(scaledValue);
	}
private:
	CUtlVector<CScheme*> m_Schemes;
	struct CachedBitmapHandle_t{
		vgui::Bitmap* bitmap;
	};
	CUtlRBTree<CachedBitmapHandle_t, int> m_Bitmaps;
};
vgui::HScheme vgui::ISchemeManager::LoadSchemeFromFileEx(vgui::VPANEL sizingPanel, const char* fileName, const char* tag) {
	return reinterpret_cast<CSchemeManager*>(this)->LoadSchemeFromFileEx(sizingPanel, fileName, tag);
}
int vgui::ISchemeManager::GetProportionalScaledValueEx(vgui::HScheme scheme, int normalizedValue) {
	return reinterpret_cast<CSchemeManager*>(this)->GetProportionalScaledValueEx(scheme, normalizedValue);
}
int vgui::ISchemeManager::GetProportionalNormalizedValueEx(vgui::HScheme scheme, int scaledValue) {
	return reinterpret_cast<CSchemeManager*>(this)->GetProportionalNormalizedValueEx(scheme, scaledValue);
}

vgui::ISchemeManager* g_pVGuiSchemeManager = nullptr;