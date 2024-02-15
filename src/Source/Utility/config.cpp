#include "rapidjson/document.h"

#include "rapidjson/stringbuffer.h"
#include <rapidjson/writer.h>

#include "interface.h"
#include "IFileSystem.h"
#include "Controls.h"

#include "config.h"



CABCConfig* g_pcfg;
void CABCConfig::Init(){
	g_pcfg = new CABCConfig();
}
CABCConfig* CABCConfig::GetConfig(){
	return g_pcfg;
}
void CABCConfig::Close(){
	g_pcfg->Save();
	delete g_pcfg;
}
CABCConfig::CABCConfig(){
	Load("abcenchance/config.json");
}
CABCConfig::CABCConfig(const char* path){
	Load(path);
}
void CABCConfig::Load(const char* path){
	m_szSavePath = path;
	FileHandle_t file = vgui::filesystem()->Open(path, "r");
	if (!file) {
		Warning("Couldn't open config file %s\n", path);
		return;
	}
	int size = vgui::filesystem()->Size(file);
	char* pMem = reinterpret_cast<char*>(malloc(size + 1));
	int bytesRead = vgui::filesystem()->Read(pMem, size, file);
	pMem[bytesRead] = 0;
	vgui::filesystem()->Close(file);
	rapidjson::Document json;
	json.Parse(pMem, bytesRead);
	//Load from json
	if (json.HasMember("favmodels")) {
		auto arr = json["favmodels"].GetArray();
		for (auto iter = arr.begin(); iter != arr.end(); iter++) {
			m_aryFavModels.push_back((*iter).GetString());
		}
	}
	//end
	free(pMem);
}
void CABCConfig::Save(){
	rapidjson::Document data;
	data.SetObject();
	rapidjson::Document::AllocatorType& allocator = data.GetAllocator();
	rapidjson::Value favmodels(rapidjson::kArrayType);
	for (auto iter = m_aryFavModels.begin(); iter != m_aryFavModels.end(); iter++) {
		rapidjson::Value val((*iter).c_str(), allocator);
		favmodels.PushBack(val, allocator);
	}
	data.AddMember("favmodels", favmodels, allocator);
	
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	data.Accept(writer);
	FileHandle_t file = vgui::filesystem()->Open(m_szSavePath.c_str(), "w");
	vgui::filesystem()->Write(buffer.GetString(), buffer.GetSize(), file);
	vgui::filesystem()->Close(file);
}