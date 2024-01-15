#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

typedef struct WAD3Lump_s {
    int offset;
    int sizeOnDisk;
    int size;
    char type;
    bool compression;
    short dummy;
    char name[16];
} WAD3Lump_t;

class WadTexture
{
public:
    WadTexture(std::string const &name, class WadFile*package, unsigned char* miptexData, WAD3Lump_t& lump);
    ~WadTexture();
    void Clear();
    std::string const &Name() const;
    class WadFile *Package() const;
    int Width() const;
    int Height() const;
    void SetPixels(unsigned char* ps, int w, int h, int bpp, int pitch);
    unsigned char* GetPixels();
    unsigned char* GetRawData();
    size_t GetRawDataSize();
private:
    unsigned char* GetPixelsFromRaw(unsigned char* miptexData);

    std::string m_szName;
    class WadFile* m_pPackage;
    int m_iWidth;
    int m_iHeight;
    int m_iBpp;
    int m_iPitch;
    unsigned char* m_pPixels = nullptr;
    WAD3Lump_t m_pLump;
};

#endif // TEXTURE_H
