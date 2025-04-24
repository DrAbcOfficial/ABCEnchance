#include <fstream>
#include <algorithm>

#include "wadfile.h"
#include "texture.h"

#include <interface.h>
#include <IFileSystem.h>
extern IFileSystem* g_pFullFileSystem;

#define HL1_WAD3_SIGNATURE "WAD3"


WadFile::WadFile(){
    m_pHeader = { 0 };
    m_aryLumps = nullptr;
}

WadFile::~WadFile(){
    Clear();

    if (m_aryLumps) {
        delete[] m_aryLumps;
        m_aryLumps = nullptr;
    }
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

    auto hFileHandle = g_pFullFileSystem->Open(filePath.c_str(), "rb");

    if (!hFileHandle)
        return false;

    WAD3Header_t header;
    g_pFullFileSystem->Read(&header, sizeof(WAD3Header_t), hFileHandle);
    if (std::string(header.signature, 4) != HL1_WAD3_SIGNATURE)
        return false;
    m_aryLumps = new WAD3Lump_t[header.lumpsCount];
    g_pFullFileSystem->Seek(hFileHandle, header.lumpsOffset, FILESYSTEM_SEEK_HEAD);
    g_pFullFileSystem->Read(m_aryLumps, header.lumpsCount * sizeof(WAD3Lump_t), hFileHandle);
    for (size_t i = 0; i < header.lumpsCount; i++){
        unsigned char *lumpData = new unsigned char[m_aryLumps[i].size];
        g_pFullFileSystem->Seek(hFileHandle, m_aryLumps[i].offset, FILESYSTEM_SEEK_HEAD);
        g_pFullFileSystem->Read(lumpData, m_aryLumps[i].size, hFileHandle);
        if (g_pFullFileSystem->EndOfFile(hFileHandle))
            break;
        m_aryTextures.push_back(new WadTexture(m_aryLumps[i].name, this, lumpData, m_aryLumps[i]));
        delete[] lumpData;
    }
    g_pFullFileSystem->Close(hFileHandle);
    m_pHeader = header;
    return true;
}

bool WadFile::SaveToFile(std::string const& filePath){
    std::ofstream stream;
    stream.open(filePath, std::ios::out | std::ios::binary);
    if (!stream.is_open())
        return false;
    //header
    stream.write(HL1_WAD3_SIGNATURE, 4);
    unsigned int buf = m_aryTextures.size();
    stream.write((char*)&buf, 4);
    buf = sizeof(WAD3Header_t);
    stream.write((char*)&buf, 4);
    
    size_t offset = 12;
    size_t lastsize = 0;
    //lump
    for (auto iter = m_aryTextures.begin(); iter != m_aryTextures.end(); iter++) {
        WAD3Lump_t lump;
        lump.compression = 0;
        lump.dummy = 0;
        lump.type = 0x43; //miptex
        strncpy(lump.name, (*iter)->Name().c_str(), sizeof(lump.name) - 1);
        lump.name[sizeof(lump.name) - 1] = 0;
        lump.offset = sizeof(WAD3Header_t) + m_aryTextures.size() * sizeof(WAD3Lump_t) + lastsize;
        lump.size = lump.sizeOnDisk = (*iter)->GetRawDataSize();
        lastsize += lump.size;
        stream.write((char*)&lump, sizeof(WAD3Lump_t));
    }
    for (auto iter = m_aryTextures.begin(); iter != m_aryTextures.end(); iter++) {
        (*iter)->GetRawData(stream);
    }
    stream.close();
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
