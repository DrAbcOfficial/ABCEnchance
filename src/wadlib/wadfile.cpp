#include "wadfile.h"
#include "texturemanager.h"
#include "texture.h"
#include <fstream>

#define HL1_WAD3_SIGNATURE "WAD3"

/* WAD3 */
typedef struct sWAD3Header
{
    char signature[4];
    int lumpsCount;
    int lumpsOffset;

} tWAD3Header;

typedef struct sWAD3Lump
{
    int offset;
    int sizeOnDisk;
    int size;
    char type;
    char compression;
    char empty0;
    char empty1;
    char name[16];

} tWAD3Lump;

WadFile::WadFile()
{
}

bool WadFile::load(const std::string &filePath, TextureManager *textureManager)
{
    if (filePath == "")
    {
        return false;
    }

    if (textureManager == nullptr)
    {
        return false;
    }

    _filePath = filePath;

    std::ifstream stream;

    stream.open(filePath, std::ios::in | std::ios::binary);

    if (!stream.is_open())
    {
        return false;
    }

    tWAD3Header header;

    stream.read((char *)&header, sizeof(tWAD3Header));

    if (std::string(header.signature, 4) != HL1_WAD3_SIGNATURE)
    {
        return false;
    }

    tWAD3Lump *lumps = new tWAD3Lump[header.lumpsCount];

    stream.seekg(header.lumpsOffset, std::ios::beg);
    stream.read((char *)lumps, header.lumpsCount * sizeof(tWAD3Lump));

    for (int i = 0; i < header.lumpsCount; i++)
    {
        // We read the lump data
        unsigned char *lumpData = new unsigned char[lumps[i].size];
        stream.seekg(lumps[i].offset, std::ios::beg);
        stream.read((char *)lumpData, lumps[i].size);

        if (!stream)
        {
            continue;
        }

        // And pass the data to the texture class who now owns the data and should cleanup
        auto texture = new Texture(lumps[i].name, this, lumpData);
        textureManager->addTexture(texture);

        delete[] lumpData;
    }

    stream.close();

    delete[] lumps;

    return true;
}
