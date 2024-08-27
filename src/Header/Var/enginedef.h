#pragma once
struct metaplugininfo {
	bool has;
	mh_plugininfo_t info;
};
using metaplugins_t = struct metaplugins_s {
	metaplugininfo renderer;
};
using overviewInfo_t = struct overviewInfo_s {
	vec3_t origin;
	float z_min, z_max;
	float zoom;
	qboolean rotated;
};
using playerstatus_t = struct playerstatus_s {
	bool onground;
	bool inwater;
	bool walking;
};