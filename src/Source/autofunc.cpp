#include <metahook.h>

#include "usercmd.h"
#include "cvardef.h"
#include "local.h"

#include "autofunc.h"

extern struct playerppmoveinfo {
	bool onground;
	bool inwater;
	bool walking;
} g_playerppmove;
static bool g_bAutoDucktap = false;
void AutoFunc::Init(){
	gCVars.pCVarAutoBunnyJump = CREATE_CVAR("cl_autojump", "0", FCVAR_VALUE, nullptr);
	gEngfuncs.pfnAddCommand("+ducktap", []() {g_bAutoDucktap = true; });
	gEngfuncs.pfnAddCommand("-ducktap", []() {g_bAutoDucktap = false; });
}

void AutoFunc::AutoJump(usercmd_s* cmd){
	//Auto jump from openag
	if (gCVars.pCVarAutoBunnyJump->value > 0) {
		static bool s_jump_was_down_last_frame = false;
		bool should_release_jump = (!g_playerppmove.onground && !g_playerppmove.inwater && g_playerppmove.walking);
		if (s_jump_was_down_last_frame && g_playerppmove.onground && !g_playerppmove.inwater && g_playerppmove.walking)
			should_release_jump = true;
		if (should_release_jump)
			cmd->buttons &= ~IN_JUMP;
		s_jump_was_down_last_frame = ((cmd->buttons & IN_JUMP) != 0);
	}
}

void AutoFunc::DuckTap(usercmd_s* cmd){
	if (g_bAutoDucktap) {
		static bool s_bDuckWasDownLastFrame = false;
		cmd->buttons |= IN_DUCK;
		bool inWater = g_playerppmove.inwater;
		bool isWalking = g_playerppmove.walking;
		bool isOnground = g_playerppmove.onground;
		bool shouldReleaseDuck = (!isOnground && !inWater && isWalking);
		if (s_bDuckWasDownLastFrame && isOnground && !inWater && isWalking)
			shouldReleaseDuck = true;
		if (shouldReleaseDuck)
			cmd->buttons &= ~IN_DUCK;
		s_bDuckWasDownLastFrame = ((cmd->buttons & IN_DUCK) != 0);
	}
}