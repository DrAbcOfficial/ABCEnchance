#pragma once
#include "cvardef.h"
#include "com_model.h"

constexpr int FCVAR_VALUE = (FCVAR_PRINTABLEONLY | FCVAR_CLIENTDLL | FCVAR_ARCHIVE);

xcommand_t Q_HOOK_COMMAND(const char* cmd, xcommand_t func);
#define HOOK_COMMAND(x, y) Q_HOOK_COMMAND(x, __UserCmd_##y)
void ADD_COMMAND(const char* cmd, void (*pfnEngSrc_function)(void));
void ConsoleWriteline(const char* x);
cvar_t* CVAR_GET_POINTER(const char* x);
float CVAR_GET_FLOAT(const char* x);
const char* CVAR_GET_STRING(const char* x);
void CVAR_SET_STRING(const char* x, const char* v);
cvar_t* CREATE_CVAR(const char* name, const char* val, int flag, cvar_callback_t callback);
void ServerCmd(const char* x);
void EngineClientCmd(const char* x);
void PlaySoundByName(const char* x, float volum);
void VEC_WorldToScreen(vec3_t world, vec3_t screen);
float ClientTime();

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
typedef struct{
	void		(*R_BloodSprite)			(float* org, int colorindex, int modelIndex, int modelIndex2, float size);
	TEMPENTITY* (*R_TempModel)				(float* pos, float* dir, float* angles, float life, int modelIndex, int soundtype);

	float*		(*GetClientColor)			(int clientIndex);
	int(__fastcall* R_CrossHair_ReDraw)		(void* pthis, int dummy, int param_1);
	void(__fastcall* TFV_ShowScoreBoard)	(void* pthis);
	void(__fastcall* TFV_ShowVGUIMenu)		(void* pthis, int dummy, int iVguiMenu);
	void(__fastcall* CStudioModelRenderer_Init)		(void* pthis, int dummy);

	void		(*EVVectorScale)			(float* pucnangle1, float scale, float* pucnangle2);
	void		(*R_NewMap)					();
	int			(*CL_IsDevOverview)			();
	void		(*R_RenderView)				(int a1);
	void		(*R_RenderScene)			();
	model_t*	(*CL_GetModelByIndex)		(int index);
	void		(*GL_Bind)					(int texnum);
	void		(__cdecl* CL_SetDevOverView)(int param_1);
	void		(*R_ForceCVars)				(bool mp);
	void(__fastcall* CClient_SoundEngine_PlayFMODSound)(void* pEngine, int dummy, int param_1, int channel, float* param_3, int param_4,
		char* param_5, float param_6, float param_7, int param_8, int param_9, int param_10,
		float param_11);
	void		(*SetPunchAngle)			(int y, float value);

	void* (__fastcall* CBasePanel_ctor)(void* pthis, int dummy);
	void(__fastcall* CBasePanel_PaintBackground)(void* pthis, int dummy);
	void (__fastcall* CBasePanel_RunMenuCommand)(void* pthis, int dummy, const char* command);

	void(__fastcall* CGameUI_Start)(void* pthis, int dummy, void* engfuncs, int idoncare, void* ibasesystem);

	void		(*pfnPlaybackEvent)			(int flags, const struct edict_s* pInvoker, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);
	//VGUI2
	char* (*V_strncpy)(char* a1, const char* a2, size_t a3);
}cl_refHookfunc_t;

//CVars
typedef struct cl_cvars_s{
	cvar_t* pDynamicBackground = nullptr;

	cvar_t* pShellEfx = nullptr;
	cvar_t* pBloodEfx = nullptr;
	cvar_t* pBloodSpriteSpeed = nullptr;
	cvar_t* pBloodSpriteNumber = nullptr;
	cvar_t* pGaussEfx = nullptr;

	cvar_t* pDangerHealth = nullptr;
	cvar_t* pDangerArmor = nullptr;
	cvar_t* pDamageScreenFilter = nullptr;
	cvar_t* pDamageScreenFactor = nullptr;
	cvar_t* pDamageScreenBase = nullptr;

	cvar_t* pDynamicCrossHair = nullptr;

	cvar_t* pDynamicHUD = nullptr;

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

	cvar_t* pDeathNotice = nullptr;
	cvar_t* pDeathNoticeTime = nullptr;

	cvar_t* pHudEfx = nullptr;

	cvar_t* pEccoCheckInfo = nullptr;
	cvar_t* pEccoEnable = nullptr;

	cvar_t* pItemHighLight = nullptr;
	cvar_t* pItemHighLightRange = nullptr;

	cvar_t* pEccoBuyMenu = nullptr;

	cvar_t* pGrenadeIndicator = nullptr;
	cvar_t* pGrenadeIndicatorRange = nullptr;
	cvar_t* pGrenadeInicatorTime = nullptr;

	cvar_t* pMotd = nullptr;
	cvar_t* pRadar = nullptr;
	cvar_t* pRadarZoom = nullptr;
	cvar_t* pRadarAvatar = nullptr;
	cvar_t* pRadarAvatarSize = nullptr;
	cvar_t* pRadarAvatarScale = nullptr;

	cvar_t* pCVarDevOverview = nullptr;
	cvar_t* pCVarDrawEntities = nullptr;
	cvar_t* pCVarDrawViewModel = nullptr;
	cvar_t* pCVarDrawDynamic = nullptr;
	cvar_t* pCVarFXAA = nullptr;
	cvar_t* pCVarWater = nullptr;
	cvar_t* pCVarShadow = nullptr;

	cvar_t* pCVarAutoBunnyJump = nullptr;

#ifdef _DEBUG
	cvar_t* pCCTV = nullptr;
#endif
	//Defualt CVars
	cvar_t* pCvarDefaultFOV = nullptr;
}cl_cvars_t;

extern cl_refHookfunc_t gHookFuncs;
extern cl_cvars_t gCVars;
extern char m_szCurrentLanguage[128];