#include "glew.h"
#include "gl_utility.h"

void GL_CreateDepthStencilTexture(int texId, int w, int h)
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
	GL_CreateDepthStencilTexture(texid, w, h);
	return texid;
}

void GL_CreateTextureColorFormat(int texid, int w, int h, int iInternalFormat)
{
	glBindTexture(GL_TEXTURE_2D, texid);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexStorage2D(GL_TEXTURE_2D, 1, iInternalFormat, w, h);

	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint GL_GenTextureColorFormat(int w, int h, int iInternalFormat)
{
	GLuint texid = GL_GenTexture();
	GL_CreateTextureColorFormat(texid, w, h, iInternalFormat);
	return texid;
}

GLuint GL_GenTextureRGBA8(int w, int h)
{
	return GL_GenTextureColorFormat(w, h, GL_RGBA8);
}

GLuint GL_GenTextureRGB8(int w, int h)
{
	return GL_GenTextureColorFormat(w, h, GL_RGB8);
}

void GL_BlitFrameBufferToFrameBufferColorOnly(GLuint src, GLuint dst, int w1, int h1, int w2, int h2)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src);
	glBlitFramebuffer(0, 0, w1, h1, 0, 0, w2, h2, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}