#pragma once
#ifndef __AMMO_H__
#define __AMMO_H__

#ifndef WRECTH
#include "wrect.h"
#endif // !WRECTH

typedef int HSPRITE;

#define MAX_WEAPON_NAME 128


#define WEAPON_FLAGS_SELECTONEMPTY	1
#define ITEM_FLAG_SELECTONEMPTY		1
#define ITEM_FLAG_NOAUTORELOAD		2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY	4
#define ITEM_FLAG_LIMITINWORLD		8
#define ITEM_FLAG_EXHAUSTIBLE		16 // A player can totally exhaust their ammo supply and lose this weapon

#define WEAPON_IS_ONTARGET 0x40

class WEAPON{
public:
	char	szName[MAX_WEAPON_NAME];
	char	szSprName[MAX_WEAPON_NAME]; //256
	int		iAmmoType;
	int		iAmmo2Type;
	int		iMax1;
	int		iMax2;
	size_t	iSlot;
	size_t	iSlotPos;
	int		iFlags;
	size_t	iId = 0;
	int		iClip;
	int		iClip2; //416

	int		iCount; //420

	int iState;//424

	HSPRITE hActive;//428
	wrect_t rcActive;//432
	HSPRITE hInactive;//436
	wrect_t rcInactive;//440
	HSPRITE	hAmmo;
	wrect_t rcAmmo;
	HSPRITE hAmmo2;
	wrect_t rcAmmo2;
	HSPRITE hCrosshair;
	wrect_t rcCrosshair;
	HSPRITE hAutoaim;
	wrect_t rcAutoaim;
	HSPRITE hZoomedCrosshair;
	wrect_t rcZoomedCrosshair;
	HSPRITE hZoomedAutoaim;
	wrect_t rcZoomedAutoaim;
};

typedef int AMMO;


#endif