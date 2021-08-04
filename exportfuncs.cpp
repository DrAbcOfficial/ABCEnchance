#include <metahook.h>
#include "cl_entity.h"
#include "cvardef.h"
#include "exportfuncs.h"
#include "mathlib.h"
#include "com_model.h"
#include "palette.h"

cl_refHookfunc_t gHookFuncs;
cl_enginefunc_t gEngfuncs;
cl_exportfuncs_t gExportfuncs;
PVOID g_dwEngineBase;
DWORD g_dwEngineSize;
DWORD g_dwEngineBuildnum;

void Sys_ErrorEx(const char *fmt, ...)
{
	char msg[4096] = { 0 };

	va_list argptr;

	va_start(argptr, fmt);
	_vsnprintf(msg, sizeof(msg), fmt, argptr);
	va_end(argptr);

	if (gEngfuncs.pfnClientCmd)
		gEngfuncs.pfnClientCmd("escape\n");

	MessageBox(NULL, msg, "Fatal Error", MB_ICONERROR);
	TerminateProcess((HANDLE)(-1), 0);
}

void FillDelegate()
{
	Fill_DelegateFunc(CL_TempEntAllocHigh);
	Fill_DelegateFunc(CL_TempEntAlloc);
}

void FillAddress()
{
	//DWORD addr = (DWORD)Search_Pattern("\x6A\x03\x6A\x01\xE8\x2A\x2A\x2A\x2A\x8B\x15");
	//Sig_AddrNotFound("Palatte Addr");
	//gHookFuncs.host_basepal = (word*)*(DWORD*)addr;
}

void InstallHook() 
{
	Fill_InlineEfxHook(R_Blood);
	Fill_InlineEfxHook(R_BloodSprite);
}

void HUD_Init(void)
{
	gExportfuncs.HUD_Init();
}

void R_Blood(float* org, float* dir, int pcolor, int speed)
{
	gHookFuncs.R_Blood(org, dir, pcolor, speed * 10);
}
void R_BloodSprite(float* org, int colorindex, int modelIndex, int modelIndex2, float size)
{
	model_t* pModel = gEngfuncs.hudGetModelByIndex(modelIndex);
	if (!pModel)
		return;
	
	TEMPENTITY* pTemp = gHookFuncs.CL_TempEntAllocHigh(org, pModel);
	// large, single blood sprite is a high-priority tent
	if (pTemp)
	{
		int	i;
		vec3_t	offset, dir;
		vec3_t	forward, right, up;

		colorindex = clamp(colorindex, 0, 256);
		pTemp->entity.curstate.scale = gEngfuncs.pfnRandomFloat((size / 25.0f), (size / 35.0f));
		pTemp->flags = FTENT_SPRANIMATE;

		pTemp->entity.curstate.rendercolor.r = base_palette1[colorindex].r;
		pTemp->entity.curstate.rendercolor.g = base_palette1[colorindex].g;
		pTemp->entity.curstate.rendercolor.b = base_palette1[colorindex].b;
		pTemp->entity.curstate.framerate = pModel->numframes * 10; //1s
		// play the whole thing once
		pTemp->die = gEngfuncs.GetClientTime() + (pModel->numframes / pTemp->entity.curstate.framerate);

		pTemp->entity.angles[2] = gEngfuncs.pfnRandomFloat(0, 360);
		pTemp->bounceFactor = 0;

		up[0] = right[0] = forward[0] = 0.0f;
		up[1] = right[1] = forward[1] = 0.0f;
		up[2] = right[2] = forward[2] = 1.0f;

		for (i = 0; i < 32; i++)
		{
			VectorCopy(org, offset);
			VectorMA(offset, gEngfuncs.pfnRandomFloat(-0.5f, 0.5f) * size, right, offset);
			VectorMA(offset, gEngfuncs.pfnRandomFloat(-0.5f, 0.5f) * size, up, offset);

			pTemp = gHookFuncs.CL_TempEntAllocHigh(org, gEngfuncs.hudGetModelByIndex(modelIndex2));
			if (!pTemp) 
				return;

			pTemp->flags = FTENT_COLLIDEWORLD | FTENT_SLOWGRAVITY;
			pTemp->entity.curstate.scale = gEngfuncs.pfnRandomFloat((size / 25.0f), (size / 35.0f));
			pTemp->entity.curstate.rendercolor.r = base_palette1[colorindex].r;
			pTemp->entity.curstate.rendercolor.g = base_palette1[colorindex].g;
			pTemp->entity.curstate.rendercolor.b = base_palette1[colorindex].b;
			pTemp->entity.curstate.frame = gEngfuncs.pfnRandomLong(0, pModel->numframes - 1);
			pTemp->die = gEngfuncs.GetClientTime() + gEngfuncs.pfnRandomFloat(1.0f, 3.0f);

			pTemp->entity.angles[2] = gEngfuncs.pfnRandomLong(0, 360);
			pTemp->bounceFactor = 0;

			srand((unsigned int)gEngfuncs.GetClientTime() * i - i);
			dir[0] = forward[0] + rand() / double(RAND_MAX) * 2 - 1;
			srand((unsigned int)gEngfuncs.GetClientTime() * i + i);
			dir[1] = forward[1] + rand() / double(RAND_MAX) * 2 - 1;
			dir[2] = forward[2];

			VectorScale(dir, gEngfuncs.pfnRandomFloat(8.0f * size, 20.0f * size), pTemp->entity.baseline.origin);

			pTemp->entity.baseline.origin[0] += gEngfuncs.pfnRandomFloat(4.0f, 128.0f) * (size);
			pTemp->entity.baseline.origin[1] += gEngfuncs.pfnRandomFloat(4.0f, 128.0f) * (size / 2);
			pTemp->entity.baseline.origin[2] += gEngfuncs.pfnRandomFloat(4.0f, 128.0f) * (size / 4);
		}
	}
}