#include <fstream>
#include <algorithm>

#include "wadfile.h"
#include "texture.h"

#define HL1_WAD3_SIGNATURE "WAD3"


WadFile::WadFile(){
}
WadFile::~WadFile(){
    Clear();
    delete[] m_aryLumps;
}
std::string WadFile::FilePath(){
    return m_szFilePath;
}
WadTexture* WadFile::Get(std::string name, bool ignoreCase){
    for (auto iter = m_aryTextures.begin(); iter != m_aryTextures.end(); iter++) {
        std::string tn = (*iter)->Name();
        std::string nn = name;
        if (ignoreCase) {
            std::transform(tn.begin(), tn.end(), tn.begin(), std::tolower);
            std::transform(nn.begin(), nn.end(), nn.begin(), std::tolower);
        }
        if (tn == nn) {
            return *iter;
        }
    }
    return nullptr;
}
bool WadFile::Load(const std::string &filePath){
    Clear();
    if (filePath == "")
        return false;
    m_szFilePath = filePath;
    std::ifstream stream;
    stream.open(filePath, std::ios::in | std::ios::binary);
    if (!stream.is_open())
        return false;
    WAD3Header_t header;
    stream.read((char *)&header, sizeof(WAD3Header_t));
    if (std::string(header.signature, 4) != HL1_WAD3_SIGNATURE)
        return false;
    m_aryLumps = new WAD3Lump_t[header.lumpsCount];
    stream.seekg(header.lumpsOffset, std::ios::beg);
    stream.read((char *)m_aryLumps, header.lumpsCount * sizeof(WAD3Lump_t));
    for (int i = 0; i < header.lumpsCount; i++){
        unsigned char *lumpData = new unsigned char[m_aryLumps[i].size];
        stream.seekg(m_aryLumps[i].offset, std::ios::beg);
        stream.read((char *)lumpData, m_aryLumps[i].size);
        if (!stream)
            continue;
        m_aryTextures.push_back(new WadTexture(m_aryLumps[i].name, this, lumpData, m_aryLumps[i]));
        delete[] lumpData;
    }
    stream.close();
    m_pHeader = header;
    return true;
}

void WadFile::Clear(){
    if (m_aryTextures.size() > 0) {
        for (auto iter = m_aryTextures.begin(); iter != m_aryTextures.end(); iter++) {
            delete* iter;
        }
        m_aryTextures.clear();
    }
}
