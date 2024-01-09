#pragma once
void EfxReset();
void R_BloodSprite(float* org, int colorindex, int modelIndex, int modelIndex2, float size);
TEMPENTITY* R_TempModel(float* pos, float* dir, float* angles, float life, int modelIndex, int soundtype);
void pfnPlaybackEvent(int flags, const struct edict_s* pInvoker, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2);