#pragma once
//========= Copyright ?1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef VOICE_COMMON_H
#define VOICE_COMMON_H
#ifdef _WIN32
#pragma once
#endif
#include <bitset>

#define VOICE_MAX_PLAYERS		32
typedef std::bitset<VOICE_MAX_PLAYERS> CPlayerBitVec;

#endif // VOICE_COMMON_H
