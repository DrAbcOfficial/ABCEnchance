#pragma once
#include "eventpp/callbacklist.h"

#define DEFINE_NETMESSAGE_EVENT(name, ...) eventpp::CallbackList<void(__VA_ARGS__)> g_Event##name

extern DEFINE_NETMESSAGE_EVENT(AmmoX, int, int);
class Weapon;
extern DEFINE_NETMESSAGE_EVENT(WeaponList, Weapon*);
extern DEFINE_NETMESSAGE_EVENT(InitHUD);
extern DEFINE_NETMESSAGE_EVENT(CustWeapon, int, const char*);
extern DEFINE_NETMESSAGE_EVENT(CurWeapon, int, int, int, int);

extern void RegisterNetworkMessageEventTrigger();