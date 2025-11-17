#include <map>
#include <string>
#include <array>
#include <regex>

#include <metahook.h>

#include "usercmd.h"
#include "cvardef.h"
#include "local.h"
#include "config.h"
#include "enginedef.h"
#include "core/events/networkmessage.h"

#include "autofunc.h"

extern playerstatus_t g_playerppmove;
static bool g_bAutoDucktap = false;

using concurrentcmd_t = struct {
	std::string cmd;
	std::string concurrent;
	xcommand_t origincall;
};
static std::map<std::string, concurrentcmd_t*> s_dicConcurrentCmds;
static std::map<std::string, std::string> s_dicEventCmd;
const std::array<std::string, 7> s_aryEvents = {
	AutoFunc::EVENTCMD_DEATH,
	AutoFunc::EVENTCMD_DAMAGE,
	AutoFunc::EVENTCMD_HEALTH,
	AutoFunc::EVENTCMD_BATTERY,
	AutoFunc::EVENTCMD_FLASHBATTERY,
	AutoFunc::EVENTCMD_ADDWEAPON,
	AutoFunc::EVENTCMD_DROPWEAPON
};

static void SetConcurrent(const char* cmd, const char* concurrent) {
	if (g_pMetaHookAPI->FindCmd(cmd) == nullptr)
		return;

	//Oh no, nononononono, oh no
	if (!stricmp(cmd, "concurrent") || !stricmp(cmd, "removeconcurrent")) {
		ConsoleWriteline("concurrent and removeconcurrent does not support this!\n");
		return;
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
	//Add events cmd
	ADD_COMMAND("events", []() {
		static auto error = []() {
			ConsoleWriteline("events [operate] [event] [cmd]\n");
			ConsoleWriteline("operate: 0 = delete, 1 = set\n");
			ConsoleWriteline("aviliable events:\n");
			for (auto iter = s_aryEvents.begin(); iter != s_aryEvents.end(); iter++) {
				std::string ss = "    [" + *iter + "]\n";
				ConsoleWriteline(ss.c_str());
			}
		};
		size_t argc = gEngfuncs.Cmd_Argc();
		if (argc < 3) {
			error();
			return;
		}
		const char* evt = gEngfuncs.Cmd_Argv(2);
		if (std::find(s_aryEvents.begin(), s_aryEvents.end(), evt) == s_aryEvents.end()) {
			error();
			return;
		}
		switch (std::atoi(gEngfuncs.Cmd_Argv(1))) {
		case 0: {
			if (s_dicEventCmd.find(evt) != s_dicEventCmd.end())
				s_dicEventCmd.erase(evt);
			break;
		}
		case 1: {
			if (argc < 4) {
				error();
				return;
			}
			s_dicEventCmd[evt] = gEngfuncs.Cmd_Argv(3); 
			break;
		}
		default:error();break;
		}
		
	});
	//Read Concurrent from json
	CABCConfig* config = abcconfig::GetConfig();
	for (auto iter = config->m_dicConcurrentCmd.begin(); iter != config->m_dicConcurrentCmd.end(); iter++) {
		SetConcurrent(iter->first.c_str(), iter->second.c_str());
	}
	s_dicEventCmd = config->m_dicEventsCmd;

	g_EventCurWeapon.append([](int state, int id, int, int) {
		if (state <= 0) {
			switch (id) {
			case -1:
			case 0:
				AutoFunc::TriggerEvent(AutoFunc::EVENTCMD_DEATH);
				break;
			}
		}
	});
	g_EventDamage.append([](int armor, int damage, int tiles, float*){
		AutoFunc::TriggerEvent(AutoFunc::EVENTCMD_DAMAGE,
		std::to_string(damage).c_str(), std::to_string(armor).c_str(), std::to_string(tiles).c_str());
	});
	g_EventBattery.append([](int battery) {
		AutoFunc::TriggerEvent(AutoFunc::EVENTCMD_BATTERY, std::to_string(battery).c_str());
	});
	g_EventHealth.append([](int health) {
		AutoFunc::TriggerEvent(AutoFunc::EVENTCMD_HEALTH, std::to_string(health).c_str());
	});
	g_EventFlashBat.append([](int flash) {
		AutoFunc::TriggerEvent(AutoFunc::EVENTCMD_FLASHBATTERY, std::to_string(flash).c_str());
	});
}

void AutoFunc::Exit(){
	CABCConfig* config = abcconfig::GetConfig();
	config->m_dicConcurrentCmd.clear();
	for (auto iter = s_dicConcurrentCmds.begin(); iter != s_dicConcurrentCmds.end(); iter++) {
		config->m_dicConcurrentCmd.emplace(std::make_pair(iter->first, iter->second->concurrent));
	}
	config->m_dicEventsCmd = s_dicEventCmd;
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

//death
void AutoFunc::TriggerEvent(const char* eventname, 
	const char* param1, const char* param2, const char* param3, const char* param4){
	if (s_dicEventCmd.find(eventname) != s_dicEventCmd.end()) {
		std::string ssbuf = s_dicEventCmd[eventname];
		if (ssbuf.size() > 0) {
			static auto replace = [](const char* key, const char* value, std::string& ss) {
				if (value) {
					std::string temp1 = ss;
					std::regex pattern(key);
					std::string temp = std::regex_replace(temp1, pattern, value);
					ss = temp;
				}
			};
			replace("\\{param1\\}", param1, ssbuf);
			replace("\\{param2\\}", param2, ssbuf);
			replace("\\{param3\\}", param3, ssbuf);
			replace("\\{param4\\}", param4, ssbuf);
			EngineClientCmd(ssbuf.c_str());
		}
	}
}