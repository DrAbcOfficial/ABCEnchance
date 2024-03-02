#include <metahook.h>

#include "pm_defs.h"
#include "pmtrace.h"
#include "event_api.h"
#include "cl_entity.h"
#include "mymathlib.h"
#include "mathlib/vector.h"
#include "com_model.h"
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
	int iGaussLoophole;
	float flGaussStartChargeTime;

	int iGunSmoke;

	int iEgonBeam;
};
EfxVarible gEfxVarible;
extern void* g_pClientSoundEngine;
extern void __fastcall CClient_SoundEngine_PlayFMODSound(void* pSoundEngine, int dummy, int flags, int entindex, float* origin,
	int channel, const char* name, float fvol, float attenuation, int extraflags, int pitch, int sentenceIndex, float soundLength);

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
			nColor = CMathlib::clamp<int>(nColor, 0, 255);
			pTemp->entity.curstate.scale = CMathlib::RANDOM_FLOAT((size / 25.0f), (size / 35.0f));
			pTemp->flags = FTENT_SPRANIMATE;
			pTemp->entity.curstate.rendercolor.r = g_arySvencoopBasePalette1[nColor].r();
			pTemp->entity.curstate.rendercolor.g = g_arySvencoopBasePalette1[nColor].g();
			pTemp->entity.curstate.rendercolor.b = g_arySvencoopBasePalette1[nColor].b();
			pTemp->entity.curstate.framerate = 10.0f * pModel->numframes; //1s
			pTemp->die = gEngfuncs.GetClientTime() + (pModel->numframes / pTemp->entity.curstate.framerate);
			pTemp->entity.angles[2] = CMathlib::RANDOM_FLOAT(0, 360);
			pTemp->bounceFactor = 0;
			up[0] = right[0] = forward[0] = 0.0f;
			up[1] = right[1] = forward[1] = 0.0f;
			up[2] = right[2] = forward[2] = 1.0f;
			for (i = 0; i < gCVars.pBloodSpriteNumber->value; i++){
				CMathlib::VectorCopy(org, offset);
				CMathlib::VectorMA(offset, CMathlib::RANDOM_FLOAT(-0.5f, 0.5f) * size, right, offset);
				CMathlib::VectorMA(offset, CMathlib::RANDOM_FLOAT(-0.5f, 0.5f) * size, up, offset);
				pTemp = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(org, gEngfuncs.hudGetModelByIndex(modelIndex2));
				if (!pTemp)
					return;
				pTemp->flags = FTENT_COLLIDEWORLD | FTENT_SLOWGRAVITY;
				pTemp->entity.curstate.scale = CMathlib::RANDOM_FLOAT((size / 25.0f), (size / 35.0f));
				pTemp->entity.curstate.rendercolor.r = g_arySvencoopBasePalette1[nColor].r();
				pTemp->entity.curstate.rendercolor.g = g_arySvencoopBasePalette1[nColor].g();
				pTemp->entity.curstate.rendercolor.b = g_arySvencoopBasePalette1[nColor].b();
				pTemp->die = gEngfuncs.GetClientTime() + CMathlib::RANDOM_FLOAT(1.0f, 3.0f);
				pTemp->entity.angles[2] = gEngfuncs.pfnRandomFloat(0.0f, 360.0f);
				pTemp->bounceFactor = 0;
				srand((unsigned int)gEngfuncs.GetClientTime() * i - i);
				dir[0] = forward[0] + float(rand()) / float(RAND_MAX) * 2.0f - 1;
				srand((unsigned int)gEngfuncs.GetClientTime() * i + i);
				dir[1] = forward[1] + float(rand()) / float(RAND_MAX) * 2.0f - 1;
				dir[2] = forward[2];
				CMathlib::VectorScale(dir, CMathlib::RANDOM_FLOAT(8.0f * size, 20.0f * size), pTemp->entity.baseline.origin);
				pTemp->entity.baseline.origin[0] += CMathlib::RANDOM_FLOAT(4.0f, gCVars.pBloodSpriteSpeed->value) * (size);
				pTemp->entity.baseline.origin[1] += CMathlib::RANDOM_FLOAT(4.0f, gCVars.pBloodSpriteSpeed->value) * (size / 2);
				pTemp->entity.baseline.origin[2] += CMathlib::RANDOM_FLOAT(4.0f, gCVars.pBloodSpriteSpeed->value) * (size / 4);
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
			ent->entity.curstate.renderamt = (int)((ent->fadeSpeed - currenttime) * 255.0f);
		};
		vec3_t smokedir = { dir[0], dir[1],CMathlib::RANDOM_FLOAT(70, 155)};
		TEMPENTITY* smoke = gHookFuncs.R_TempModel(pos, smokedir, angles, life/2, gEfxVarible.iGunSmoke, 0);
		if (smoke) {
			smoke->flags = FTENT_SPRANIMATE | FTENT_FADEOUT;
			smoke->entity.baseline.scale = CMathlib::RANDOM_FLOAT(0.07f, 0.15f);
			smoke->entity.curstate.renderamt = CMathlib::RANDOM_LONG(10, 25);
			smoke->entity.curstate.rendermode = kRenderTransAlpha;
			smoke->entity.curstate.framerate = CMathlib::RANDOM_FLOAT(25, 35);
			smoke->fadeSpeed = 20.0f;
		}
	}
	return tent;
}

void CreateGaussLoophole(pmtrace_t* tr) {
	TEMPENTITY* pTemp = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(tr->endpos, gEngfuncs.hudGetModelByIndex(gEfxVarible.iGaussLoophole));
	if (!pTemp)
		return;
	CMathlib::VectorAngles(tr->plane.normal, pTemp->entity.angles);
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

	CMathlib::AngleVectors(vecViewAngle, vecForward, nullptr, nullptr);
	CMathlib::VectorCopy(vecForward, vecDir);
	CMathlib::VectorNormalize(vecDir);

	CMathlib::AngleVectors(vecViewAngle, vecForward, NULL, NULL);
	CMathlib::VectorCopy(local->curstate.origin, vecSrc);
	gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(viewOfs);
	CMathlib::VectorAdd(vecSrc, viewOfs, vecSrc);
	CMathlib::VectorMultipiler(vecForward, 8192);
	CMathlib::VectorAdd(vecSrc, vecForward, vecDest);

	while (flDamage > 10 && nMaxHits > 0) {
		//减一次反射次数
		nMaxHits--;
		//做忽略玩家的射线
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecDest, PM_STUDIO_BOX, entignore, &tr);
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
			gEngfuncs.pEfxAPI->R_BeamEntPoint(local->index + (gExportfuncs.CL_IsThirdPerson() ? 0 : 4096), tr.endpos, gEfxVarible.iGaussBeam, 0.2f,
				bparam1 ? (float)GAUSS_LASER_P_WIDTH : (float)GAUSS_LASER_S_WIDTH, 0, 1, 0, 0, 0, 1, bparam1 ? 0.8f : 1.0f, bparam1 ? 0.0f : 1.0f);
		}
		else
			gEngfuncs.pEfxAPI->R_BeamPoints(vecSrc, tr.endpos, gEfxVarible.iGaussBeam, 0.2f,
				bparam1 ? (float)GAUSS_LASER_P_WIDTH : (float)GAUSS_LASER_S_WIDTH, 0, 1, 0, 0, 0, 1, bparam1 ? 0.8f : 1.0f, bparam1 ? 0.0f : 1.0f);
		cl_entity_t* hit = gEngfuncs.GetEntityByIndex(tr.ent);
		//可反射高斯
		if (hit && hit->model && hit->model->type == mod_brush) {
			//绘制落点模型
			if (gEfxVarible.iGaussLoophole)
				CreateGaussLoophole(&tr);
			//清空无视实体
			entignore = -1;
			//与击中面法线做点乘，取负数，判断入射角
			
			CMathlib::VectorCopy(tr.plane.normal, vecNormal);
			float n = -CMathlib::DotProduct(vecNormal, vecDir);
			//角度小于60°
			if (n < 0.5) {
				// reflect
				//向量和相加乘二取得终点坐标
				vecReflect[0] = 2.0f * vecNormal[0] * n + vecDir[0];
				vecReflect[1] = 2.0f * vecNormal[1] * n + vecDir[1];
				vecReflect[2] = 2.0f * vecNormal[2] * n + vecDir[2];
				//取得新的射线坐标和方向
				CMathlib::VectorCopy(vecReflect, vecDir);
				CMathlib::VectorCopy(tr.endpos, vecSrc);
				vecDest[0] = vecSrc[0] + vecDir[0] * 8192.0f;
				vecDest[1] = vecSrc[1] + vecDir[1] * 8192.0f;
				vecDest[2] = vecSrc[2] + vecDir[2] * 8192.0f;
				//落点绘制随机散射波动Spr
				for (int i = 0; i < CMathlib::RANDOM_LONG(0, 4); i++) {
					vecRandom[0] = vecSrc[0] + CMathlib::RANDOM_FLOAT(GAUSS_WAVE_LENGTH / 2, GAUSS_WAVE_LENGTH) *
						(vecNormal[0] * n * CMathlib::RANDOM_FLOAT(1, 3) + vecDir[0] * CMathlib::RANDOM_FLOAT(-3, 3));
					vecRandom[1] = vecSrc[1] + CMathlib::RANDOM_FLOAT(GAUSS_WAVE_LENGTH / 2, GAUSS_WAVE_LENGTH) *
						(vecNormal[1] * n * CMathlib::RANDOM_FLOAT(1, 3) + vecDir[1] * CMathlib::RANDOM_FLOAT(-3, 3));
					vecRandom[2] = vecSrc[2] + CMathlib::RANDOM_FLOAT(GAUSS_WAVE_LENGTH / 2, GAUSS_WAVE_LENGTH) *
						(vecNormal[2] * n * CMathlib::RANDOM_FLOAT(1, 3) + vecDir[2] * CMathlib::RANDOM_FLOAT(-3, 3));
					gEngfuncs.pEfxAPI->R_BeamPoints(vecSrc, vecRandom, gEfxVarible.iGaussWaveBeam, 0.2f,
						bparam1 ? (float)GAUSS_LASER_P_WIDTH : (float)GAUSS_LASER_S_WIDTH, 0, 1, 0, 0, 0, 1, 0.8f, 0);
				}
				// lose energy
				if (n == 0)
					n = 0.1f;
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
					gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecDest, PM_STUDIO_BOX, entignore, &beam_tr);
					//反射激光落点是固体
					if (beam_tr.allsolid == 0) {
						//以第一次穿透的入口点为重点重置穿透激光
						gEngfuncs.pEventAPI->EV_SetTraceHull(2);
						gEngfuncs.pEventAPI->EV_PlayerTrace(beam_tr.endpos, tr.endpos, PM_STUDIO_BOX, entignore, &beam_tr);
						//求该射线长度为n
						CMathlib::VectorSubtract(beam_tr.endpos, tr.endpos, vecLength);
						n = (float)CMathlib::VectorLength(vecLength);
						//如果长度比伤害小
						//射线的后面一点做球
						CMathlib::VectorCopy(beam_tr.endpos, vecRandomSrc);
						vecRandomSrc[0] += vecDir[0] * flDamage;
						vecRandomSrc[1] += vecDir[1] * flDamage;
						vecRandomSrc[2] += vecDir[2] * flDamage;
						for (int i = 0; i < CMathlib::RANDOM_LONG(8, 14); i++) {
							vecRandom[0] = vecRandomSrc[0] + vecDir[0] * CMathlib::RANDOM_FLOAT(-512, 512);
							vecRandom[1] = vecRandomSrc[1] + vecDir[1] * CMathlib::RANDOM_FLOAT(-512, 512);
							vecRandom[2] = vecRandomSrc[2] + vecDir[2] * CMathlib::RANDOM_FLOAT(-512, 512);
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
			CMathlib::VectorAdd(tr.endpos, vecDir, vecSrc);
			entignore = tr.ent;
		}
	}
}

void EV_StopPreviousGauss(){
	int idx = gEngfuncs.GetViewModel()->index;
	// Make sure we don't have a gauss spin event in the queue for this guy
	gEngfuncs.pEventAPI->EV_KillEvents(idx, "events/gaussspin.sc");
	//SND_STOP 32
	CClient_SoundEngine_PlayFMODSound(g_pClientSoundEngine, 0, 32, idx, 0, CHAN_WEAPON, "ambience/pulsemachine.wav", 0.0f, 0.0f, 0, 0, -1, 0.0f);
}
void pfnPlaybackEvent (int flags, const struct edict_s* pInvoker, unsigned short eventindex, 
	float delay, float* origin, float* angles, 
	float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) {
	//高斯flag 3, index 12
	//高斯蓄力flag1, index 13
	switch (eventindex) {
		case 12: {
			if (gCVars.pGaussEfx->value > 0) {
				EV_StopPreviousGauss();
				gEngfuncs.pEventAPI->EV_WeaponAnimation(6, 0);
				auto view = gEngfuncs.GetViewModel();
				CClient_SoundEngine_PlayFMODSound(g_pClientSoundEngine, 0, 0, view->index, view->origin, CHAN_WEAPON, "weapons/gauss2.wav", 1.0f, 1.0f, 0, 100, -1, 0.0f);
				gHookFuncs.SetPunchAngle(0, -1.5f);
				//f1 伤害
				//b1 是否左键
				DoGaussFire(fparam1, bparam1);
				return;
			}
		}
	}
	gHookFuncs.pfnPlaybackEvent(flags, pInvoker, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);
}

static color24 s_egonParticleColor;
static int s_egonMessageNum;
static bool s_egonShooter = false;

void ShootEgonParticle() {
	model_t* pModel = gEngfuncs.hudGetModelByIndex(gEfxVarible.iEgonBeam);
	if (!pModel)
		return;
	auto local = gEngfuncs.GetLocalPlayer();
	Vector vecAngles;
	gEngfuncs.GetViewAngles(vecAngles);
	Vector vecForward;
	CMathlib::AngleVectors(vecAngles, vecForward, nullptr, nullptr);
	Vector vecEnd;
	VectorMA(local->origin, 4096, vecForward, vecEnd);
	pmtrace_t tr;
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(local->index - 1);
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(local->origin, vecEnd, PM_STUDIO_BOX, -1, &tr);
	Vector vecSrc = local->attachment[0];
	TEMPENTITY* tent = gEngfuncs.pEfxAPI->CL_TempEntAlloc(vecSrc, pModel);
	if (tent) {
		Vector vecLength = tr.endpos;
		vecLength -= vecSrc;
		float flSpeed = 3600;
		Vector vecVelocity = vecLength.Normalize() * flSpeed;
		CMathlib::VectorCopy(vecVelocity, tent->entity.baseline.origin);
		tent->flags = FTENT_SPRANIMATELOOP | FTENT_COLLIDEALL | FTENT_FADEOUT;
		tent->clientIndex = gEngfuncs.GetLocalPlayer()->index;
		tent->bounceFactor = 0;
		tent->entity.baseline.renderamt = 80;
		tent->entity.curstate.movetype = MOVETYPE_FLY;
		tent->entity.curstate.solid = SOLID_SLIDEBOX;
		tent->entity.curstate.owner = gEngfuncs.GetLocalPlayer()->index;
		tent->entity.curstate.scale = CMathlib::RANDOM_FLOAT(0.2f, 0.4f);
		tent->entity.curstate.renderamt = 255;
		tent->entity.curstate.rendercolor = s_egonParticleColor;
		tent->entity.curstate.rendermode = kRenderTransAdd;
		tent->entity.curstate.frame = 0;
		tent->entity.curstate.framerate = 10.0f * pModel->numframes; //1s
		tent->hitcallback = [](struct tempent_s* ent, struct pmtrace_s* ptr) {
			if (ent->entity.curstate.iuser1 != 1) {
				gEngfuncs.pEfxAPI->R_DecalShoot(
					gEngfuncs.pEfxAPI->Draw_DecalIndex(gEngfuncs.pEfxAPI->Draw_DecalIndexFromName(const_cast<char*>("{smscorch2"))),
					gEngfuncs.pEventAPI->EV_IndexFromTrace(ptr), 0, ptr->endpos, 0);
				ent->entity.curstate.iuser1 = 1;
			}
			};
		tent->die = gEngfuncs.GetClientTime() + vecLength.Length() / flSpeed;
	}
}
void StartEgonParticle(unsigned char r, unsigned char g, unsigned char b, struct cl_entity_s* ent) {
	s_egonParticleColor = { r,g,b };
	s_egonMessageNum = ent->curstate.messagenum;
	s_egonShooter = true;
}

void EFX_Reset() {
	s_egonMessageNum = 0;
	gEfxVarible.iGaussBeam = PrecacheExtraModel("sprites/laserbeam.spr");
	gEfxVarible.iGaussWaveBeam = PrecacheExtraModel("abcenchance/spr/gauss_wave.spr");
	gEfxVarible.iGaussChargeSprite = PrecacheExtraModel("abcenchance/spr/gauss_spark.spr");
	gEfxVarible.iGaussLoophole = PrecacheExtraModel("abcenchance/mdl/gauss_loophole.mdl");
	gEfxVarible.iGunSmoke = PrecacheExtraModel("sprites/Puff1.spr");
	gEfxVarible.iEgonBeam = PrecacheExtraModel("abcenchance/spr/egon_ball.spr");
}
void EFX_Frame() {
	auto local = gEngfuncs.GetLocalPlayer();
	if (local && s_egonMessageNum != local->curstate.messagenum)
		s_egonShooter = false;
	if (s_egonShooter)
		ShootEgonParticle();
}