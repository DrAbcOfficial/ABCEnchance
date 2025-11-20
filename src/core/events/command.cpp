#include "metahook.h"
#include "utility/util.h"
#include "command.h"
#define DEFINE_USERCMD_HOOK(name) \
	void(*pfnUserCmd_##name)(); \
	static void __UserCmd_##name()
#define ADD_HOOK(cmd, name) pfnUserCmd_##name = HOOK_COMMAND(cmd, name);

DEFINE_COMMAND_EVENT(Slot, int);
DEFINE_USERCMD_HOOK(Slot1) {
	bool ret = true;
	g_EventCmdSlot.forEach([&](const eventpp::CallbackList<bool(int)>::Handle& handle,
		const eventpp::CallbackList<bool(int)>::Callback& callback) {
			ret &= callback(0);
	});
	if (ret)
		pfnUserCmd_Slot1();
}
DEFINE_USERCMD_HOOK(Slot2) {
	bool ret = true;
	g_EventCmdSlot.forEach([&](const eventpp::CallbackList<bool(int)>::Handle& handle,
		const eventpp::CallbackList<bool(int)>::Callback& callback) {
			ret &= callback(1);
		});
	if (ret)
		pfnUserCmd_Slot2();
}
DEFINE_USERCMD_HOOK(Slot3) {
	bool ret = true;
	g_EventCmdSlot.forEach([&](const eventpp::CallbackList<bool(int)>::Handle& handle,
		const eventpp::CallbackList<bool(int)>::Callback& callback) {
			ret &= callback(2);
		});
	if (ret)
		pfnUserCmd_Slot3();
}
DEFINE_USERCMD_HOOK(Slot4) {
	bool ret = true;
	g_EventCmdSlot.forEach([&](const eventpp::CallbackList<bool(int)>::Handle& handle,
		const eventpp::CallbackList<bool(int)>::Callback& callback) {
			ret &= callback(3);
		});
	if (ret)
		pfnUserCmd_Slot4();
}
DEFINE_USERCMD_HOOK(Slot5) {
	bool ret = true;
	g_EventCmdSlot.forEach([&](const eventpp::CallbackList<bool(int)>::Handle& handle,
		const eventpp::CallbackList<bool(int)>::Callback& callback) {
			ret &= callback(4);
		});
	if (ret)
		pfnUserCmd_Slot5();
}
DEFINE_USERCMD_HOOK(Slot6) {
	bool ret = true;
	g_EventCmdSlot.forEach([&](const eventpp::CallbackList<bool(int)>::Handle& handle,
		const eventpp::CallbackList<bool(int)>::Callback& callback) {
			ret &= callback(5);
		});
	if (ret)
		pfnUserCmd_Slot6();
}
DEFINE_USERCMD_HOOK(Slot7) {
	bool ret = true;
	g_EventCmdSlot.forEach([&](const eventpp::CallbackList<bool(int)>::Handle& handle,
		const eventpp::CallbackList<bool(int)>::Callback& callback) {
			ret &= callback(6);
		});
	if (ret)
		pfnUserCmd_Slot7();
}
DEFINE_USERCMD_HOOK(Slot8) {
	bool ret = true;
	g_EventCmdSlot.forEach([&](const eventpp::CallbackList<bool(int)>::Handle& handle,
		const eventpp::CallbackList<bool(int)>::Callback& callback) {
			ret &= callback(7);
		});
	if (ret)
		pfnUserCmd_Slot8();
}
DEFINE_USERCMD_HOOK(Slot9) {
	bool ret = true;
	g_EventCmdSlot.forEach([&](const eventpp::CallbackList<bool(int)>::Handle& handle,
		const eventpp::CallbackList<bool(int)>::Callback& callback) {
			ret &= callback(8);
		});
	if (ret)
		pfnUserCmd_Slot9();
}
DEFINE_USERCMD_HOOK(Slot10) {
	bool ret = true;
	g_EventCmdSlot.forEach([&](const eventpp::CallbackList<bool(int)>::Handle& handle,
		const eventpp::CallbackList<bool(int)>::Callback& callback) {
			ret &= callback(9);
		});
	if (ret)
		pfnUserCmd_Slot10();
}

DEFINE_COMMAND_EVENT(MissionBrief);
DEFINE_USERCMD_HOOK(MissionBrief) {
	bool ret = true;
	g_EventCmdMissionBrief.forEach([&](const eventpp::CallbackList<bool()>::Handle& handle,
		const eventpp::CallbackList<bool()>::Callback& callback) {
			ret &= callback();
		});
	if (ret)
		pfnUserCmd_MissionBrief();
}
DEFINE_COMMAND_EVENT(NextWeapon);
DEFINE_USERCMD_HOOK(NextWeapon)
{
	bool ret = true;
	g_EventCmdNextWeapon.forEach([&](const eventpp::CallbackList<bool()>::Handle& handle,
		const eventpp::CallbackList<bool()>::Callback& callback) {
			ret &= callback();
		});
	if (ret)
		pfnUserCmd_NextWeapon();
}
DEFINE_COMMAND_EVENT(PrevWeapon);
DEFINE_USERCMD_HOOK(PrevWeapon)
{
	bool ret = true;
	g_EventCmdPrevWeapon.forEach([&](const eventpp::CallbackList<bool()>::Handle& handle,
		const eventpp::CallbackList<bool()>::Callback& callback) {
			ret &= callback();
		});
	if (ret)
		pfnUserCmd_PrevWeapon();
}
DEFINE_COMMAND_EVENT(OpenScoreboard);
DEFINE_USERCMD_HOOK(OpenScoreboard)
{
	bool ret = true;
	g_EventCmdOpenScoreboard.forEach([&](const eventpp::CallbackList<bool()>::Handle& handle,
		const eventpp::CallbackList<bool()>::Callback& callback) {
			ret &= callback();
		});
	if (ret)
		pfnUserCmd_OpenScoreboard();
}
DEFINE_COMMAND_EVENT(CloseScoreboard);
DEFINE_USERCMD_HOOK(CloseScoreboard)
{
	bool ret = true;
	g_EventCmdCloseScoreboard.forEach([&](const eventpp::CallbackList<bool()>::Handle& handle,
		const eventpp::CallbackList<bool()>::Callback& callback) {
			ret &= callback();
		});
	if (ret)
		pfnUserCmd_CloseScoreboard();
}
DEFINE_COMMAND_EVENT(VoteMenu);
DEFINE_USERCMD_HOOK(VoteMenu)
{
	bool ret = true;
	g_EventCmdVoteMenu.forEach([&](const eventpp::CallbackList<bool()>::Handle& handle,
		const eventpp::CallbackList<bool()>::Callback& callback) {
			ret &= callback();
		});
	if (ret)
		pfnUserCmd_VoteMenu();
}
DEFINE_COMMAND_EVENT(Attack1);
DEFINE_USERCMD_HOOK(Attack1)
{
	bool ret = true;
	g_EventCmdAttack1.forEach([&](const eventpp::CallbackList<bool()>::Handle& handle,
		const eventpp::CallbackList<bool()>::Callback& callback) {
			ret &= callback();
		});
	if (ret)
		pfnUserCmd_Attack1();
}
void RegisterCommandEvents() {
	ADD_HOOK("slot1", Slot1);
	ADD_HOOK("slot2", Slot2);
	ADD_HOOK("slot3", Slot3);
	ADD_HOOK("slot4", Slot4);
	ADD_HOOK("slot5", Slot5);
	ADD_HOOK("slot6", Slot6);
	ADD_HOOK("slot7", Slot7);
	ADD_HOOK("slot8", Slot8);
	ADD_HOOK("slot9", Slot9);
	ADD_HOOK("slot10", Slot10);
	ADD_HOOK("missionbriefing", MissionBrief);
	ADD_HOOK("invnext", NextWeapon);
	ADD_HOOK("invprev", PrevWeapon);
	ADD_HOOK("+showscores", OpenScoreboard);
	ADD_HOOK("-showscores", CloseScoreboard);
	ADD_HOOK("votemenu", VoteMenu);
	ADD_HOOK("+attack", Attack1);
}