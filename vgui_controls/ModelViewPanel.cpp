#include <metahook.h>
#include <algorithm>

#include <studio.h>
#include <com_model.h>


#include <glew.h>

#include <mymathlib.h>
#include <exportfuncs.h>

#include "ModelViewPanel.h"

#undef clamp

using namespace vgui;

class IStudioModel {
public:
	virtual ~IStudioModel() {

	};
	virtual void Init(const char* modelname) = 0;
	virtual void DrawModel(void) = 0;
	virtual void AdvanceFrame(float dt) = 0;
	virtual void ExtractBbox(float* mins, float* maxs) = 0;
	virtual int SetSequence(int iSequence) = 0;
	virtual int GetSequence(void) = 0;
	virtual void GetSequenceInfo(float* pflFrameRate, float* pflGroundSpeed) = 0;
	virtual float SetController(int iController, float flValue) = 0;
	virtual float SetMouth(float flValue) = 0;
	virtual float SetBlending(int iBlender, float flValue) = 0;
	virtual int SetBodygroup(int iGroup, int iValue) = 0;
	virtual int SetSkin(int iValue) = 0;
	virtual void SetFrame(int iValue) = 0;
	virtual bool IsValid() = 0;
	virtual void SetAmbientLight(int light) = 0;
	virtual void SetShadeLight(int light) = 0;
	virtual void SetLightColor(int r, int g, int b) = 0;
	virtual void SetLightOrigin(float x, float y, float z) = 0;
};
class StudioModel : public IStudioModel {
public:
	virtual void Init(const char* modelname) {
		m_pstudiomdl = LoadModel(modelname);
		if (!m_pstudiomdl)
			return;
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
	virtual void DrawModel(void) {
		if (!m_pstudiomdl)
			return;
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

		glShadeModel (GL_SMOOTH);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

		SetUpBones();

		SetupLighting();

		for (i = 0; i < hdr->numbodyparts; i++)
		{
			SetupModel(i);
			DrawPoints();
		}

		glShadeModel(GL_FLAT);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glPopMatrix();
	}
	virtual void AdvanceFrame(float dt) {
		if (!m_pstudiomdl)
			return;
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

	virtual void ExtractBbox(float* mins, float* maxs) {
		if (!m_pstudiomdl)
			return;
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

	virtual int SetSequence(int iSequence) {
		if (!m_pstudiomdl)
			return 0;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		if (iSequence > hdr->numseq)
			iSequence = 0;
		if (iSequence < 0)
			iSequence = hdr->numseq - 1;

		m_sequence = iSequence;
		m_frame = 0;

		return m_sequence;
	}
	virtual int GetSequence(void) {
		return m_sequence;
	}
	virtual void GetSequenceInfo(float* pflFrameRate, float* pflGroundSpeed) {
		if (!m_pstudiomdl)
			return;
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

	virtual float SetController(int iController, float flValue) {
		if (!m_pstudiomdl)
			return 0;
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
	virtual float SetMouth(float flValue) {
		if (!m_pstudiomdl)
			return 0;
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
	virtual float SetBlending(int iBlender, float flValue) {
		if (!m_pstudiomdl)
			return 0;
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
	virtual int SetBodygroup(int iGroup, int iValue) {
		if (!m_pstudiomdl)
			return 0;
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

	virtual int SetSkin(int iValue) {
		if (!m_pstudiomdl)
			return 0;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		if (iValue < hdr->numskinfamilies)
		{
			return m_skinnum;
		}

		m_skinnum = iValue;

		return iValue;
	}

	virtual void SetFrame(int iValue) {
		m_frame = iValue;
	}
	virtual bool IsValid() {
		return m_pstudiomdl != nullptr && m_ptexturemdl != nullptr;
	}

	virtual void SetAmbientLight(int light) {
		m_ambientlight = light;
	}
	virtual void SetShadeLight(int light) {
		m_shadelight = light;
	}
	virtual void SetLightColor(int r, int g, int b) {
		m_lightcolor[0] = (float)r / 255.0f;
		m_lightcolor[1] = (float)g / 255.0f;
		m_lightcolor[2] = (float)b / 255.0f;
	}
	virtual void SetLightOrigin(float x, float y, float z) {
		m_lightvec[0] = x;
		m_lightvec[1] = y;
		m_lightvec[2] = -z + -1.0f;
		SetupLighting();
	}
private:
	// entity settings
	vec3_t					m_origin;
	vec3_t					m_angles;
	int						m_bodynum;			// bodypart selection	
	int						m_skinnum;			// skin group selection
	byte					m_controller[4];	// bone controllers
	byte					m_blending[2];		// animation blending
	int						m_sequence;			// sequence index
	float					m_frame;			// frame
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
	vec3_t			m_lightvalues[MAXSTUDIOVERTS];	// light surface normals
	vec3_t			m_blightvec[MAXSTUDIOBONES];	// light vectors in bone reference frames
	float			m_bonetransform[MAXSTUDIOBONES][3][4];	// bone transformation matrix
	int				m_chrome[MAXSTUDIOVERTS][2];	// texture coords for surface normals
	int				m_chromeage[MAXSTUDIOBONES];	// last time chrome vectors were updated
	vec3_t			m_chromeup[MAXSTUDIOBONES];		// chrome vector "up" in bone reference frames
	vec3_t			m_chromeright[MAXSTUDIOBONES];	// chrome vector "right" in bone reference frames

	vec3_t			m_lightvec = { 0.0f,0.0f,-1.0f };						// light vector in model reference frame
	vec3_t			m_lightcolor = { 1.0f,1.0f,1.0f };
	int				m_ambientlight = 32;					// ambient world light
	float			m_shadelight = 192;					// direct world light

	model_t* LoadModel(const char* modelname) {
		return gEngineStudio.Mod_ForName(modelname, false);
	}

	studiohdr_t* GetStudioHdr(model_t* mdl) {
		return reinterpret_cast<studiohdr_t*>(gEngineStudio.Mod_Extradata(mdl));
	}

	model_t* LoadDemandSequences(char* modelname) {
		return gEngineStudio.Mod_ForName(modelname, false);
	}

	studioseqhdr_t* GetSequenceHdr(model_t* mdl) {
		return reinterpret_cast<studioseqhdr_t*>(gEngineStudio.Mod_Extradata(mdl));
	}


	void CalcBoneAdj(void) {
		if (!m_pstudiomdl)
			return;
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
		if (!m_pstudiomdl)
			return;
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

		if (!CMathlib::VectorCompare(angle1, angle2))
		{
			CMathlib::AngleQuaternion(angle1, q1);
			CMathlib::AngleQuaternion(angle2, q2);
			CMathlib::QuaternionSlerp(q1, q2, s, q);
		}
		else
		{
			CMathlib::AngleQuaternion(angle1, q);
		}
	}
	void CalcBonePosition(int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* pos) {
		if (!m_pstudiomdl)
			return;
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
		if (!m_pstudiomdl)
			return;
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
		if (!m_pstudiomdl)
			return nullptr;
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
		if (!m_pstudiomdl)
			return;
		int			i;
		vec4_t		q3;
		float		s1;

		if (s < 0) s = 0;
		else if (s > 1.0) s = 1.0;

		s1 = 1.0 - s;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		for (i = 0; i < hdr->numbones; i++)
		{
			CMathlib::QuaternionSlerp(q1[i], q2[i], s, q3);
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
		if (!m_pstudiomdl)
			return;
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
			CMathlib::QuaternionMatrix(q[i], bonematrix);

			bonematrix[0][3] = pos[i][0];
			bonematrix[1][3] = pos[i][1];
			bonematrix[2][3] = pos[i][2];

			if (pbones[i].parent == -1) {
				memcpy(m_bonetransform[i], bonematrix, sizeof(float) * 12);
			}
			else {
				CMathlib::R_ConcatTransforms(m_bonetransform[pbones[i].parent], bonematrix, m_bonetransform[i]);
			}
		}
	}
	void DrawPoints(void) {

#if 0//Not supproted in Core Profile

		if (!m_pstudiomdl)
			return;
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
			CMathlib::VectorTransform(pstudioverts[i], m_bonetransform[pvertbone[i]], m_pxformverts[i]);
		}

		//
		// clip and draw all triangles
		//

		lv = (float*)m_pvlightvalues;
		for (j = 0; j < m_pmodel->nummesh; j++)
		{
			int flags = ptexture[pskinref[pmesh[j].skinref]].flags;
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
			GL_Bind(ptexture[pskinref[pmesh->skinref]].index);

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
#endif
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
			lightcos = CMathlib::DotProduct(normal, m_blightvec[bone]); // -1 colinear, 1 opposite

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
			CMathlib::VectorScale(m_origin, -1, tmp);
			tmp[0] += m_bonetransform[bone][0][3];
			tmp[1] += m_bonetransform[bone][1][3];
			tmp[2] += m_bonetransform[bone][2][3];
			CMathlib::VectorNormalize(tmp);
			CMathlib::CrossProduct(tmp, m_vright, chromeupvec);
			CMathlib::VectorNormalize(chromeupvec);
			CMathlib::CrossProduct(tmp, chromeupvec, chromerightvec);
			CMathlib::VectorNormalize(chromerightvec);

			CMathlib::VectorIRotate(chromeupvec, m_bonetransform[bone], m_chromeup[bone]);
			CMathlib::VectorIRotate(chromerightvec, m_bonetransform[bone], m_chromeright[bone]);

			m_chromeage[bone] = m_smodels_total;
		}

		// calc s coord
		n = CMathlib::DotProduct(normal, m_chromeright[bone]);
		pchrome[0] = (n + 1.0) * 32; // FIX: make this a float

		// calc t coord
		n = CMathlib::DotProduct(normal, m_chromeup[bone]);
		pchrome[1] = (n + 1.0) * 32; // FIX: make this a float
	}
	void SetupLighting(void) {
		if (!m_pstudiomdl)
			return;
		auto hdr = GetStudioHdr(m_pstudiomdl);
		// TODO: only do it for bones that actually have textures
		for (int i = 0; i < hdr->numbones; i++){
			CMathlib::VectorIRotate(m_lightvec, m_bonetransform[i], m_blightvec[i]);
		}
	}
	void SetupModel(int bodypart) {
		if (!m_pstudiomdl)
			return;
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

mnode_t node;
class EStudioModel : public IStudioModel {
public:
	virtual ~EStudioModel() {

	};
	virtual void Init(const char* modelname) = 0;
	virtual void DrawModel(void) = 0;
	virtual void AdvanceFrame(float dt) = 0;
	virtual void ExtractBbox(float* mins, float* maxs) = 0;
	virtual int SetSequence(int iSequence) = 0;
	virtual int GetSequence(void) = 0;
	virtual void GetSequenceInfo(float* pflFrameRate, float* pflGroundSpeed) = 0;
	virtual float SetController(int iController, float flValue) = 0;
	virtual float SetMouth(float flValue) = 0;
	virtual float SetBlending(int iBlender, float flValue) = 0;
	virtual int SetBodygroup(int iGroup, int iValue) = 0;
	virtual int SetSkin(int iValue) = 0;
	virtual void SetFrame(int iValue) = 0;
	virtual bool IsValid() = 0;
	virtual void SetAmbientLight(int light) = 0;
	virtual void SetShadeLight(int light) = 0;
	virtual void SetLightColor(int r, int g, int b) = 0;
	virtual void SetLightOrigin(float x, float y, float z) = 0;
private:
	cl_entity_t m_pEntity;

};

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
ModelViewPanel::ModelViewPanel(Panel *parent, const char *name) : BaseClass(parent, name){
	m_Renderer = new StudioModel();
	SetModelPos(0, 0, 0);
	SetModelRotate(0, 0, 0);
}

vgui::ModelViewPanel::~ModelViewPanel(){
	if (m_hBufferFBO)
		glDeleteFramebuffers(1, &m_hBufferFBO);
	if (m_hBufferTex)
		glDeleteTextures(1, &m_hBufferTex);
	if (m_hBufferRBO)
		glDeleteRenderbuffers(1, &m_hBufferRBO);
	delete m_Renderer;
}

void vgui::ModelViewPanel::SetupTexBuffer(){
	if (!m_hBufferFBO && !m_hBufferTex) {
		glGenFramebuffers(1, &m_hBufferFBO);
#if 0//need refactor
		int w, h;
		GetSize(w, h);
		m_hBufferTex = GL_GenTextureRGBA8(w, h);

		glGenRenderbuffers(1, &m_hBufferRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_hBufferRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, m_hBufferFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_hBufferTex, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_hBufferRBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);

		m_iFboWidth = w;
		m_iFboHeight = h;
#endif
	}
}

void vgui::ModelViewPanel::LoadModel(const char* model){
	ChangeModel(model);
	SetSequnce(0);
	SetController(0, 0.0);
	SetController(1, 0.0);
	SetController(2, 0.0);
	SetController(3, 0.0);
	SetMouth(0);
	SetBlend(0, 0.0);
	SetBlend(0, 0.0);
}

void vgui::ModelViewPanel::ChangeModel(const char* model){
	if (!std::strcmp(m_szModel, model))
		return;
	std::strcpy(m_szModel, model);
	m_Renderer->Init(model);
}

void vgui::ModelViewPanel::Render(){
	if (m_Renderer->IsValid()) {
		if (GetAnimate()) {
			float ticktime = gEngfuncs.GetClientTime();
			float differ = ticktime - m_flPrevAnimeTime;
			m_Renderer->AdvanceFrame(differ * m_flFrameRate);
			m_flPrevAnimeTime = ticktime;
		}
		m_Renderer->DrawModel();
	}
}

//     ^Z
//     |
//     |   /Y
//     |  /
//     |/
//     O---------->X
void vgui::ModelViewPanel::SetModelPos(float x, float y, float z){
	//swap y and z to fit game world
	m_aryOrigin[0] = x;
	m_aryOrigin[1] = z;
	m_aryOrigin[2] = -y;
}

/// <summary>
/// Rotate Model
/// </summary>
void vgui::ModelViewPanel::SetModelRotate(float pitch, float yaw, float roll){
	//swap y and z to fit game world
	m_aryRotate[0] = fmodf(pitch - 90, 360);
	m_aryRotate[1] = fmodf(roll - 0, 360);
	m_aryRotate[2] = fmodf(-yaw - 90, 360);
}

void vgui::ModelViewPanel::GetModelPos(float& x, float& y, float& z){
	x = m_aryOrigin[0];
	z = m_aryOrigin[1];
	y = -m_aryOrigin[2];
}

void vgui::ModelViewPanel::GetModelRotate(float& pitch, float& yaw, float& roll){
	pitch = fmodf(m_aryRotate[0] + 90, 360);
	roll = fmodf(m_aryRotate[1], 360);
	yaw = fmodf(-m_aryRotate[2] - 90, 360);
}

float vgui::ModelViewPanel::GetFOV(){
	return m_flFov;
}
void vgui::ModelViewPanel::SetFOV(float fov){
	m_flFov = fov;
}
bool vgui::ModelViewPanel::GetAnimate(){
	return m_bAnimate;
}
void vgui::ModelViewPanel::SetAnimate(bool state){
	m_bAnimate = state;
}
int vgui::ModelViewPanel::GetFrame(){
	return m_iFrame;
}
void vgui::ModelViewPanel::SetFrame(int frame){
	m_iFrame = frame;
	m_Renderer->SetFrame(m_iFrame);
}
float vgui::ModelViewPanel::GetFrameRate(){
	return m_flFrameRate;
}
void vgui::ModelViewPanel::SetFrameRate(float rate){
	m_flFrameRate = rate;
}
int vgui::ModelViewPanel::GetSequence(){
	return m_iSequence;
}
void vgui::ModelViewPanel::SetSequnce(int seq){
	m_iSequence = seq;
	m_Renderer->SetSequence(m_iSequence);
}
int vgui::ModelViewPanel::GetSkin(){
	return m_iSkin;
}
void vgui::ModelViewPanel::SetSkin(int skin){
	m_iSkin = skin;
	m_Renderer->SetSkin(skin);
}
int vgui::ModelViewPanel::GetGroup(){
	return m_iGroup;
}
int vgui::ModelViewPanel::GetBody(){
	return m_iBody;
}
void vgui::ModelViewPanel::SetBodygroup(int group, int body){
	m_iGroup = group;
	m_iBody = body;
	m_Renderer->SetBodygroup(group, body);
}
float vgui::ModelViewPanel::GetBlend(int blend){
	int idx = std::clamp(blend, 0, 1);
	return m_aryBlend[idx].m_flValue;
}
void vgui::ModelViewPanel::SetBlend(int blend, float value){
	int idx = std::clamp(blend, 0, 1);
	m_aryBlend[idx].m_iIdx = idx;
	m_aryBlend[idx].m_flValue = value;
	m_Renderer->SetBlending(idx, value);
}
float vgui::ModelViewPanel::GetController(int idx){
	int i = std::clamp(idx, 0, 3);
	return m_aryController[i].m_flValue;
}

void vgui::ModelViewPanel::SetController(int idx, float value){
	int i = std::clamp(idx, 0, 3);
	m_aryController[i].m_iIdx = idx;
	m_aryController[i].m_flValue = value;
	m_Renderer->SetController(i, value);
}

int vgui::ModelViewPanel::GetMouth(){
	return m_iMouth;
}

void vgui::ModelViewPanel::SetMouth(int mouth){
	m_iMouth = mouth;
}

void vgui::ModelViewPanel::SetAmbientLight(int light){
	m_Renderer->SetAmbientLight(light);
}

void vgui::ModelViewPanel::SetShadeLight(int light){
	m_Renderer->SetShadeLight(light);
}

void vgui::ModelViewPanel::SetLightColor(int r, int g, int b){
	m_Renderer->SetLightColor(r, g, b);
}

void vgui::ModelViewPanel::SetLightOrigin(float x, float y, float z){
	m_Renderer->SetLightOrigin(x, y, z);
}

//-----------------------------------------------------------------------------
// Purpose: draws the graph
//-----------------------------------------------------------------------------
void ModelViewPanel::Paint(){

#if 0//Not supported in Core Profile

	GLfloat oldProjection[16], oldModelView[16];
	glGetFloatv(GL_PROJECTION_MATRIX, oldProjection);
	glGetFloatv(GL_MODELVIEW_MATRIX, oldModelView);

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_hBufferFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, m_iFboWidth, m_iFboHeight);

	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	float flNear = 4.0f;
	float flFar = 1024.0f;
	float flAspect = (float)m_iFboWidth / (float)m_iFboHeight;
	float f = 1.0f / tanf(m_flFov * 0.5f * M_PI / 180.0f); // �ӳ��ǵ�����ֵ
	float a = (flFar + flNear) / (flNear - flFar); // ��Զƽ��ı�ֵ
	float b = (2.0f * flFar * flNear) / (flNear - flFar); // ��Զƽ��ĳ˻�
	vec_t matrix[16] = {
		f / flAspect, 0, 0, 0,
		0, f, 0, 0,
		0, 0, a, -1.0f,
		0, 0, b, 0
	};
	glLoadMatrixf(matrix);
	glTranslatef(m_aryOrigin[0], m_aryOrigin[1], m_aryOrigin[2]);

	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(m_aryRotate[0], 1, 0, 0);
	glRotatef(m_aryRotate[1], 0, 1, 0);
	glRotatef(m_aryRotate[2], 0, 0, 1);

	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	Render();
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glPopMatrix();
	glPopMatrix();
	glPopAttrib();
	glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(oldProjection);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(oldModelView);

	GL_Bind(m_hBufferTex);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Color fgcolor = GetFgColor();
	int w, h;
	GetSize(w, h);
	glColor4ub(fgcolor.r(), fgcolor.g(), fgcolor.b(), fgcolor.a());
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(0, h);
	glTexCoord2f(1, 0);
	glVertex2f(w, h);
	glTexCoord2f(1, 1);
	glVertex2f(w, 0);
	glTexCoord2f(0, 1);
	glVertex2f(0, 0);
	glEnd();
	glDisable(GL_BLEND);

#endif
}

void vgui::ModelViewPanel::ApplySettings(KeyValues* inResourceData){
	BaseClass::ApplySettings(inResourceData);
	LoadModel(inResourceData->GetString("model", ""));

	SetFOV(inResourceData->GetFloat("fov", 90));
	SetAnimate(inResourceData->GetBool("animate", false));
	SetFrame(inResourceData->GetInt("frame", 0));
	SetSequnce(inResourceData->GetInt("sequence", 0));
	SetSkin(inResourceData->GetInt("skin", 0));
	SetBodygroup(inResourceData->GetInt("group", 0), inResourceData->GetInt("body", 0));
	SetBlend(0, inResourceData->GetFloat("blend_0", 0));
	SetBlend(1, inResourceData->GetFloat("blend_1", 0));
	SetController(0, inResourceData->GetFloat("controller_0", 0));
	SetController(1, inResourceData->GetFloat("controller_1", 0));
	SetController(2, inResourceData->GetFloat("controller_2", 0));
	SetController(3, inResourceData->GetFloat("controller_3", 0));
	SetMouth(inResourceData->GetInt("mouth", 0));
	const char* buf = inResourceData->GetString("origin", "0 0 0");
	float a, b, c;
	int result = std::sscanf(buf, "%f %f %f", &a, &b, &c);
	if (result != EOF)
		SetModelPos(a, b, c);
	
	buf = inResourceData->GetString("rotate", "0 0 0");
	result = std::sscanf(buf, "%f %f %f", &a, &b, &c);
	if (result != EOF)
		SetModelRotate(a, b, c);

	int rr, gg, bb;
	buf = inResourceData->GetString("light_color", "255 255 255");
	result = std::sscanf(buf, "%d %d %d", &rr, &gg, &bb);
	if (result != EOF)
		SetLightColor(rr, gg, bb);

	SetAmbientLight(inResourceData->GetInt("ambient_light", 32));
	SetShadeLight(inResourceData->GetInt("shade_light", 192));

	buf = inResourceData->GetString("light_origin", "0 0 0");
	result = std::sscanf(buf, "%f %f %f", &a, &b, &c);
	if (result != EOF)
		SetLightOrigin(a, b, c);
}
