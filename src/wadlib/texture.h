#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>

class Texture
{
protected:
    std::string _name;
    class TexturePackage *_package;
    int _width;
    int _height;
    unsigned char* _pixels;
public:
    Texture(std::string const &name, class TexturePackage *package, unsigned char* miptexData);
    unsigned char* GetPixels();
    std::string const &Name() const;
    class TexturePackage const *Package() const;
    int Width() const;
    int Height() const;
private:
    unsigned char* GetPixelsFromRaw(unsigned char* miptexData);
};

#endif // TEXTURE_H
