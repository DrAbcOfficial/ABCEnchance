#include <metahook.h>
#include "pm_defs.h"
#include "pmtrace.h"
#include "event_api.h"
#include "cl_entity.h"
#include "mathlib.h"
#include "com_model.h"
#include "palette.h"
#include "extraprecache.h"
#include "exportfuncs.h"
#include "local.h"

#define GAUSS_LASER_SPR "sprites/laserbeam.spr"
#define GAUSS_WAVE_SPR "abcenchance/spr/gauss_wave.spr"
#define GAUSS_CHARGE_SPR "abcenchance/spr/gauss_spark.spr"
#define GAUSS_LOOPHOLE_MDL "abcenchance/mdl/gauss_loophole.mdl"

#define GAUSS_FIRE_SOUND "weapons/gauss2.wav"
#define GAUSS_WAVE_LENGTH 48
#define GAUSS_LASER_P_WIDTH 2
#define GAUSS_LASER_S_WIDTH 4
using namespace mathlib;

struct EfxVarible{
	int iGaussBeam;
	int iGaussWaveBeam;
	int iGaussChargeSprite;
	float flGaussStartChargeTime;
	model_t* iGaussLoophole;
	mstudioevent_t pGaussFireSoundEvent = { 0,5004,0,GAUSS_FIRE_SOUND };
	TEMPENTITY* pChargeGlow = nullptr;
};
EfxVarible gEfxVarible;
void EfxReset() {
	gEfxVarible.iGaussBeam = PrecacheExtraModel((char*)GAUSS_LASER_SPR);
	gEfxVarible.iGaussWaveBeam = PrecacheExtraModel((char*)GAUSS_WAVE_SPR);
	gEfxVarible.iGaussChargeSprite = PrecacheExtraModel((char*)GAUSS_CHARGE_SPR);
	gEfxVarible.iGaussLoophole = gEngfuncs.hudGetModelByIndex(PrecacheExtraModel((char*)GAUSS_LOOPHOLE_MDL));
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
			nColor = clamp(nColor, 0, 255);
			pTemp->entity.curstate.scale = gEngfuncs.pfnRandomFloat((size / 25.0f), (size / 35.0f));
			pTemp->flags = FTENT_SPRANIMATE;
			pTemp->entity.curstate.rendercolor.r = base_palette1[nColor].r;
			pTemp->entity.curstate.rendercolor.g = base_palette1[nColor].g;
			pTemp->entity.curstate.rendercolor.b = base_palette1[nColor].b;
			pTemp->entity.curstate.framerate = pModel->numframes * 10; //1s
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
				pTemp = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(org, gEngfuncs.hudGetModelByIndex(modelIndex2));
				if (!pTemp)
					return;
				pTemp->flags = FTENT_COLLIDEWORLD | FTENT_SLOWGRAVITY;
				pTemp->entity.curstate.scale = gEngfuncs.pfnRandomFloat((size / 25.0f), (size / 35.0f));
				pTemp->entity.curstate.rendercolor.r = base_palette1[nColor].r;
				pTemp->entity.curstate.rendercolor.g = base_palette1[nColor].g;
				pTemp->entity.curstate.rendercolor.b = base_palette1[nColor].b;
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
		}
	}
	gHookFuncs.R_BloodSprite(org, colorindex, modelIndex, modelIndex2, size);
}
void GaussLoopholeCallback(TEMPENTITY* ent, float frametime, float currenttime) {
	ent->entity.curstate.scale = ent->die - currenttime;
}
void GaussChargeCallback(TEMPENTITY* ent, float frametime, float currenttime) {
	cl_entity_t* view = gEngfuncs.GetViewModel();
	VectorCopy(view->attachment[0], ent->entity.origin);
	ent->entity.curstate.scale = min(0.5, (currenttime - gEfxVarible.flGaussStartChargeTime) / 2);
}
void CreateGaussLoophole(pmtrace_t* tr) {
	TEMPENTITY* pTemp = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(tr->endpos, gEfxVarible.iGaussLoophole);
	if (!pTemp)
		return;
	VectorAngles(tr->plane.normal, pTemp->entity.angles);
	pTemp->entity.angles[0] += 90;
	pTemp->entity.angles[2] += 180;
	pTemp->die = gEngfuncs.GetClientTime() + 1.0f;
	pTemp->flags = FTENT_CLIENTCUSTOM | FTENT_FADEOUT;
	pTemp->callback = GaussLoopholeCallback;
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

	AngleVectors(vecViewAngle, vecForward, nullptr, nullptr);
	VectorCopy(vecForward, vecDir);
	VectorNormalize(vecDir);

	AngleVectors(vecViewAngle, vecForward, NULL, NULL);
	VectorCopy(local->curstate.origin, vecSrc);
	gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(viewOfs);
	VectorAdd(vecSrc, viewOfs, vecSrc);
	VectorMultipiler(vecForward, 8192);
	VectorAdd(vecSrc, vecForward, vecDest);

	while (flDamage > 10 && nMaxHits > 0) {
		//减一次反射次数
		nMaxHits--;
		//做忽略玩家的射线
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecDest, PM_NORMAL, entignore, &tr);
		//高斯直击
		if (tr.allsolid != 0)
			break;
		//你打到了虚空
		if (tr.ent < 0)
			break;
		//初始入射
		if (fFirstBeam) {
			local->curstate.effects |= EF_MUZZLEFLASH;
			fFirstBeam = false;
			gEngfuncs.pEfxAPI->R_BeamEntPoint(local->index + (gExportfuncs.CL_IsThirdPerson() ? 0 : 4096), tr.endpos, gEfxVarible.iGaussBeam, 0.2,
				bparam1 ? GAUSS_LASER_P_WIDTH : GAUSS_LASER_S_WIDTH, 0, 1, 0, 0, 0, 1, 0.8, 0);
		}
		else
			gEngfuncs.pEfxAPI->R_BeamPoints(vecSrc, tr.endpos, gEfxVarible.iGaussBeam, 0.2,
				bparam1 ? GAUSS_LASER_P_WIDTH : GAUSS_LASER_S_WIDTH, 0, 1, 0, 0, 0, 1, 0.8, 0);
		cl_entity_t* hit = gEngfuncs.GetEntityByIndex(tr.ent);
		//可反射高斯
		if (hit && hit->model && hit->model->type == mod_brush) {
			//绘制落点模型
			if (gEfxVarible.iGaussLoophole)
				CreateGaussLoophole(&tr);
			//清空无视实体
			entignore = -1;
			//与击中面法线做点乘，取负数，判断入射角
			
			VectorCopy(tr.plane.normal, vecNormal);
			float n = -DotProduct(vecNormal, vecDir);
			//角度小于60°
			if (n < 0.5) {
				// reflect
				//向量和相加乘二取得终点坐标
				vecReflect[0] = 2.0 * vecNormal[0] * n + vecDir[0];
				vecReflect[1] = 2.0 * vecNormal[1] * n + vecDir[1];
				vecReflect[2] = 2.0 * vecNormal[2] * n + vecDir[2];
				//取得新的射线坐标和方向
				VectorCopy(vecReflect, vecDir);
				VectorCopy(tr.endpos, vecSrc);
				vecDest[0] = vecSrc[0] + vecDir[0] * 8192;
				vecDest[1] = vecSrc[1] + vecDir[1] * 8192;
				vecDest[2] = vecSrc[2] + vecDir[2] * 8192;
				//落点绘制随机散射波动Spr
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
				//不可反射高斯
				//反射光打到人了
				//已经挨了一拳了，不要反射了
				//即穿透直击
				if (fHasPunched)
					break;
				fHasPunched = true;
				//如果右键没法直击到，那就尝试穿墙（左键没法穿墙的）
				if (!bparam1) {
					vecSrc[0] = tr.endpos[0] + vecDir[0] * 8;
					vecSrc[1] = tr.endpos[1] + vecDir[1] * 8;
					vecSrc[2] = tr.endpos[2] + vecDir[2] * 8;
					//以第一次直击的入射点为起点，向玩家的前方8单位做穿透激光起点，以玩家的鼠标所指方向为终点
					gEngfuncs.pEventAPI->EV_SetTraceHull(2);
					gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecDest, PM_NORMAL, entignore, &beam_tr);
					//反射激光落点是固体
					if (beam_tr.allsolid == 0) {
						//以第一次穿透的入口点为重点重置穿透激光
						gEngfuncs.pEventAPI->EV_SetTraceHull(2);
						gEngfuncs.pEventAPI->EV_PlayerTrace(beam_tr.endpos, tr.endpos, PM_NORMAL, entignore, &beam_tr);
						//求该射线长度为n
						VectorSubtract(beam_tr.endpos, tr.endpos, vecLength);
						n = VectorLength(vecLength);
						//如果长度比伤害小
						//射线的后面一点做球
						VectorCopy(beam_tr.endpos, vecRandomSrc);
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
							//不能为0的，长度永远为1
							if (n == 0)
								n = 1;
							//每折射一次伤害少n
							flDamage -= n;
							//下一次主激光点为折射激光点终点，法线重置为向前
							vecSrc[0] = beam_tr.endpos[0] + vecDir[0];
							vecSrc[1] = beam_tr.endpos[1] + vecDir[1];
							vecSrc[2] = beam_tr.endpos[2] + vecDir[2];
						}
					}
					else
						//被生物阻挡，停止循环
						flDamage = 0;
				}
				else {
					//大于60°，不穿透，折射一次后停止循环
					flDamage = 0;
				}
			}
		}
		else {
			//不可反射表面
			//以终点向前尝试一次穿透后停止
			VectorAdd(tr.endpos, vecDir, vecSrc);
			entignore = tr.ent;
		}
	}
}
void pfnPlaybackEvent (int flags, const struct edict_s* pInvoker, unsigned short eventindex, 
	float delay, float* origin, float* angles, 
	float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) {
	//高斯flag 3, index 12
	//高斯蓄力flag1, index 13
	switch (eventindex) {
	case 12: {
		if (gCVars.pGaussEfx->value > 0) {
			gEngfuncs.pEventAPI->EV_WeaponAnimation(6, 0);
			gExportfuncs.HUD_StudioEvent(&gEfxVarible.pGaussFireSoundEvent, gEngfuncs.GetViewModel());
			gHookFuncs.SetPunchAngle(0, -1.5f);
			//f1 伤害
			//b1 是否左键
			DoGaussFire(fparam1, bparam1);
			break;
		}
	}
	default:gHookFuncs.pfnPlaybackEvent(flags, pInvoker, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2); break;
	}
}