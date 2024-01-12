#ifndef IGAMEUIFUNCS_H
#define IGAMEUIFUNCS_H

#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

typedef struct vmode_s{
	int width;
	int height;
	int bpp;
} vmode_t;

class IGameUIFuncs : public IBaseInterface
{
public:
	virtual bool IsKeyDown(char const *keyname, bool &isdown) = 0;
	virtual const char *Key_NameForKey(int keynum) = 0;
	virtual const char *Key_BindingForKey(int keynum) = 0;
	virtual vgui::KeyCode GetVGUI2KeyCodeForBind(const char *bind) = 0;
	virtual void GetVideoModes(vmode_t **liststart, int *count) = 0;
	virtual void GetCurrentVideoMode(int *wide, int *tall, int *bpp) = 0;
	virtual void GetCurrentRenderer(char *name, int namelen, int *windowed) = 0;
	virtual bool IsConnectedToVACSecureServer(void) = 0;
	virtual int Key_KeyStringToKeyNum(const char *string) = 0;
};

#define VENGINE_GAMEUIFUNCS_VERSION "VENGINE_GAMEUIFUNCS_VERSION001"

#endif