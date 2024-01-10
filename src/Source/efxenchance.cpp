#include <metahook.h>
#include "pm_defs.h"
#include "pmtrace.h"
#include "event_api.h"
#include "cl_entity.h"
#include "mymathlib.h"
#include "com_model.h"
#include "Color.h"
#include "palette.h"
#include "extraprecache.h"
#include "exportfuncs.h"
#include "local.h"
#include "efxenchance.h"

#define GAUSS_WAVE_LENGTH 48
#define GAUSS_LASER_P_WIDTH 2
#define GAUSS_LASER_S_WIDTH 4

struct EfxVarible{
	int iGaussBeam;
	int iGaussWaveBeam;
	int iGaussChargeSprite;
	float flGaussStartChargeTime;
	model_t* iGaussLoophole;
	mstudioevent_t pGaussFireSoundEvent = { 0, 5004, 0, "weapons/gauss2.wav" };

	int iGunSmoke;
};
EfxVarible gEfxVarible;
void EfxReset() {
	gEfxVarible.iGaussBeam = PrecacheExtraModel("sprites/laserbeam.spr");
	gEfxVarible.iGaussWaveBeam = PrecacheExtraModel("abcenchance/spr/gauss_wave.spr");
	gEfxVarible.iGaussChargeSprite = PrecacheExtraModel("abcenchance/spr/gauss_spark.spr");
	gEfxVarible.iGaussLoophole = gEngfuncs.hudGetModelByIndex(PrecacheExtraModel("abcenchance/mdl/gauss_loophole.mdl"));
	gEfxVarible.iGunSmoke = PrecacheExtraModel("sprites/Puff1.spr");
}
void R_BloodSprite(float* org, int colorindex, int modelIndex, int modelIndex2, float size){
	if(gCVars.pBloodEfx->value > 0){
		model_t* pModel = gEngfuncs.hudGetModelByIndex(modelIndex);
		if (!pModel)
			return;
		TEMPENTITY* pTemp = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(org, pModel);
		if (pTemp){
			int	i;
			vec3_t	offset, dir;
			vec3_t	forward, right, up;
			int nColor = colorindex;
			nColor = mathlib::clamp(nColor, 0, 255);
			pTemp->entity.curstate.scale = RANDOM_FLOAT((size / 25.0f), (size / 35.0f));
			pTemp->flags = FTENT_SPRANIMATE;
			pTemp->entity.curstate.rendercolor.r = base_palette1[nColor].r();
			pTemp->entity.curstate.rendercolor.g = base_palette1[nColor].g();
			pTemp->entity.curstate.rendercolor.b = base_palette1[nColor].b();
			pTemp->entity.curstate.framerate = 10.0f * pModel->numframes; //1s
			pTemp->die = gEngfuncs.GetClientTime() + (pModel->numframes / pTemp->entity.curstate.framerate);
			pTemp->entity.angles[2] = RANDOM_FLOAT(0, 360);
			pTemp->bounceFactor = 0;
			up[0] = right[0] = forward[0] = 0.0f;
			up[1] = right[1] = forward[1] = 0.0f;
			up[2] = right[2] = forward[2] = 1.0f;
			for (i = 0; i < gCVars.pBloodSpriteNumber->value; i++){
				mathlib::VectorCopy(org, offset);
				mathlib::VectorMA(offset, RANDOM_FLOAT(-0.5f, 0.5f) * size, right, offset);
				mathlib::VectorMA(offset, RANDOM_FLOAT(-0.5f, 0.5f) * size, up, offset);
				pTemp = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(org, gEngfuncs.hudGetModelByIndex(modelIndex2));
				if (!pTemp)
					return;
				pTemp->flags = FTENT_COLLIDEWORLD | FTENT_SLOWGRAVITY;
				pTemp->entity.curstate.scale = RANDOM_FLOAT((size / 25.0f), (size / 35.0f));
				pTemp->entity.curstate.rendercolor.r = base_palette1[nColor].r();
				pTemp->entity.curstate.rendercolor.g = base_palette1[nColor].g();
				pTemp->entity.curstate.rendercolor.b = base_palette1[nColor].b();
				pTemp->die = gEngfuncs.GetClientTime() + RANDOM_FLOAT(1.0f, 3.0f);
				pTemp->entity.angles[2] = gEngfuncs.pfnRandomFloat(0.0f, 360.0f);
				pTemp->bounceFactor = 0;
				srand((unsigned int)gEngfuncs.GetClientTime() * i - i);
				dir[0] = forward[0] + float(rand()) / float(RAND_MAX) * 2.0f - 1;
				srand((unsigned int)gEngfuncs.GetClientTime() * i + i);
				dir[1] = forward[1] + float(rand()) / float(RAND_MAX) * 2.0f - 1;
				dir[2] = forward[2];
				mathlib::VectorScale(dir, RANDOM_FLOAT(8.0f * size, 20.0f * size), pTemp->entity.baseline.origin);
				pTemp->entity.baseline.origin[0] += RANDOM_FLOAT(4.0f, gCVars.pBloodSpriteSpeed->value) * (size);
				pTemp->entity.baseline.origin[1] += RANDOM_FLOAT(4.0f, gCVars.pBloodSpriteSpeed->value) * (size / 2);
				pTemp->entity.baseline.origin[2] += RANDOM_FLOAT(4.0f, gCVars.pBloodSpriteSpeed->value) * (size / 4);
			}
		}
	}
	gHookFuncs.R_BloodSprite(org, colorindex, modelIndex, modelIndex2, size);
}
TEMPENTITY* R_TempModel(float* pos, float* dir, float* angles, float life, int modelIndex, int soundtype){
	TEMPENTITY* tent = gHookFuncs.R_TempModel(pos, dir, angles, life, modelIndex, soundtype);
	if (tent && gCVars.pShellEfx->value > 0 && soundtype > 0) {
		tent->flags |= FTENT_COLLIDEALL | FTENT_SLOWGRAVITY | FTENT_CLIENTCUSTOM;
		tent->entity.curstate.rendermode = kRenderTransTexture;
		tent->entity.curstate.renderamt = 255;
		tent->fadeSpeed = gEngfuncs.GetClientTime() + life;
		tent->callback = [](struct tempent_s* ent, float frametime, float currenttime) {
			ent->entity.curstate.renderamt = (ent->fadeSpeed - currenttime) * 255;
		};
		vec3_t smokedir = { dir[0], dir[1],RANDOM_FLOAT(70, 155)};
		TEMPENTITY* smoke = gHookFuncs.R_TempModel(pos, smokedir, angles, life/2, gEfxVarible.iGunSmoke, 0);
		if (smoke) {
			smoke->flags = FTENT_SPRANIMATE | FTENT_FADEOUT;
			smoke->entity.baseline.scale = RANDOM_FLOAT(0.07, 0.15);
			smoke->entity.curstate.renderamt = RANDOM_LONG(10, 25);
			smoke->entity.curstate.rendermode = kRenderTransAlpha;
			smoke->entity.curstate.framerate = RANDOM_FLOAT(25, 35);
			smoke->fadeSpeed = 20.0f;
		}
	}
	return tent;
}

void CreateGaussLoophole(pmtrace_t* tr) {
	TEMPENTITY* pTemp = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(tr->endpos, gEfxVarible.iGaussLoophole);
	if (!pTemp)
		return;
	mathlib::VectorAngles(tr->plane.normal, pTemp->entity.angles);
	pTemp->entity.angles[0] += 90;
	pTemp->entity.angles[2] += 180;
	pTemp->die = gEngfuncs.GetClientTime() + 1.0f;
	pTemp->flags = FTENT_CLIENTCUSTOM | FTENT_FADEOUT;
	pTemp->callback = [](TEMPENTITY* ent, float frametime, float currenttime) {
		ent->entity.curstate.scale = ent->die - currenttime;
	};
}
void DoGaussFire(float fparam1, int bparam1) {
	pmtrace_t tr, beam_tr;
	vec3_t vecForward, vecViewAngle;
	vec3_t vecSrc, vecDest, vecDir, viewOfs;
	vec3_t vecNormal, vecReflect, vecRandom, vecRandomSrc, vecLength;
	cl_entity_t* local = gEngfuncs.GetLocalPlayer();
	int entignore = local->index;
	bool fHasPunched = false;
	bool fFirstBeam = true;
	int	nMaxHits = 10;
	float flDamage = fparam1;

	gEngfuncs.GetViewAngles(vecViewAngle);

	mathlib::AngleVectors(vecViewAngle, vecForward, nullptr, nullptr);
	mathlib::VectorCopy(vecForward, vecDir);
	mathlib::VectorNormalize(vecDir);

	mathlib::AngleVectors(vecViewAngle, vecForward, NULL, NULL);
	mathlib::VectorCopy(local->curstate.origin, vecSrc);
	gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(viewOfs);
	mathlib::VectorAdd(vecSrc, viewOfs, vecSrc);
	mathlib::VectorMultipiler(vecForward, 8192);
	mathlib::VectorAdd(vecSrc, vecForward, vecDest);

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
			gEngfuncs.pEfxAPI->R_BeamEntPoint(local->index + (gExportfuncs.CL_IsThirdPerson() ? 0 : 4096), tr.endpos, gEfxVarible.iGaussBeam, 0.2,
				bparam1 ? GAUSS_LASER_P_WIDTH : GAUSS_LASER_S_WIDTH, 0, 1, 0, 0, 0, 1, bparam1 ? 0.8 : 1, bparam1 ? 0 : 1);
		}
		else
			gEngfuncs.pEfxAPI->R_BeamPoints(vecSrc, tr.endpos, gEfxVarible.iGaussBeam, 0.2,
				bparam1 ? GAUSS_LASER_P_WIDTH : GAUSS_LASER_S_WIDTH, 0, 1, 0, 0, 0, 1, bparam1 ? 0.8 : 1, bparam1 ? 0 : 1);
		cl_entity_t* hit = gEngfuncs.GetEntityByIndex(tr.ent);
		//�ɷ����˹
		if (hit && hit->model && hit->model->type == mod_brush) {
			//�������ģ��
			if (gEfxVarible.iGaussLoophole)
				CreateGaussLoophole(&tr);
			//�������ʵ��
			entignore = -1;
			//������淨������ˣ�ȡ�������ж������
			
			mathlib::VectorCopy(tr.plane.normal, vecNormal);
			float n = -mathlib::DotProduct(vecNormal, vecDir);
			//�Ƕ�С��60��
			if (n < 0.5) {
				// reflect
				//��������ӳ˶�ȡ���յ�����
				vecReflect[0] = 2.0 * vecNormal[0] * n + vecDir[0];
				vecReflect[1] = 2.0 * vecNormal[1] * n + vecDir[1];
				vecReflect[2] = 2.0 * vecNormal[2] * n + vecDir[2];
				//ȡ���µ���������ͷ���
				mathlib::VectorCopy(vecReflect, vecDir);
				mathlib::VectorCopy(tr.endpos, vecSrc);
				vecDest[0] = vecSrc[0] + vecDir[0] * 8192;
				vecDest[1] = vecSrc[1] + vecDir[1] * 8192;
				vecDest[2] = vecSrc[2] + vecDir[2] * 8192;
				//���������ɢ�䲨��Spr
				for (int i = 0; i < RANDOM_LONG(0, 4); i++) {
					vecRandom[0] = vecSrc[0] + RANDOM_FLOAT(GAUSS_WAVE_LENGTH / 2, GAUSS_WAVE_LENGTH) * 
						(vecNormal[0] * n * RANDOM_FLOAT(1, 3) + vecDir[0] * RANDOM_FLOAT(-3, 3));
					vecRandom[1] = vecSrc[1] + RANDOM_FLOAT(GAUSS_WAVE_LENGTH / 2, GAUSS_WAVE_LENGTH) * 
						(vecNormal[1] * n * RANDOM_FLOAT(1, 3) + vecDir[1] * RANDOM_FLOAT(-3, 3));
					vecRandom[2] = vecSrc[2] + RANDOM_FLOAT(GAUSS_WAVE_LENGTH / 2, GAUSS_WAVE_LENGTH) * 
						(vecNormal[2] * n * RANDOM_FLOAT(1, 3) + vecDir[2] * RANDOM_FLOAT(-3, 3));
					gEngfuncs.pEfxAPI->R_BeamPoints(vecSrc, vecRandom, gEfxVarible.iGaussWaveBeam, 0.2,
						bparam1 ? GAUSS_LASER_P_WIDTH : GAUSS_LASER_S_WIDTH, 0, 1, 0, 0, 0, 1, 0.8, 0);
				}
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
						mathlib::VectorSubtract(beam_tr.endpos, tr.endpos, vecLength);
						n = mathlib::VectorLength(vecLength);
						//������ȱ��˺�С
						//���ߵĺ���һ������
						mathlib::VectorCopy(beam_tr.endpos, vecRandomSrc);
						vecRandomSrc[0] += vecDir[0] * flDamage;
						vecRandomSrc[1] += vecDir[1] * flDamage;
						vecRandomSrc[2] += vecDir[2] * flDamage;
						for (int i = 0; i < RANDOM_LONG(8, 14); i++) {
							vecRandom[0] = vecRandomSrc[0] + vecDir[0] * RANDOM_FLOAT(-512, 512);
							vecRandom[1] = vecRandomSrc[1] + vecDir[1] * RANDOM_FLOAT(-512, 512);
							vecRandom[2] = vecRandomSrc[2] + vecDir[2] * RANDOM_FLOAT(-512, 512);
							gEngfuncs.pEventAPI->EV_SetTraceHull(2);
							gEngfuncs.pEventAPI->EV_PlayerTrace(vecRandomSrc, vecRandom, PM_STUDIO_IGNORE, entignore, &beam_tr);
							if (beam_tr.ent == 0)
								CreateGaussLoophole(&beam_tr);
						}
						if (n < flDamage) {
							//����Ϊ0�ģ�������ԶΪ1
							if (n == 0)
								n = 1;
							//ÿ����һ���˺���n
							flDamage -= n;
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
					flDamage = 0;
				}
			}
		}
		else {
			//���ɷ������
			//���յ���ǰ����һ�δ�͸��ֹͣ
			mathlib::VectorAdd(tr.endpos, vecDir, vecSrc);
			entignore = tr.ent;
		}
	}
}
void EV_StopPreviousGauss(){
	int idx = gEngfuncs.GetViewModel()->index;
	// Make sure we don't have a gauss spin event in the queue for this guy
	gEngfuncs.pEventAPI->EV_KillEvents(idx, "events/gaussspin.sc");
	gEngfuncs.pEventAPI->EV_StopSound(idx, CHAN_WEAPON, "ambience/pulsemachine.wav");
}
void pfnPlaybackEvent (int flags, const struct edict_s* pInvoker, unsigned short eventindex, 
	float delay, float* origin, float* angles, 
	float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) {
	//��˹flag 3, index 12
	//��˹����flag1, index 13
	switch (eventindex) {
		case 12: {
			if (gCVars.pGaussEfx->value > 0) {
				EV_StopPreviousGauss();
				gEngfuncs.pEventAPI->EV_WeaponAnimation(6, 0);
				gExportfuncs.HUD_StudioEvent(&gEfxVarible.pGaussFireSoundEvent, gEngfuncs.GetViewModel());
				gHookFuncs.SetPunchAngle(0, -1.5f);
				//f1 �˺�
				//b1 �Ƿ����
				DoGaussFire(fparam1, bparam1);
				return;
			}
		}
	}
	gHookFuncs.pfnPlaybackEvent(flags, pInvoker, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);
}