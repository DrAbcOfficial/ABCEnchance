#include <map>
#include <string>

#include <metahook.h>

#include "usercmd.h"
#include "cvardef.h"
#include "local.h"
#include "config.h"

#include "autofunc.h"

extern struct playerppmoveinfo {
	bool onground;
	bool inwater;
	bool walking;
} g_playerppmove;
static bool g_bAutoDucktap = false;

using concurrentcmd_t = struct {
	std::string cmd;
	std::string concurrent;
	xcommand_t origincall;
};
static std::map<std::string, concurrentcmd_t*> s_dicConcurrentCmds;


static void SetConcurrent(const char* cmd, const char* concurrent) {
	if (g_pMetaHookAPI->FindCmd(cmd) == nullptr)
		return;

	//Oh no, nononononono, oh no
	if (!stricmp(cmd, "concurrent") || !stricmp(cmd, "removeconcurrent")) {
		ConsoleWriteline("concurrent and removeconcurrent does not support this!\n");
	}
	//FIXME: yes, this is stupid, but i dont wanna do a parser for now
	std::string ss = concurrent;
	if (ss.find(cmd) != std::string::npos) {
		ConsoleWriteline("There are recursive calls in concurrent command!\n");
		return;
	}
	if (s_dicConcurrentCmds.find(cmd) == s_dicConcurrentCmds.end()) {
		concurrentcmd_t* concurrentobj = new concurrentcmd_t();
		concurrentobj->cmd = cmd;
		concurrentobj->concurrent = concurrent;
		concurrentobj->origincall = g_pMetaHookAPI->HookCmd(cmd, [](){
			const char* nowcmd = gEngfuncs.Cmd_Argv(0);
			auto finditer = s_dicConcurrentCmds.find(nowcmd);
			if (finditer != s_dicConcurrentCmds.end()) {
				gEngfuncs.pfnClientCmd(finditer->second->concurrent.c_str());
				finditer->second->origincall();
			}
		});
		s_dicConcurrentCmds.emplace(std::make_pair(cmd, concurrentobj));
	}
	else {
		auto obj = s_dicConcurrentCmds[cmd];
		obj->concurrent = concurrent;
	}
}
static void RemoveConcurrent(const char* cmd) {
	auto iter = s_dicConcurrentCmds.find(cmd);
	if (iter != s_dicConcurrentCmds.end()) {
		concurrentcmd_t* obj = iter->second;
		g_pMetaHookAPI->HookCmd(obj->cmd.c_str(), obj->origincall);
		delete obj;
		s_dicConcurrentCmds.erase(iter);
	}
	else
		ConsoleWriteline("There is no concurrent in this command!\n");
}

void AutoFunc::Init(){
	gCVars.pCVarAutoBunnyJump = CREATE_CVAR("cl_autojump", "0", FCVAR_VALUE, nullptr);
	ADD_COMMAND("+ducktap", []() {g_bAutoDucktap = true; });
	ADD_COMMAND("-ducktap", []() {g_bAutoDucktap = false; });


	//Add concurrent cmd
	ADD_COMMAND("concurrent", []() {
		size_t argc = gEngfuncs.Cmd_Argc();
		if (argc < 3) {
			ConsoleWriteline("concurrent [target command] [concurrent command]\n");
			return;
		}
		SetConcurrent(gEngfuncs.Cmd_Argv(1), gEngfuncs.Cmd_Argv(2));
	});
	ADD_COMMAND("removeconcurrent", [](){
		size_t argc = gEngfuncs.Cmd_Argc();
		if (argc < 2) {
			ConsoleWriteline("remove_concurrent [target command]\n");
			return;
		}
		RemoveConcurrent(gEngfuncs.Cmd_Argv(1));
	});
	//Read Concurrent from json
	CABCConfig* config = abcconfig::GetConfig();
	for (auto iter = config->m_dicConcurrentCmd.begin(); iter != config->m_dicConcurrentCmd.end(); iter++) {
		SetConcurrent(iter->first.c_str(), iter->second.c_str());
	}
}

void AutoFunc::Exit(){
	CABCConfig* config = abcconfig::GetConfig();
	config->m_dicConcurrentCmd.clear();
	for (auto iter = s_dicConcurrentCmds.begin(); iter != s_dicConcurrentCmds.end(); iter++) {
		config->m_dicConcurrentCmd.emplace(std::make_pair(iter->first, iter->second->concurrent));
	}
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


void AutoFunc::ConcurrentRun(usercmd_s* cmd){
	//if(s_dicConcurrentCmds.find(cmd.))
}
