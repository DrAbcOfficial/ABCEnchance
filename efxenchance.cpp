#include <metahook.h>
#include "pm_defs.h"
#include "pmtrace.h"
#include "event_api.h"
#include "cl_entity.h"
#include "mathlib.h"
#include "com_model.h"
#include "palette.h"
#include "local.h"


void R_RicochetSprite(float* pos, struct model_s* pmodel, float duration, float scale){
	//stack overflow
	//gHookFuncs.R_SparkEffect(pos, (int)gCVars.pRicochetNumber->value, 4, 32);
	gHookFuncs.R_RicochetSprite(pos, pmodel, duration, scale);
}
void R_Explosion(float* pos, int model, float scale, float framerate, int flags){
	int	i;
	vec3_t	offset, dir;
	vec3_t	forward, right, up;
	for (int i = 0; i < gCVars.pExpSmokeNumber->value; i++){
		VectorCopy(pos, offset);
		VectorMA(offset, gEngfuncs.pfnRandomFloat(-0.5f, 0.5f) * scale, forward, offset);
		VectorMA(offset, gEngfuncs.pfnRandomFloat(-0.5f, 0.5f) * scale, right, offset);
		VectorMA(offset, gEngfuncs.pfnRandomFloat(-0.5f, 0.5f) * scale, up, offset);

		TEMPENTITY* pTemp = gHookFuncs.CL_TempEntAllocHigh(pos, NULL);
		if (!pTemp)
			return;
		pTemp->flags = FTENT_COLLIDEWORLD | FTENT_SLOWGRAVITY | FTENT_SMOKETRAIL | FTENT_NOMODEL;
		pTemp->die = gEngfuncs.GetClientTime() + 5;
		pTemp->entity.angles[2] = gEngfuncs.pfnRandomLong(0, 360);
		pTemp->bounceFactor = 0;

		srand((unsigned int)gEngfuncs.GetClientTime() * i - i);
		dir[0] = forward[0] + rand() / double(RAND_MAX) * 2 - 1;
		srand((unsigned int)gEngfuncs.GetClientTime() * i + i);
		dir[1] = forward[1] + rand() / double(RAND_MAX) * 2 - 1;
		dir[2] = forward[2];

		VectorScale(dir, gEngfuncs.pfnRandomFloat(8.0f * scale, 20.0f * scale), pTemp->entity.baseline.origin);
		pTemp->entity.baseline.origin[0] += gEngfuncs.pfnRandomFloat(4.0f, gCVars.pExpSmokeSpeed->value) * (scale);
		pTemp->entity.baseline.origin[1] += gEngfuncs.pfnRandomFloat(4.0f, gCVars.pExpSmokeSpeed->value) * (scale / 2);
		pTemp->entity.baseline.origin[2] += gEngfuncs.pfnRandomFloat(4.0f, gCVars.pExpSmokeSpeed->value) * (scale / 4);
	}
	gHookFuncs.R_Explosion(pos, model, scale, framerate, flags);
}
void R_Blood(float* org, float* dir, int pcolor, int speed){
	//Todo::创建时飙血
	gHookFuncs.R_Blood(org, dir, pcolor, speed);
}
void R_BloodSprite(float* org, int colorindex, int modelIndex, int modelIndex2, float size){
	model_t* pModel = gEngfuncs.hudGetModelByIndex(modelIndex);
	if (!pModel)
		return;
	TEMPENTITY* pTemp = gHookFuncs.CL_TempEntAllocHigh(org, pModel);
	if (pTemp){
		int	i;
		vec3_t	offset, dir;
		vec3_t	forward, right, up;
		int nColor = colorindex;

		nColor = clamp(nColor, 0, 256);

		pTemp->entity.curstate.scale = gEngfuncs.pfnRandomFloat((size / 25.0f), (size / 35.0f));
		pTemp->flags = FTENT_SPRANIMATE;

		pTemp->entity.curstate.rendercolor.r = base_palette1[nColor].r;
		pTemp->entity.curstate.rendercolor.g = base_palette1[nColor].g;
		pTemp->entity.curstate.rendercolor.b = base_palette1[nColor].b;
		pTemp->entity.curstate.framerate = pModel->numframes * 10; //1s
		// play the whole thing once
		pTemp->die = gEngfuncs.GetClientTime() + (pModel->numframes / pTemp->entity.curstate.framerate);

		pTemp->entity.angles[2] = gEngfuncs.pfnRandomFloat(0, 360);
		pTemp->bounceFactor = 0;

		up[0] = right[0] = forward[0] = 0.0f;
		up[1] = right[1] = forward[1] = 0.0f;
		up[2] = right[2] = forward[2] = 1.0f;

		for (i = 0; i < gCVars.pBloodSpriteNumber->value; i++){
			VectorCopy(org, offset);
			VectorMA(offset, gEngfuncs.pfnRandomFloat(-0.5f, 0.5f) * size, right, offset);
			VectorMA(offset, gEngfuncs.pfnRandomFloat(-0.5f, 0.5f) * size, up, offset);

			pTemp = gHookFuncs.CL_TempEntAllocHigh(org, gEngfuncs.hudGetModelByIndex(modelIndex2));
			if (!pTemp)
				return;

			pTemp->flags = FTENT_COLLIDEWORLD | FTENT_SLOWGRAVITY;
			pTemp->entity.curstate.scale = gEngfuncs.pfnRandomFloat((size / 25.0f), (size / 35.0f));
			pTemp->entity.curstate.rendercolor.r = base_palette1[nColor].r;
			pTemp->entity.curstate.rendercolor.g = base_palette1[nColor].g;
			pTemp->entity.curstate.rendercolor.b = base_palette1[nColor].b;

			//TOO MANY ERROR!
			//pTemp->entity.curstate.frame = gEngfuncs.pfnRandomLong(0, pModel->numframes - 1);

			pTemp->die = gEngfuncs.GetClientTime() + gEngfuncs.pfnRandomFloat(1.0f, 3.0f);

			pTemp->entity.angles[2] = gEngfuncs.pfnRandomLong(0, 360);
			pTemp->bounceFactor = 0;

			srand((unsigned int)gEngfuncs.GetClientTime() * i - i);
			dir[0] = forward[0] + rand() / double(RAND_MAX) * 2 - 1;
			srand((unsigned int)gEngfuncs.GetClientTime() * i + i);
			dir[1] = forward[1] + rand() / double(RAND_MAX) * 2 - 1;
			dir[2] = forward[2];

			VectorScale(dir, gEngfuncs.pfnRandomFloat(8.0f * size, 20.0f * size), pTemp->entity.baseline.origin);

			pTemp->entity.baseline.origin[0] += gEngfuncs.pfnRandomFloat(4.0f, gCVars.pBloodSpriteSpeed->value) * (size);
			pTemp->entity.baseline.origin[1] += gEngfuncs.pfnRandomFloat(4.0f, gCVars.pBloodSpriteSpeed->value) * (size / 2);
			pTemp->entity.baseline.origin[2] += gEngfuncs.pfnRandomFloat(4.0f, gCVars.pBloodSpriteSpeed->value) * (size / 4);
		}
		vec3_t nOrg;
		nOrg[0] = org[0];
		nOrg[1] = org[1];
		nOrg[2] = org[2];
		gHookFuncs.R_BloodStream(nOrg, dir, nColor == 247 ? 70 : nColor, gEngfuncs.pfnRandomLong(4, gCVars.pBloodSpriteSpeed->value));
	}
}
void pfnPlaybackEvent (int flags, const struct edict_s* pInvoker, unsigned short eventindex, 
	float delay, float* origin, float* angles, 
	float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) {
	//高斯flag 3, index 12
	//高斯蓄力flag1, index 13
	gEngfuncs.Con_Printf("flag: %d index: %d\n",flags, (int)eventindex);
	switch (eventindex) {
	case 12: {
		//f1 亮度
		//b1 是否左键
		//待完善
		pmtrace_t tr;
		vec3_t vForward;
		vec3_t vViewAngle;
		gEngfuncs.GetViewAngles(vViewAngle);
		cl_entity_t* local = gEngfuncs.GetLocalPlayer();
		AngleVectors(vViewAngle, vForward, NULL, NULL);
		vec3_t vecSrc, viewOfs, vecEnd;
		VectorCopy(local->curstate.origin, vecSrc);
		gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(viewOfs);
		VectorAdd(vecSrc, viewOfs, vecSrc);
		vForward[0] *= 8192;
		vForward[1] *= 8192;
		vForward[2] *= 8192;
		VectorAdd(vecSrc, vForward, vecEnd);
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_NORMAL, NULL, &tr);
		
		int modelindex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/laserbeam.spr");
		if (modelindex < 0)
			modelindex = gEngfuncs.pfnSPR_Load("sprites/laserbeam.spr");
		cl_entity_t* view = gEngfuncs.GetViewModel();
		gEngfuncs.pEfxAPI->R_BeamPoints(view->attachment[0], tr.endpos, modelindex, 0.2, 4, 0, 1, 1, 0, 0, 255, 255, 255);
		gEngfuncs.pEventAPI->EV_WeaponAnimation(6, 0);
		break;
	}
	default:gHookFuncs.pfnPlaybackEvent(flags, pInvoker, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2); break;
	}
}