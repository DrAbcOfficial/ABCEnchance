#ifndef WADFILE_H
#define WADFILE_H
#include <string>
#include <vector>

#include "texture.h"

/* WAD3 */
typedef struct WAD3Header_s {
    char signature[4];
    unsigned int lumpsCount;
    unsigned int lumpsOffset;
} WAD3Header_t;

class WadFile{
public:
    WadFile();
    ~WadFile();
    std::string FilePath();
    WadTexture* Get(std::string name, bool ignoreCase = true);
    bool Load(std::string const &filePath);
    bool SaveToFile(std::string const& filePath);
private:
    void Clear();
    std::string m_szFilePath;
    std::vector<WadTexture*> m_aryTextures;
    WAD3Header_t m_pHeader;
    WAD3Lump_t* m_aryLumps;
};

#endif // WADFILE_H
