#pragma once

namespace AutoFunc {
	void Init();
	void Exit();

	void AutoJump(struct usercmd_s* cmd);
	void DuckTap(struct usercmd_s* cmd);

	constexpr char EVENTCMD_DEATH[] = "death";
	constexpr char EVENTCMD_DAMAGE[] = "damage";
	constexpr char EVENTCMD_HEALTH[] = "health";
	constexpr char EVENTCMD_BATTERY[] = "battery";
	constexpr char EVENTCMD_FLASHBATTERY[] = "flash_battery";
	constexpr char EVENTCMD_ADDWEAPON[] = "add_weapon";
	constexpr char EVENTCMD_DROPWEAPON[] = "drop_weapon";
	void TriggerEvent(const char* eventname, 
		const char* param1 = nullptr, const char* param2 = nullptr, 
		const char* param3 = nullptr, const char* param4 = nullptr);
}