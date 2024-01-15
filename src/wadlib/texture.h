#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

typedef struct WAD3Lump_s {
    int offset;
    int sizeOnDisk;
    int size;
    char type;
    char compression;
    char empty0;
    char empty1;
    char name[16];

} WAD3Lump_t;

class WadTexture
{
public:
    WadTexture(std::string const &name, class WadFile*package, unsigned char* miptexData, WAD3Lump_t& lump);
    std::string const &Name() const;
    class WadFile *Package() const;
    int Width() const;
    int Height() const;
    unsigned char* GetPixels();
private:
    unsigned char* GetPixelsFromRaw(unsigned char* miptexData);

    std::string m_szName;
    class WadFile* m_pPackage;
    int m_iWidth;
    int m_iHeight;
    unsigned char* m_pPixels;
    WAD3Lump_t m_pLump;
};

#endif // TEXTURE_H
