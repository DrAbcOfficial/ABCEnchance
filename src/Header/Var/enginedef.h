#pragma once
struct metaplugininfo {
	bool has;
	mh_plugininfo_t info;
};
using metaplugins_t = struct metaplugins_s {
	metaplugininfo renderer;
};
using playerstatus_t = struct playerstatus_s {
	bool onground;
	bool inwater;
	bool walking;
};