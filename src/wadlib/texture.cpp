#include "texture.h"
#include "FreeImage/FreeImage.h"

#pragma comment(lib,"FreeImage/FreeImage.lib")

typedef struct BSPMipTexHeader_s{
    char name[16];
    unsigned int width;
    unsigned int height;
    unsigned int offsets[4];

} BSPMipTexHeader_t;

WadTexture::WadTexture(std::string const &name, WadFile*package, unsigned char* miptexData, WAD3Lump_t& lump){
    m_szName = name;
    m_pPackage = package;
    m_pPixels = GetPixelsFromRaw(miptexData);
    memcpy(&m_pLump, &lump, sizeof(WAD3Lump_t));
 }

WadTexture::~WadTexture(){
    Clear();
}

unsigned char* WadTexture::GetPixels(){
    return m_pPixels;
}

size_t WadTexture::GetRawDataSize(){
    int size = m_iWidth * m_iHeight;
    return sizeof(BSPMipTexHeader_t) + size + (size / 4) + (size / 16) + (size / 64) + sizeof(short) + 256 * 3;
}

unsigned char* WadTexture::GetRawData() {
    int size = m_iWidth * m_iHeight;
    unsigned char* rawData = new unsigned char[GetRawDataSize()];
    BSPMipTexHeader_t header;
    std::strncpy(header.name, m_szName.c_str(), 16);
    header.width = m_iWidth;
    header.height = m_iHeight;
    header.offsets[0] = sizeof(BSPMipTexHeader_t);
    header.offsets[1] = sizeof(BSPMipTexHeader_t) + size;
    header.offsets[2] = sizeof(BSPMipTexHeader_t) + size + (size / 4);
    header.offsets[3] = sizeof(BSPMipTexHeader_t) + size + (size / 4) + (size / 16);
    memcpy(rawData, &header, sizeof(BSPMipTexHeader_t));
    FIBITMAP* img = FreeImage_ConvertFromRawBits(m_pPixels, m_iWidth, m_iHeight, m_iPitch, m_iBpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
    FIBITMAP* nimg = FreeImage_ColorQuantizeEx(img);
    FreeImage_Unload(img);
    //Palette
    RGBQUAD* palette = FreeImage_GetPalette(nimg);
    size_t palletstart = sizeof(BSPMipTexHeader_t) + size + (size / 4) + (size / 16) + (size / 64) + sizeof(short);
    for (size_t i = 0; i < 256;i++) {
        RGBQUAD* p = palette + i;
        size_t pi = i * 3 + palletstart;
        rawData[pi + 0] = p->rgbRed;
        rawData[pi + 1] = p->rgbGreen;
        rawData[pi + 2] = p->rgbBlue;
    }
    //mip 0
    BYTE* bits = FreeImage_GetBits(nimg);
    memcpy(rawData + sizeof(BSPMipTexHeader_t), bits, size);
    //mip 1
    img = FreeImage_Rescale(nimg, m_iWidth / 2, m_iHeight / 2);
    FreeImage_Unload(nimg);
    bits = FreeImage_GetBits(img);
    memcpy(rawData + sizeof(BSPMipTexHeader_t) + size, bits, size / 4);
    //mip 2
    nimg = FreeImage_Rescale(img, m_iWidth / 4, m_iHeight / 4);
    FreeImage_Unload(img);
    bits = FreeImage_GetBits(nimg);
    memcpy(rawData + sizeof(BSPMipTexHeader_t) + size + (size / 4), bits, size / 16);
    //mip 3
    img = FreeImage_Rescale(nimg, m_iWidth / 8, m_iHeight / 8);
    FreeImage_Unload(nimg);
    bits = FreeImage_GetBits(img);
    memcpy(rawData + sizeof(BSPMipTexHeader_t) + size + (size / 4) + (size / 16), bits, size / 64);
    FreeImage_Unload(img);
    return rawData;
}
unsigned char* WadTexture::GetPixelsFromRaw(unsigned char* miptexData){
    if (miptexData == nullptr)
        return nullptr;
    auto miptex = (BSPMipTexHeader_t*)miptexData;
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
    m_iBpp = bpp;
    m_iPitch = bpp * m_iWidth;
    return pixels;
}
void WadTexture::Clear(){
    if (m_pPixels)
        delete[] m_pPixels;
    m_pPixels = nullptr;
    m_iWidth = 0;
    m_iHeight = 0;
    m_iBpp = 0;
    m_iPitch = 0;

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

void WadTexture::SetPixels(unsigned char* ps, int w, int h, int bpp, int pitch){
    Clear();
    m_pPixels = ps;
    m_iWidth = w;
    m_iHeight = h;
    m_iBpp = bpp;
    m_iPitch = pitch;
}
