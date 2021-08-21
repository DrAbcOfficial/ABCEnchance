#include <metahook.h>
#include "hud.h"
#include "weapon.h"
#include "baseammo.h"
int CHudAmmo::Init(void) { return 0; }
int CHudAmmo::VidInit(void) { return 0; }
int CHudAmmo::Draw(float flTime) { return 0; }
void CHudAmmo::Think(void) {}
void CHudAmmo::Reset(void) {}
int CHudAmmo::DrawWList(float flTime) { return 0; }
int CHudAmmo::MsgFunc_CurWeapon(const char* pszName, int iSize, void* pbuf) { return 0; }
int CHudAmmo::MsgFunc_WeaponList(const char* pszName, int iSize, void* pbuf) { return 0; }
int CHudAmmo::MsgFunc_AmmoX(const char* pszName, int iSize, void* pbuf) { return 0; }
int CHudAmmo::MsgFunc_AmmoPickup(const char* pszName, int iSize, void* pbuf) { return 0; }
int CHudAmmo::MsgFunc_WeapPickup(const char* pszName, int iSize, void* pbuf) { return 0; }
int CHudAmmo::MsgFunc_ItemPickup(const char* pszName, int iSize, void* pbuf) { return 0; }
int CHudAmmo::MsgFunc_HideWeapon(const char* pszName, int iSize, void* pbuf) { return 0; }

void CHudAmmo::SlotInput(int iSlot) {}
void _cdecl CHudAmmo::UserCmd_Slot1(void) {}
void _cdecl CHudAmmo::UserCmd_Slot2(void) {}
void _cdecl CHudAmmo::UserCmd_Slot3(void) {}
void _cdecl CHudAmmo::UserCmd_Slot4(void) {}
void _cdecl CHudAmmo::UserCmd_Slot5(void) {}
void _cdecl CHudAmmo::UserCmd_Slot6(void) {}
void _cdecl CHudAmmo::UserCmd_Slot7(void) {}
void _cdecl CHudAmmo::UserCmd_Slot8(void) {}
void _cdecl CHudAmmo::UserCmd_Slot9(void) {}
void _cdecl CHudAmmo::UserCmd_Slot10(void) {}
void _cdecl CHudAmmo::UserCmd_Close(void) {}
void _cdecl CHudAmmo::UserCmd_NextWeapon(void) {}
void _cdecl CHudAmmo::UserCmd_PrevWeapon(void) {}