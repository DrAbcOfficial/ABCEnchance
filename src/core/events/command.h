#pragma once
#include "eventpp/callbacklist.h"
#pragma once
#define DEFINE_COMMAND_EVENT(name, ...) eventpp::CallbackList<bool(__VA_ARGS__)> g_EventCmd##name
extern DEFINE_COMMAND_EVENT(Slot, int);
extern DEFINE_COMMAND_EVENT(MissionBrief);
extern DEFINE_COMMAND_EVENT(NextWeapon);
extern DEFINE_COMMAND_EVENT(PrevWeapon);
extern DEFINE_COMMAND_EVENT(OpenScoreboard);
extern DEFINE_COMMAND_EVENT(CloseScoreboard);
extern DEFINE_COMMAND_EVENT(VoteMenu);
extern DEFINE_COMMAND_EVENT(Attack1);
extern void RegisterCommandEvents();
