#pragma once
#include "cvar_hook.h"
#include "com_model.h"

#define FCVAR_VALUE (FCVAR_PRINTABLEONLY | FCVAR_CLIENTDLL | FCVAR_ARCHIVE)

#define BASE_GWR_SELECTED 842100225
#define BASE_GWR_UNSELECTED 842100224

//Lazy Dizzy Short Marco
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

//Hooked Address
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
	void		(*SetPunchAngle)			(int y, float value);
	bool		(*NET_StringToAdr)			(char* param_1, netadr_s* param_2);

	void		(*pfnPlaybackEvent)			(int flags, const struct edict_s* pInvoker, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);
	//VGUI2
	char* (*V_strncpy)(char* a1, const char* a2, size_t a3);
	PVOID(*VGUIClient001_CreateInterface)(HINTERFACEMODULE hModule);
}cl_refHookfunc_t;

//CVars
typedef struct{
	cvar_t* pBloodEfx = nullptr;
	cvar_t* pBloodSpriteSpeed = nullptr;
	cvar_t* pBloodSpriteNumber = nullptr;
	cvar_t* pGaussEfx = nullptr;

	cvar_t* pPlayerTitle = nullptr;
	cvar_t* pPlayerTitleLength = nullptr;
	cvar_t* pPlayerTitleHeight = nullptr;
	cvar_t* pPlayerTitleDangerHealth = nullptr;

	cvar_t* pHealthArmorStyle = nullptr;
	cvar_t* pDangerHealth = nullptr;
	cvar_t* pDangerArmor = nullptr;
	cvar_t* pDamageScreenFilter = nullptr;
	cvar_t* pDamageScreenFactor = nullptr;
	cvar_t* pDamageScreenBase = nullptr;

	cvar_t* pDynamicCrossHair = nullptr;
	cvar_t* pDynamicCrossHairAH = nullptr;
	cvar_t* pDynamicCrossHairL = nullptr;
	cvar_t* pDynamicCrossHairW = nullptr;
	cvar_t* pDynamicCrossHairO = nullptr;
	cvar_t* pDynamicCrossHairM = nullptr;
	cvar_t* pDynamicCrossHairA = nullptr;
	cvar_t* pDynamicCrossHairCR = nullptr;
	cvar_t* pDynamicCrossHairCG = nullptr;
	cvar_t* pDynamicCrossHairCB = nullptr;
	cvar_t* pDynamicCrossHairOTD = nullptr;
	cvar_t* pDynamicCrossHairOTDW = nullptr;
	cvar_t* pDynamicCrossHairT = nullptr;
	cvar_t* pDynamicCrossHairD = nullptr;

	cvar_t* pDynamicHUD = nullptr;

	cvar_t* pAmmoMenuDrawPos = nullptr;
	cvar_t* pAmmoMenuDrawRainbow = nullptr;

	cvar_t* pModelLag = nullptr;
	cvar_t* pModelLagAutoStop = nullptr;
	cvar_t* pModelLagValue = nullptr;
	cvar_t* pModelSlide = nullptr;
	cvar_t* pModelSlideAngle = nullptr;
	cvar_t* pModelSlideLength = nullptr;
	cvar_t* pModelSlideHeight = nullptr;

	cvar_t* pCamIdealHeight = nullptr;
	cvar_t* pCamIdealRight = nullptr;

	cvar_t* pRadar = nullptr;
	cvar_t* pRadarZoom = nullptr;
	cvar_t* pRadarSize = nullptr;
	cvar_t* pRadarSizeTime = nullptr;
	cvar_t* pRadarGap = nullptr;
	cvar_t* pRadarUpdateInterval = nullptr;
	cvar_t* pRadarRoundRadius = nullptr;

	cvar_t* pDeathNotice = nullptr;
	cvar_t* pDeathNoticeTime = nullptr;

	cvar_t* pHudVote = nullptr;

	cvar_t* pHudEfx = nullptr;

	cvar_t* pEccoCheckInfo = nullptr;
	cvar_t* pEccoEnable = nullptr;

	cvar_t* pItemHighLight = nullptr;
	cvar_t* pItemHighLightRange = nullptr;

	cvar_t* pEccoBuyMenu = nullptr;

	cvar_t* pGrenadeIndicator = nullptr;
	cvar_t* pGrenadeIndicatorRange = nullptr;
	cvar_t* pGrenadeInicatorTime = nullptr;

	cvar_t* pCVarDevOverview = nullptr;
	cvar_t* pCVarDrawEntities = nullptr;
	cvar_t* pCVarDrawViewModel = nullptr;
	cvar_t* pCVarDrawDynamic = nullptr;
	cvar_t* pCVarFXAA = nullptr;
	cvar_t* pCVarWater = nullptr;
	cvar_t* pCVarShadow = nullptr;

#ifdef _DEBUG
	cvar_t* pCCTV = nullptr;
#endif
	//Defualt CVars
	cvar_t* pCvarDefaultFOV = nullptr;
}cl_cvars_t;

extern cl_refHookfunc_t gHookFuncs;
extern cl_cvars_t gCVars;
extern char m_szCurrentLanguage[128];