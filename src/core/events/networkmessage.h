#pragma once
#include "eventpp/callbacklist.h"

#define DEFINE_NETMESSAGE_EVENT(name, ...) eventpp::CallbackList<void(__VA_ARGS__)> g_Event##name

extern DEFINE_NETMESSAGE_EVENT(AmmoX, int, int);
class Weapon;
extern DEFINE_NETMESSAGE_EVENT(WeaponList, Weapon*);
extern DEFINE_NETMESSAGE_EVENT(InitHUD);
extern DEFINE_NETMESSAGE_EVENT(CustWeapon, int, const char*);
extern DEFINE_NETMESSAGE_EVENT(CurWeapon, int, int, int, int);
extern DEFINE_NETMESSAGE_EVENT(HideWeapon, int);
extern DEFINE_NETMESSAGE_EVENT(HideHUD, int);
extern DEFINE_NETMESSAGE_EVENT(WeaponSpr, int, const char*);
extern DEFINE_NETMESSAGE_EVENT(WeapPickup, int);
extern DEFINE_NETMESSAGE_EVENT(AmmoPickup, int, int);
extern DEFINE_NETMESSAGE_EVENT(ItemPickup, const char*);
extern DEFINE_NETMESSAGE_EVENT(Damage, int ,int ,int, float[3]);
extern DEFINE_NETMESSAGE_EVENT(Battery, int);
extern DEFINE_NETMESSAGE_EVENT(Health, int);
extern DEFINE_NETMESSAGE_EVENT(ScoreInfo, int, float, int, float, float, int, int, int);
extern DEFINE_NETMESSAGE_EVENT(Spectator, int, bool);
extern DEFINE_NETMESSAGE_EVENT(ServerName, const char*);
extern DEFINE_NETMESSAGE_EVENT(NextMap, const char*);
extern DEFINE_NETMESSAGE_EVENT(TimeEnd, int);
extern DEFINE_NETMESSAGE_EVENT(ShowMenu, int, int, int, const char*);
extern DEFINE_NETMESSAGE_EVENT(VoteMenu, int, const char*, const char*, const char*);
extern DEFINE_NETMESSAGE_EVENT(EndVote);
extern DEFINE_NETMESSAGE_EVENT(MOTD, int, const char*);
extern DEFINE_NETMESSAGE_EVENT(FlashBat, int);
extern DEFINE_NETMESSAGE_EVENT(Flashlight, bool, int);
extern DEFINE_NETMESSAGE_EVENT(ClExtrasInfo);
extern DEFINE_NETMESSAGE_EVENT(MapList, int, int, int, const char**);

enum MetaHookMsgType {
	MHSV_CMD_QUERY_PLUGIN = 1,
	MHSV_CMD_QUERY_CVAR,
	MHSV_CMD_ABC_CUSTOM = 77
};
enum class ABCCustomMsg {
	POPNUMBER = 0
};
using mh_package_t = struct mh_package_s {
	ABCCustomMsg subtype;
	using popnum_t = struct popnum_s
	{
		float origin[3];
		int value;
		int color[4];
	};
	union {
		popnum_t popnum;
	};
};
extern DEFINE_NETMESSAGE_EVENT(MetaHook, int, mh_package_t*);
extern eventpp::CallbackList<bool(int, const char*, const char*, const char*, const char*, const char*)> g_EventTextMsg;
extern void RegisterNetworkMessageEventTrigger();