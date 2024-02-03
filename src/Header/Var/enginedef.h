#pragma once
struct metaplugininfo {
	bool has;
	mh_plugininfo_t info;
};
typedef struct metaplugins_s{
	metaplugininfo renderer;
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