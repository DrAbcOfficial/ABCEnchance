#pragma once
void GL_UploadDepthStencilTexture(int texId, int w, int h);
GLuint GL_GenTexture(void);
GLuint GL_GenDepthStencilTexture(int w, int h);
void GL_UploadTextureColorFormat(int texid, int w, int h, int iInternalFormat);
GLuint GL_GenTextureColorFormat(int w, int h, int iInternalFormat);
GLuint GL_GenTextureRGBA8(int w, int h);
GLuint GL_GenTextureRGB8(int w, int h);
void GL_BlitFrameBufferToFrameBufferColorOnly(GLuint src, GLuint dst, int w1, int h1, int w2, int h2);