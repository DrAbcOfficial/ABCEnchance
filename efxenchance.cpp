#include <metahook.h>
#include "pm_defs.h"
#include "pmtrace.h"
#include "event_api.h"
#include "cl_entity.h"
#include "mathlib.h"
#include "com_model.h"
#include "palette.h"
#include "local.h"

#define GAUSS_LASER_SPR "sprites/smoke.spr"
#define GAUSS_FIRE_SOUND "weapons/gauss2.wav"
struct EfxVarible{
	int iGaussBeam;
	int iGaussWaveBeam;
	int iGaussLoophole;
	mstudioevent_t pGaussFireSoundEvent = { 0,5004,0,GAUSS_FIRE_SOUND };
};
EfxVarible gEfxVarible;
void EfxReset() {
	gEfxVarible.iGaussBeam = gEngfuncs.pEventAPI->EV_FindModelIndex(GAUSS_LASER_SPR);
	if (gEfxVarible.iGaussBeam < 0)
		gEfxVarible.iGaussBeam = gEngfuncs.pfnSPR_Load(GAUSS_LASER_SPR);
}
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
	//Todo::����ʱ�Ѫ
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
void DoGaussFire(float fparam1, bool bparam1) {
	pmtrace_t tr, beam_tr;
	vec3_t vecForward;
	vec3_t vecViewAngle;
	vec3_t vecSrc, vecDest, vecDir;
	gEngfuncs.GetViewAngles(vecViewAngle);
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	cl_entity_t* view = gEngfuncs.GetViewModel();
	AngleVectors(vecViewAngle, vecForward, NULL, NULL);
	VectorCopy(vecForward, vecDir);
	VectorNormalize(vecDir);
	VectorCopy(view->attachment[0], vecSrc);
	VectorMultipiler(vecForward, 8192);
	VectorAdd(vecSrc, vecForward, vecDest);
	int entignore = local->index;
	bool fHasPunched = false;
	bool fFirstBeam = true;
	int	nMaxHits = 10;
	float flDamage = fparam1;
	while (flDamage > 10 && nMaxHits > 0) {
		//��һ�η������
		nMaxHits--;
		//��������ҵ�����
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecDest, PM_NORMAL, entignore, &tr);
		//��˹ֱ��
		if (tr.allsolid != 0)
			break;
		//��������
		if (tr.ent < 0)
			break;
		//��ʼ����
		if (fFirstBeam) {
			local->curstate.effects |= EF_MUZZLEFLASH;
			fFirstBeam = false;
		}
		gEngfuncs.pEfxAPI->R_BeamPoints(vecSrc, tr.endpos, gEfxVarible.iGaussBeam, 0.2,
				bparam1 ? 10 : 25, 0, 1, 0, 0, 0, 1, 0.8, 0);
		cl_entity_t* hit = gEngfuncs.GetEntityByIndex(tr.ent);
		//�ɷ����˹
		if (tr.ent == 0 || hit->curstate.solid == SOLID_BSP || hit->curstate.movetype == MOVETYPE_PUSHSTEP) {
			//�������ʵ��
			entignore = -1;
			//������淨������ˣ�ȡ�������ж������
			vec3_t vecNormal;
			VectorCopy(tr.plane.normal, vecNormal);
			//ȡ�÷��߷����෴����ת
			//VectorReverse(vecNormal);
			float n = -DotProduct(vecNormal, vecDir);
			//�Ƕ�С��60��
			if (n < 0.5) {
				// reflect
				vec3_t vecReflect;
				//��������ӳ˶�ȡ���յ�����
				vecReflect[0] = 2.0 * vecNormal[0] * n + vecDir[0];
				vecReflect[1] = 2.0 * vecNormal[1] * n + vecDir[1];
				vecReflect[2] = 2.0 * vecNormal[2] * n + vecDir[2];
				//ȡ���µ���������ͷ���
				VectorCopy(vecReflect, vecDir);
				vecSrc[0] = tr.endpos[0] + vecDir[0] * 8;
				vecSrc[1] = tr.endpos[1] + vecDir[1] * 8;
				vecSrc[2] = tr.endpos[2] + vecDir[2] * 8;

				vecDest[0] = vecSrc[0] + vecDir[0] * 8192;
				vecDest[1] = vecSrc[1] + vecDir[1] * 8192;
				vecDest[2] = vecSrc[2] + vecDir[2] * 8192;
				// lose energy
				if (n == 0)
					n = 0.1;
				flDamage *= 1 - n;
			}
			else {
				//���ɷ����˹
				//����������
				//�Ѿ�����һȭ�ˣ���Ҫ������
				//����͸ֱ��
				if (fHasPunched)
					break;
				fHasPunched = true;
				//����Ҽ�û��ֱ�������Ǿͳ��Դ�ǽ�����û����ǽ�ģ�
				if (!bparam1) {
					vecSrc[0] = tr.endpos[0] + vecDir[0] * 8;
					vecSrc[1] = tr.endpos[1] + vecDir[1] * 8;
					vecSrc[2] = tr.endpos[2] + vecDir[2] * 8;
					//�Ե�һ��ֱ���������Ϊ��㣬����ҵ�ǰ��8��λ����͸������㣬����ҵ������ָ����Ϊ�յ�
					gEngfuncs.pEventAPI->EV_SetTraceHull(2);
					gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecDest, PM_NORMAL, entignore, &beam_tr);
					//���伤������ǹ���
					if (beam_tr.allsolid == 0) {
						//�Ե�һ�δ�͸����ڵ�Ϊ�ص����ô�͸����
						gEngfuncs.pEventAPI->EV_SetTraceHull(2);
						gEngfuncs.pEventAPI->EV_PlayerTrace(beam_tr.endpos, tr.endpos, PM_NORMAL, entignore, &beam_tr);
						//������߳���Ϊn
						vec3_t vecLength;
						VectorSubtract(beam_tr.endpos, tr.endpos, vecLength)
							n = VectorLength(vecLength);
						//������ȱ��˺�С
						if (n < flDamage) {
							//����Ϊ0�ģ�������ԶΪ1
							if (n == 0)
								n = 1;
							//ÿ����һ���˺���n
							flDamage -= n;
							//���ߵĺ���һ������

							//��һ���������Ϊ���伤����յ㣬��������Ϊ��ǰ
							vecSrc[0] = beam_tr.endpos[0] + vecDir[0];
							vecSrc[1] = beam_tr.endpos[1] + vecDir[1];
							vecSrc[2] = beam_tr.endpos[2] + vecDir[2];
						}
					}
					else
						//�������赲��ֹͣѭ��
						flDamage = 0;
				}
				else {
					//����60�㣬����͸������һ�κ�ֹͣѭ��
					if (bparam1) {
						//��Ч
					}
					flDamage = 0;
				}
			}
		}
		else {
			//���ɷ������
			//���յ���ǰ����һ�δ�͸��ֹͣ
			VectorAdd(tr.endpos, vecDir, vecSrc);
			entignore = hit->index;
		}
	}
}
void pfnPlaybackEvent (int flags, const struct edict_s* pInvoker, unsigned short eventindex, 
	float delay, float* origin, float* angles, 
	float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) {
	//��˹flag 3, index 12
	//��˹����flag1, index 13
	//gEngfuncs.Con_Printf("flag: %d index: %d\n",flags, (int)eventindex);
	switch (eventindex) {
	case 12: {
		gEngfuncs.pEventAPI->EV_WeaponAnimation(6, 0);
		gExportfuncs.HUD_StudioEvent(&gEfxVarible.pGaussFireSoundEvent, gEngfuncs.GetViewModel());
		//f1 �˺�
		//b1 �Ƿ����
		DoGaussFire(fparam1, bparam1);
		break;
	}
	default:gHookFuncs.pfnPlaybackEvent(flags, pInvoker, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2); break;
	}
}