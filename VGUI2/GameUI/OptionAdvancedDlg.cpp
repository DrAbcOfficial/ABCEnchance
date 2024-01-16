#include <algorithm>

#include "interface.h"
#include "IFileSystem.h"

#include "vgui_controls/PropertySheet.h"
#include "vgui_controls/Label.h"
#include "vgui_controls/UrlLabel.h"
#include "vgui_controls/Slider.h"
#include "vgui_controls/ListPanel.h"
#include "vgui_controls/TextEntry.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/CrossHairDisplay.h"
#include "vgui_controls/CvarSlider.h"
#include "vgui_controls/CvarToggleCheckButton.h"
#include "vgui_controls/ImagePanel.h"
#include "vgui_controls/FileOpenDialog.h"

#include "FreeImage/FreeImage.h"
#include "wadlib/wadfile.h"

#include "vgui2/tga_image.h"

#include <GaussianBlurPanel.h>
#include <ModelViewPanel.h>

#include "OptionAdvancedDlg.h"
#include <filesystem>

#pragma comment(lib,"FreeImage/FreeImage.lib")

extern const char* CVAR_GET_STRING(const char* x);
extern void CVAR_SET_STRING(const char* x, const char* v);
extern float CVAR_GET_FLOAT(const char* x);
extern size_t ScreenWidth();
extern size_t ScreenHeight();

using namespace vgui;

COptionsAdvanceSubMultiPlay::COptionsAdvanceSubMultiPlay(Panel* parent) : BaseClass(parent, "OptionsAdvanceSubMultiPlay"){
	m_pModelViewer = new ModelViewPanel(this, "ModelViewer");
	m_pModelController = new Slider(this, "ModelController");
	m_pModelList = new ListPanel(this, "ModelList");
	m_pModelFilter = new TextEntry(this, "ModelFilter");
	m_pModelFilter->SendNewLine(true);
	m_pModelFilterButton = new Button(this, "ModelFilterButton", "#GameUI_ABC_SubmitModelFilter", this, "FilterModel");
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

	CTGAImage* bitmap = new CTGAImage();
	m_pSpary = new ImagePanel(this, "SparyImage");
	m_pSpary->SetImage(bitmap);
	m_pLoadSpary = new Button(this, "SparyLoadButton", "#GameUI_ABC_LoadSpary", this, "OpenLoadSparyDialog");

	m_pUrl = new URLLabel(this, "URLLable", "ABCEnchance  ||  " __DATE__, "https://github.com/DrAbcOfficial/ABCEnchance");

	LoadControlSettings("abcenchance/res/gameui/OptionsAdvanceSubMultiPlay.res");

	float y, r;
	m_pModelViewer->SetupTexBuffer();
	m_pModelViewer->GetModelRotate(r, y, r);

	m_pModelController->SetRange(0, 360);
	m_pModelController->AddActionSignalTarget(this);
	m_pModelController->SetValue(y, true);

	m_pModelList->AddColumnHeader(0, "modelname", "#GameUI_ABC_Model", 0, ListPanel::ColumnFlags_e::COLUMN_RESIZEWITHWINDOW);
	m_pModelList->AddActionSignalTarget(this);
	m_pModelList->SetMultiselectEnabled(false);

	m_pModelFilter->AddActionSignalTarget(this);
	m_pCrosshairDot->AddActionSignalTarget(this);
	m_pCrosshairT->AddActionSignalTarget(this);
	m_pCrosshairOutline->AddActionSignalTarget(this);
}
vgui::COptionsAdvanceSubMultiPlay::~COptionsAdvanceSubMultiPlay(){
	delete m_pSparyWad;
}
void COptionsAdvanceSubMultiPlay::ResetModel(){
	ChangeModel(CVAR_GET_STRING("model"));
}
void COptionsAdvanceSubMultiPlay::ChangeModel(const char* mdl){
	char path[MAX_PATH] = {};
	std::snprintf(path, MAX_PATH, "models/player/%s/%s.mdl", mdl, mdl);
	m_pModelViewer->ChangeModel(path);
}
void COptionsAdvanceSubMultiPlay::BuildModelList(const char* filter){
	FileFindHandle_t findHandle;
	auto pszFileName = filesystem()->FindFirst("models/player/*.*", &findHandle);
	if (!pszFileName)
		return;
	if (pszFileName){
		do{
			if (filesystem()->FindIsDirectory(findHandle) && *pszFileName != '.') {
				m_aryModelList.insert(pszFileName);
			}
		} while ((pszFileName = filesystem()->FindNext(findHandle)) != nullptr);
	}
	filesystem()->FindClose(findHandle);

	size_t counter = 0;
	size_t plr = 0;
	std::string flt = filter ? filter : "";
	std::transform(flt.begin(), flt.end(), flt.begin(), ::tolower);
	const char* playermdl = CVAR_GET_STRING("model");
	for (auto iter = m_aryModelList.begin(); iter != m_aryModelList.end(); iter++) {
		const char* mdl = iter->c_str();
		std::string str = mdl;
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		if ((flt.size() == 0) || (str.find(flt) != std::string::npos)) {
			if (!std::strcmp(mdl, playermdl))
				plr = counter;
			counter++;
			KeyValues kv(mdl, "modelname", mdl);
			m_pModelList->AddItem(&kv, 0, plr == counter, false);
		}
	}
	m_pModelList->SetSingleSelectedItem(plr);
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
void vgui::COptionsAdvanceSubMultiPlay::OnTextNewLine() {
	FilterModel();
}
void vgui::COptionsAdvanceSubMultiPlay::OnButtonChanged(){
	m_pCrosshairDisplay->SetDot(m_pCrosshairDot->IsSelected());
	m_pCrosshairDisplay->SetT(m_pCrosshairT->IsSelected());
	m_pCrosshairDisplay->SetOutline(m_pCrosshairOutline->IsSelected());
	m_pCrosshairDisplay->InvalidateLayout();
}
void vgui::COptionsAdvanceSubMultiPlay::OnFileSelected(const char* fullpath) {
	FIBITMAP* img = nullptr;
	char ext[4];
	std::strcpy(ext, std::filesystem::path(fullpath).extension().u8string().c_str());
	if (!std::strncmp(ext, ".tga", 4))
		img = FreeImage_Load(FREE_IMAGE_FORMAT::FIF_TARGA, fullpath, 0);
	else if(!std::strncmp(ext, ".bmp", 4))
		img = FreeImage_Load(FREE_IMAGE_FORMAT::FIF_BMP, fullpath, 0);
	else if (!std::strncmp(ext, ".jpg", 4))
		img = FreeImage_Load(FREE_IMAGE_FORMAT::FIF_JPEG, fullpath, 0);
	else if (!std::strncmp(ext, ".png", 4))
		img = FreeImage_Load(FREE_IMAGE_FORMAT::FIF_PNG, fullpath, 0);
	if (img) {
		std::ofstream stream;
		char wadpath[MAX_PATH];
		filesystem()->GetLocalPath("tempdecal.wad", wadpath, MAX_PATH);
		stream.open(wadpath, std::ios::out | std::ios::binary);
		if (!stream.is_open()) {
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
		img = FreeImage_ColorQuantizeEx(nimg, FIQ_WUQUANT, 256);
		FreeImage_Unload(nimg);
		//header
		stream.write("WAD3", 4);
		unsigned int headerbuf = 1;
		stream.write((char*)&headerbuf, 4);
		headerbuf = sizeof(WAD3Header_t);
		stream.write((char*)&headerbuf, 4);
		//lump
		size_t size = nw * nh;
		WAD3Lump_t lump;
		lump.compression = 0;
		lump.dummy = 0;
		lump.type = 0x43; //miptex
		std::strncpy(lump.name, "{LOGO", 16);
		lump.offset = sizeof(WAD3Header_t) + sizeof(WAD3Lump_t);
		lump.size = lump.sizeOnDisk = sizeof(BSPMipTexHeader_t) + size + (size / 4) + (size / 16) + (size / 64) + sizeof(short) + 256 * 3;
		stream.write((char*)&lump, sizeof(WAD3Lump_t));
		//mips header
		BSPMipTexHeader_t header;
		std::strncpy(header.name, "{LOGO", 16);
		header.width = nw;
		header.height = nh;
		header.offsets[0] = sizeof(BSPMipTexHeader_t);
		header.offsets[1] = sizeof(BSPMipTexHeader_t) + size;
		header.offsets[2] = sizeof(BSPMipTexHeader_t) + size + (size / 4);
		header.offsets[3] = sizeof(BSPMipTexHeader_t) + size + (size / 4) + (size / 16);
		stream.write((char*)&header, sizeof(BSPMipTexHeader_t));
		BYTE* bits = FreeImage_GetBits(img);
		BYTE* flipped = new BYTE[size];
		for (int i = 0; i < nh; i++) {
			memcpy(flipped + i * nw, bits + (nh - i - 1) * nw, nw);
		}
		//mips data
		auto write_mips = [&](int mips_level) {
			int lv = pow(2, mips_level);
			for (size_t i = 0; i < (nh/lv); i++) {
				for (size_t j = 0; j < (nw/lv); j++) {
					char buf = flipped[i * lv * nw + j * lv];
					stream.write((char*)&buf, 1);
				}
			}
		};
		for (size_t i = 0; i < 4; i++) {
			write_mips(i);
		}
		delete[] flipped;
		short dummy = 0;
		stream.write((char*)&dummy, sizeof(short));
		//Palette
		RGBQUAD* palette = FreeImage_GetPalette(img);
		for (size_t i = 0; i < 256; i++) {
			RGBQUAD p = palette[i];
			stream.write((char*)&p.rgbRed, 1);
			stream.write((char*)&p.rgbGreen, 1);
			stream.write((char*)&p.rgbBlue, 1);
		}
		stream.close();
		FreeImage_Unload(img);

		m_pSparyWad->Load(wadpath);
		WadTexture* tex = m_pSparyWad->Get("{logo");
		if (tex)
			SetSparyPixel(tex->GetPixels(), tex->Width(), tex->Height());
	}
}

void vgui::COptionsAdvanceSubMultiPlay::OnResetData(){
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
		char fullpath[MAX_PATH];
		filesystem()->GetLocalPath("tempdecal.wad", fullpath, MAX_PATH);
		m_pSparyWad->Load(fullpath);
		tex = m_pSparyWad->Get("{logo");
	}

	if(tex)
		SetSparyPixel(tex->GetPixels(), tex->Width(), tex->Height());
	
}

void vgui::COptionsAdvanceSubMultiPlay::OnApplyChanges(){
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

void vgui::COptionsAdvanceSubMultiPlay::OnCommand(const char* cmd){
	if (!std::strcmp(cmd, "FilterModel"))
		FilterModel();
	else if (!std::strcmp(cmd, "OpenLoadSparyDialog")) {
		if (m_pFileDialog) {
			delete m_pFileDialog;
			m_pFileDialog = nullptr;
		}
		m_pFileDialog = new FileOpenDialog(this, "#GameUI_ABC_LoadSpary", FileOpenDialogType_t::FOD_OPEN, new KeyValues("FileInfo"));
		m_pFileDialog->SetProportional(false);
		m_pFileDialog->AddFilter("*.tga;*.png;*.bmp;*.jpg", "#GameUI_ABC_SparyFilter", true, "image");
		m_pFileDialog->Activate();
		int w, h;
		m_pFileDialog->GetSize(w, h);
		m_pFileDialog->SetPos((ScreenWidth() - w) / 2, (ScreenHeight() - h) / 2);
	}
}

void COptionsAdvanceSubMultiPlay::ApplySchemeSettings(IScheme* pScheme){
	BaseClass::ApplySchemeSettings(pScheme);
	m_pModelViewer->SetBgColor(GetSchemeColor("ModelViewer/BgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pModelController->SetBgColor(GetSchemeColor("ModelViewer/SliderBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
	m_pPlayerName->SetBgColor(GetSchemeColor("ModelViewer/NameTagBgColor", GetSchemeColor("Panel.BgColor", pScheme), pScheme));
}

void vgui::COptionsAdvanceSubMultiPlay::FilterModel(){
	char buf[MAX_PATH];
	m_pModelFilter->GetText(buf, MAX_PATH);
	if (std::strlen(buf) > 0) {
		m_pModelList->RemoveAll();
		m_pModelList->ResetScrollBar();
		BuildModelList(buf);
	}
	else
		BuildModelList(nullptr);
}

void vgui::COptionsAdvanceSubMultiPlay::GetValidateSparySize(size_t& ow, size_t& oh){
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
	w = static_cast<size_t>(w);
	h = static_cast<size_t>(h);
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

void vgui::COptionsAdvanceSubMultiPlay::SetSparyPixel(unsigned char* pixels, size_t wide, size_t height){
	CTGAImage* img = reinterpret_cast<CTGAImage*>(m_pSpary->GetImage());
	vgui::surface()->DrawSetTextureRGBA(img->GetTextureId(), pixels, wide, height, true, false);
	img->SetSize(wide, height);
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
	AddPage(m_pMultiPlayPage, "#GameUI_ABC_MultiPlayPage");
}
void COptionsAdvanceDialog::Activate(){
	BaseClass::Activate();
	input()->SetAppModalSurface(GetVPanel());
}
void COptionsAdvanceDialog::ApplySettings(KeyValues* inResourceData){
	BaseClass::ApplySettings(inResourceData);
}
void COptionsAdvanceDialog::ApplySchemeSettings(IScheme* pScheme){
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
