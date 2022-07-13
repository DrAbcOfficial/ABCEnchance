#pragma once
#include "cvar_hook.h"
#include "com_model.h"

#define FCVAR_VALUE (FCVAR_PRINTABLEONLY | FCVAR_CLIENTDLL | FCVAR_ARCHIVE)

#define BASE_GWR_SELECTED 842100225
#define BASE_GWR_UNSELECTED 842100224

#define V_snprintf sprintf_s

#define HOOK_COMMAND(x, y) g_pMetaHookAPI->HookCmd((char*)x, __UserCmd_##y)
#define ADD_COMMAND(x, y) gEngfuncs.pfnAddCommand((char*)x, y)
#define ConsoleWriteline(x) gEngfuncs.Con_Printf(x)
#define CVAR_GET_POINTER(x) gEngfuncs.pfnGetCvarPointer(x)
#define CVAR_GET_FLOAT(x) gEngfuncs.pfnGetCvarFloat(x)
#define CVAR_GET_STRING(x) gEngfuncs.pfnGetCvarString(x)
#define SPR_Load (*gEngfuncs.pfnSPR_Load)
#define SPR_Set (*gEngfuncs.pfnSPR_Set)
#define SPR_Frames (*gEngfuncs.pfnSPR_Frames)
#define SPR_GetList(x, y) (*gEngfuncs.pfnSPR_GetList)((char*)x, y)
// SPR_Draw  draws a the current sprite as solid
#define SPR_Draw (*gEngfuncs.pfnSPR_Draw)
// SPR_DrawHoles  draws the current sprites,  with color index255 not drawn (transparent)
#define SPR_DrawHoles (*gEngfuncs.pfnSPR_DrawHoles)
// SPR_DrawAdditive  adds the sprites RGB values to the background  (additive transulency)
#define SPR_DrawAdditive (*gEngfuncs.pfnSPR_DrawAdditive)
// SPR_EnableScissor  sets a clipping rect for HUD sprites.  (0,0) is the top-left hand corner of the screen.
#define SPR_EnableScissor (*gEngfuncs.pfnSPR_EnableScissor)
// SPR_DisableScissor  disables the clipping rect
#define SPR_DisableScissor (*gEngfuncs.pfnSPR_DisableScissor)
#define FillRGBA (*gEngfuncs.pfnFillRGBA)
// ScreenHeight returns the height of the screen, in pixels
#define ScreenHeight (gScreenInfo.iHeight)
// ScreenWidth returns the width of the screen, in pixels
#define ScreenWidth (gScreenInfo.iWidth)
#define GetScreenInfo (*gEngfuncs.pfnGetScreenInfo)
#define ServerCmd(x) (*gEngfuncs.pfnServerCmd)((char*)x)
#define EngineClientCmd(x) (*gEngfuncs.pfnClientCmd)((char*)x)
#define SetCrosshair (*gEngfuncs.pfnSetCrosshair)
#define PlaySoundByName(x, y) (*gEngfuncs.pfnPlaySoundByName)((char*)x, y)
#define VEC_WorldToScreen(w, s) (*gEngfuncs.pTriAPI->WorldToScreen)(w, s);s[0]=(1.0f+s[0])*ScreenWidth/2;s[1]=(1.0f-s[1])*ScreenHeight/2

typedef struct{
	void		(*R_BloodSprite)			(float* org, int colorindex, int modelIndex, int modelIndex2, float size);

	float*		(*GetClientColor)			(int clientIndex);
	int(__fastcall* R_CrossHair_ReDraw)	(void* pthis, int dummy, int param_1);
	void		(*EVVectorScale)			(float* pucnangle1, float scale, float* pucnangle2);
	void		(*R_NewMap)					(void);
	int			(*CL_IsDevOverview)			(void);
	void		(*R_RenderView)				(int a1);
	void		(*R_RenderScene)			(void);
	model_t*	(*CL_GetModelByIndex)		(int index);
	void		(*GL_Bind)					(int texnum);
	void		(__cdecl* CL_SetDevOverView)(int param_1);
	void		(*R_ForceCVars)				(qboolean mp);
	void		(*Cvar_DirectSet)			(cvar_t* var, char* value);
	void		(*SetPunchAngle)				(int y, float value);

	void		(*pfnPlaybackEvent)			(int flags, const struct edict_s* pInvoker, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);
}cl_refHookfunc_t;

typedef struct{
	cvar_t* pBloodEfx;
	cvar_t* pBloodSpriteSpeed;
	cvar_t* pBloodSpriteNumber;
	cvar_t* pGaussEfx;

	cvar_t* pPlayerTitle;
	cvar_t* pPlayerTitleLength;
	cvar_t* pPlayerTitleHeight;
	cvar_t* pPlayerTitleDangerHealth;

	cvar_t* pHealthArmorStyle;
	cvar_t* pDangerHealth;
	cvar_t* pDangerArmor;
	cvar_t* pDamageScreenFilter;
	cvar_t* pDamageScreenFactor;
	cvar_t* pDamageScreenBase;

	cvar_t* pDynamicCrossHair;
	cvar_t* pDynamicCrossHairAH;
	cvar_t* pDynamicCrossHairL;
	cvar_t* pDynamicCrossHairW;
	cvar_t* pDynamicCrossHairO;
	cvar_t* pDynamicCrossHairM;
	cvar_t* pDynamicCrossHairA;
	cvar_t* pDynamicCrossHairCR;
	cvar_t* pDynamicCrossHairCG;
	cvar_t* pDynamicCrossHairCB;
	cvar_t* pDynamicCrossHairOTD;
	cvar_t* pDynamicCrossHairOTDW;
	cvar_t* pDynamicCrossHairT;
	cvar_t* pDynamicCrossHairD;

	cvar_t* pDynamicHUD;

	cvar_t* pAmmoCSlot[10];
	cvar_t* pAmmoMenuDrawPos;
	cvar_t* pAmmoMenuDrawRainbow;
	cvar_t* pAmmoMenuStyle;

	cvar_t* pModelLag;
	cvar_t* pModelLagAutoStop;
	cvar_t* pModelLagValue;
	cvar_t* pModelSlide;
	cvar_t* pModelSlideAngle;
	cvar_t* pModelSlideLength;
	cvar_t* pModelSlideHeight;

	cvar_t* pCamIdealHeight;
	cvar_t* pCamIdealRight;

	cvar_t* pRadar;
	cvar_t* pRadarZoom;
	cvar_t* pRadarSize;
	cvar_t* pRadarSizeTime;
	cvar_t* pRadarGap;
	cvar_t* pRadarUpdateInterval;
	cvar_t* pRadarRoundRadius;

	cvar_t* pDeathNoticeTime;

	cvar_t* pHudEfx;

	cvar_t* pEccoCheckInfo;
	cvar_t* pEccoEnable;

	cvar_t* pItemHighLight;
	cvar_t* pItemHighLightRange;

	cvar_t* pEccoBuyMenu;

	cvar_t* pGrenadeIndicator;
	cvar_t* pGrenadeIndicatorRange;
	cvar_t* pGrenadeInicatorTime;

	cvar_t* pCVarDevOverview;
	cvar_t* pCVarDrawEntities;
	cvar_t* pCVarDrawViewModel;
	cvar_t* pCVarDrawDynamic;
	cvar_t* pCVarFXAA;
	cvar_t* pCVarWater;
	cvar_t* pCVarShadow;

#ifdef _DEBUG
	cvar_t* pCCTV;
#endif
	//Defualt CVars
	cvar_t* pCvarDefaultFOV;
}cl_cvars_t;

extern cl_refHookfunc_t gHookFuncs;
extern cl_cvars_t gCVars;