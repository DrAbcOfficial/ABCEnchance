#pragma once
#include "cvardef.h"
#include "com_model.h"

constexpr int FCVAR_VALUE = (FCVAR_PRINTABLEONLY | FCVAR_CLIENTDLL | FCVAR_ARCHIVE);

extern xcommand_t Q_HOOK_COMMAND(const char* cmd, xcommand_t func);
#define HOOK_COMMAND(x, y) Q_HOOK_COMMAND(x, __UserCmd_##y)
extern void ADD_COMMAND(const char* cmd, void (*pfnEngSrc_function)(void));
extern void ConsoleWriteline(const char* x);
extern cvar_t* CVAR_GET_POINTER(const char* x);
extern float CVAR_GET_FLOAT(const char* x);
extern const char* CVAR_GET_STRING(const char* x);
extern void CVAR_SET_STRING(const char* x, const char* v);
extern void CVAR_SET_FLOAT(const char* x, float v);
extern cvar_t* CREATE_CVAR(const char* name, const char* val, int flag, cvar_callback_t callback);
extern void ServerCmd(const char* x);
extern void EngineClientCmd(const char* x);
extern void PlaySoundByName(const char* x, float volum);
extern void VEC_WorldToScreen(vec3_t world, vec3_t screen);
extern float ClientTime();

//Lazy Dizzy Short Marco
#define V_snprintf sprintf_s
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
#define SetCrosshair (*gEngfuncs.pfnSetCrosshair)
#define FillRGBA (*gEngfuncs.pfnFillRGBA)
#define GetScreenInfo (*gEngfuncs.pfnGetScreenInfo)

//Hooked Address
using cl_refHookfunc_t = struct{
#pragma region Efx
	void		(*R_BloodSprite)			(float* org, int colorindex, int modelIndex, int modelIndex2, float size);
	TEMPENTITY* (*R_TempModel)				(float* pos, float* dir, float* angles, float life, int modelIndex, int soundtype);
	void		(*pfnPlaybackEvent)			(int flags, const struct edict_s* pInvoker, unsigned short eventindex, float delay, float* origin, 
		float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);
#pragma endregion
#pragma region Client
	float* (*GetClientColor)			(int clientIndex);
	void (*EVVectorScale)			(float* pucnangle1, float scale, float* pucnangle2);

	void (__fastcall* TFV_ShowScoreBoard)	(void* pthis);
	void (__fastcall* TFV_ShowVGUIMenu)		(void* pthis, int dummy, int iVguiMenu);

	void (__fastcall* CStudioModelRenderer_Init)		(void* pthis, int dummy);

	void (__fastcall* CClient_SoundEngine_Initialize)(void* pSoundEngine, int);
	void (__fastcall* CClient_SoundEngine_PlayFMODSound)(void* pSoundEngine, int, int flags, int entindex, float* origin,
		 int channel, const char* name, float fvol, float attenuation, int extraflags, int pitch, int sentenceIndex, float soundLength);

	int (__fastcall* R_CrossHair_ReDraw)		(void* pthis, int dummy, int param_1);

	void (*V_PunchAxis)				(int axis, float punch);
#pragma endregion
#pragma region Engine
	void (*CEngineClient_RenderView)				(ref_params_s* param, bool bClearColor, bool bDrawViewModel, int iPassIndex);
	/*(mspriteframe_s *)*/void* (*R_GetSpriteFrame) (/* (msprite_s*) */ void* pSprite, int frame);

	void (*R_NewMap)					();

	model_t* (*CL_GetModelByIndex)		(int index);
#pragma endregion
#pragma region VGUI
	char* (*V_strncpy)(char* a1, const char* a2, size_t a3);
#pragma endregion
#pragma region GameUI
	void* (__fastcall* CBasePanel_ctor)(void* pthis, int dummy);
	void (__fastcall* CBasePanel_PaintBackground)(void* pthis, int dummy);
#pragma endregion
};

//CVars
using cl_cvars_t = struct{
	cvar_t* pDynamicBackground;

	cvar_t* pShellEfx;
	cvar_t* pBloodEfx;
	cvar_t* pBloodSpriteSpeed;
	cvar_t* pBloodSpriteNumber;
	cvar_t* pGaussEfx;
	cvar_t* pEgonEfx;

	cvar_t* pDangerHealth;
	cvar_t* pDangerArmor;
	cvar_t* pDamageScreenFilter;
	cvar_t* pDamageScreenFactor;
	cvar_t* pDamageScreenBase;

	cvar_t* pDynamicCrossHair;

	cvar_t* pAmmoMenuDrawRainbow;

	cvar_t* pModelLag;
	cvar_t* pModelLagAutoStop;
	cvar_t* pModelLagValue;
	cvar_t* pModelSlide;
	cvar_t* pModelSlideAngle;
	cvar_t* pModelSlideLength;
	cvar_t* pModelSlideHeight;

	cvar_t* pCamIdealHeight;
	cvar_t* pCamIdealRight;

	cvar_t* pDeathNotice;
	cvar_t* pDeathNoticeTime;

	cvar_t* pHudEfx;

	cvar_t* pEccoCheckInfo;
	cvar_t* pEccoEnable;

	cvar_t* pItemHighLight;
	cvar_t* pItemHighLightRange;
	cvar_t* pItemHighLightNameFOV;
	cvar_t* pItemHighLightAimFOV;
	cvar_t* pItemHighLightPickup;
	cvar_t* pItemHighLightName;

	cvar_t* pEccoBuyMenu;

	cvar_t* pGrenadeIndicator;
	cvar_t* pGrenadeIndicatorRange;
	cvar_t* pGrenadeInicatorTime;

	cvar_t* pMotd;

	cvar_t* pRadar;
	cvar_t* pRadarZoom;
	cvar_t* pRadarAvatar;
	cvar_t* pRadarAvatarSize;
	cvar_t* pRadarAvatarScale;
	cvar_t* pRadarZMin;
	cvar_t* pRadarZMax;

	cvar_t* pCVarDevOverview;
	cvar_t* pCVarDrawEntities;
	cvar_t* pCVarDrawViewModel;
	cvar_t* pCVarDrawDynamic;
	cvar_t* pCVarFXAA;
	cvar_t* pCVarWater;
	cvar_t* pCVarShadow;
	cvar_t* pCVarDeferredLighting;
	cvar_t* pCVarGammaBlend;

	cvar_t* pCVarAutoBunnyJump;
#ifdef _DEBUG
	cvar_t* pCCTV;
#endif
	//Defualt CVars
	cvar_t* pCvarDefaultFOV;
};

extern cl_refHookfunc_t gHookFuncs;
extern cl_cvars_t gCVars;