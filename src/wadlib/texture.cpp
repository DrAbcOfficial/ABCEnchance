#include "texture.h"

typedef struct sBSPMipTexHeader{
    char name[16];
    unsigned int width;
    unsigned int height;
    unsigned int offsets[4];

} tBSPMipTexHeader;

WadTexture::WadTexture(std::string const &name, WadFile*package, unsigned char* miptexData, WAD3Lump_t& lump){
    m_szName = name;
    m_pPackage = package;
    m_pPixels = GetPixelsFromRaw(miptexData);
    memcpy(&m_pLump, &lump, sizeof(WAD3Lump_t));
 }

unsigned char* WadTexture::GetPixels(){
    return m_pPixels;
}

unsigned char* WadTexture::GetPixelsFromRaw(unsigned char* miptexData){
    if (miptexData == nullptr)
        return nullptr;
    auto miptex = (tBSPMipTexHeader*)miptexData;
    if (miptex->height <= 0 || miptex->height > 1024 || miptex->width <= 0 || miptex->width > 1024)
        return nullptr;
    m_iWidth = miptex->width;
    m_iHeight = miptex->height;

    const int bpp = 4;
    int size = miptex->width * miptex->height;
    int paletteOffset = miptex->offsets[0] + size + (size / 4) + (size / 16) + (size / 64) + sizeof(short);
    auto source0 = miptexData + miptex->offsets[0];
    auto palette = miptexData + paletteOffset;
    auto pixels = new unsigned char[size * bpp];
    unsigned char r, g, b, a;
    for (int i = 0; i < size; i++){
        r = palette[source0[i] * 3];
        g = palette[source0[i] * 3 + 1];
        b = palette[source0[i] * 3 + 2];
        a = 255;
        if (miptex->name[0] == '{' && source0[i] == 255)
            r = g = b = a = 0;
        pixels[i * bpp + 0] = r;
        pixels[i * bpp + 1] = g;
        pixels[i * bpp + 2] = b;
        pixels[i * bpp + 3] = a;
    }
    return pixels;
}
std::string const &WadTexture::Name() const{
    return m_szName;
}
WadFile *WadTexture::Package() const{
    return m_pPackage;
}
int WadTexture::Width() const{
    return m_iWidth;
}
int WadTexture::Height() const{
    return m_iHeight;
}