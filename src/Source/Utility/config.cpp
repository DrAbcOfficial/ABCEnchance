#include "interface.h"
#include "IFileSystem.h"
#include "Controls.h"

#include "config.h"

extern void ConsoleWarnMessage(const char* format, ...);
constexpr char g_szConfigPath[] = "abcenchance/config.json";
static CABCConfig s_cfg;
void abcconfig::LoadJson(){
	FileHandle_t file = vgui::filesystem()->Open(g_szConfigPath, "r");
	if (!file) {
		ConsoleWarnMessage("Couldn't open config file %s\n", g_szConfigPath);
		return;
	}
	int size = vgui::filesystem()->Size(file);
	char* pMem = reinterpret_cast<char*>(malloc(size + 1));
	int bytesRead = vgui::filesystem()->Read(pMem, size, file);
	pMem[bytesRead] = 0;
	vgui::filesystem()->Close(file);
	aigc::JsonHelper::JsonToObject(s_cfg, pMem);
	free(pMem);
}

void abcconfig::SaveJson(){
	FileHandle_t file = vgui::filesystem()->Open(g_szConfigPath, "w");
	std::string json = "";
	aigc::JsonHelper::ObjectToJson(s_cfg, json);
	vgui::filesystem()->Write(json.c_str(), json.size(), file);
	vgui::filesystem()->Close(file);
}

CABCConfig* abcconfig::GetConfig(){
	return &s_cfg;
}
