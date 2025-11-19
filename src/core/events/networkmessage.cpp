#pragma once
#include <metahook.h>
#include "parsemsg.h"
#include "core/resource/weaponresource.h"
#include "strtools.h"
#include "hud/Viewport.h"
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
DEFINE_NETMESSAGE_HOOK(HideWeapon, int)
{
	BEGIN_READ(pbuf, iSize);
	int token = READ_BYTE();
	g_EventHideWeapon(token);
	return m_pfnHideWeapon(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(HideHUD, int)
{
	BEGIN_READ(pbuf, iSize);
	int token = READ_BYTE();
	g_EventHideHUD(token);
	return m_pfnHideHUD(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(WeaponSpr, int, const char*)
{
	BEGIN_READ(pbuf, iSize);
	int id = READ_SHORT();
	std::string_view name = READ_STRING();
	if (name.size() > 0)
		g_EventWeaponSpr(id, name.data());
	return m_pfnWeaponSpr(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(WeapPickup, int)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_SHORT();
	g_EventWeapPickup(iIndex);
	return m_pfnWeapPickup(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(AmmoPickup, int, int)
{
	BEGIN_READ(pbuf, iSize);
	int iIndex = READ_BYTE();
	int iCount = READ_LONG();
	g_EventAmmoPickup(iIndex, iCount);
	return m_pfnAmmoPickup(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(ItemPickup, const char*)
{
	BEGIN_READ(pbuf, iSize);
	const char* szName = READ_STRING();
	g_EventItemPickup(szName);
	return m_pfnItemPickup(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(Damage, int, int, int, float[3])
{
	BEGIN_READ(pbuf, iSize);
	int armor = READ_BYTE();
	int damageTaken = READ_BYTE();
	int tiles = READ_LONG();
	if (armor + damageTaken + tiles == 0)
		return m_pfnDamage(pszName, iSize, pbuf);
	vec3_t vecFrom{};
	for (size_t i = 0; i < 3; i++) {
		vecFrom[i] = READ_COORD();
	}
	g_EventDamage(armor, damageTaken, tiles, vecFrom);
	return m_pfnDamage(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(Battery, int)
{
	BEGIN_READ(pbuf, iSize);
	int battery = READ_SHORT();
	g_EventBattery(battery);
	return m_pfnBattery(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(Health, int)
{
	BEGIN_READ(pbuf, iSize);
	int health = READ_LONG();
	g_EventHealth(health);
	return m_pfnHealth(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(ScoreInfo, int, float, int, float, float, int, int, int)
{
	BEGIN_READ(pbuf, iSize);
	int clientIndex = READ_BYTE();
	//wtf is not this shit
	if (clientIndex >= 1 && clientIndex <= 33) {
		float flFrags = READ_FLOAT();
		int iDeath = READ_LONG();
		float flHealth = READ_FLOAT();
		float flArmor = READ_FLOAT();
		int iTeamNumber = READ_BYTE();
		int iHideExtra = READ_BYTE();
		int iAdmin = READ_BYTE();
		g_EventScoreInfo(clientIndex, flFrags, iDeath, flHealth, flArmor, iTeamNumber, iHideExtra, iAdmin);
	}
	return m_pfnScoreInfo(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(Spectator, int, bool)
{
	BEGIN_READ(pbuf, iSize);
	int clientIndex = READ_BYTE();
	if (clientIndex > 0 && clientIndex <= 32) {
		int beSpectator = READ_BYTE();
		g_EventSpectator(clientIndex, beSpectator != 0);
	}
	return m_pfnSpectator(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(ServerName, const char*)
{
	BEGIN_READ(pbuf, iSize);
	const char* name = READ_STRING();
	g_EventServerName(name);
	return m_pfnServerName(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(NextMap, const char*)
{
	BEGIN_READ(pbuf, iSize);
	const char* map = READ_STRING();
	g_EventNextMap(map);
	return m_pfnNextMap(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(TimeEnd, int)
{
	BEGIN_READ(pbuf, iSize);
	int time = READ_LONG();
	g_EventTimeEnd(time);
	return m_pfnTimeEnd(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(ShowMenu, int, int, int, const char*)
{
	BEGIN_READ(pbuf, iSize);
	int slot = READ_SHORT();
	int time = READ_CHAR();
	int bits = READ_BYTE();
	const char* message = READ_STRING();
	g_EventShowMenu(slot, time, bits, message);
	//block hahahaha
	return 1;
}
DEFINE_NETMESSAGE_HOOK(VoteMenu, int, const char*, const char*, const char*)
{
	if (!GetBaseViewPort()->IsVoteEnable())
		return m_pfnVoteMenu(pszName, iSize, pbuf);

	BEGIN_READ(pbuf, iSize);
	int iVoteType = READ_BYTE();
	const char* content = READ_STRING();
	const char* yes = READ_STRING();
	const char* no = READ_STRING();
	g_EventVoteMenu(iVoteType, content, yes, no);
	return 1;
}
DEFINE_NETMESSAGE_HOOK(EndVote)
{
	g_EventEndVote();
	return m_pfnEndVote(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(MOTD, int ,const char*)
{
	BEGIN_READ(pbuf, iSize);
	int code = READ_BYTE();
	char* msg = READ_STRING();
	g_EventMOTD(code, msg);
	return m_pfnMOTD(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(FlashBat, int)
{
	BEGIN_READ(pbuf, iSize);
	int flash = READ_BYTE();
	g_EventFlashBat(flash);
	return m_pfnFlashBat(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(Flashlight, bool, int)
{
	BEGIN_READ(pbuf, iSize);
	int on = READ_BYTE();
	int battery = READ_BYTE();
	g_EventFlashlight(on > 0, battery);
	return m_pfnFlashlight(pszName, iSize, pbuf);
}
eventpp::CallbackList<bool(int, const char*, const char*, const char*, const char*, const char*)> g_EventTextMsg; 
static pfnUserMsgHook m_pfnTextMsg; 
static int __MsgFunc_TextMsg(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int target = READ_BYTE();
	const char* msg = READ_STRING();
	const char* sstr1 = READ_STRING();
	const char* sstr2 = READ_STRING();
	const char* sstr3 = READ_STRING();
	const char* sstr4 = READ_STRING();
	bool ret = true;
	g_EventTextMsg.forEach([&](const eventpp::CallbackList<bool(int, const char*, const char*, const char*, const char*, const char*)>::Handle& handle, 
		const eventpp::CallbackList<bool(int, const char*, const char*, const char*, const char*, const char*)>::Callback& callback) {
			ret &= callback(target, msg, sstr1, sstr2, sstr3, sstr4);
	});
	return ret ? m_pfnTextMsg(pszName, iSize, pbuf) : 1;
}
DEFINE_NETMESSAGE_HOOK(ClExtrasInfo)
{
	//Why Encrypt it? is it aes?
	//Funny Encrypt here, plain text length 33, sent length 105
	//x3 Network traffic, lets fuck more server operator
	/*BEGIN_READ(pbuf, iSize);
	int plainDataLength = READ_LONG();
	int ivLength = READ_LONG();
	std::vector<byte> iv{};
	for (int i = 0; i < ivLength; i++) {
		iv.push_back(READ_BYTE());
	}
	int encryptLength = READ_LONG();
	std::vector<byte> encrypt{};
	for (int i = 0; i < encryptLength; i++) {
		encrypt.push_back(READ_BYTE());
	}
	int enctryptDigestLength = READ_LONG();
	std::vector<byte> digest{};
	for (int i = 0; i < enctryptDigestLength; i++) {
		digest.push_back(READ_BYTE());
	}*/
	int result = m_pfnClExtrasInfo(pszName, iSize, pbuf);
	g_EventClExtrasInfo();
	return result;
}
DEFINE_NETMESSAGE_HOOK(MapList, int, int, int, const char**)
{
	BEGIN_READ(pbuf, iSize);
	int clearall = READ_BYTE();
	if (clearall == 0) {
		int all_count = READ_SHORT();
		g_EventMapList(clearall, all_count, 0, nullptr);
	}
	else {
		int start = READ_SHORT();
		int end = READ_SHORT();
		std::vector<const char*> maps{};
		for (int i = start; i < end; i++) {
			char* map = READ_STRING();
			maps.push_back(map);
		}
		g_EventMapList(clearall, start, end, maps.data());
	}
	return m_pfnMapList(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(MetaHook, int, mh_package_t*)
{
	BEGIN_READ(pbuf, iSize);
	int type = READ_BYTE();
	switch (type) {
		case MetaHookMsgType::MHSV_CMD_ABC_CUSTOM: {
			ABCCustomMsg sstype = static_cast<ABCCustomMsg>(READ_BYTE());
			switch (sstype) {
				case ABCCustomMsg::POPNUMBER: {
					mh_package_t package{
						sstype,
						mh_package_t::popnum_t{
							READ_COORD(), READ_COORD(), READ_COORD(),
							READ_LONG(),
							READ_BYTE(), READ_BYTE() , READ_BYTE() ,READ_BYTE()
						}
					};
					g_EventMetaHook(type, &package);
					break;
				}
			}
			break;
		}
		default:break;
	}
	return m_pfnMetaHook ? m_pfnMetaHook(pszName, iSize, pbuf) : 0;
}

void RegisterNetworkMessageEventTrigger(){
	ADD_HOOK(AmmoX);
	ADD_HOOK(WeaponList);
	ADD_HOOK(InitHUD);
	ADD_HOOK(CustWeapon);
	ADD_HOOK(CurWeapon);
	ADD_HOOK(HideWeapon);
	ADD_HOOK(HideHUD);
	ADD_HOOK(WeaponSpr);
	ADD_HOOK(WeapPickup);
	ADD_HOOK(AmmoPickup);
	ADD_HOOK(ItemPickup);
	ADD_HOOK(Damage);
	ADD_HOOK(Battery);
	ADD_HOOK(Health);
	ADD_HOOK(ScoreInfo);
	ADD_HOOK(Spectator);
	ADD_HOOK(ServerName);
	ADD_HOOK(NextMap);
	ADD_HOOK(TimeEnd);
	ADD_HOOK(ShowMenu);
	ADD_HOOK(VoteMenu);
	ADD_HOOK(EndVote);
	ADD_HOOK(MOTD);
	ADD_HOOK(FlashBat);
	ADD_HOOK(Flashlight);
	ADD_HOOK(TextMsg);
	ADD_HOOK(ClExtrasInfo);
	ADD_HOOK(MapList);
	if (!m_pfnMetaHook)
		gEngfuncs.pfnHookUserMsg("MetaHook", __MsgFunc_MetaHook);
}


