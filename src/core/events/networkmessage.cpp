#pragma once
#include <metahook.h>
#include "core/resource/weaponresource.h"
#include "core/library/NetworkMessageReader.h"
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
	NetworkMessageReader msg(pbuf, iSize);
	int iIndex = msg.readByte();
	int iCount = msg.readLong();
	g_EventAmmoX(iIndex, iCount);
	return m_pfnAmmoX(pszName, iSize, pbuf);
}

DEFINE_NETMESSAGE_HOOK(WeaponList, Weapon*)
{
	NetworkMessageReader msg(pbuf, iSize);
	Weapon recived_weapon{};
	//read data
	auto weaponName = msg.readString();
	if (weaponName.length() >= MAX_WEAPON_NAME) {
		return m_pfnWeaponList(pszName, iSize, pbuf);
	}
	
	Q_strcpy(recived_weapon.szName, weaponName.c_str());
	Q_strcpy(recived_weapon.szSprName, recived_weapon.szName);
	recived_weapon.iAmmoType = msg.readChar();
	recived_weapon.iMax1 = msg.readLong();
	if (recived_weapon.iMax1 == 255)
		recived_weapon.iMax1 = -1;
	recived_weapon.iAmmo2Type = msg.readChar();
	recived_weapon.iMax2 = msg.readLong();
	if (recived_weapon.iMax2 == 255)
		recived_weapon.iMax2 = -1;
	recived_weapon.iSlot = msg.readChar();
	recived_weapon.iSlotPos = msg.readChar();
	recived_weapon.iId = msg.readShort();
	recived_weapon.iFlags = msg.readByte();
	if (recived_weapon.iId > 0 && recived_weapon.iSlot < MAX_WEAPON_SLOT)
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
	NetworkMessageReader msg(pbuf, iSize);
	int id = msg.readShort();
	std::string name = msg.readString();
	if (name.size() != 0) {
		g_EventCustWeapon(id, name.c_str());
	}
	return m_pfnCustWeapon(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(CurWeapon, int, int, int, int)
{
	NetworkMessageReader msg(pbuf, iSize);
	int iState = msg.readByte();
	int iId = msg.readShort();
	int iClip = msg.readLong();
	int iClip2 = msg.readLong();
	g_EventCurWeapon(iState, iId, iClip, iClip2);
	return m_pfnCurWeapon(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(HideWeapon, int)
{
	NetworkMessageReader msg(pbuf, iSize);
	int token = msg.readByte();
	g_EventHideWeapon(token);
	return m_pfnHideWeapon(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(HideHUD, int)
{
	NetworkMessageReader msg(pbuf, iSize);
	int token = msg.readByte();
	g_EventHideHUD(token);
	return m_pfnHideHUD(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(WeaponSpr, int, const char*)
{
	NetworkMessageReader msg(pbuf, iSize);
	int id = msg.readShort();
	std::string name = msg.readString();
	if (name.size() > 0)
		g_EventWeaponSpr(id, name.c_str());
	return m_pfnWeaponSpr(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(WeapPickup, int)
{
	NetworkMessageReader msg(pbuf, iSize);
	int iIndex = msg.readShort();
	g_EventWeapPickup(iIndex);
	return m_pfnWeapPickup(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(AmmoPickup, int, int)
{
	NetworkMessageReader msg(pbuf, iSize);
	int iIndex = msg.readByte();
	int iCount = msg.readLong();
	g_EventAmmoPickup(iIndex, iCount);
	return m_pfnAmmoPickup(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(ItemPickup, const char*)
{
	NetworkMessageReader msg(pbuf, iSize);
	auto szName = msg.readString();
	g_EventItemPickup(szName.c_str());
	return m_pfnItemPickup(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(Damage, int, int, int, float[3])
{
	NetworkMessageReader msg(pbuf, iSize);
	int armor = msg.readByte();
	int damageTaken = msg.readByte();
	int tiles = msg.readLong();
	if (armor + damageTaken + tiles == 0)
		return m_pfnDamage(pszName, iSize, pbuf);
	vec3_t vecFrom{};
	for (size_t i = 0; i < 3; i++) {
		vecFrom[i] = msg.readCoord();
	}
	g_EventDamage(armor, damageTaken, tiles, vecFrom);
	return m_pfnDamage(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(Battery, int)
{
	NetworkMessageReader msg(pbuf, iSize);
	int battery = msg.readShort();
	g_EventBattery(battery);
	return m_pfnBattery(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(Health, int)
{
	NetworkMessageReader msg(pbuf, iSize);
	int health = msg.readLong();
	g_EventHealth(health);
	return m_pfnHealth(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(ScoreInfo, int, float, int, float, float, int, int, int)
{
	NetworkMessageReader msg(pbuf, iSize);
	int clientIndex = msg.readByte();
	//wtf is not this shit
	if (clientIndex >= 1 && clientIndex <= 33) {
		float flFrags = msg.readFloat();
		int iDeath = msg.readLong();
		float flHealth = msg.readFloat();
		float flArmor = msg.readFloat();
		int iTeamNumber = msg.readByte();
		int iHideExtra = msg.readByte();
		int iAdmin = msg.readByte();
		g_EventScoreInfo(clientIndex, flFrags, iDeath, flHealth, flArmor, iTeamNumber, iHideExtra, iAdmin);
	}
	return m_pfnScoreInfo(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(Spectator, int, bool)
{
	NetworkMessageReader msg(pbuf, iSize);
	int clientIndex = msg.readByte();
	if (clientIndex > 0 && clientIndex <= 32) {
		int beSpectator = msg.readByte();
		g_EventSpectator(clientIndex, beSpectator != 0);
	}
	return m_pfnSpectator(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(ServerName, const char*)
{
	NetworkMessageReader msg(pbuf, iSize);
	auto name = msg.readString();
	g_EventServerName(name.c_str());
	return m_pfnServerName(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(NextMap, const char*)
{
	NetworkMessageReader msg(pbuf, iSize);
	auto map = msg.readString();
	g_EventNextMap(map.c_str());
	return m_pfnNextMap(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(TimeEnd, int)
{
	NetworkMessageReader msg(pbuf, iSize);
	int time = msg.readLong();
	g_EventTimeEnd(time);
	return m_pfnTimeEnd(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(ShowMenu, int, int, int, std::string&)
{
	NetworkMessageReader msg(pbuf, iSize);
	int slot = msg.readShort();
	int time = msg.readChar();
	int bits = msg.readByte();
	auto message = msg.readString();
	g_EventShowMenu(slot, time, bits, message);
	//block hahahaha
	return 1;
}
DEFINE_NETMESSAGE_HOOK(VoteMenu, int, const char*, const char*, const char*)
{
	if (!GetBaseViewPort()->IsVoteEnable())
		return m_pfnVoteMenu(pszName, iSize, pbuf);

	NetworkMessageReader msg(pbuf, iSize);
	int iVoteType = msg.readByte();
	auto content = msg.readString();
	auto yes = msg.readString();
	auto no = msg.readString();
	g_EventVoteMenu(iVoteType, content.c_str(), yes.c_str(), no.c_str());
	return 1;
}
DEFINE_NETMESSAGE_HOOK(EndVote)
{
	g_EventEndVote();
	return m_pfnEndVote(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(MOTD, int ,const char*)
{
	NetworkMessageReader msg(pbuf, iSize);
	int code = msg.readByte();
	auto info = msg.readString();
	g_EventMOTD(code, info.c_str());
	return m_pfnMOTD(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(FlashBat, int)
{
	NetworkMessageReader msg(pbuf, iSize);
	int flash = msg.readByte();
	g_EventFlashBat(flash);
	return m_pfnFlashBat(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(Flashlight, bool, int)
{
	NetworkMessageReader msg(pbuf, iSize);
	int on = msg.readByte();
	int battery = msg.readByte();
	g_EventFlashlight(on > 0, battery);
	return m_pfnFlashlight(pszName, iSize, pbuf);
}
eventpp::CallbackList<bool(int, const char*, const char*, const char*, const char*, const char*)> g_EventTextMsg; 
static pfnUserMsgHook m_pfnTextMsg; 
static int __MsgFunc_TextMsg(const char* pszName, int iSize, void* pbuf)
{
	NetworkMessageReader msg(pbuf, iSize);
	int target = msg.readByte();
	std::string info = msg.readString();
	std::string sstr1 = msg.readString();
	std::string sstr2 = msg.readString();
	std::string sstr3 = msg.readString();
	std::string sstr4 = msg.readString();
	bool ret = true;
	g_EventTextMsg.forEach([&](const eventpp::CallbackList<bool(int, const char*, const char*, const char*, const char*, const char*)>::Handle& handle, 
		const eventpp::CallbackList<bool(int, const char*, const char*, const char*, const char*, const char*)>::Callback& callback) {
			ret &= callback(target, info.c_str(), sstr1.c_str(), sstr2.c_str(), sstr3.c_str(), sstr4.c_str());
	});
	return ret ? m_pfnTextMsg(pszName, iSize, pbuf) : 1;
}
DEFINE_NETMESSAGE_HOOK(ClExtrasInfo)
{
	//Why Encrypt it? is it aes?
	//Funny Encrypt here, plain text length 33, sent length 105
	//x3 Network traffic, lets fuck more server operator
	/*NetworkMessageReader msg(pbuf, iSize);
	int plainDataLength = msg.readLong();
	int ivLength = msg.readLong();
	std::vector<byte> iv{};
	for (int i = 0; i < ivLength; i++) {
		iv.push_back(msg.readByte());
	}
	int encryptLength = msg.readLong();
	std::vector<byte> encrypt{};
	for (int i = 0; i < encryptLength; i++) {
		encrypt.push_back(msg.readByte());
	}
	int enctryptDigestLength = msg.readLong();
	std::vector<byte> digest{};
	for (int i = 0; i < enctryptDigestLength; i++) {
		digest.push_back(msg.readByte());
	}*/
	int result = m_pfnClExtrasInfo(pszName, iSize, pbuf);
	g_EventClExtrasInfo();
	return result;
}
DEFINE_NETMESSAGE_HOOK(MapList, int, int, int, const char**)
{
	NetworkMessageReader msg(pbuf, iSize);
	int clearall = msg.readByte();
	if (clearall == 0) {
		int all_count = msg.readShort();
		g_EventMapList(clearall, all_count, 0, nullptr);
	}
	else {
		int start = msg.readShort();
		int end = msg.readShort();
		std::vector<std::string> maps{};
		for (int i = start; i < end; i++) {
			maps.push_back(msg.readString());
		}
		std::vector<const char*> cstrVec;
		cstrVec.reserve(maps.size());
		for (const auto& s : maps) {
			cstrVec.push_back(s.c_str());
		}
		g_EventMapList(clearall, start, end, cstrVec.data());
	}
	return m_pfnMapList(pszName, iSize, pbuf);
}
DEFINE_NETMESSAGE_HOOK(MetaHook, int, mh_package_t*)
{
	NetworkMessageReader msg(pbuf, iSize);
	int type = msg.readByte();
	switch (type) {
		case MetaHookMsgType::MHSV_CMD_ABC_CUSTOM: {
			ABCCustomMsg sstype = static_cast<ABCCustomMsg>(msg.readByte());
			switch (sstype) {
				case ABCCustomMsg::POPNUMBER: {
					mh_package_t package{
						sstype,
						mh_package_t::popnum_t{
							msg.readCoord(), msg.readCoord(), msg.readCoord(),
							msg.readLong(),
							msg.readByte(), msg.readByte() , msg.readByte() ,msg.readByte()
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


