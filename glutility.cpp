#include "glew.h"
#include "glutility.h"
void GL_UploadDepthStencilTexture(int texId, int w, int h)
{
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, w, h);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint GL_GenTexture(void)
{
	GLuint tex;
	glGenTextures(1, &tex);
	return tex;
}

GLuint GL_GenDepthStencilTexture(int w, int h)
{
	GLuint texid = GL_GenTexture();
	GL_UploadDepthStencilTexture(texid, w, h);
	return texid;
}
void GL_UploadTextureColorFormat(int texid, int w, int h, int iInternalFormat)
{
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//glTexStorage2D doesnt work with qglCopyTexImage2D so we use glTexImage2D here
	glTexImage2D(GL_TEXTURE_2D, 0, iInternalFormat, w, h, 0, GL_RGBA,
		(iInternalFormat != GL_RGBA && iInternalFormat != GL_RGBA8) ? GL_FLOAT : GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}
GLuint GL_GenTextureColorFormat(int w, int h, int iInternalFormat)
{
	GLuint texid = GL_GenTexture();
	GL_UploadTextureColorFormat(texid, w, h, iInternalFormat);
	return texid;
}
GLuint GL_GenTextureRGBA8(int w, int h)
{
	return GL_GenTextureColorFormat(w, h, GL_RGBA8);
}