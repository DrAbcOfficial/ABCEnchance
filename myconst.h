#pragma once
#define DAGER_HEALTH 45
#define FCVAR_VALUE (FCVAR_PRINTABLEONLY | FCVAR_CLIENTDLL | FCVAR_ARCHIVE)
#define BASE_GWR_SELECTED 842100225
#define BASE_GWR_UNSELECTED 842100224
#define CenterPos2OpenGLPos(vec, hw, hh) (vec[0])+=(hw);(vec[1])=(hh)-(vec[1])