#ifndef TEXTUREPACKAGE_H
#define TEXTUREPACKAGE_H

#include <string>

class TexturePackage
{
protected:
    std::string _filePath;

public:
    TexturePackage();

    std::string const &filePath() const;

    virtual bool load(std::string const &filePath, class TextureManager *textureManager) = 0;
};

#endif // TEXTUREPACKAGE_H
