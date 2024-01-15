#ifndef WADFILE_H
#define WADFILE_H

#include "texturepackage.h"

class WadFile : public TexturePackage
{
public:
    WadFile();

    virtual bool load(std::string const &filePath, class TextureManager *textureManager);
};

#endif // WADFILE_H
