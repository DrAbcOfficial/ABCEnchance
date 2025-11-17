#pragma once
#include <metahook.h>
#include "parsemsg.h"
#include "core/resource/weaponresource.h"
#include "strtools.h"
#include "networkmessage.h"

#define DEFINE_NETMESSAGE_HOOK(name, ...) \
	DEFINE_NETMESSAGE_EVENT(name, __VA_ARGS__); \
	static pfnUserMsgHook m_pfn##name; \
	static int __MsgFunc_##name(const char* pszName, int iSize, void* pbuf)
#define ADD_HOOK(name) m_pfn##name = HOOK_MESSAGE(name)

DEFINE_NETMESSAGE_HOOK(AmmoX, int, int)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_LONG();
	g_EventAmmoX(iIndex, iCount);
	return m_pfnAmmoX(pszName, iSize, pbuf);
}

DEFINE_NETMESSAGE_HOOK(WeaponList, Weapon*)
{
	BEGIN_READ(pbuf, iSize);
	Weapon recived_weapon{};
	//read data
	Q_strcpy(recived_weapon.szName, READ_STRING());
	Q_strcpy(recived_weapon.szSprName, recived_weapon.szName);
	recived_weapon.iAmmoType = READ_CHAR();
	recived_weapon.iMax1 = READ_LONG();
	if (recived_weapon.iMax1 == 255)
		recived_weapon.iMax1 = -1;
	recived_weapon.iAmmo2Type = READ_CHAR();
	recived_weapon.iMax2 = READ_LONG();
	if (recived_weapon.iMax2 == 255)
		recived_weapon.iMax2 = -1;
	recived_weapon.iSlot = READ_CHAR();
	recived_weapon.iSlotPos = READ_CHAR();
	recived_weapon.iId = READ_SHORT();
	recived_weapon.iFlags = READ_BYTE();
	g_EventWeaponList(&recived_weapon);
	return m_pfnWeaponList(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(InitHUD)
{
	g_EventInitHUD();
	return m_pfnInitHUD(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(CustWeapon, int, const char*)
{
	BEGIN_READ(pbuf, iSize);
	int id = READ_SHORT();
	std::string_view name = READ_STRING();
	if (name.size() != 0) {
		g_EventCustWeapon(id, name.data());
	}
	return m_pfnCustWeapon(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(CurWeapon, int, int, int, int)
{
	BEGIN_READ(pbuf, iSize);
	int iState = READ_BYTE();
	int iId = READ_SHORT();
	int iClip = READ_LONG();
	int iClip2 = READ_LONG();
	g_EventCurWeapon(iState, iId, iClip, iClip2);
	return m_pfnCurWeapon(pszName, iSize, pbuf);
}

void RegisterNetworkMessageEventTrigger(){
	ADD_HOOK(AmmoX);
	ADD_HOOK(WeaponList);
	ADD_HOOK(InitHUD);
	ADD_HOOK(CustWeapon);
	ADD_HOOK(CurWeapon);
}


