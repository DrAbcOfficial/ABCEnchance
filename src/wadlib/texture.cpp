#include "texture.h"
#include "texturepackage.h"

typedef struct sBSPMipTexHeader
{
    char name[16];
    unsigned int width;
    unsigned int height;
    unsigned int offsets[4];

} tBSPMipTexHeader;

Texture::Texture(std::string const &name, TexturePackage *package, unsigned char* miptexData)
    : _name(name), _package(package), _pixels(GetPixelsFromRaw(miptexData)){
 }

unsigned char* Texture::GetPixels(){
    return _pixels;
}

unsigned char* Texture::GetPixelsFromRaw(unsigned char* miptexData){
    if (miptexData == nullptr)
    {
        return nullptr;
    }

    auto miptex = (tBSPMipTexHeader*)miptexData;

    if (miptex->height <= 0 || miptex->height > 1024 || miptex->width <= 0 || miptex->width > 1024)
    {
        return nullptr;
    }

    _width = miptex->width;
    _height = miptex->height;

    const int bpp = 4;
    int size = miptex->width * miptex->height;
    int paletteOffset = miptex->offsets[0] + size + (size / 4) + (size / 16) + (size / 64) + sizeof(short);

    // Get the miptex data and palette
    auto source0 = miptexData + miptex->offsets[0];
    auto palette = miptexData + paletteOffset;

    auto pixels = new unsigned char[size * bpp];

    unsigned char r, g, b, a;
    for (int i = 0; i < size; i++)
    {
        r = palette[source0[i] * 3];
        g = palette[source0[i] * 3 + 1];
        b = palette[source0[i] * 3 + 2];
        a = 255;

        // Do we need a transparent pixel
        if (miptex->name[0] == '{' && source0[i] == 255)
            r = g = b = a = 0;

        pixels[i * bpp + 0] = r;
        pixels[i * bpp + 1] = g;
        pixels[i * bpp + 2] = b;
        pixels[i * bpp + 3] = a;
    }

    return pixels;
}

std::string const &Texture::Name() const
{
    return _name;
}

TexturePackage const *Texture::Package() const
{
    return _package;
}

int Texture::Width() const
{
    return _width;
}

int Texture::Height() const
{
    return _height;
}