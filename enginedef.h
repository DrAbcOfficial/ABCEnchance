#pragma once
typedef struct metaplugins_s{
	bool renderer;
	bool captionmod;
}metaplugins_t;
typedef struct refdef_s{
	vec3_t vieworg;
	vec3_t viewangles;
	color24 ambientlight;
	qboolean onlyClientDraws;

	//Only available in SvEngine
	qboolean useCamera;
	vec3_t r_camera_origin;
}refdef_t;
typedef struct overviewInfo_s{
	vec3_t origin;
	float z_min, z_max;
	float zoom;
	qboolean rotated;
}overviewInfo_t;
typedef struct baseweapon_s{
	char szName[128]; //0x0000 
	int iAmmoType; //0x0080 
	int iAmmoType2; //0x0084 
	int iMax; //0x0088 
	int iMax2; //0x008C 
	int iSlot; //0x0090 
	int iPos; //0x0094 
	int iFlag; //0x0098 
	int iId; //0x009C 
	int iClip; //0x00A0 
	int iClip2; //0x00A4 
	int bSeleted; //0x00A8 
	int iUnkown1; //0x00AC 
	char szName2[128]; //0x00B0 
	//Idk whats dis, idc whats dis
	char pad_0x0130[0x588]; //0x0130
}baseweapon_t;