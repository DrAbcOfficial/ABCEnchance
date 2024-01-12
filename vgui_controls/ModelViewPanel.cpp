#include <metahook.h>

#include <studio.h>
#include <com_model.h>

#include "local.h"
#include "VGUI.h"

#include <glew.h>
#include <gl_utility.h>
#include <gl_draw.h>

#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/IVGUI.h>

#include <mymathlib.h>
#include <exportfuncs.h>
#include "StudioModelRenderer.h"

#include "ModelViewPanel.h"
#include <vguilocal.h>

using namespace vgui;

class StudioModel {
public:
	void Init(const char* modelname) {
		m_pstudiomdl = LoadModel(modelname);
		auto hdr = GetStudioHdr(m_pstudiomdl);
		// preload textures
		if (hdr->numtextures == 0) {
			char texturename[256];

			strcpy(texturename, modelname);
			strcpy(&texturename[strlen(texturename) - 4], "T.mdl");
			m_ptexturemdl = LoadModel(texturename);
		}
		else
			m_ptexturemdl = m_pstudiomdl;

		// preload animations
		if (hdr->numseqgroups > 1)
		{
			for (int i = 1; i < hdr->numseqgroups; i++)
			{
				char seqgroupname[256];

				strcpy(seqgroupname, modelname);
				sprintf(&seqgroupname[strlen(seqgroupname) - 4], "%02d.mdl", i);

				m_panimmdl[i] = LoadDemandSequences(seqgroupname);
			}
		}
	}
	void DrawModel(void) {
		int i;

		m_smodels_total++; // render data cache cookie

		m_pxformverts = &m_xformverts[0];
		m_pvlightvalues = &m_lightvalues[0];

		auto hdr = GetStudioHdr(m_pstudiomdl);
		if (hdr->numbodyparts == 0)
			return;

		glPushMatrix();

		glTranslatef(m_origin[0], m_origin[1], m_origin[2]);

		glRotatef(m_angles[1], 0, 0, 1);
		glRotatef(m_angles[0], 0, 1, 0);
		glRotatef(m_angles[2], 1, 0, 0);

		// glShadeModel (GL_SMOOTH);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

		SetUpBones();

		SetupLighting();

		for (i = 0; i < hdr->numbodyparts; i++)
		{
			SetupModel(i);
			DrawPoints();
		}

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		// glShadeModel (GL_FLAT);

		// glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

		glPopMatrix();
	}
	void AdvanceFrame(float dt) {
		mstudioseqdesc_t* pseqdesc;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		pseqdesc = (mstudioseqdesc_t*)((byte*)hdr + hdr->seqindex) + m_sequence;

		if (dt > 0.1)
			dt = (float)0.1;
		m_frame += dt * pseqdesc->fps;

		if (pseqdesc->numframes <= 1)
		{
			m_frame = 0;
		}
		else
		{
			// wrap
			m_frame -= (int)(m_frame / (pseqdesc->numframes - 1)) * (pseqdesc->numframes - 1);
		}
	}

	void ExtractBbox(float* mins, float* maxs) {
		mstudioseqdesc_t* pseqdesc;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		pseqdesc = (mstudioseqdesc_t*)((byte*)hdr + hdr->seqindex);

		mins[0] = pseqdesc[m_sequence].bbmin[0];
		mins[1] = pseqdesc[m_sequence].bbmin[1];
		mins[2] = pseqdesc[m_sequence].bbmin[2];

		maxs[0] = pseqdesc[m_sequence].bbmax[0];
		maxs[1] = pseqdesc[m_sequence].bbmax[1];
		maxs[2] = pseqdesc[m_sequence].bbmax[2];
	}

	int SetSequence(int iSequence) {
		auto hdr = GetStudioHdr(m_pstudiomdl);
		if (iSequence > hdr->numseq)
			iSequence = 0;
		if (iSequence < 0)
			iSequence = hdr->numseq - 1;

		m_sequence = iSequence;
		m_frame = 0;

		return m_sequence;
	}
	int GetSequence(void) {
		return m_sequence;
	}
	void GetSequenceInfo(float* pflFrameRate, float* pflGroundSpeed) {
		mstudioseqdesc_t* pseqdesc;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		pseqdesc = (mstudioseqdesc_t*)((byte*)hdr + hdr->seqindex) + (int)m_sequence;

		if (pseqdesc->numframes > 1)
		{
			*pflFrameRate = 256 * pseqdesc->fps / (pseqdesc->numframes - 1);
			*pflGroundSpeed = sqrt(pseqdesc->linearmovement[0] * pseqdesc->linearmovement[0] + pseqdesc->linearmovement[1] * pseqdesc->linearmovement[1] + pseqdesc->linearmovement[2] * pseqdesc->linearmovement[2]);
			*pflGroundSpeed = *pflGroundSpeed * pseqdesc->fps / (pseqdesc->numframes - 1);
		}
		else
		{
			*pflFrameRate = 256.0;
			*pflGroundSpeed = 0.0;
		}
	}

	float SetController(int iController, float flValue) {
		int i;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		mstudiobonecontroller_t* pbonecontroller = (mstudiobonecontroller_t*)((byte*)hdr + hdr->bonecontrollerindex);

		// find first controller that matches the index
		for (i = 0; i < hdr->numbonecontrollers; i++, pbonecontroller++)
		{
			if (pbonecontroller->index == iController)
				break;
		}
		if (i >= hdr->numbonecontrollers)
			return flValue;

		// wrap 0..360 if it's a rotational controller
		if (pbonecontroller->type & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
		{
			// ugly hack, invert value if end < start
			if (pbonecontroller->end < pbonecontroller->start)
				flValue = -flValue;

			// does the controller not wrap?
			if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
			{
				if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0) + 180)
					flValue = flValue - 360;
				if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0) - 180)
					flValue = flValue + 360;
			}
			else
			{
				if (flValue > 360)
					flValue = flValue - (int)(flValue / 360.0) * 360.0;
				else if (flValue < 0)
					flValue = flValue + (int)((flValue / -360.0) + 1) * 360.0;
			}
		}

		int setting = 255 * (flValue - pbonecontroller->start) / (pbonecontroller->end - pbonecontroller->start);

		if (setting < 0) setting = 0;
		if (setting > 255) setting = 255;
		m_controller[iController] = setting;

		return setting * (1.0 / 255.0) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
	}
	float SetMouth(float flValue) {
		auto hdr = GetStudioHdr(m_pstudiomdl);
		mstudiobonecontroller_t* pbonecontroller = (mstudiobonecontroller_t*)((byte*)hdr + hdr->bonecontrollerindex);

		// find first controller that matches the mouth
		for (int i = 0; i < hdr->numbonecontrollers; i++, pbonecontroller++)
		{
			if (pbonecontroller->index == 4)
				break;
		}

		// wrap 0..360 if it's a rotational controller
		if (pbonecontroller->type & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
		{
			// ugly hack, invert value if end < start
			if (pbonecontroller->end < pbonecontroller->start)
				flValue = -flValue;

			// does the controller not wrap?
			if (pbonecontroller->start + 359.0 >= pbonecontroller->end)
			{
				if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0) + 180)
					flValue = flValue - 360;
				if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0) - 180)
					flValue = flValue + 360;
			}
			else
			{
				if (flValue > 360)
					flValue = flValue - (int)(flValue / 360.0) * 360.0;
				else if (flValue < 0)
					flValue = flValue + (int)((flValue / -360.0) + 1) * 360.0;
			}
		}

		int setting = 64 * (flValue - pbonecontroller->start) / (pbonecontroller->end - pbonecontroller->start);

		if (setting < 0) setting = 0;
		if (setting > 64) setting = 64;
		m_mouth = setting;

		return setting * (1.0 / 64.0) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
	}
	float SetBlending(int iBlender, float flValue) {
		mstudioseqdesc_t* pseqdesc;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		pseqdesc = (mstudioseqdesc_t*)((byte*)hdr + hdr->seqindex) + (int)m_sequence;

		if (pseqdesc->blendtype[iBlender] == 0)
			return flValue;

		if (pseqdesc->blendtype[iBlender] & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
		{
			// ugly hack, invert value if end < start
			if (pseqdesc->blendend[iBlender] < pseqdesc->blendstart[iBlender])
				flValue = -flValue;

			// does the controller not wrap?
			if (pseqdesc->blendstart[iBlender] + 359.0 >= pseqdesc->blendend[iBlender])
			{
				if (flValue > ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0) + 180)
					flValue = flValue - 360;
				if (flValue < ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0) - 180)
					flValue = flValue + 360;
			}
		}

		int setting = 255 * (flValue - pseqdesc->blendstart[iBlender]) / (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]);

		if (setting < 0) setting = 0;
		if (setting > 255) setting = 255;

		m_blending[iBlender] = setting;

		return setting * (1.0 / 255.0) * (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]) + pseqdesc->blendstart[iBlender];
	}
	int SetBodygroup(int iGroup, int iValue) {
		auto hdr = GetStudioHdr(m_pstudiomdl);
		if (iGroup > hdr->numbodyparts)
			return -1;

		mstudiobodyparts_t* pbodypart = (mstudiobodyparts_t*)((byte*)hdr + hdr->bodypartindex) + iGroup;

		int iCurrent = (m_bodynum / pbodypart->base) % pbodypart->nummodels;

		if (iValue >= pbodypart->nummodels)
			return iCurrent;

		m_bodynum = (m_bodynum - (iCurrent * pbodypart->base) + (iValue * pbodypart->base));

		return iValue;
	}

	int SetSkin(int iValue) {
		auto hdr = GetStudioHdr(m_pstudiomdl);
		if (iValue < hdr->numskinfamilies)
		{
			return m_skinnum;
		}

		m_skinnum = iValue;

		return iValue;
	}

	// entity settings
	vec3_t					m_origin;
	vec3_t					m_angles;
	int						m_sequence;			// sequence index
	float					m_frame;			// frame
	int						m_bodynum;			// bodypart selection	
	int						m_skinnum;			// skin group selection
	byte					m_controller[4];	// bone controllers
	byte					m_blending[2];		// animation blending
	byte					m_mouth;			// mouth position

	// internal data
	model_t* m_pstudiomdl;
	mstudiomodel_t* m_pmodel;
	model_t* m_ptexturemdl;
	model_t* m_panimmdl[32];

	vec4_t					m_adj;				// FIX: non persistant, make static

	//Before everything, we need a studio model
	int				m_smodels_total;				// cookie
	float			m_lambert = 1.5;
	vec3_t			m_vright; // needs to be set to viewer's right in order for chrome to work
	vec3_t* m_pxformverts;
	vec3_t* m_pvlightvalues;
	vec3_t			m_xformverts[MAXSTUDIOVERTS];	// transformed vertices
	vec3_t			m_lightvec;						// light vector in model reference frame
	vec3_t			m_lightvalues[MAXSTUDIOVERTS];	// light surface normals
	vec3_t			m_blightvec[MAXSTUDIOBONES];	// light vectors in bone reference frames
	float			m_bonetransform[MAXSTUDIOBONES][3][4];	// bone transformation matrix
	int				m_chrome[MAXSTUDIOVERTS][2];	// texture coords for surface normals
	int				m_chromeage[MAXSTUDIOBONES];	// last time chrome vectors were updated
	vec3_t			m_chromeup[MAXSTUDIOBONES];		// chrome vector "up" in bone reference frames
	vec3_t			m_chromeright[MAXSTUDIOBONES];	// chrome vector "right" in bone reference frames
	vec3_t			m_lightcolor;
	int				m_ambientlight;					// ambient world light
	float			m_shadelight;					// direct world light

	model_t* LoadModel(const char* modelname) {
		return IEngineStudio.Mod_ForName(modelname, false);
	}

	studiohdr_t* GetStudioHdr(model_t* mdl) {
		return reinterpret_cast<studiohdr_t*>(IEngineStudio.Mod_Extradata(mdl));
	}

	model_t* LoadDemandSequences(char* modelname) {
		return IEngineStudio.Mod_ForName(modelname, false);
	}

	studioseqhdr_t* GetSequenceHdr(model_t* mdl) {
		return reinterpret_cast<studioseqhdr_t*>(IEngineStudio.Mod_Extradata(mdl));
	}


	void CalcBoneAdj(void) {
		int					i, j;
		float				value;
		mstudiobonecontroller_t* pbonecontroller;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		pbonecontroller = (mstudiobonecontroller_t*)((byte*)hdr + hdr->bonecontrollerindex);

		for (j = 0; j < hdr->numbonecontrollers; j++)
		{
			i = pbonecontroller[j].index;
			if (i <= 3)
			{
				// check for 360% wrapping
				if (pbonecontroller[j].type & STUDIO_RLOOP)
				{
					value = m_controller[i] * (360.0 / 256.0) + pbonecontroller[j].start;
				}
				else
				{
					value = m_controller[i] / 255.0;
					if (value < 0) value = 0;
					if (value > 1.0) value = 1.0;
					value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
				}
				// Con_DPrintf( "%d %d %f : %f\n", m_controller[j], m_prevcontroller[j], value, dadt );
			}
			else
			{
				value = m_mouth / 64.0;
				if (value > 1.0) value = 1.0;
				value = (1.0 - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
				// Con_DPrintf("%d %f\n", mouthopen, value );
			}
			switch (pbonecontroller[j].type & STUDIO_TYPES)
			{
			case STUDIO_XR:
			case STUDIO_YR:
			case STUDIO_ZR:
				m_adj[j] = value * (mathlib::Q_PI / 180.0);
				break;
			case STUDIO_X:
			case STUDIO_Y:
			case STUDIO_Z:
				m_adj[j] = value;
				break;
			}
		}
	}
	void CalcBoneQuaternion(int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* q) {
		int					j, k;
		vec4_t				q1, q2;
		vec3_t				angle1, angle2;
		mstudioanimvalue_t* panimvalue;

		for (j = 0; j < 3; j++)
		{
			if (panim->offset[j + 3] == 0)
			{
				angle2[j] = angle1[j] = pbone->value[j + 3]; // default;
			}
			else
			{
				panimvalue = (mstudioanimvalue_t*)((byte*)panim + panim->offset[j + 3]);
				k = frame;
				while (panimvalue->num.total <= k)
				{
					k -= panimvalue->num.total;
					panimvalue += panimvalue->num.valid + 1;
				}
				// Bah, missing blend!
				if (panimvalue->num.valid > k)
				{
					angle1[j] = panimvalue[k + 1].value;

					if (panimvalue->num.valid > k + 1)
					{
						angle2[j] = panimvalue[k + 2].value;
					}
					else
					{
						if (panimvalue->num.total > k + 1)
							angle2[j] = angle1[j];
						else
							angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
					}
				}
				else
				{
					angle1[j] = panimvalue[panimvalue->num.valid].value;
					if (panimvalue->num.total > k + 1)
					{
						angle2[j] = angle1[j];
					}
					else
					{
						angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
					}
				}
				angle1[j] = pbone->value[j + 3] + angle1[j] * pbone->scale[j + 3];
				angle2[j] = pbone->value[j + 3] + angle2[j] * pbone->scale[j + 3];
			}

			if (pbone->bonecontroller[j + 3] != -1)
			{
				angle1[j] += m_adj[pbone->bonecontroller[j + 3]];
				angle2[j] += m_adj[pbone->bonecontroller[j + 3]];
			}
		}

		if (!mathlib::VectorCompare(angle1, angle2))
		{
			mathlib::AngleQuaternion(angle1, q1);
			mathlib::AngleQuaternion(angle2, q2);
			mathlib::QuaternionSlerp(q1, q2, s, q);
		}
		else
		{
			mathlib::AngleQuaternion(angle1, q);
		}
	}
	void CalcBonePosition(int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* pos) {
		int					j, k;
		mstudioanimvalue_t* panimvalue;

		for (j = 0; j < 3; j++)
		{
			pos[j] = pbone->value[j]; // default;
			if (panim->offset[j] != 0)
			{
				panimvalue = (mstudioanimvalue_t*)((byte*)panim + panim->offset[j]);

				k = frame;
				// find span of values that includes the frame we want
				while (panimvalue->num.total <= k)
				{
					k -= panimvalue->num.total;
					panimvalue += panimvalue->num.valid + 1;
				}
				// if we're inside the span
				if (panimvalue->num.valid > k)
				{
					// and there's more data in the span
					if (panimvalue->num.valid > k + 1)
					{
						pos[j] += (panimvalue[k + 1].value * (1.0 - s) + s * panimvalue[k + 2].value) * pbone->scale[j];
					}
					else
					{
						pos[j] += panimvalue[k + 1].value * pbone->scale[j];
					}
				}
				else
				{
					// are we at the end of the repeating values section and there's another section with data?
					if (panimvalue->num.total <= k + 1)
					{
						pos[j] += (panimvalue[panimvalue->num.valid].value * (1.0 - s) + s * panimvalue[panimvalue->num.valid + 2].value) * pbone->scale[j];
					}
					else
					{
						pos[j] += panimvalue[panimvalue->num.valid].value * pbone->scale[j];
					}
				}
			}
			if (pbone->bonecontroller[j] != -1)
			{
				pos[j] += m_adj[pbone->bonecontroller[j]];
			}
		}
	}
	void CalcRotations(vec3_t* pos, vec4_t* q, mstudioseqdesc_t* pseqdesc, mstudioanim_t* panim, float f) {
		int					i;
		int					frame;
		mstudiobone_t* pbone;
		float				s;

		frame = (int)f;
		s = (f - frame);

		// add in programatic controllers
		CalcBoneAdj();
		auto hdr = GetStudioHdr(m_pstudiomdl);
		pbone = (mstudiobone_t*)((byte*)hdr + hdr->boneindex);
		for (i = 0; i < hdr->numbones; i++, pbone++, panim++)
		{
			CalcBoneQuaternion(frame, s, pbone, panim, q[i]);
			CalcBonePosition(frame, s, pbone, panim, pos[i]);
		}

		if (pseqdesc->motiontype & STUDIO_X)
			pos[pseqdesc->motionbone][0] = 0.0;
		if (pseqdesc->motiontype & STUDIO_Y)
			pos[pseqdesc->motionbone][1] = 0.0;
		if (pseqdesc->motiontype & STUDIO_Z)
			pos[pseqdesc->motionbone][2] = 0.0;
	}
	mstudioanim_t* GetAnim(mstudioseqdesc_t* pseqdesc) {
		mstudioseqgroup_t* pseqgroup;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		pseqgroup = (mstudioseqgroup_t*)((byte*)hdr + hdr->seqgroupindex) + pseqdesc->seqgroup;

		if (pseqdesc->seqgroup == 0)
		{
			return (mstudioanim_t*)((byte*)hdr + pseqgroup->data + pseqdesc->animindex);
		}
		auto seqhdr = GetSequenceHdr(m_panimmdl[pseqdesc->seqgroup]);
		return (mstudioanim_t*)((byte*)seqhdr + pseqdesc->animindex);
	}
	void SlerpBones(vec4_t q1[], vec3_t pos1[], vec4_t q2[], vec3_t pos2[], float s) {
		int			i;
		vec4_t		q3;
		float		s1;

		if (s < 0) s = 0;
		else if (s > 1.0) s = 1.0;

		s1 = 1.0 - s;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		for (i = 0; i < hdr->numbones; i++)
		{
			mathlib::QuaternionSlerp(q1[i], q2[i], s, q3);
			q1[i][0] = q3[0];
			q1[i][1] = q3[1];
			q1[i][2] = q3[2];
			q1[i][3] = q3[3];
			pos1[i][0] = pos1[i][0] * s1 + pos2[i][0] * s;
			pos1[i][1] = pos1[i][1] * s1 + pos2[i][1] * s;
			pos1[i][2] = pos1[i][2] * s1 + pos2[i][2] * s;
		}
	}

	void SetUpBones(void) {
		int					i;

		mstudiobone_t* pbones;
		mstudioseqdesc_t* pseqdesc;
		mstudioanim_t* panim;

		static vec3_t		pos[MAXSTUDIOBONES];
		float				bonematrix[3][4];
		static vec4_t		q[MAXSTUDIOBONES];

		static vec3_t		pos2[MAXSTUDIOBONES];
		static vec4_t		q2[MAXSTUDIOBONES];
		static vec3_t		pos3[MAXSTUDIOBONES];
		static vec4_t		q3[MAXSTUDIOBONES];
		static vec3_t		pos4[MAXSTUDIOBONES];
		static vec4_t		q4[MAXSTUDIOBONES];
		auto hdr = GetStudioHdr(m_pstudiomdl);

		if (m_sequence >= hdr->numseq) {
			m_sequence = 0;
		}

		pseqdesc = (mstudioseqdesc_t*)((byte*)hdr + hdr->seqindex) + m_sequence;

		panim = GetAnim(pseqdesc);
		CalcRotations(pos, q, pseqdesc, panim, m_frame);

		if (pseqdesc->numblends > 1)
		{
			float				s;

			panim += hdr->numbones;
			CalcRotations(pos2, q2, pseqdesc, panim, m_frame);
			s = m_blending[0] / 255.0;

			SlerpBones(q, pos, q2, pos2, s);

			if (pseqdesc->numblends == 4)
			{
				panim += hdr->numbones;
				CalcRotations(pos3, q3, pseqdesc, panim, m_frame);

				panim += hdr->numbones;
				CalcRotations(pos4, q4, pseqdesc, panim, m_frame);

				s = m_blending[0] / 255.0;
				SlerpBones(q3, pos3, q4, pos4, s);

				s = m_blending[1] / 255.0;
				SlerpBones(q, pos, q3, pos3, s);
			}
		}

		pbones = (mstudiobone_t*)((byte*)hdr + hdr->boneindex);

		for (i = 0; i < hdr->numbones; i++) {
			mathlib::QuaternionMatrix(q[i], bonematrix);

			bonematrix[0][3] = pos[i][0];
			bonematrix[1][3] = pos[i][1];
			bonematrix[2][3] = pos[i][2];

			if (pbones[i].parent == -1) {
				memcpy(m_bonetransform[i], bonematrix, sizeof(float) * 12);
			}
			else {
				mathlib::R_ConcatTransforms(m_bonetransform[pbones[i].parent], bonematrix, m_bonetransform[i]);
			}
		}
	}

	void DrawPoints(void) {
		int					i, j;
		mstudiomesh_t* pmesh;
		byte* pvertbone;
		byte* pnormbone;
		vec3_t* pstudioverts;
		vec3_t* pstudionorms;
		mstudiotexture_t* ptexture;
		float* av;
		float* lv;
		float				lv_tmp;
		short* pskinref;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		auto texhdr = GetStudioHdr(m_ptexturemdl);
		pvertbone = ((byte*)hdr + m_pmodel->vertinfoindex);
		pnormbone = ((byte*)hdr + m_pmodel->norminfoindex);
		ptexture = (mstudiotexture_t*)((byte*)texhdr + texhdr->textureindex);

		pmesh = (mstudiomesh_t*)((byte*)hdr + m_pmodel->meshindex);

		pstudioverts = (vec3_t*)((byte*)hdr + m_pmodel->vertindex);
		pstudionorms = (vec3_t*)((byte*)hdr + m_pmodel->normindex);

		pskinref = (short*)((byte*)texhdr + texhdr->skinindex);
		if (m_skinnum != 0 && m_skinnum < texhdr->numskinfamilies)
			pskinref += (m_skinnum * texhdr->numskinref);

		for (i = 0; i < m_pmodel->numverts; i++)
		{
			mathlib::VectorTransform(pstudioverts[i], m_bonetransform[pvertbone[i]], m_pxformverts[i]);
		}

		//
		// clip and draw all triangles
		//

		lv = (float*)m_pvlightvalues;
		for (j = 0; j < m_pmodel->nummesh; j++)
		{
			int flags;
			flags = ptexture[pskinref[pmesh[j].skinref]].flags;
			for (i = 0; i < pmesh[j].numnorms; i++, lv += 3, pstudionorms++, pnormbone++)
			{
				Lighting(&lv_tmp, *pnormbone, flags, (float*)pstudionorms);

				// FIX: move this check out of the inner loop
				if (flags & STUDIO_NF_CHROME)
					Chrome(m_chrome[(float(*)[3])lv - m_pvlightvalues], *pnormbone, (float*)pstudionorms);

				lv[0] = lv_tmp * m_lightcolor[0];
				lv[1] = lv_tmp * m_lightcolor[1];
				lv[2] = lv_tmp * m_lightcolor[2];
			}
		}

		glCullFace(GL_FRONT);

		for (j = 0; j < m_pmodel->nummesh; j++)
		{
			float s, t;
			short* ptricmds;

			pmesh = (mstudiomesh_t*)((byte*)hdr + m_pmodel->meshindex) + j;
			ptricmds = (short*)((byte*)hdr + pmesh->triindex);

			s = 1.0 / (float)ptexture[pskinref[pmesh->skinref]].width;
			t = 1.0 / (float)ptexture[pskinref[pmesh->skinref]].height;

			glBindTexture(GL_TEXTURE_2D, ptexture[pskinref[pmesh->skinref]].index);

			if (ptexture[pskinref[pmesh->skinref]].flags & STUDIO_NF_CHROME)
			{
				while (i = *(ptricmds++))
				{
					if (i < 0)
					{
						glBegin(GL_TRIANGLE_FAN);
						i = -i;
					}
					else
					{
						glBegin(GL_TRIANGLE_STRIP);
					}


					for (; i > 0; i--, ptricmds += 4)
					{
						// FIX: put these in as integer coords, not floats
						glTexCoord2f(m_chrome[ptricmds[1]][0] * s, m_chrome[ptricmds[1]][1] * t);

						lv = m_pvlightvalues[ptricmds[1]];
						glColor4f(lv[0], lv[1], lv[2], 1.0);

						av = m_pxformverts[ptricmds[0]];
						glVertex3f(av[0], av[1], av[2]);
					}
					glEnd();
				}
			}
			else
			{
				while (i = *(ptricmds++))
				{
					if (i < 0)
					{
						glBegin(GL_TRIANGLE_FAN);
						i = -i;
					}
					else
					{
						glBegin(GL_TRIANGLE_STRIP);
					}


					for (; i > 0; i--, ptricmds += 4)
					{
						// FIX: put these in as integer coords, not floats
						glTexCoord2f(ptricmds[2] * s, ptricmds[3] * t);

						lv = m_pvlightvalues[ptricmds[1]];
						glColor4f(lv[0], lv[1], lv[2], 1.0);

						av = m_pxformverts[ptricmds[0]];
						glVertex3f(av[0], av[1], av[2]);
					}
					glEnd();
				}
			}
		}
	}

	void Lighting(float* lv, int bone, int flags, vec3_t normal) {
		float 	illum;
		float	lightcos;

		illum = m_ambientlight;

		if (flags & STUDIO_NF_FLATSHADE)
		{
			illum += m_shadelight * 0.8;
		}
		else
		{
			float r;
			lightcos = mathlib::DotProduct(normal, m_blightvec[bone]); // -1 colinear, 1 opposite

			if (lightcos > 1.0)
				lightcos = 1;

			illum += m_shadelight;

			r = m_lambert;
			if (r <= 1.0) r = 1.0;

			lightcos = (lightcos + (r - 1.0)) / r; 		// do modified hemispherical lighting
			if (lightcos > 0.0)
			{
				illum -= m_shadelight * lightcos;
			}
			if (illum <= 0)
				illum = 0;
		}

		if (illum > 255)
			illum = 255;
		*lv = illum / 255.0;	// Light from 0 to 1.0
	}
	void Chrome(int* pchrome, int bone, vec3_t normal) {
		float n;

		if (m_chromeage[bone] != m_smodels_total)
		{
			// calculate vectors from the viewer to the bone. This roughly adjusts for position
			vec3_t chromeupvec;		// g_chrome t vector in world reference frame
			vec3_t chromerightvec;	// g_chrome s vector in world reference frame
			vec3_t tmp;				// vector pointing at bone in world reference frame
			mathlib::VectorScale(m_origin, -1, tmp);
			tmp[0] += m_bonetransform[bone][0][3];
			tmp[1] += m_bonetransform[bone][1][3];
			tmp[2] += m_bonetransform[bone][2][3];
			mathlib::VectorNormalize(tmp);
			mathlib::CrossProduct(tmp, m_vright, chromeupvec);
			mathlib::VectorNormalize(chromeupvec);
			mathlib::CrossProduct(tmp, chromeupvec, chromerightvec);
			mathlib::VectorNormalize(chromerightvec);

			mathlib::VectorIRotate(chromeupvec, m_bonetransform[bone], m_chromeup[bone]);
			mathlib::VectorIRotate(chromerightvec, m_bonetransform[bone], m_chromeright[bone]);

			m_chromeage[bone] = m_smodels_total;
		}

		// calc s coord
		n = mathlib::DotProduct(normal, m_chromeright[bone]);
		pchrome[0] = (n + 1.0) * 32; // FIX: make this a float

		// calc t coord
		n = mathlib::DotProduct(normal, m_chromeup[bone]);
		pchrome[1] = (n + 1.0) * 32; // FIX: make this a float
	}

	void SetupLighting(void) {
		int i;
		m_ambientlight = 32;
		m_shadelight = 192;

		m_lightvec[0] = 0;
		m_lightvec[1] = 0;
		m_lightvec[2] = -1.0;

		m_lightcolor[0] = 1.0;
		m_lightcolor[1] = 1.0;
		m_lightcolor[2] = 1.0;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		// TODO: only do it for bones that actually have textures
		for (i = 0; i < hdr->numbones; i++)
		{
			mathlib::VectorIRotate(m_lightvec, m_bonetransform[i], m_blightvec[i]);
		}
	}

	void SetupModel(int bodypart) {
		int index;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		if (bodypart > hdr->numbodyparts)
		{
			// Con_DPrintf ("StudioModel::SetupModel: no such bodypart %d\n", bodypart);
			bodypart = 0;
		}

		mstudiobodyparts_t* pbodypart = (mstudiobodyparts_t*)((byte*)hdr + hdr->bodypartindex) + bodypart;

		index = m_bodynum / pbodypart->base;
		index = index % pbodypart->nummodels;

		m_pmodel = (mstudiomodel_t*)((byte*)hdr + pbodypart->modelindex) + index;
	}
};

DECLARE_BUILD_FACTORY(ModelViewPanel);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
ModelViewPanel::ModelViewPanel(Panel *parent, const char *name) : BaseClass(parent, name){
	m_Renderer = new StudioModel();

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);

	glGenFramebuffers(1, &m_hBufferFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hBufferFBO);
	int w, h;
	GetSize(w, h);
	m_hBufferTex = GL_GenTextureRGBA8(w, h);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_hBufferTex, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);
}

vgui::ModelViewPanel::~ModelViewPanel(){
	if (m_hBufferFBO)
		glDeleteFramebuffers(1, &m_hBufferFBO);
	delete m_Renderer;
}

void vgui::ModelViewPanel::LoadModel(const char* model){
	m_Renderer->Init(model);
	m_Renderer->SetSequence(0);
	m_Renderer->SetController(0, 0.0);
	m_Renderer->SetController(1, 0.0);
	m_Renderer->SetController(2, 0.0);
	m_Renderer->SetController(3, 0.0);
	m_Renderer->SetMouth(0);
}

void vgui::ModelViewPanel::Render(){
	int w, h;
	GetSize(w, h);
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
	GL_BlitFrameBufferToFrameBufferColorOnly(m_oldFrameBuffer, m_hBufferFBO, w, h, w, h);
	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);

	m_Renderer->SetBlending(0, 0.0);
	m_Renderer->SetBlending(1, 0.0);
	m_Renderer->DrawModel();
}

void vgui::ModelViewPanel::Pan(int x, int y) {
	m_aryOrigin[0] += (x - m_iOriginalxpos) / 500;
	m_aryOrigin[1] -= (y - m_iOriginalypos) / 500;
	m_iOriginalxpos = x;
	m_iOriginalypos = y;
}

void vgui::ModelViewPanel::Zoom(int x, int y){
	m_aryOrigin[2] += (x - m_iOriginalxpos) / 20;
	m_iOriginalypos = x;
}


//-----------------------------------------------------------------------------
// Purpose: draws the graph
//-----------------------------------------------------------------------------
void ModelViewPanel::Paint(){
	m_flFov = 90;
	m_aryOrigin[0] = 0;
	m_aryOrigin[1] = 0;
	m_aryOrigin[2] = -40;
	m_aryRotate[0] = -90;
	m_aryRotate[1] = 0;
	m_aryRotate[2] = 235;

	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	int w, h;
	GetSize(w, h);
	int x, y;
	vgui::ipanel()->GetAbsPos(this->GetVPanel(), x, y);
	y = gScreenInfo.iHeight - (y + h);
	glViewport(x, y, w, h);
	float flNear = 4.0f;
	float flFar = 1024.0f;
	auto rlbt = tan(m_flFov * (M_PI / 360.0)) * flNear;
	glFrustum(-rlbt, rlbt, -rlbt, rlbt, flNear, flFar);
	glCullFace(GL_FRONT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(m_aryOrigin[0], m_aryOrigin[1], m_aryOrigin[2]);

	glRotatef(m_aryRotate[0], 1, 0, 0);
	glRotatef(m_aryRotate[1], 0, 1, 0);
	glRotatef(m_aryRotate[2], 0, 0, 1);

	glEnable(GL_DEPTH_TEST);

	Render();

	glDisable(GL_DEPTH_TEST);

	glPopMatrix();
	glViewport(0, 0, gScreenInfo.iWidth, gScreenInfo.iHeight);

	glBind(m_hBufferTex);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(255, 255, 255, 255);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex2f(0, h);
	glTexCoord2f(1, 1);
	glVertex2f(w, h);
	glTexCoord2f(1, 0);
	glVertex2f(w, 0);
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	glEnd();
	glDisable(GL_BLEND);
}

//-----------------------------------------------------------------------------
// Purpose: sets up colors
//-----------------------------------------------------------------------------
void ModelViewPanel::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}
