#pragma once
void R_Blood(float* org, float* dir, int pcolor, int speed);
void R_BloodSprite(float* org, int colorindex, int modelIndex, int modelIndex2, float size);
void R_Explosion(float* pos, int model, float scale, float framerate, int flags);
void R_RicochetSprite(float* pos, struct model_s* pmodel, float duration, float scale);