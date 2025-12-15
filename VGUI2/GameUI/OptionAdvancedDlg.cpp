#include <algorithm>
#include "core/module/config.h"

#include "interface.h"
#include "IFileSystem.h"

#include "vgui/IGameUIFuncs.h"

#include "vgui_controls/PropertySheet.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/UrlLabel.h"
#include "vgui_controls/Slider.h"
#include "vgui_controls/ListPanel.h"
#include "vgui_controls/TextEntry.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/CrossHairDisplay.h"
#include "vgui_controls/CvarLabelSlider.h"
#include "vgui_controls/CvarToggleCheckButton.h"
#include "vgui_controls/CvarComboBox.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/FileOpenDialog.h"
#include "vgui_controls/Menu.h"
#include "vgui_controls/MemoryBitmap.h"
#include "vgui_controls/GroupBox.h"
#include "vgui_controls/MessageBox.h"
#include "vgui_controls/GaussianBlurPanel.h"
#include "vgui_controls/ModelViewPanel.h"

#include <FreeImage.h>
#include "wadlib/wadfile.h"

#ifdef __HAS_NETEASE_API
#include "NeteaseApi.h"
#endif

#include "OptionAdvancedDlg.h"

extern const char* CVAR_GET_STRING(const char* x);
extern void CVAR_SET_STRING(const char* x, const char* v);
extern float CVAR_GET_FLOAT(const char* x);
extern void CVAR_SET_FLOAT(const char* x, float v);
extern IGameUIFuncs* GameUIFuncs();
const char* FAVMODEL_ICON = "#GameUI_ABC_Favorite";

using namespace vgui;

class ModelListPanel : public ListPanel {
public:
	ModelListPanel(Panel* parent, const char* name) : ListPanel(parent, name) {
	}
	void OnMouseDoublePressed(MouseCode code) {
		if (GetItemCount() > 0) {
			int itemID = GetSelectedItem(0);
			PostActionSignal(new KeyValues("FavChange", "itemID", itemID, "add", !std::strcmp(GetItem(itemID)->GetString("fav"), FAVMODEL_ICON) ? 0 : 1));
		}
	}
};
class ModelFilterButton : public CheckButton {
public:
	ModelFilterButton(Panel* parent, const char* name, const char* content) : CheckButton(parent, name, content) {

	}
	void SetSelected(bool state) override {
		KeyValues* msg = new KeyValues("FilterModelChecked", "state", (int)state);
		PostActionSignal(msg);
		Button::SetSelected(state);
	}
};

COptionsAdvanceSubMultiPlay::COptionsAdvanceSubMultiPlay(Panel* parent) : BaseClass(parent, "OptionsAdvanceSubMultiPlay") {
	m_pModelViewer = new ModelViewPanel(this, "ModelViewer");
	m_pModelController = new Slider(this, "ModelController");
	m_pModelList = new ModelListPanel(this, "ModelList");
	m_pModelFilter = new TextEntry(this, "ModelFilter");
	m_pModelFilterButton = new Button(this, "ModelFilterButton", "#GameUI_ABC_SubmitModelFilter", this, "FilterModel");
	m_pFavCheckButton = new ModelFilterButton(this, "ModelFavFilter", "#GameUI_ABC_FilteFavOnly");
	m_pPlayerName = new Label(this, "PlayerName", CVAR_GET_STRING("name"));
	m_pCrosshairDisplay = new CrossHairDisplay(this, "CrossHairDisplay");

	m_pColorLabel = new Label(this, "LabelC", "#GameUI_ABC_ColorLabel");
	m_pCrosshairr = new CCvarSlider(this, "CrosshairR", "#GameUI_ABC_ColorLabel", 0.0f, 255.0f, "cl_crosshaircolor_r");
	m_pCrosshairg = new CCvarSlider(this, "CrosshairG", "#GameUI_ABC_ColorLabel", 0.0f, 255.0f, "cl_crosshaircolor_g");
	m_pCrosshairb = new CCvarSlider(this, "CrosshairB", "#GameUI_ABC_ColorLabel", 0.0f, 255.0f, "cl_crosshaircolor_b");
	m_pCrosshaira = new CCvarSlider(this, "CrosshairA", "#GameUI_ABC_ColorLabel", 0.0f, 255.0f, "cl_crosshairalpha");
	m_OutlineLabel = new Label(this, "LabelOC", "#GameUI_ABC_OutlineColorLabel");
	m_pCrosshairOutliner = new CCvarSlider(this, "CrosshairOR", "#GameUI_ABC_OutlineColorLabel", 0.0f, 255.0f, "cl_crosshaircolor_outline_r");
	m_pCrosshairOutlineg = new CCvarSlider(this, "CrosshairOG", "#GameUI_ABC_OutlineColorLabel", 0.0f, 255.0f, "cl_crosshaircolor_outline_g");
	m_pCrosshairOutlineb = new CCvarSlider(this, "CrosshairOB", "#GameUI_ABC_OutlineColorLabel", 0.0f, 255.0f, "cl_crosshaircolor_outline_b");
	m_pCrosshairOutlinea = new CCvarSlider(this, "CrosshairOA", "#GameUI_ABC_OutlineColorLabel", 0.0f, 255.0f, "cl_crosshair_outline_alpha");
	m_pWidthLabel = new Label(this, "LabelW", "#GameUI_ABC_CrosshairWidth");
	m_pCrosshairWidth = new CCvarSlider(this, "CrosshairW", "#GameUI_ABC_CrosshairWidth", 0.0f, 100.0f, "cl_crosshairthickness");
	m_pLengthLabel = new Label(this, "LabelL", "#GameUI_ABC_CrosshairLength");
	m_pCrosshairLength = new CCvarSlider(this, "CrosshairL", "#GameUI_ABC_CrosshairLength", 0.0f, 100.0f, "cl_crosshairsize");
	m_pOutlineWidthLabel = new Label(this, "LabelOW", "#GameUI_ABC_CrosshairOutlineWidth");
	m_pCrosshairOutlineWidth = new CCvarSlider(this, "CrosshairO", "#GameUI_ABC_CrosshairOutlineWidth", 0.0f, 100.0f, "cl_crosshair_outline");
	m_pOffsetLable = new Label(this, "LabelOF", "#GameUI_ABC_CrosshairOffset");
	m_pCrosshairOffset = new CCvarSlider(this, "CrosshairOF", "#GameUI_ABC_CrosshairOffset", 0.0f, 100.0f, "cl_crosshairgap");
	m_pCrosshairDot = new CCvarToggleCheckButton(this, "CrosshairDot", "#GameUI_ABC_CrosshairD", "cl_crosshairdot");
	m_pCrosshairT = new CCvarToggleCheckButton(this, "CrosshairT", "#GameUI_ABC_CrosshairT", "cl_crosshair_t");
	m_pCrosshairOutline = new CCvarToggleCheckButton(this, "CrosshairOTL", "#GameUI_ABC_CrosshairOTL", "cl_crosshair_outline_draw");

	m_pSpliter = new Panel(this, "Spliter");

	m_pSparyWad = new WadFile();

	m_pSpary = new ImagePanel(this, "SparyImage");
	m_pLoadSpary = new Button(this, "SparyLoadButton", "#GameUI_ABC_LoadSpary", this, "OpenLoadSparyDialog");

	m_pUrl = new URLLabel(this, "URLLable", "ABCEnchance  ||  " __DATE__, "https://github.com/DrAbcOfficial/ABCEnchance");

	LoadControlSettings("abcenchance/res/gameui/OptionsAdvanceSubMultiPlay.res");

	float y, r;
	m_pModelViewer->GetModelRotate(r, y, r);

	m_pModelController->SetRange(0, 360);
	m_pModelController->AddActionSignalTarget(this);
	m_pModelController->SetValue(y, true);

	m_pModelList->AddColumnHeader(0, "modelname", "#GameUI_ABC_Model", 0, ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW);
	m_pModelList->AddColumnHeader(1, "fav", "#GameUI_ABC_FavModel", m_pModelList->GetWide() / 5, ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW & ListPanel::ColumnFlags_e::COLUMN_FIXEDSIZE);
	m_pModelList->AddActionSignalTarget(this);
	m_pModelList->SetMultiselectEnabled(false);

	m_pModelFilter->AddActionSignalTarget(this);
	m_pCrosshairDot->AddActionSignalTarget(this);
	m_pCrosshairT->AddActionSignalTarget(this);
	m_pCrosshairOutline->AddActionSignalTarget(this);
	m_pFavCheckButton->AddActionSignalTarget(this);
}

COptionsAdvanceSubMultiPlay::~COptionsAdvanceSubMultiPlay() {
	if (m_pSparyWad) {
		delete m_pSparyWad;
		m_pSparyWad = nullptr;
	}
}

void COptionsAdvanceSubMultiPlay::ResetModel() {
	ChangeModel(CVAR_GET_STRING("model"));
}

void COptionsAdvanceSubMultiPlay::ChangeModel(const char* mdl) {
	char path[MAX_PATH]{};
	std::snprintf(path, MAX_PATH, "models/player/%s/%s.mdl", mdl, mdl);
	m_pModelViewer->ChangeModel(path);
}

void COptionsAdvanceSubMultiPlay::BuildModelList(const char* filter) {
	m_pModelList->RemoveAll();
	m_aryModelList.clear();
	FileFindHandle_t findHandle;
	auto pszFileName = filesystem()->FindFirst("models/player/*.*", &findHandle);
	if (!pszFileName)
		return;
	if (pszFileName) {
		do {
			if (filesystem()->FindIsDirectory(findHandle) && *pszFileName != '.') {
				m_aryModelList.insert(std::make_pair<std::string, bool>(pszFileName, false));
			}
		} while ((pszFileName = filesystem()->FindNext(findHandle)) != nullptr);
	}
	filesystem()->FindClose(findHandle);

	auto cfg = abcconfig::GetConfig();
	for (auto iter = m_aryModelList.begin(); iter != m_aryModelList.end(); iter++) {
		const std::string& name = iter->first;
		if (std::find(cfg->m_aryFavModels.begin(), cfg->m_aryFavModels.end(), name) != cfg->m_aryFavModels.end())
			iter->second = true;
	}

	size_t counter = 0;
	size_t plr = 0;
	std::string flt = filter ? filter : "";
	std::transform(flt.begin(), flt.end(), flt.begin(), ::tolower);
	const char* playermdl = CVAR_GET_STRING("model");
	for (auto iter = m_aryModelList.begin(); iter != m_aryModelList.end(); iter++) {
		if (m_pFavCheckButton->IsSelected() && !iter->second)
			continue;
		const char* mdl = iter->first.c_str();
		std::string str = mdl;
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		if ((flt.size() == 0) || (str.find(flt) != std::string::npos)) {
			if (!std::strcmp(mdl, playermdl))
				plr = counter;
			counter++;
			KeyValues kv(mdl, "modelname", mdl, "fav", iter->second ? FAVMODEL_ICON : "");
			m_pModelList->AddItem(&kv, 0, plr == counter, false);
		}
	}
	m_pModelList->SetSingleSelectedItem(plr);
}

void COptionsAdvanceSubMultiPlay::OnFavChange(int itemID, int add) {
	auto item = m_pModelList->GetItem(itemID);
	if (item) {
		bool bIsAdd = add == 1;
		std::string name = std::string(item->GetName());
		item->SetString("fav", bIsAdd ? FAVMODEL_ICON : "");
		auto it = m_aryModelList.find(name);
		if (it != m_aryModelList.end())
			it->second = bIsAdd;

		auto cfg = abcconfig::GetConfig();
		auto cfgit = std::find(cfg->m_aryFavModels.begin(), cfg->m_aryFavModels.end(), name);
		if (bIsAdd && cfgit == cfg->m_aryFavModels.end())
			cfg->m_aryFavModels.insert(name);
		else if (!bIsAdd && cfgit != cfg->m_aryFavModels.end())
			cfg->m_aryFavModels.erase(cfgit);
	}
}

void COptionsAdvanceSubMultiPlay::OnOpenContextMenu(int itemID) {
	if (itemID < 0)
		return;
	Menu* menu = new Menu(this, "FavMenu");
	menu->SetAutoDelete(true);
	menu->SetVisible(false);
	menu->AddActionSignalTarget(this);
	if (!std::strcmp(m_pModelList->GetItem(itemID)->GetString("fav"), FAVMODEL_ICON))
		menu->AddMenuItem("AddFav", "#GameUI_ABC_RemoveFav", new KeyValues("FavChange", "itemID", itemID, "add", 0), this);
	else
		menu->AddMenuItem("AddFav", "#GameUI_ABC_AddFav", new KeyValues("FavChange", "itemID", itemID, "add", 1), this);
	menu->PositionRelativeToPanel(this, Menu::CURSOR, 0, true);
	menu->MakePopup();
}

void COptionsAdvanceSubMultiPlay::OnFilterModelChecked(int state) {
	FilterModel();
}

void COptionsAdvanceSubMultiPlay::OnSliderMoved() {
	m_pModelViewer->SetModelRotate(0, m_pModelController->GetValue(), 0);

	m_pCrosshairDisplay->SetLength(m_pCrosshairLength->GetValue());
	m_pCrosshairDisplay->SetWidth(m_pCrosshairWidth->GetValue());
	m_pCrosshairDisplay->SetOffset(m_pCrosshairOffset->GetValue());
	Color crosshairColor(m_pCrosshairr->GetValue(), m_pCrosshairg->GetValue(), m_pCrosshairb->GetValue(), m_pCrosshaira->GetValue());
	m_pCrosshairDisplay->SetCrossHairColor(crosshairColor);
	Color outlineColor(m_pCrosshairOutliner->GetValue(), m_pCrosshairOutlineg->GetValue(), m_pCrosshairOutlineb->GetValue(), m_pCrosshairOutlinea->GetValue());
	m_pCrosshairDisplay->SetOutlineColor(outlineColor);
	m_pCrosshairDisplay->SetOutlineWidth(m_pCrosshairOutlineWidth->GetValue());
	m_pCrosshairDisplay->InvalidateLayout();
}

void COptionsAdvanceSubMultiPlay::OnItemSelected() {
	ChangeModel(m_pModelList->GetItem(m_pModelList->GetSelectedItem(0))->GetName());
}

void COptionsAdvanceSubMultiPlay::OnButtonChanged() {
	m_pCrosshairDisplay->SetDot(m_pCrosshairDot->IsSelected());
	m_pCrosshairDisplay->SetT(m_pCrosshairT->IsSelected());
	m_pCrosshairDisplay->SetOutline(m_pCrosshairOutline->IsSelected());
	m_pCrosshairDisplay->InvalidateLayout();
}

void COptionsAdvanceSubMultiPlay::OnFileSelected(const char* fullpath) {

	FIBITMAP* img = nullptr;
	wchar_t wszFullPath[MAX_PATH]{};
	Q_UTF8ToUnicode(fullpath, wszFullPath, MAX_PATH);

	auto fiFormat = FreeImage_GetFileTypeU(wszFullPath);

	if (!FreeImage_FIFSupportsReading(fiFormat))
		return;

	img = FreeImage_LoadU(fiFormat, wszFullPath, 0);

	if (img) {
		auto hFileHandle = g_pFullFileSystem->Open("tempdecal.wad", "wb");
		if (!hFileHandle) {
			FreeImage_Unload(img);
			return;
		}

		size_t w = FreeImage_GetWidth(img);
		size_t h = FreeImage_GetHeight(img);
		size_t nw = w;
		size_t nh = h;
		GetValidateSparySize(nw, nh);
		FIBITMAP* nimg = FreeImage_Rescale(img, nw, nh);
		FreeImage_Unload(img);

		size_t bpp = FreeImage_GetBPP(nimg);
		size_t pitch = FreeImage_GetPitch(nimg);
		//Quantize Transparent Pixel
		BYTE* bits = FreeImage_GetBits(nimg);
		size_t bitnum = bpp / 8;
		bits += pitch * (nh - 1);
		for (size_t y = 0; y < nh; y++) {
			BYTE* pixel = (BYTE*)bits;
			for (size_t x = 0; x < nw; x++) {
				switch (bitnum) {
				case 1: //8bpp
				case 2: //16bpp
				case 3: break; //24bpp
					//32bpp
				case 4: {
					BYTE alpha = pixel[FI_RGBA_ALPHA];
					if (alpha < 125) {
						pixel[FI_RGBA_RED] = 0;
						pixel[FI_RGBA_GREEN] = 0;
						pixel[FI_RGBA_BLUE] = 255;
					}
					pixel[FI_RGBA_ALPHA] = 255;
					break;
				}
					  //wtf
				default: break;
				}
				pixel += bitnum;
			}
			bits -= pitch;
		}

		int numPalette = 256;
		auto qimg = FreeImage_ColorQuantizeEx(nimg, FIQ_WUQUANT, numPalette);

		//swap pallete
		RGBQUAD* palette = FreeImage_GetPalette(qimg);

		int bluindex = -1;

		for (int i = 0; i < numPalette; i++) {
			RGBQUAD p = palette[i];
			if (p.rgbRed == 0 && p.rgbGreen == 0 && p.rgbBlue == 255) {
				auto tem = palette[255];
				palette[255] = palette[i];
				palette[i] = tem;
				bluindex = (int)i;
				break;
			}
		}

		if (bluindex == -1)
		{
			FreeImage_Unload(qimg);
			numPalette = 255;
			qimg = FreeImage_ColorQuantizeEx(nimg, FIQ_WUQUANT, numPalette);
		}

		//header
		//Magic
		g_pFullFileSystem->Write("WAD3", 4, hFileHandle);
		//Num of lump
		unsigned int headerbuf = 1;
		g_pFullFileSystem->Write(&headerbuf, 4, hFileHandle);
		//lump offset
		headerbuf = 0;
		g_pFullFileSystem->Write(&headerbuf, 4, hFileHandle);
		size_t size = nw * nh;
		//mips header
		BSPMipTexHeader_t header = { 0 };
		Q_strncpy(header.name, "{LOGO", 16);
		header.width = nw;
		header.height = nh;
		header.offsets[0] = sizeof(BSPMipTexHeader_t);
		header.offsets[1] = sizeof(BSPMipTexHeader_t) + size;
		header.offsets[2] = sizeof(BSPMipTexHeader_t) + size + (size / 4);
		header.offsets[3] = sizeof(BSPMipTexHeader_t) + size + (size / 4) + (size / 16);
		g_pFullFileSystem->Write(&header, sizeof(BSPMipTexHeader_t), hFileHandle);
		bits = FreeImage_GetBits(qimg);
		BYTE* flipped = new BYTE[size];
		for (size_t i = 0; i < nh; i++) {
			memcpy(flipped + i * nw, bits + (nh - i - 1) * nw, nw);
		}

		if (bluindex != -1) {
			for (size_t i = 0; i < size; i++) {
				if ((int)flipped[i] == bluindex)
					flipped[i] = 255;
				else if (flipped[i] == 255)
					flipped[i] = (BYTE)bluindex;
			}
		}

		//mips data
		const  static auto write_mips = [&](int mips_level) {
			int lv = pow(2, mips_level);
			for (size_t i = 0; i < (nh / lv); i++) {
				for (size_t j = 0; j < (nw / lv); j++) {
					BYTE buf = flipped[i * lv * nw + j * lv];
					g_pFullFileSystem->Write(&buf, 1, hFileHandle);
				}
			}
			};

		for (size_t i = 0; i < 4; i++) {
			write_mips(i);
		}

		delete[] flipped;
		short colorused = 256;
		g_pFullFileSystem->Write(&colorused, sizeof(short), hFileHandle);

		for (int i = 0; i < numPalette; i++) {
			RGBQUAD p = palette[i];
			g_pFullFileSystem->Write(&p.rgbRed, 1, hFileHandle);
			g_pFullFileSystem->Write(&p.rgbGreen, 1, hFileHandle);
			g_pFullFileSystem->Write(&p.rgbBlue, 1, hFileHandle);
		}

		if (numPalette == 255) {
			RGBQUAD p = { 255, 0, 0, 255 };
			g_pFullFileSystem->Write(&p.rgbRed, 1, hFileHandle);
			g_pFullFileSystem->Write(&p.rgbGreen, 1, hFileHandle);
			g_pFullFileSystem->Write(&p.rgbBlue, 1, hFileHandle);
		}

		//dummy pad
		headerbuf = g_pFullFileSystem->Tell(hFileHandle);
		const auto requiredPadding = [](int length, int padToMultipleOf) {
			int excess = length % padToMultipleOf;
			return excess == 0 ? 0 : padToMultipleOf - excess;
			};
		int padding = requiredPadding(headerbuf, 4);
		headerbuf = 0;
		for (int i = 0; i < padding; i++) {
			g_pFullFileSystem->Write(&headerbuf, 1, hFileHandle);
		}
		int lumpoffset = g_pFullFileSystem->Tell(hFileHandle);
		//lump
		int lumpsize = sizeof(BSPMipTexHeader_t) + size + (size / 4) + (size / 16) + (size / 64) + sizeof(short) + 256 * 3 + requiredPadding(2 + 256 * 3, 4);
		WAD3Lump_t lump = {
			sizeof(WAD3Header_t),
			lumpsize,
			lumpsize,
			0x43, //miptex
			0,
			0,
			"{LOGO\0"
		};
		g_pFullFileSystem->Write(&lump, sizeof(WAD3Lump_t), hFileHandle);
		g_pFullFileSystem->Seek(hFileHandle, 8, FILESYSTEM_SEEK_HEAD);
		g_pFullFileSystem->Write(&lumpoffset, 4, hFileHandle);
		g_pFullFileSystem->Close(hFileHandle);

		FreeImage_Unload(nimg);

		FreeImage_Unload(qimg);

		m_pSparyWad->Load("tempdecal.wad");

		WadTexture* tex = m_pSparyWad->Get("{logo");
		if (tex)
			SetSparyPixel(tex->GetPixels(), tex->Width(), tex->Height());
	}
}

void COptionsAdvanceSubMultiPlay::OnResetData() {
	ResetModel();
	BuildModelList();

	m_pCrosshairDisplay->SetLength(CVAR_GET_FLOAT("cl_crosshairsize"));
	m_pCrosshairDisplay->SetWidth(CVAR_GET_FLOAT("cl_crosshairthickness"));
	m_pCrosshairDisplay->SetOffset(CVAR_GET_FLOAT("cl_crosshairgap"));
	Color crosshairColor(CVAR_GET_FLOAT("cl_crosshaircolor_r"), CVAR_GET_FLOAT("cl_crosshaircolor_g"), CVAR_GET_FLOAT("cl_crosshaircolor_b"), CVAR_GET_FLOAT("cl_crosshairalpha"));
	m_pCrosshairDisplay->SetCrossHairColor(crosshairColor);
	Color outlineColor(CVAR_GET_FLOAT("cl_crosshaircolor_outline_r"), CVAR_GET_FLOAT("cl_crosshaircolor_outline_g"), CVAR_GET_FLOAT("cl_crosshaircolor_outline_b"), CVAR_GET_FLOAT("cl_crosshair_outline_alpha"));
	m_pCrosshairDisplay->SetOutlineColor(outlineColor);
	m_pCrosshairDisplay->SetOutline(CVAR_GET_FLOAT("cl_crosshair_outline_draw") > 0);
	m_pCrosshairDisplay->SetOutlineWidth(CVAR_GET_FLOAT("cl_crosshair_outline"));
	m_pCrosshairDisplay->SetDot(CVAR_GET_FLOAT("cl_crosshairdot") > 0);
	m_pCrosshairDisplay->SetT(CVAR_GET_FLOAT("cl_crosshair_t") > 0);
	m_pCrosshairDisplay->InvalidateLayout();

	WadTexture* tex = m_pSparyWad->Get("{logo");

	if (!tex) {
		m_pSparyWad->Load("tempdecal.wad");
		tex = m_pSparyWad->Get("{logo");
	}

	if (tex)
		SetSparyPixel(tex->GetPixels(), tex->Width(), tex->Height());

}

void COptionsAdvanceSubMultiPlay::OnApplyChanges() {
	BaseClass::OnApplyChanges();
	int mdlIndex = m_pModelList->GetSelectedItem(0);
	if (mdlIndex != -1)
		CVAR_SET_STRING("model", m_pModelList->GetItem(mdlIndex)->GetName());

	m_pCrosshairr->ApplyChanges();
	m_pCrosshairg->ApplyChanges();
	m_pCrosshairb->ApplyChanges();
	m_pCrosshaira->ApplyChanges();

	m_pCrosshairOutliner->ApplyChanges();
	m_pCrosshairOutlineg->ApplyChanges();
	m_pCrosshairOutlineb->ApplyChanges();
	m_pCrosshairOutlinea->ApplyChanges();

	m_pCrosshairWidth->ApplyChanges();
	m_pCrosshairLength->ApplyChanges();
	m_pCrosshairOutlineWidth->ApplyChanges();
	m_pCrosshairOffset->ApplyChanges();
	m_pCrosshairDot->ApplyChanges();
	m_pCrosshairT->ApplyChanges();
	m_pCrosshairOutline->ApplyChanges();
}

void COptionsAdvanceSubMultiPlay::OnCommand(const char* cmd) {
	if (!std::strcmp(cmd, "FilterModel"))
		FilterModel();
	else if (!std::strcmp(cmd, "OpenLoadSparyDialog")) {
		auto filedialog = new FileOpenDialog(this, "#GameUI_ABC_LoadSpary", FileOpenDialogType_t::FOD_OPEN, new KeyValues("FileInfo"));
		filedialog->SetProportional(false);
		filedialog->SetAutoDelete(true);
		filedialog->AddFilter("*.tga;*.png;*.bmp;*.jpg", "#GameUI_ABC_SparyFilter", true, "image");
		filedialog->Activate();
		int w, h;
		filedialog->GetSize(w, h);
		int screen_w, screen_h;
		vgui::surface()->GetScreenSize(screen_w, screen_h);
		filedialog->SetPos((screen_w - w) / 2, (screen_h - h) / 2);
		input()->SetAppModalSurface(filedialog->GetVPanel());
	}
}

void COptionsAdvanceSubMultiPlay::ApplySchemeSettings(IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);
	m_pModelViewer->SetBgColor(GetSchemeColor("ModelViewer/BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pModelController->SetBgColor(GetSchemeColor("ModelViewer/SliderBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pPlayerName->SetBgColor(GetSchemeColor("ModelViewer/NameTagBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}

void COptionsAdvanceSubMultiPlay::FilterModel() {

	char buf[MAX_PATH]{};

	m_pModelFilter->GetText(buf, sizeof(buf));

	if (std::strlen(buf) > 0) {
		m_pModelList->RemoveAll();
		m_pModelList->ResetScrollBar();
		BuildModelList(buf);
	}
	else {
		BuildModelList(nullptr);
	}
}

void COptionsAdvanceSubMultiPlay::GetValidateSparySize(size_t& ow, size_t& oh) {
	float w = static_cast<float>(ow);
	float h = static_cast<float>(oh);
	if (w * h > 14336.0f) {
		if (w > h) {
			h = h / w * 256.0f;
			w = 256.0f;
		}
		else {
			w = w / h * 256.0f;
			h = 256.0f;
		}
		while (w * h > 14436.0f) {
			w *= 0.95f;
			h *= 0.95f;
		}
	}
	w = static_cast<float>(static_cast<size_t>(w));
	h = static_cast<float>(static_cast<size_t>(h));
	float gap = 16.0f;
	float dw = fmodf(w, gap);
	if (dw > gap / 2.0f)
		w = w + (gap - dw);
	else
		w = w - dw;
	float dh = fmodf(h, gap);
	if (dh > gap / 2)
		h = h + (gap - dh);
	else
		h = h - dh;
	ow = static_cast<size_t>(w);
	oh = static_cast<size_t>(h);
}

void COptionsAdvanceSubMultiPlay::SetSparyPixel(unsigned char* pixels, size_t wide, size_t height) {
	m_pSpary->SetImage(new MemoryBitmap(pixels, wide, height));
}

class KeyBindingBox : public MessageBox {
public:
	KeyBindingBox(Panel* parent, const char* msg, Panel* target) : MessageBox(msg, msg, parent) {
		AddActionSignalTarget(parent);
		m_pTarget = target;
	}
	void OnKeyCodeTyped(KeyCode code) override {
		auto kv = new KeyValues("KeyBinded");
		kv->SetPtr("target", m_pTarget);
		kv->SetInt("code", (int)code);
		PostActionSignal(kv);
		Close();
	}
private:
	Panel* m_pTarget;
};

class KeyBindingButton : public Button {
public:
	KeyBindingButton(Panel* parent, const char* panelName, const char* text, Panel* pActionSignalTarget = NULL, const char* pCmd = NULL) :
		Button(parent, panelName, text, pActionSignalTarget, pCmd) {
	}
	int GetKeyCode() const {
		return m_iCode;
	}
	void SetKeyCode(int code) {
		m_iCode = code;
	}
private:
	int m_iCode = 0;
};

COptionsAdvanceSubOtherOption::COptionsAdvanceSubOtherOption(Panel* parent) : BaseClass(parent, "OptionsAdvanceSubOtherOption") {
	m_pNewHud = new CCvarToggleCheckButton(this, "NewHud", "#GameUI_ABC_Cvar_NewHud", "cl_hud_enable");
	m_pDynamicBackground = new CCvarToggleCheckButton(this, "DynamicBackground", "#GameUI_ABC_Cvar_DynamicBackground", "hud_dynamic_background");

	m_pScoreBoard = new GroupBox(this, "ScoreGroup", "#GameUI_ABC_ScoreOptions", 8);
	m_pScoreShowAvatar = new CCvarToggleCheckButton(this, "ScoreShowAvatar", "#GameUI_ABC_Cvar_ScoreShowAvatar", "hud_scoreboard_showavatars");
	m_pScoreShowLoss = new CCvarToggleCheckButton(this, "ScoreShowLoss", "#GameUI_ABC_Cvar_ScoreShowLoss", "hud_scoreboard_showloss");
	m_pScoreShowSteamId = new CCvarToggleCheckButton(this, "ScoreShowSteamId", "#GameUI_ABC_Cvar_ScoreShowSteamId", "hud_scoreboard_showsteamid");
	m_pScoreShowSteamIdType = new CCvarComboBox(this, "ScoreShowSteamIdType", "#GameUI_ABC_Cvar_ScoreShowSteamIdType", 2, "hud_scoreboard_showsteamidtype");
	m_pScoreShowSteamIdType->AddItem("#GameUI_ABC_SteamID", nullptr);
	m_pScoreShowSteamIdType->AddItem("#GameUI_ABC_SteamID64", nullptr);
	m_pScoreShowRealName = new CCvarToggleCheckButton(this, "ScoreShowRealName", "#GameUI_ABC_Cvar_ScoreShowRealName", "hud_scoreboard_showrealname");
	m_pScoreSpacingNormal = new CCvarLabelSlider(this, "ScoreSpacingNormal", "#GameUI_ABC_Cvar_ScoreSpacingNormal", "#GameUI_ABC_Cvar_ScoreSpacingNormal", 0, 10, "hud_scoreboard_spacing_normal");
	m_pScoreSpacingCompact = new CCvarLabelSlider(this, "ScoreSpacingCompact", "#GameUI_ABC_Cvar_ScoreSpacingCompact", "#GameUI_ABC_Cvar_ScoreSpacingCompact", 0, 10, "hud_scoreboard_spacing_compact");
	m_pScoreSize = new CCvarComboBox(this, "ScoreSize", "#GameUI_ABC_Cvar_ScoreSize", 3, "hud_scoreboard_size");
	m_pScoreSize->AddItem("#GameUI_ABC_Cvar_ScoreSizeAuto", nullptr);
	m_pScoreSize->AddItem("#GameUI_ABC_Cvar_ScoreSizeNormal", nullptr);
	m_pScoreSize->AddItem("#GameUI_ABC_Cvar_ScoreSizeCompact", nullptr);

	m_pPopNumber = new CCvarToggleCheckButton(this, "PopNumber", "#GameUI_ABC_Cvar_PopNumber", "hud_popnumber");

	m_pPlayerTitleGroup = new GroupBox(this, "PlayerTitleGroup", "#GameUI_ABC_PlayerTitleOptions", 2);
	m_pPlayerTitle = new CCvarToggleCheckButton(this, "PlayerTitle", "#GameUI_ABC_Cvar_PlayerTitle", "hud_popnumber");
	m_pPlayerTitleDanger = new CCvarLabelSlider(this, "PlayerTitleDanger", "#GameUI_ABC_Cvar_PlayerTitleDanger", "#GameUI_ABC_Cvar_PlayerTitleDanger", 0, 100, "hud_playerinfo_danger");

	m_pCameraGroup = new GroupBox(this, "CameraGroup", "#GameUI_ABC_CameraGroup", 2);
	m_pCameraHeightValue = new CCvarLabelSlider(this, "CameraHeight", "#GameUI_ABC_Cvar_CameraHeight", "#GameUI_ABC_Cvar_CameraHeight", -100, 100, "cam_idealheight");
	m_pCameraRightValue = new CCvarLabelSlider(this, "CameraRight", "#GameUI_ABC_Cvar_CameraRight", "#GameUI_ABC_Cvar_CameraRight", -100, 100, "cam_idealright");

	m_pModelLagGroup = new GroupBox(this, "ModelLagGroup", "#GameUI_ABC_ModelLagGroup", 7);
	m_pModelLag = new CCvarToggleCheckButton(this, "ModelLag", "#GameUI_ABC_Cvar_ModelLag", "cl_modellag");
	m_pModelLagAutoStop = new CCvarToggleCheckButton(this, "ModelLagAutoStop", "#GameUI_ABC_Cvar_ModelLagAutoStop", "cl_modellag_autostop");
	m_pModelLagAutoValue = new CCvarLabelSlider(this, "ModelLagValue", "#GameUI_ABC_Cvar_ModelLagValue", "#GameUI_ABC_Cvar_ModelLagValue", 0.0f, 5.0f, "cl_modellag_value", false, true);
	m_pModelSlide = new CCvarToggleCheckButton(this, "ModelSlide", "#GameUI_ABC_Cvar_ModelSlide", "cl_modelslide");
	m_pModelSlideAngle = new CCvarLabelSlider(this, "ModelSlideAngle", "#GameUI_ABC_Cvar_ModelSlideAngle", "#GameUI_ABC_Cvar_ModelSlideAngle", 0.0f, 90.0f, "cl_modelslide_angle", false, true);
	m_pModelSlideLength = new CCvarLabelSlider(this, "ModelSlideLength", "#GameUI_ABC_Cvar_ModelSlideLength", "#GameUI_ABC_Cvar_ModelSlideLength", -30.0f, 30.0f, "cl_modelslide_length");
	m_pModelSlideHeight = new CCvarLabelSlider(this, "ModelSlideHeight", "#GameUI_ABC_Cvar_ModelSlideHeight", "#GameUI_ABC_Cvar_ModelSlideHeight", -30.0f, 30.0f, "cl_modelslide_height");

	m_pHealthArmor = new GroupBox(this, "HPAPGroup", "#GameUI_ABC_HPAPGroup", 5);
	m_pDamageShock = new CCvarToggleCheckButton(this, "DamageShock", "#GameUI_ABC_Cvar_DamageShock", "hud_damageshock_factor");
	m_pDamageShockFactor = new CCvarLabelSlider(this, "DamageShockFactor", "#GameUI_ABC_Cvar_DamageShockFactor", "#GameUI_ABC_Cvar_DamageShockFactor", 0.0f, 0.1f, "hud_damageshock_factor", false, true);
	m_pDamageShockDmg = new CCvarLabelSlider(this, "DamageShockDmg", "#GameUI_ABC_Cvar_DamageShockDmg", "#GameUI_ABC_Cvar_DamageShockDmg", 0, 100, "hud_damageshock_base");
	m_pDangerHealth = new CCvarLabelSlider(this, "DangerHealth", "#GameUI_ABC_Cvar_DangerHealth", "#GameUI_ABC_Cvar_DangerHealth", 0, 100, "hud_danger_health");
	m_pDangerArmor = new CCvarLabelSlider(this, "DangerArmor", "#GameUI_ABC_Cvar_DangerArmor", "#GameUI_ABC_Cvar_DangerArmor", 0, 100, "hud_danger_armor");

	m_pItemHighLightGroup = new GroupBox(this, "ItemHighLightGroup", "#GameUI_ABC_ItemHighLightGroup", 2);
	m_pItemHighLight = new CCvarToggleCheckButton(this, "ItemHighLight", "#GameUI_ABC_Cvar_ItemHighLight", "hud_highlight_item");
	m_pItemHighLightRange = new CCvarLabelSlider(this, "ItemHighLightRange", "#GameUI_ABC_Cvar_ItemHighLightRange", "#GameUI_ABC_Cvar_ItemHighLightRange", 0, 344, "hud_highlight_item_range");

	m_pAutojump = new CCvarToggleCheckButton(this, "AutoJump", "#GameUI_ABC_Cvar_AutoJump", "cl_autojump");
	m_pRainbowMenu = new CCvarToggleCheckButton(this, "RainbowMenu", "#GameUI_ABC_Cvar_RainbowMenu", "hud_rainbow_menu");
	m_pEfxHud = new CCvarToggleCheckButton(this, "ExfHud", "#GameUI_ABC_Cvar_ExfHud", "hud_efx");
	m_pSidePanel = new CCvarToggleCheckButton(this, "SideHud", "#GameUI_ABC_Cvar_SideHud", "hud_sideinfo");
	m_pMotd = new CCvarComboBox(this, "Motd", "#GameUI_ABC_Cvar_Motd", 3, "hud_motd");
	m_pMotd->AddItem("#GameUI_ABC_Cvar_MotdNone", new KeyValues("value", "value", -1));
	m_pMotd->AddItem("#GameUI_ABC_Cvar_MotdVanillia", new KeyValues("value", "value", 0));
	m_pMotd->AddItem("#GameUI_ABC_Cvar_MotdNew", new KeyValues("value", "value", 1));

	m_pDeathNoticeGroup = new GroupBox(this, "DeathNoticeGroup", "#GameUI_ABC_DeathNoticeGroup", 3);
	m_pDeathNotice = new CCvarToggleCheckButton(this, "DeathNotice", "#GameUI_ABC_Cvar_DeathNotice", "hud_deathnotice");
	m_pDeathNoticeTime = new CCvarLabelSlider(this, "DeathNoticeTime", "#GameUI_ABC_Cvar_DeathNoticeTime", "#GameUI_ABC_Cvar_DeathNoticeTime", 0.0f, 30.0f, "hud_deathnotice_time", false, true);
	m_pDeathNoticeCount = new CCvarLabelSlider(this, "DeathNoticeCount", "#GameUI_ABC_Cvar_DeathNoticeCount", "#GameUI_ABC_Cvar_DeathNoticeCount", 0, 10, "hud_deathnotice_count");

	m_pGrenadeIndicatorGroup = new GroupBox(this, "GrenadeIndicatorGroup", "#GameUI_ABC_GrenadeIndicatorGroup", 3);
	m_pGrenadeIndicator = new CCvarToggleCheckButton(this, "GrenadeIndicator", "#GameUI_ABC_Cvar_GrenadeIndicator", "hud_indicator_grenade");
	m_pGrenadeIndicatorTime = new CCvarLabelSlider(this, "GrenadeIndicatorTime", "#GameUI_ABC_Cvar_GrenadeIndicatorTime", "#GameUI_ABC_Cvar_GrenadeIndicatorTime", 0.0f, 30.0f, "hud_indicator_grenade_time", false, true);
	m_pGrenadeIndicatorRange = new CCvarLabelSlider(this, "GrenadeIndicatorRange", "#GameUI_ABC_Cvar_GrenadeIndicatorRange", "#GameUI_ABC_Cvar_GrenadeIndicatorRange", 0, 1024, "hud_indicator_grenade_range");

	m_pVoteGroup = new GroupBox(this, "VoteGroup", "#GameUI_ABC_VoteGroup", 3);
	m_pVote = new CCvarToggleCheckButton(this, "Vote", "#GameUI_ABC_Cvar_Vote", "hud_vote");
	m_pVoteYes = new Label(this, "VoteYes", "#GameUI_ABC_Cvar_VoteYes");
	m_pVoteNo = new Label(this, "VoteNo", "#GameUI_ABC_Cvar_VoteNo");
	m_pVoteYesButton = new KeyBindingButton(this, "VoteBindingYes", "#GameUI_ABC_Cvar_BindgKeyBoard", this, "VoteYesBind");
	m_pVoteNoButton = new KeyBindingButton(this, "VoteBindingNo", "#GameUI_ABC_Cvar_BindgKeyBoard", this, "VoteNoBind");

	m_pEfxGroup = new GroupBox(this, "EfxGroup", "#GameUI_ABC_EfxGroup", 5);
	m_pShellEfx = new CCvarToggleCheckButton(this, "ShellEfx", "#GameUI_ABC_Cvar_ShellEfx", "abc_shellefx");
	m_pGaussEfx = new CCvarToggleCheckButton(this, "GaussEfx", "#GameUI_ABC_Cvar_GaussEfx", "abc_gaussefx");
	m_pBloodEfx = new CCvarToggleCheckButton(this, "BloodEfx", "#GameUI_ABC_Cvar_BloodEfx", "abc_bloodefx");
	m_pBloodSpriteSpeed = new CCvarLabelSlider(this, "BloodEfxSpeed", "#GameUI_ABC_Cvar_BloodEfxSpeed", "#GameUI_ABC_Cvar_BloodEfxSpeed", 0, 256, "abc_bloodsprite_speed");
	m_pBloodSpriteCount = new CCvarLabelSlider(this, "BloodEfxCount", "#GameUI_ABC_Cvar_BloodEfxCount", "#GameUI_ABC_Cvar_BloodEfxCount", 0, 64, "abc_bloodsprite_num");

	LoadControlSettings("abcenchance/res/gameui/OptionsAdvanceSubOtherOption.res");

	m_pNewHud->SetEnabled(false);
	m_pNewHud->SetSelected(true);
}

void COptionsAdvanceSubOtherOption::OnResetData() {
	BaseClass::OnResetData();
	m_pVoteYesButton->SetText(GameUIFuncs()->Key_NameForKey(CVAR_GET_FLOAT("hud_votekey_yes")));
	m_pVoteNoButton->SetText(GameUIFuncs()->Key_NameForKey(CVAR_GET_FLOAT("hud_votekey_no")));
}

void COptionsAdvanceSubOtherOption::OnApplyChanges() {
	//m_pNewHud->ApplyChanges();
	m_pDynamicBackground->ApplyChanges();

	m_pScoreShowAvatar->ApplyChanges();
	m_pScoreShowLoss->ApplyChanges();
	m_pScoreShowSteamId->ApplyChanges();
	m_pScoreShowSteamIdType->ApplyChanges();
	m_pScoreShowRealName->ApplyChanges();
	m_pScoreSpacingNormal->ApplyChanges();
	m_pScoreSpacingCompact->ApplyChanges();
	m_pScoreSize->ApplyChanges();

	m_pPopNumber->ApplyChanges();

	m_pPlayerTitle->ApplyChanges();
	m_pPlayerTitleDanger->ApplyChanges();

	m_pCameraHeightValue->ApplyChanges();
	m_pCameraRightValue->ApplyChanges();

	m_pModelLag->ApplyChanges();
	m_pModelLagAutoStop->ApplyChanges();
	m_pModelLagAutoValue->ApplyChanges();
	m_pModelSlide->ApplyChanges();
	m_pModelSlideAngle->ApplyChanges();
	m_pModelSlideLength->ApplyChanges();
	m_pModelSlideHeight->ApplyChanges();

	m_pDamageShock->ApplyChanges();
	m_pDamageShockFactor->ApplyChanges();
	m_pDamageShockDmg->ApplyChanges();
	m_pDangerHealth->ApplyChanges();
	m_pDangerArmor->ApplyChanges();

	m_pItemHighLight->ApplyChanges();
	m_pItemHighLightRange->ApplyChanges();

	m_pAutojump->ApplyChanges();
	m_pRainbowMenu->ApplyChanges();
	m_pEfxHud->ApplyChanges();
	m_pSidePanel->ApplyChanges();
	m_pMotd->ApplyChanges();

	m_pDeathNotice->ApplyChanges();
	m_pDeathNoticeTime->ApplyChanges();
	m_pDeathNoticeCount->ApplyChanges();

	m_pGrenadeIndicator->ApplyChanges();
	m_pGrenadeIndicatorTime->ApplyChanges();
	m_pGrenadeIndicatorRange->ApplyChanges();

	m_pVote->ApplyChanges();

	CVAR_SET_FLOAT("hud_votekey_yes", reinterpret_cast<KeyBindingButton*>(m_pVoteYesButton)->GetKeyCode());
	CVAR_SET_FLOAT("hud_votekey_no", reinterpret_cast<KeyBindingButton*>(m_pVoteNoButton)->GetKeyCode());

	m_pShellEfx->ApplyChanges();
	m_pGaussEfx->ApplyChanges();
	m_pBloodEfx->ApplyChanges();
	m_pBloodSpriteSpeed->ApplyChanges();
	m_pBloodSpriteCount->ApplyChanges();
}

void COptionsAdvanceSubOtherOption::OnKeyBinded(Panel* target, int code) {

	/*
	// build key to text translation table
	// first byte unshifted key
	// second byte shifted key
	// the rest is the name of the key
	_keyTrans[KEY_0]			="0)KEY_0";
	_keyTrans[KEY_1]			="1!KEY_1";
	_keyTrans[KEY_2]			="2@KEY_2";
	_keyTrans[KEY_3]			="3#KEY_3";
	_keyTrans[KEY_4]			="4$KEY_4";
	_keyTrans[KEY_5]			="5%KEY_5";
	_keyTrans[KEY_6]			="6^KEY_6";
	_keyTrans[KEY_7]			="7&KEY_7";
	_keyTrans[KEY_8]			="8*KEY_8";
	*/

	char buf[64]{};
	vgui::input()->GetKeyCodeText((KeyCode)code, buf, sizeof(buf));

	auto pKey = V_strstr(buf, "KEY_");

	wchar_t wbuf[32]{};
	if (pKey)
	{
		Q_UTF8ToUnicode(pKey + (sizeof("KEY_") - 1), wbuf, sizeof(wbuf));
	}
	else
	{
		Q_UTF8ToUnicode(buf, wbuf, sizeof(wbuf));
	}

	KeyBindingButton* btn = reinterpret_cast<KeyBindingButton*>(target);
	btn->SetText(wbuf);
	btn->SetKeyCode(GameUIFuncs()->Key_KeyStringToKeyNum(buf + 4));
}

void COptionsAdvanceSubOtherOption::OnCommand(const char* cmd) {
	static auto popkeybindbox = [](Panel* parent, Panel* target) {
		auto msgbox = new KeyBindingBox(parent, "#GameUI_ABC_Cvar_AnyKeyBinding", target);
		msgbox->SetAutoDelete(true);
		msgbox->SetProportional(false);
		int w, h;
		parent->GetSize(w, h);
		int screen_w, screen_h;
		vgui::surface()->GetScreenSize(screen_w, screen_h);
		msgbox->SetPos((screen_w - w) / 2, (screen_h - h) / 2);
		msgbox->Activate();
		msgbox->MakePopup();
		input()->SetAppModalSurface(msgbox->GetVPanel());
		};
	if (!Q_strcmp(cmd, "VoteYesBind"))
		popkeybindbox(this, m_pVoteYesButton);
	else if (!Q_strcmp(cmd, "VoteNoBind"))
		popkeybindbox(this, m_pVoteNoButton);
	else
		BaseClass::OnCommand(cmd);
}

COptionsAdvanceDialog::COptionsAdvanceDialog(Panel* parent) : BaseClass(parent, "OptionsAdvanceDialog") {
	SetDeleteSelfOnClose(true);
	SetSizeable(false);
	SetMoveable(false);
	SetProportional(true);
	SetScheme("OptionsAdvanceDialogScheme");
	m_pBlur = new GaussianBlurPanel(this, "BlurPanel");
	LoadControlSettings("abcenchance/res/gameui/OptionsAdvanceDialog.res");
	m_pMultiPlayPage = new COptionsAdvanceSubMultiPlay(this);
	m_pOtherOption = new COptionsAdvanceSubOtherOption(this);
	AddPage(m_pMultiPlayPage, "#GameUI_ABC_MultiPlayPage");
	AddPage(m_pOtherOption, "#GameUI_ABC_OtherOption");

	EnableApplyButton(true);
}
void COptionsAdvanceDialog::ApplySchemeSettings(IScheme* pScheme) {
	BaseClass::ApplySchemeSettings(pScheme);

	SetBgColor(GetSchemeColor("OptionsAdvanceDialog/BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	SetBorder(pScheme->GetBorder("DialogBorder"));

	auto sheet = GetPropertySheet();
	sheet->SetBgColor(GetSchemeColor("OptionsAdvanceDialog/SheetBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	sheet->SetBorder(pScheme->GetBorder("SheetBorder"));

	m_pMultiPlayPage->SetBgColor(GetSchemeColor("OptionsAdvanceDialog/SheetBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pMultiPlayPage->SetBorder(pScheme->GetBorder("SheetBorder"));

	//ofc
	m_pBlur->SetBgColor(Color(255, 255, 255, 255));
}