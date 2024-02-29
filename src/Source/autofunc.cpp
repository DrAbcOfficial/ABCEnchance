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
	ADD_COMMAND("+ducktap", []() {g_bAutoDucktap = true; });
	ADD_COMMAND("-ducktap", []() {g_bAutoDucktap = false; });
}

void AutoFunc::AutoJump(usercmd_s* cmd){
	//Auto jump from openag
	if (gCVars.pCVarAutoBunnyJump->value > 0) {
		static bool s_bJumpWasDownLastFrame = false;
		bool shouldReleaseJump = (!g_playerppmove.onground && !g_playerppmove.inwater && g_playerppmove.walking);
		if (s_bJumpWasDownLastFrame && g_playerppmove.onground && !g_playerppmove.inwater && g_playerppmove.walking)
			shouldReleaseJump = true;
		if (shouldReleaseJump)
			cmd->buttons &= ~IN_JUMP;
		s_bJumpWasDownLastFrame = ((cmd->buttons & IN_JUMP) != 0);
	}
}

void AutoFunc::DuckTap(usercmd_s* cmd){
	//Ducktap from bfhl
	if (g_bAutoDucktap) {
		static bool s_bDuckWasDownLastFrame = false;
		cmd->buttons |= IN_DUCK;
		bool shouldReleaseDuck = (!g_playerppmove.onground && !g_playerppmove.inwater && g_playerppmove.walking);
		if (s_bDuckWasDownLastFrame && g_playerppmove.onground && !g_playerppmove.inwater && g_playerppmove.walking)
			shouldReleaseDuck = true;
		if (shouldReleaseDuck)
			cmd->buttons &= ~IN_DUCK;
		s_bDuckWasDownLastFrame = ((cmd->buttons & IN_DUCK) != 0);
	}
}