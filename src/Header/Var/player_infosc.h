#pragma once
#if !defined( COM_MODEL_H )
#include "com_model.h"
#endif

typedef struct player_infosc_s{
	// User id on server
	int		userid;

	// User info string
	char	userinfo[MAX_INFO_STRING];

	// Name
	char	name[MAX_SCOREBOARDNAME];

	// Spectator or not, unused
	int		spectator;

	int		ping;
	int		packet_loss;

	// skin information
	char	model[MAX_QPATH];
	int		topcolor;
	int		bottomcolor;

	// last frame rendered
	int		renderframe;

	// Gait frame estimation
	int		gaitsequence;
	float	gaitframe;
	float	gaityaw;
	vec3_t	prevgaitorigin;

	customization_t customdata;

	//Sven Co-op Only
	char hashedcdkey[16];
	uint64 m_nSteamID;
}player_infosc_t;